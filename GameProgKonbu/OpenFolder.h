#pragma once
#include <Shlobj.h>
#pragma comment(lib, "Ole32.lib")

//  フォルダを選択する
//　処理は止まります
//@param p_hWnd        親ウインドウのハンドル
//@param p_cSetStr     初期選択ディレクトリ
//@param p_cRootStr    初期ディレクトリ
//@param p_cCaptionStr 説明の文字列
//@param p_uiFlags     表示フラグ
dxle::tstring BrowseForFolder(HWND p_hWnd, const TCHAR* p_cSetStr
	, const TCHAR* p_cRootStr, const TCHAR* p_cCaptionStr,
	UINT p_uiFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI);
