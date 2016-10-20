//#include "DxLib.h"
#include "resource.h"
#include "Option.h"
#include "Mouse.h"
#include "Data.h"

namespace {
	///DxLib初期化処理
	///@return true:失敗
	bool MyDxLib_Init();
}
int APIENTRY _tWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int)
{
	try
	{
		if (MyDxLib_Init() != false){
			return -1;
		}

		std::unique_ptr<Sequence> seqence = std::make_unique<Option_Sequence>();
		while (ProcessMessage() == 0)
		{
			KeyInputData::GetIns().Update();
			Mouse::GetIns().update();
			Data::GetIns().update();

			auto new_seq = seqence->update();
			if (new_seq){
				seqence = std::move(new_seq);
			}
			else {
				seqence->draw();
			}

			ScreenFlip();
			ClearDrawScreen();
		}

		DxLib_End();

		return 0;
	}
	//例外処理
	catch (const std::exception& error) {
		DxLib_End();
		MessageBoxA(NULL, error.what(), "error", MB_OK);
	}
	catch (...) {
		DxLib_End();
		MessageBoxA(NULL, "unknown error", "unknown error", MB_OK);
	}
	return -1;
}

namespace {
	bool MyDxLib_Init()
	{
#	ifndef _DEBUG
		SetOutApplicationLogValidFlag(FALSE);//Log.txtを作成しない
#	endif
		ChangeWindowMode(TRUE);
		{
			int width_max=640, height_max=480;
			int width_tmp=0, height_tmp=0;
			for (int i = 0, num = GetDisplayNum(); i < num; ++i) {
				GetDisplayMaxResolution(&width_tmp, &height_tmp, i);
				width_max  = std::max( width_max,  width_tmp);
				height_max = std::max(height_max, height_tmp);
			}
			SetGraphMode(width_max, height_max, 32);
		}
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);
		SetWindowSize(320, 240);
		SetWindowIconID(IDI_ICON1);//左上にアイコンを表示
		SetMainWindowText(_T("Combu Contest"));

		SetAlwaysRunFlag(TRUE);
		// 最大化ボタンが存在するウインドウモードに変更
		SetWindowStyleMode(10);

		if (DxLib_Init() == -1){ return true; }

		SetDrawScreen(DX_SCREEN_BACK);

		return false;
	}
}
