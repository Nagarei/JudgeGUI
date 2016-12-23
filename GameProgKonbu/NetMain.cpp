#ifdef PARENT_MODE
#include "NetMain.h"
#include "test.h"

namespace {
}

NetMain::NetMain(Server_info server_info_)
	: server_info(std::move(server_info_))
	, NetHandles()
{
	compile_taskmanager::start();
}

std::unique_ptr<Sequence> NetMain::update()
{
	//通信

	// 新しい接続があったらそのネットワークハンドルを得る
	{int new_NetHandle = DxLib::GetNewAcceptNetWork();
	if (new_NetHandle != -1) {
		NewHandles.push_back(new_NetHandle);
	}
	}

	auto get_data_bace = [](void* buf_ptr, size_t size, unsigned& DataLength, int nethandle) {
		if (DataLength < size) { IEを送る; }
		DxLib::NetWorkRecv(nethandle, buf_ptr, size); 
		DataLength -= size;
	};
	// 新規接続の処理
	for (auto& newhandle : NewHandles)
	{
		// データの量を取得
		auto DataLength = (unsigned)DxLib::GetNetWorkDataLength(newhandle);
		バージョンデータ取得;
	}
	// 送られてきたデータのチェック
	for (auto& nethandle : NetHandles)
	{
		// データの量を取得
		auto DataLength = (unsigned)DxLib::GetNetWorkDataLength(nethandle);

		// 取得してない受信データ量が０じゃない場合
		if (DataLength > 0) {

			Send_Data::Type type;
			do {
#				define GET_data_(buf) get_data_bace(&buf, sizeof(buf), DataLength, nethandle)


				GET_data_(type);
				if (type == Send_Data::Type::Submit) {
					Send_Data::Submit_head head;
					GET_data_(head);
					auto codestr_buf = std::make_unique<TCHAR[]>(head.length);
					get_data_bace(codestr_buf.get(), head.length, DataLength, nethandle);
					//テスト登録
					compile_taskmanager::set_test(
						0,
						std::make_unique<test_Local>(
							problem_dir_set{ head.problem_num, CONTEST_PROBLEM_DIR,
								_T("LOG\\"), problem_name[head.problem_num] },
							head.user_name,
							WriteTempFile(codestr_buf.get(), head.length)
						)
					);
				}
				else {

				}


#				undef GET_data_
			} while (DataLength > 0);
		GETDATAROOP_BREAK:
		}
	}


	// 新たに切断されたネットワークハンドルを得る
	for (int LostNetWorkHandle; (LostNetWorkHandle = DxLib::GetLostNetWork()) != -1;)
	{
		// 切断された接続を削除
		NetHandles.erase(std::find(NetHandles.begin(), NetHandles.end(), LostNetWorkHandle));
		DxLib::CloseNetWork(LostNetWorkHandle);
	}

	return nullptr;
}



#endif // PARENT_MODE
