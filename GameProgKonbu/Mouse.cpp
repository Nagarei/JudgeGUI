#include "Mouse.h"

void Mouse::update()
{
	//現在情報の取得
	DxLib::GetMousePoint(&now_pos.x, &now_pos.y);
	int now_input = DxLib::GetMouseInput();//ログの後にキューに積むため保留

	//ログのチェック
	{int log_input;
	for (dxle::pointi log_pos; DxLib::GetMouseInputLog(&log_input, &log_pos.x, &log_pos.y) == 0;)
	{
		if (log_input & ~now_input)
		{
			//今押されていないが、前に押されていた
			//=>クリックとして処理
			click_point.emplace_back(click_data{ log_pos, log_input });
		}
		else
		{
			//押し続けている
			//=>ドラッグとして処理
			drag_data data;
			data.start = log_pos;
			data.value = now_pos - data.start;
			data.is_drag = (data.value != 0);
			erase_drag(now_input & log_input);
			drag_point[now_input & log_input] = data;
		}
	}}

	//ドラッグの処理
	//既存の物を更新
	for (auto i = drag_point.begin(), iend = drag_point.end(); i != iend;)
	{
		if (now_input & i->first) {
			//押し続けている
			i->second.value = now_pos - i->second.start;
			i->second.is_drag |= (i->second.value != 0);
			now_input &= ~i->first;//処理済み登録
			++i;
		}
		else {
			//離された
			i = erase_drag(i);
		}
	}

	//未処理の物をとりあえずドラッグとして処理
	for (size_t i = 0; i < sizeof(int); ++i)
	{
		if (now_input & (1 << i)) {
			//新規入力
			//とりあえずドラッグとして処理
			drag_data data;
			data.start = now_pos;
			//data.value = 0;
			data.is_drag = false;
			//erase_drag(now_input & (1 << i));
			drag_point[now_input & (1 << i)] = data;
		}
	}
}
void Mouse::erase_drag(int input)
{
	//入力の後処理
	auto iter = drag_point.find(input);
	if (iter != drag_point.end()) {
		erase_drag(iter);
	}
}

auto Mouse::erase_drag(decltype(drag_point)::const_iterator iter)->decltype(drag_point)::iterator
{
	if (iter->second.is_drag == false) {
		//クリックとして処理
		assert(iter->second.value == 0);
		click_point.emplace_back(click_data{ iter->second.start, iter->first });
	}
	return drag_point.erase(iter);
}
