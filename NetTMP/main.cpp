//#include "DxLib.h"
#include "resource.h"
#include "Mouse.h"
//#include "Data.h"
#include "SetLocale.h"
#include "fps.h"
#include "test.h"
#include "NetCommon.h"
#include "Tasks.h"

namespace {
	//DxLib初期化処理
	//@return true:失敗
	bool MyDxLib_Init();

	//@return ファイル名
	dxle::tstring WriteTempFile(const TCHAR* str_raw, uint32_t length);
}
int APIENTRY _tWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int)
{
	try
	{
		if (MyDxLib_Init() != false){
			return -1;
		}

		const auto white = DxLib::GetColor(255, 255, 255);
		//通信処理
		Server_info server_info;
		std::vector<int> NetHandles;// ネットワークハンドル

		//自機の情報取得
		do {
			//ポート番号取得
			DrawString(0, 0, _T("ポート番号を入力してください。(Escで65534になります)"), white);
			server_info.port_num = KeyInputNumber(0, 20, MAXUINT16, 0, TRUE);
			if (server_info.port_num == MAXUINT16 + 1) { server_info.port_num = 65534; }//キャンセル時処理
			DrawFormatString(0, 20, white, _T("%d"), server_info.port_num);
			if (PreparationListenNetWork(server_info.port_num) == 0) {//接続受付
				break;//success
			}
			//error and continue
			DrawString(0, 40, _T("エラーです。入力しなおしてください。"), white);
			WaitKey();
			ClearDrawScreen();
		} while (!ProcessMessage());
		ClearDrawScreen();
		//IPアドレス取得
		DxLib::GetMyIPAddress(&server_info.ipdata);
		//情報書き出し
		WriteServerInfo(server_info);

		auto draw = [&server_info, &NetHandles, &white]() {
			ClearDrawScreen();
			DrawFormatString(0, 20, white, _T("ポート: %d"), server_info.port_num);
			DrawFormatString(0, 40, white, _T("IPアドレス: %d.%d.%d.%d"),
				server_info.ipdata.d1, server_info.ipdata.d2,
				server_info.ipdata.d3, server_info.ipdata.d4);
			DrawFormatString(0, 60, white, _T("接続: %ud機"), NetHandles.size());
			//タスクの数;
		};
		//メインループ
		draw();
		while (ProcessMessage() == 0)
		{
			bool data_updated = false;

			//通信
			{
				// 新しい接続があったらそのネットワークハンドルを得る
				{int new_NetHandle = DxLib::GetNewAcceptNetWork();
				if (new_NetHandle != -1) {
					NetHandles.push_back(new_NetHandle);
					data_updated = true;
				}}

				// データが送られて来た
				for (auto& nethandle : NetHandles)
				{
					auto DataLength = (unsigned)DxLib::GetNetWorkDataLength(nethandle);// データの量を取得

					// 取得してない受信データ量が０じゃない場合
					if (DataLength > 0) {
						data_updated = true;

						Send_Data::Type type;
						do {
#							define GET_data_ex_(buf_ptr, size)\
								if (DataLength < size) { goto GETDATAROOP_BREAK; }\
								DxLib::NetWorkRecv(nethandle, buf_ptr, size);\
								DataLength -= size
#							define GET_data_(buf) GET_data_ex_(&buf, sizeof(buf))


							GET_data_(type);
							switch (type)
							{
							case Send_Data::Type::Submit: {
								Send_Data::Submit_head head;
								GET_data_(head);
								auto codestr_buf = std::make_unique<TCHAR[]>(head.length);
								GET_data_ex_(codestr_buf.get(), head.length);
								//テスト登録
								compile_taskmanager::set_test(
									head.problem_num,
									WriteTempFile(codestr_buf.get(), head.length)
									);
								break;
							}
							default:
								break;
							}


#							undef GET_data_
#							undef GET_data_ex_
						} while (DataLength > 0);
						GETDATAROOP_BREAK:
					}
				}


				// 新たに切断されたネットワークハンドルを得る
				for (int LostNetWorkHandle; (LostNetWorkHandle = DxLib::GetLostNetWork()) != -1;)
				{
					// 切断された接続を削除
					data_updated = true;
					NetHandles.erase(std::find(NetHandles.begin(), NetHandles.end(), LostNetWorkHandle));
					DxLib::CloseNetWork(LostNetWorkHandle);
				}
			}

			//描画
			if (data_updated) {
				draw();
			}

			//テスト処理
			compile_taskmanager::update();

			Sleep(17);
		}

		DxLib_End();

		return 0;
	}
	//例外処理
	catch (const std::exception& error) {
		MessageBoxA(NULL, error.what(), "error", MB_OK);
		DxLib_End();
	}
	catch (...) {
		MessageBoxA(NULL, "unknown error", "unknown error", MB_OK);
		DxLib_End();
	}
	return -1;
}

namespace {
	bool MyDxLib_Init()
	{
		SetLocale();

#	ifndef _DEBUG
		SetOutApplicationLogValidFlag(FALSE);//Log.txtを作成しない
#	endif
		ChangeWindowMode(TRUE);
		SetWindowSize(320, 240);
		SetWindowIconID(IDI_ICON1);//左上にアイコンを表示
		SetMainWindowText(_T("Combu Contest Parent"));

		SetAlwaysRunFlag(TRUE);

		if (DxLib_Init() == -1){ return true; }

		SetFontSize(20);
		//SetDrawScreen(DX_SCREEN_BACK);

		return false;
	}
}

//template<typename T, size_t N, size_t... Ns>
//class md_array : public md_array<md_array<T, Ns...>,N>
//{
//public:
//	void fill_all(const T& v) {
//		for (auto& i : *this) {
//			i.fill_all(v);
//		}
//	}
//};
//template<typename T, size_t N>
//class md_array<T, N> : public std::array<T, N>
//{
//public:
//	void fill_all(const T& v) {
//		std::array<T, N>::fill(v);
//	}
//};

//void f()
//{
//	md_array<int, 5, 4, 3> arr;//int arr[5][4][3]
//	arr[4][3][2] = 0;
//	arr.fill_all(0);
//	arr[0].fill_all(0);
//}
