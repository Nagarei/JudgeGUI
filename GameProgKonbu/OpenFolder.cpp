#include "OpenFolder.h"
#include "my_utility.h"

namespace
{

	struct SP_ITEMIDLIST {
	private:
		std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)> value;
	public:
		SP_ITEMIDLIST() :value(nullptr, CoTaskMemFree) {}
		explicit SP_ITEMIDLIST(ITEMIDLIST* value_)
			:value(value_, CoTaskMemFree) {}
		ITEMIDLIST* get() { return value.get(); }
		operator LPITEMIDLIST() { return value.get(); }
		LPITEMIDLIST operator->() { return value.get(); }
	};

	SP_ITEMIDLIST GetItemIDList(const TCHAR* p_cFileStr)
	{
		if (p_cFileStr == nullptr || *p_cFileStr == '\0') {
			return SP_ITEMIDLIST{};
		}

		LPSHELLFOLDER pDesktopFolder;

		if (::SHGetDesktopFolder(&pDesktopFolder) != NOERROR) {
			return SP_ITEMIDLIST{};
		}
		FINALLY([&]() {
			pDesktopFolder->Release();
		});

		//文字コード変換
#ifdef UNICODE
		WCHAR ochPath[MAX_PATH];
		StrCpyW(ochPath, p_cFileStr);
#else
		OLECHAR ochPath[MAX_PATH];
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, p_cFileStr, -1, ochPath, MAX_PATH);
#endif
		//　実際にITEMIDLISTを取得します。

		LPITEMIDLIST pIDL;
		ULONG chEaten;	//文字列のサイズを受け取ります。
		ULONG dwAttributes;	//属性を受け取ります。
		auto hRes = pDesktopFolder->ParseDisplayName(NULL, NULL, ochPath, &chEaten, &pIDL, &dwAttributes);
		if (hRes != NOERROR) {
			pIDL = NULL;
		}

		return SP_ITEMIDLIST{ pIDL };
	}

	int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{
		//　初期化時にフォルダを選択させます。
		if (uMsg == BFFM_INITIALIZED)
		{
			LPITEMIDLIST pSetIDL = *reinterpret_cast<SP_ITEMIDLIST*>(lpData);
			::SendMessage(hwnd, BFFM_ENABLEOK, TRUE, TRUE);
			::SendMessage(hwnd, BFFM_SETSELECTION, FALSE, (LPARAM)pSetIDL);
		}
		else if (uMsg == BFFM_VALIDATEFAILED)
		{
			//ユーザーがエディットボックスに無効な名前をタイプしました
			::SendMessage(hwnd, BFFM_ENABLEOK, FALSE, FALSE);
		}
		else
		{
			//フォルダ選択ダイアログでフォルダが選択された時に処理が必要であればここに記載
			::SendMessage(hwnd, BFFM_ENABLEOK, TRUE, TRUE);
			//TCHAR chText[MAX_PATH];
			//if (::SHGetPathFromIDList(lParam, chText)) {
			//	::SendMessage(hwnd, BFFM_SETSTATUSTEXT, TRUE, (LPARAM)chText);
			//}
		}

		return 0;
	}

}//namespace

//  フォルダを選択する
//　処理は止まります
//@param p_hWnd        親ウインドウのハンドル
//@param p_cSetStr     初期選択ディレクトリ
//@param p_cRootStr    初期ディレクトリ
//@param p_cCaptionStr 説明の文字列
//@param p_uiFlags     表示フラグ
dxle::tstring BrowseForFolder(HWND p_hWnd, const TCHAR* p_cSetStr
	, const TCHAR* p_cRootStr, const TCHAR* p_cCaptionStr, UINT p_uiFlags)
{
	TCHAR chPutFolder[MAX_PATH * 2] = _T("");

	auto pRootIDL = GetItemIDList(p_cRootStr);
	auto pSetIDL  = GetItemIDList(p_cSetStr);

	BROWSEINFO		stBInfo;
	dxle::tstring	cRetStr;

	//　構造体を初期化します。
	memset(&stBInfo, 0, sizeof(stBInfo));
	stBInfo.pidlRoot = pRootIDL;	//ルートフォルダです。
	stBInfo.hwndOwner = p_hWnd;	//表示するダイアログの親ウィンドウのハンドルです。
	stBInfo.pszDisplayName = chPutFolder;	//選択されているフォルダ名を受けます。
	stBInfo.lpszTitle = p_cCaptionStr;	//説明の文字列です。
	stBInfo.ulFlags = p_uiFlags;	//表示フラグです。
	stBInfo.lpfn = (BFFCALLBACK)BrowseCallbackProc;	//プロシージャへのポインタです。
	stBInfo.lParam = reinterpret_cast<LPARAM>(&pSetIDL);	//選択するフォルダへのＩＤです。

	//　ダイアログボックスを表示します。
	//　処理は止まります
	LPITEMIDLIST pidlRetFolder = ::SHBrowseForFolder(&stBInfo);

	//　pidlRetFolderにはそのフォルダを表すアイテムＩＤリストへのポインタが
	//　入っています。chPutFolderには選択されたフォルダ名（非フルパス）だけ
	//　しか入っていないので、このポインタを利用します。

	if (pidlRetFolder == NULL)
	{
		//入力はキャンセルされました
		chPutFolder[0] = _T('\0');
	}
	else
	{
		//　フルパスを取得します。
		::SHGetPathFromIDList(pidlRetFolder, chPutFolder);
		::CoTaskMemFree(pidlRetFolder);
	}

	return chPutFolder;
}
