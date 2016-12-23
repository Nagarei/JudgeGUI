#ifdef PARENT_MODE
#include "NetMain.h"
#include "test.h"

namespace {
	constexpr int32_t port_min = 49152;
	constexpr int32_t port_max = 65535;//閉区間注意！！
}

NetInit::NetInit()
	: port_input_handle(DxLib::MakeKeyInput(
		std::numeric_limits<uint16_t>::digits10 + 1, FALSE, FALSE, TRUE, FALSE, FALSE
	))
	, is_error(false)
{
	SetFontSize(20);
	SetKeyInputString(_T("65534"), port_input_handle);
	SetActiveKeyInput(port_input_handle);
}
NetInit::~NetInit()
{
	DeleteKeyInput(port_input_handle);
}

std::unique_ptr<Sequence> NetInit::update()
{
	switch (CheckKeyInput(port_input_handle))
	{
	case 0:
		break;
	case 1: {
		//入力終了
		auto& port_num = server_info.port_num;
		TCHAR str_buf[std::numeric_limits<uint16_t>::digits10 + 1 + 1];
		GetKeyInputString(str_buf, port_input_handle);
		port_num = DxLib::atoiDx(str_buf);
		if (port_min <= port_num && port_num <= port_max) {
			//ポート開放
			if (PreparationListenNetWork(port_num) == 0) {
				//success
				//情報書き出し
				DxLib::GetMyIPAddress(&server_info.ipdata);
				WriteServerInfo(server_info);
				//return ~;
			}
		}
		SetActiveKeyInput(port_input_handle);
		is_error = true;
	}
		break;
	case 2:
	default:
		throw std::runtime_error("DxLib::CheckKeyInputが不正値を返しました");
		break;
	}
	return nullptr;
}

void NetInit::draw() const
{
	const auto white = DxLib::GetColor(255, 255, 255);

	DrawString(0, 0, _T("ポート番号を入力してください"), white);
	DrawFormatString(0, 20, white, _T("ヒント：[%d,%d]で他プロ"),
		port_min, port_max);
	DrawString(0, 40, _T("　　　　セスが未使用の値を入力"), white);
	DrawKeyInputString(0, 60, port_input_handle);
	DrawKeyInputModeString(320, 0);// 入力モードを描画
	if (is_error) {
		DrawString(0, 80, _T("エラーです　再入力してください"), white);
	}
}

#endif // PARENT_MODE
