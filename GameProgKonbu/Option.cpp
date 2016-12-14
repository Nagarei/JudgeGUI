#include "Option.h"
#include "Contest.h"
#include "Mouse.h"

Option Option::ins;
namespace {
}

Option_Sequence::Option_Sequence()
	: selecting(Select::local)
	, str_graph(_T("data/mode_select.png"))
	, name_input_handle(-1)
	, buttons()
	, is_username_inputing(false)
{
	DxLib::SetDefaultFontState(_T("ＭＳ ゴシック"), 35, 2);
	DxLib::SetKeyInputCursorBrinkFlag(TRUE);
	DxLib::SetKeyInputCursorBrinkTime(500);

	DxLib::SetWindowSizeChangeEnableFlag(FALSE, FALSE);
	DxLib::SetWindowSize(320, 240);
	//ユーザー名取得
	TCHAR user_name_buf[UNLEN + 1] = _T("unknown");
	DWORD user_name_buf_length = sizeof(user_name_buf) / sizeof(user_name_buf[0]);
	GetUserName(user_name_buf, &user_name_buf_length);
	if (user_name_buf_length <= sizeof(user_name_buf) / sizeof(user_name_buf[0])) {
		strncpyDx(Option::ins.username, user_name_buf, Option::username_length - 1);
	}
	Option::ins.username[Option::username_length - 1] = _T('\0');

	//ボタン
	buttons[0] = Button{ { 5,   5 }, { 303, 38 }, _T("") };
	buttons[1] = Button{ { 5,  52 }, { 303, 38 }, _T("") };
	buttons[2] = Button{ { 5, 100 }, { 303, 38 }, _T("") };
	buttons[3] = Button{ { 4, 191 }, { 303, 38 }, _T("") };
	constexpr dxle::rgb selecting_color{ 0, 197, 30 };
	constexpr dxle::rgb normal_color{ 154, 130, 0 };
	for (auto& i : buttons) {
		i.set_on_color(selecting_color, selecting_color, selecting_color);
		i.set_out_color(normal_color, normal_color, normal_color);
	}
}
Option_Sequence::~Option_Sequence()
{
	DxLib::SetWindowSizeChangeEnableFlag(TRUE, FALSE);
	DxLib::SetWindowSize(640, 480);
	DxLib::SetWindowMinSize(320, 240);
	DxLib::DeleteKeyInput(name_input_handle);
}

std::unique_ptr<Sequence> Option_Sequence::update()
{
	//名前入力中
	if (is_username_inputing)
	{
		auto& mouse = Mouse::GetIns();
		mouse.click_log_clear();
		switch (CheckKeyInput(name_input_handle))
		{
		case 0:
			break;
		case 1:
			//入力終了
			GetKeyInputString(Option::ins.username, name_input_handle);
			is_username_inputing = false;
			break;
		case 2:
		default:
			SetActiveKeyInput(-1);
			SetKeyInputString(Option::GetIns().username, name_input_handle);
			is_username_inputing = false;
			break;
		}
		return nullptr;
	}
	//else

	//キーボード入力
	auto& key = KeyInputData::GetIns();
	if (key.GetDirectionKeyInput(KeyInputData::KEY_UP)) {//上キーが押されている場合
		//一つ上に
		--selecting;
	}
	if (key.GetDirectionKeyInput(KeyInputData::KEY_DOWN)) {//下キーが押されている場合
		//一つ下に
		++selecting;
	}

	auto select_end = [this](Select input)->std::unique_ptr<Sequence> {
		switch (input)
		{
		case Option_Sequence::Select::local:
			//local
			Data::GetIns().InitProblem(_T("Problems"), _T("Problems"), Option::ins.username, false);
			return std::make_unique<Contest>(0);
		case Option_Sequence::Select::server_contact:
			Data::GetIns().InitProblem(_T("Z:\\競技プログラミング\\問題セット\\Problems"), _T("Z:\\競技プログラミング\\問題セット\\Problems"), Option::ins.username, false);
			return std::make_unique<Contest>(0);
		case Option_Sequence::Select::contest:
			Data::GetIns().InitProblem(_T("Z:\\競技プログラミング\\contest\\"), _T("Z:\\競技プログラミング\\contest\\LOGLOGLOG"), Option::ins.username, true);
			return std::make_unique<Contest>(0);
			break;
		case Option_Sequence::Select::name:
			if (name_input_handle == -1) {
				name_input_handle = DxLib::MakeKeyInput(Option::username_length - 1, TRUE, FALSE, FALSE, FALSE, FALSE);
				SetKeyInputString(Option::GetIns().username, name_input_handle);
			}
			else {
				ReStartKeyInput(name_input_handle);
			}
			is_username_inputing = true;
			SetActiveKeyInput(name_input_handle);
			return nullptr;
		default:
			assert(false);
			break;
		}
		return nullptr;
	};

	if (key.GetNewKeyInput(KEY_INPUT_RETURN)) {
		return select_end(selecting);
	}

	//マウス入力
	auto& mouse = Mouse::GetIns();
	while (!mouse.click_log_is_empty())
	{
		auto click = mouse.click_log_front(); mouse.click_log_pop();
		if (click.type == MOUSE_INPUT_LEFT)
		{
			if (buttons[0].IsInArea(click.pos)) {
				return select_end(Select::local);
			}
			if (buttons[1].IsInArea(click.pos)) {
				return select_end(Select::server_contact);
			}
			if (buttons[2].IsInArea(click.pos)) {
				return select_end(Select::contest);
			}
			if (buttons[3].IsInArea(click.pos)) {
				return select_end(Select::name);
			}
		}
	}

	if (buttons[0].IsInArea(mouse.get_now_pos())) {
		selecting = Select::local;
	}
	else if (buttons[1].IsInArea(mouse.get_now_pos())) {
		selecting = Select::server_contact;
	}
	else if (buttons[2].IsInArea(mouse.get_now_pos())) {
		selecting = Select::contest;
	}
	else if (buttons[3].IsInArea(mouse.get_now_pos())) {
		selecting = Select::name;
	}

	return nullptr;
}

void Option_Sequence::draw() const
{

	buttons[0].draw_box(selecting == Select::local);
	buttons[1].draw_box(selecting == Select::server_contact);
	buttons[2].draw_box(selecting == Select::contest);
	if (is_username_inputing) {
		auto area = buttons[3].get_area();
		DrawButton(area.first, area.second, dxle::color_tag::magenta, dxle::color_tag::magenta);
	}
	else {
		buttons[3].draw_box(selecting == Select::name);
	}

	//名前の標示
	if (is_username_inputing)
	{//名前入力中

		// 入力モードを描画
		//DrawKeyInputModeString(320, 0);

		// 入力途中の文字列を描画
		auto area = buttons[3].get_area();
		SetDrawArea(area.first.x + 110, area.first.y, area.first.x + area.second.width, area.first.y + area.second.height);
		DrawKeyInputString(area.first.x + 110, area.first.y, name_input_handle);
		SetDrawAreaFull();
	}
	else
	{
		auto pos1 = buttons[3].get_area().first;
		DrawString(pos1.x + 110, pos1.y, Option::GetIns().username, dxle::dx_color_param(dxle::color_tag::white).get());
	}

	str_graph.DrawGraph({ 0,0 }, true);
}
