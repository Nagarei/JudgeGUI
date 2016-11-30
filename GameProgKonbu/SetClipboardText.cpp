
#include "SetClipboardText.h"
#include "popup.h"

void My_SetClipboardText(dxle::tstring str)
{
	for (auto iter = str.begin(), iter_end = str.end(); iter != iter_end; ++iter) {
		if (*iter == _T('\n')) {
			if (iter == str.begin() || *(iter - 1) != _T('\r')) {
				iter = str.insert(iter, _T('\r'));
				iter_end = str.end();
				++iter;
			}
		}
	}
	//クリップボードにコピー
	SetClipboardText(str.c_str());
	popup::set(_T("コピーしました"));
}
