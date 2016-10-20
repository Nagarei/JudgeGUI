#include <time.h>
#include <direct.h>
#include "Shlwapi.h"
#include "DxLib.h"
#include "resource.h"

#pragma comment(lib, "shlwapi.lib")

#define DATA_DIR_NAME "\\\\SERVER-PC\\Server\\WorkSpace\\CombChat"	//データ管理用フォルダ

#define WIN_SIZE_X		640							//窓横
#define WIN_SIZE_Y		480							//窓縦
#define MAX_STRING_SIZE	54							//文字の最大
#define DISP_SENTENCE	28

#define STR_ERR		-1								//エラー
#define STR_MSG		1								//文字列がメッセージ
#define STR_CONT	2								//文字列が前の文の続き
#define STR_FILE	3								//文字列がファイル
#define STR_INFO	4								//文字列が情報

#define BUFFER_TYPE_STRING	1						//バッファーが文字列
#define BUFFER_TYPE_FILE	2						//バッファーがファイル
#define BUFFER_TYPE_INFO	3						//バッファーが情報

struct Chat {										//チャット内部データ
	int StrAttribute;
	char UserName[MAX_PATH];
	char DrawString[MAX_PATH];
	int FileNum;
};

struct SendBuffer {
	int BufferType;				//バッファーの種類
	char BufferStr[1024];		//バッファーの内容
};

bool GetChatName(char *ChatNameBuffer, int nSize);	//チャットでの名前を取得
int SendStr(const char *SendString);				//文字列出力
int SendFile(const char *FilePath);					//ファイルアップロード&書き込み
int CommandStrToInt(const char *Command);			//コマンドを数値に変換
Chat* ReadChat(int *nBuffer);						//コマンド読み込み
int SendInfo(const char *InfoStr);					//情報送信

char ChatFilePath[MAX_PATH];
char ChatName[MAX_PATH];
char DateStr[MAX_PATH];

void ShutDownCallback(void *Data)
{
	char Logout[MAX_PATH];

	sprintf_s(Logout, "%sが退出しました。", ChatName);

	SendInfo(Logout);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	SetOutApplicationLogValidFlag(FALSE);				//ログを出さない
	ChangeWindowMode(TRUE);								//ウィンドウモードにする
	SetAlwaysRunFlag(TRUE);								//常に動作
	SetGraphMode(WIN_SIZE_X, WIN_SIZE_Y, 16);			//解像度変更
	SetMainWindowText("昆布チャット");					//タイトル変更
	SetWindowIconID(IDI_ICON1);							//アイコン変更

	//シャットダウン時に実行
	SetShutdownCallbackFunction(ShutDownCallback, NULL, "Exiting...");

	//ウィンドウ非表示
	SetWindowVisibleFlag(FALSE);

	if (DxLib_Init() == -1) return -1;					//DXライブラリ初期化処理

	SetWindowUserCloseEnableFlag(FALSE);				//閉じる無効
	SetDragFileValidFlag(TRUE);							//D&Dを有効
	SetDrawScreen(DX_SCREEN_BACK);						//描画先画面を裏にする
	SetFontSize(16);									//フォントサイズ指定

	int LastReadTime = 0;
	time_t timer = time(NULL);
	struct tm t_st;
	localtime_s(&t_st, &timer);
	sprintf_s(ChatFilePath, "%s\\%4d-%02d-%02d\\Chat.dat", DATA_DIR_NAME, 1900 + t_st.tm_year, 1 + t_st.tm_mon, t_st.tm_mday);
	GetChatName(ChatName, MAX_PATH);
	sprintf_s(DateStr, "%s\\%4d-%02d-%02d", DATA_DIR_NAME, 1900 + t_st.tm_year, 1 + t_st.tm_mon, t_st.tm_mday);
	_mkdir(DATA_DIR_NAME);
	while (PathFileExists(DATA_DIR_NAME) == FALSE){
		_mkdir(DATA_DIR_NAME);
	}
	_mkdir(DateStr);
	FILETIME LastWriteTime_Now;
	FILETIME LastWriteTime_Last = { 0, 0 };
	Chat *ChatDatas = NULL;
	int ChatDataNum = 0;
	bool Scroll = false;
	SendBuffer *SBuffer = NULL;
	int BufferNum = 0;
	int TopSentence = 0;
	int KeyInputHandle;
	int MouseX, MouseY;
	int MouseState;
	bool ScrollArrowUP = false, ScrollArrowDOWN = false, ScrollBar = false;
	bool SentenceClicking = false;

	KeyInputHandle = MakeKeyInput(1023, FALSE, FALSE, FALSE);
	SetActiveKeyInput(KeyInputHandle);

	SetFontSize(28);											//フォントサイズ指定
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_8X8);				//フォントタイプ指定

	int FontHandle = CreateFontToHandle(NULL, 16, -1);

	char Login[MAX_PATH];

	sprintf_s(Login, "%sが入室しました。", ChatName);

	if (SendInfo(Login) == -1) {
		BufferNum++;
		SBuffer = (SendBuffer *)realloc(SBuffer, sizeof(SendBuffer)*BufferNum);
		SBuffer[BufferNum - 1].BufferType = BUFFER_TYPE_INFO;
		strcpy_s(SBuffer[BufferNum - 1].BufferStr, Login);
	}

	//ウィンドウ表示
	SetWindowVisibleFlag(TRUE);

	while (ProcessMessage() == 0) {

		ClearDrawScreen();										//画面のクリア

		/*********************処理*********************/
		MouseState = GetMouseInput();
		GetMousePoint(&MouseX, &MouseY);

		//毎秒ファイル確認
		if (GetNowCount() >= LastReadTime + 100) {
			SetDragFileValidFlag(TRUE);							//D&Dを有効
			HANDLE hFile = CreateFile(ChatFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE) {
				GetFileTime(hFile, NULL, NULL, &LastWriteTime_Now);
				if (LastWriteTime_Now.dwHighDateTime != LastWriteTime_Last.dwHighDateTime || LastWriteTime_Now.dwLowDateTime != LastWriteTime_Last.dwLowDateTime) {
					//ファイル読み込み
					Chat *TmpChatDatas = NULL;
					int TmpChatDataNum;
					TmpChatDatas = ReadChat(&TmpChatDataNum);

					if (TmpChatDataNum != -1) {
						ChatDataNum = TmpChatDataNum;
						TopSentence = ChatDataNum - DISP_SENTENCE;
						if (ChatDataNum > 28) {
							Scroll = true;							//28行以上でスクロール
						}
						ChatDatas = TmpChatDatas;
						//最後に読み込んだ時間とファイル更新日時記録
						LastWriteTime_Last = LastWriteTime_Now;
						LastReadTime = GetNowCount();

						if (GetWindowActiveFlag() == FALSE) {
							FlashWindow(GetMainWindowHandle(), TRUE);		//非アクティブ時に更新されたらアイコン点滅
						}
					}
				}
				CloseHandle(hFile);
			}
			else {
				_mkdir(DATA_DIR_NAME);
				_mkdir(DateStr);
			}
		}

			while (GetDragFileNum() > 0) {								//ファイルD&D処理
				char FilePath[MAX_PATH];

				GetDragFilePath(FilePath);							//D&Dファイル名取得

				if (SendFile(FilePath) == -1) {						//ファイルアップロード&書き込み
					BufferNum++;
					SBuffer = (SendBuffer *)realloc(SBuffer, sizeof(SendBuffer)*BufferNum);
					SBuffer[BufferNum - 1].BufferType = BUFFER_TYPE_FILE;
					strcpy_s(SBuffer[BufferNum - 1].BufferStr, FilePath);
				}
			}

		//閉じられたら最小化
		if (GetWindowUserCloseFlag(TRUE)) {
			ShowWindow(GetMainWindowHandle(), SW_MINIMIZE);
		}

		//文字列送信
		if (CheckKeyInput(KeyInputHandle)) {
			char Send[1024];
			GetKeyInputString(Send, KeyInputHandle);
			if (!(Send[0] == '\0')) {
				if (SendStr(Send) == -1) {
					BufferNum++;
					SBuffer = (SendBuffer *)realloc(SBuffer, sizeof(SendBuffer)*BufferNum);
					SBuffer[BufferNum - 1].BufferType = BUFFER_TYPE_STRING;
					strcpy_s(SBuffer[BufferNum - 1].BufferStr, Send);
				}
			}
			SetActiveKeyInput(KeyInputHandle);
			SetKeyInputString("", KeyInputHandle);
		}

		//バッファー消化
		while (BufferNum > 0) {
			if (SBuffer[0].BufferType == BUFFER_TYPE_STRING) {
				if (SendStr(SBuffer[0].BufferStr) == -1) {
					break;
				}
				else {
					BufferNum--;
					for (int i = 0; i < BufferNum; i++) {
						SBuffer[i] = SBuffer[i + 1];
					}
					SBuffer = (SendBuffer *)realloc(SBuffer, sizeof(SendBuffer)*BufferNum);
				}
			}
			else if (SBuffer[0].BufferType == BUFFER_TYPE_FILE) {
				if (SendFile(SBuffer[0].BufferStr) == -1) {
					break;
				}
				else {
					BufferNum--;
					for (int i = 0; i < BufferNum; i++) {
						SBuffer[i] = SBuffer[i + 1];
					}
					SBuffer = (SendBuffer *)realloc(SBuffer, sizeof(SendBuffer)*BufferNum);
				}
			}
			else if (SBuffer[0].BufferType == BUFFER_TYPE_INFO) {
				if (SendInfo(SBuffer[0].BufferStr) == -1) {
					break;
				}
				else {
					BufferNum--;
					for (int i = 0; i < BufferNum; i++) {
						SBuffer[i] = SBuffer[i + 1];
					}
					SBuffer = (SendBuffer *)realloc(SBuffer, sizeof(SendBuffer)*BufferNum);
				}
			}
		}

		//スクロール
		if (Scroll) {
			ScrollArrowUP = false;
			ScrollArrowDOWN = false;
			if (!(MouseState&MOUSE_INPUT_LEFT)) {
				ScrollBar = false;
			}
			TopSentence -= GetMouseWheelRotVol() * 3;
			if (CheckHitKey(KEY_INPUT_UP)) {
				TopSentence -= 2;
			}
			if (CheckHitKey(KEY_INPUT_DOWN)) {
				TopSentence += 2;
			}
			if (MouseX >= WIN_SIZE_X - 17 && MouseX < WIN_SIZE_X) {
				if (MouseY >= 0 && MouseY < 15) {
					ScrollArrowUP = true;
				}
				else if (MouseY >= WIN_SIZE_Y - 15 && MouseY < WIN_SIZE_Y) {
					ScrollArrowDOWN = true;
				}
				else if (MouseY >= 15 + (WIN_SIZE_Y - 30)*TopSentence / (ChatDataNum) && MouseY < 15 + (WIN_SIZE_Y - 30)*(TopSentence + DISP_SENTENCE) / (ChatDataNum)) {
					ScrollBar = true;
				}
				if (MouseState&MOUSE_INPUT_LEFT) {
					if (MouseY >= 0 && MouseY < 15) {
						TopSentence -= 2;
					}
					else if (MouseY >= WIN_SIZE_Y - 15 && MouseY < WIN_SIZE_Y) {
						TopSentence += 2;
					}
					else {
						TopSentence = (MouseY - 15)*ChatDataNum / (WIN_SIZE_Y - 30) - DISP_SENTENCE / 2;
					}
				}
			}
			if ((MouseState&MOUSE_INPUT_LEFT) && ScrollBar) {
				TopSentence = (MouseY - 15)*ChatDataNum / (WIN_SIZE_Y - 30) - DISP_SENTENCE / 2;
			}
		}
		//補正
		if (TopSentence > ChatDataNum - DISP_SENTENCE) {
			TopSentence = ChatDataNum - DISP_SENTENCE;
		}
		if (TopSentence < 0) {
			TopSentence = 0;
		}

		//ファイルクリック
		int SelectingNum = -1;
		int SelectingNum_End = -1;
		if (MouseX >= 0 && MouseX < WIN_SIZE_X - 17) {
			if (MouseY >= 0 && MouseY < 16 * DISP_SENTENCE) {
				SelectingNum = MouseY / 16 + TopSentence;
			}
		}
		if (SelectingNum > ChatDataNum) {
			SelectingNum = -1;
		}
		SelectingNum_End = SelectingNum;
		if (SelectingNum != -1 && (ChatDatas[SelectingNum].StrAttribute == STR_CONT || ChatDatas[SelectingNum].StrAttribute == STR_MSG)){
			while (ChatDatas[SelectingNum].StrAttribute == STR_CONT){
				SelectingNum--;
			}
			SelectingNum_End = SelectingNum;
			SelectingNum_End++;
			while (ChatDatas[SelectingNum_End].StrAttribute == STR_CONT){
				SelectingNum_End++;
			}
			SelectingNum_End--;
		}

		char DLPath[MAX_PATH];
		ExpandEnvironmentStrings("%UserProfile%", DLPath, MAX_PATH);
		sprintf_s(DLPath, "%s\\Documents\\昆布チャット", DLPath);

		if ((MouseState&MOUSE_INPUT_LEFT) && SentenceClicking == false) {
			SentenceClicking = true;
			if (SelectingNum != -1) {
				if (ChatDatas[SelectingNum].StrAttribute == STR_FILE) {
					//ファイルがクリックされた
					_mkdir(DLPath);
					char UpLoadFilePath[MAX_PATH];
					char LocalFilePath[MAX_PATH];

					sprintf_s(UpLoadFilePath, "%s\\%d.dat", DateStr, ChatDatas[SelectingNum].FileNum);
					sprintf_s(LocalFilePath, "%s\\%s", DLPath, ChatDatas[SelectingNum].DrawString);

					CopyFile(UpLoadFilePath, LocalFilePath, FALSE);

					ShellExecute(NULL, "explore", DLPath, NULL, NULL, SW_SHOWNORMAL);
				}
				else if (ChatDatas[SelectingNum].StrAttribute == STR_MSG){
					//文字列がクリックされた
					//クリップボードへコピー
					char CopyStr[1024];
					CopyStr[0] = '\0';
					for (int i = SelectingNum; i <= SelectingNum_End; i++){
						sprintf_s(CopyStr, "%s%s", CopyStr, ChatDatas[i].DrawString);
					}
					SetClipboardText(CopyStr);
				}
			}
		}
		else if (!(MouseState&MOUSE_INPUT_LEFT)) {
			SentenceClicking = false;
		}

		/*********************描画*********************/

		DrawBox(0, 0, WIN_SIZE_X, WIN_SIZE_Y, GetColor(255, 255, 255), TRUE);

		for (int i = TopSentence, DispSentence = 0; i < TopSentence + DISP_SENTENCE && i < ChatDataNum; i++, DispSentence++) {
			if (SelectingNum != -1 && i == SelectingNum && GetWindowActiveFlag()) {
				DrawBox(0, (SelectingNum - TopSentence) * 16, WIN_SIZE_X - 17, (SelectingNum_End - TopSentence + 1) * 16, GetColor(229, 243, 255), TRUE);
			}
			if (SelectingNum != -1 && i == SelectingNum && GetWindowActiveFlag() == FALSE && ChatDatas[SelectingNum].StrAttribute == STR_FILE) {
				DrawBox(0, (SelectingNum - TopSentence) * 16, WIN_SIZE_X - 17, (SelectingNum_End - TopSentence + 1) * 16, GetColor(229, 243, 255), TRUE);
			}
			switch (ChatDatas[i].StrAttribute) {
			case STR_MSG:
				DrawStringToHandle(118 - GetDrawStringWidthToHandle(ChatDatas[i].UserName, 12, FontHandle) - 7, 16 * DispSentence, ChatDatas[i].UserName, GetColor(0, 0, 0), FontHandle);
				DrawStringToHandle(118, 16 * DispSentence, ":", GetColor(0, 0, 0), FontHandle);
				DrawStringToHandle(128, 16 * DispSentence, ChatDatas[i].DrawString, GetColor(0, 0, 0), FontHandle);
				break;
			case STR_CONT:
				DrawStringToHandle(128, 16 * DispSentence, ChatDatas[i].DrawString, GetColor(0, 0, 0), FontHandle);
				break;
			case STR_FILE:
				DrawStringToHandle(118 - GetDrawStringWidthToHandle(ChatDatas[i].UserName, 12, FontHandle) - 7, 16 * DispSentence, ChatDatas[i].UserName, GetColor(0, 0, 0), FontHandle);
				DrawStringToHandle(118, 16 * DispSentence, ":", GetColor(0, 0, 0), FontHandle);
				DrawStringToHandle(128, 16 * DispSentence, ChatDatas[i].DrawString, GetColor(0, 51, 204), FontHandle);
				break;
			case STR_INFO:
				DrawStringToHandle(128, 16 * DispSentence, ChatDatas[i].DrawString, GetColor(53, 178, 0), FontHandle);
				break;
			}
		}

		//スクロールバー
		DrawBox(WIN_SIZE_X - 17, 0, WIN_SIZE_X, WIN_SIZE_Y, GetColor(230, 231, 232), TRUE);
		if (Scroll == true) {
			DrawTriangle(WIN_SIZE_X - 8, 5, WIN_SIZE_X - 13, 9, WIN_SIZE_X - 4, 9, GetColor(134, 137, 153), TRUE);
			DrawTriangle(WIN_SIZE_X - 8, WIN_SIZE_Y - 5, WIN_SIZE_X - 13, WIN_SIZE_Y - 9, WIN_SIZE_X - 4, WIN_SIZE_Y - 9, GetColor(134, 137, 153), TRUE);
			DrawBox(WIN_SIZE_X - 12, 15 + (WIN_SIZE_Y - 30)*TopSentence / (ChatDataNum), WIN_SIZE_X - 5, 15 + (WIN_SIZE_Y - 30)*(TopSentence + DISP_SENTENCE) / (ChatDataNum), GetColor(208, 209, 215), TRUE);
			if (ScrollArrowUP) {
				DrawTriangle(WIN_SIZE_X - 8, 5, WIN_SIZE_X - 13, 9, WIN_SIZE_X - 4, 9, GetColor(28, 151, 234), TRUE);
			}
			if (ScrollArrowDOWN) {
				DrawTriangle(WIN_SIZE_X - 8, WIN_SIZE_Y - 5, WIN_SIZE_X - 13, WIN_SIZE_Y - 9, WIN_SIZE_X - 4, WIN_SIZE_Y - 9, GetColor(28, 151, 234), TRUE);
			}
			if (ScrollBar) {
				DrawBox(WIN_SIZE_X - 12, 15 + (WIN_SIZE_Y - 30)*TopSentence / (ChatDataNum), WIN_SIZE_X - 5, 15 + (WIN_SIZE_Y - 30)*(TopSentence + DISP_SENTENCE) / (ChatDataNum), GetColor(136, 136, 136), TRUE);
			}
		}
		else {
			DrawTriangle(WIN_SIZE_X - 8, 5, WIN_SIZE_X - 13, 9, WIN_SIZE_X - 4, 9, GetColor(202, 203, 211), TRUE);
			DrawTriangle(WIN_SIZE_X - 8, WIN_SIZE_Y - 5, WIN_SIZE_X - 13, WIN_SIZE_Y - 9, WIN_SIZE_X - 4, WIN_SIZE_Y - 9, GetColor(202, 203, 211), TRUE);
		}

		//記入バー
		DrawBox(0, WIN_SIZE_Y - 30, WIN_SIZE_X - 17, WIN_SIZE_Y, GetColor(128, 128, 128), TRUE);
		SetDrawArea(0, WIN_SIZE_Y - 30, WIN_SIZE_X - 17, WIN_SIZE_Y);

		DrawKeyInputString(0, WIN_SIZE_Y - 30, KeyInputHandle);
		SetDrawArea(0, 0, WIN_SIZE_X, WIN_SIZE_Y);

		ScreenFlip();											//裏画面反映

		if (GetWindowCloseFlag() == TRUE){
			break;
		}

	}

	char Logout[MAX_PATH];

	sprintf_s(Logout, "%sが退出しました。", ChatName);

	SendInfo(Logout);

	//メモリ開放
	if (ChatDatas != NULL) {
		free(ChatDatas);
	}
	if (SBuffer != NULL) {
		free(SBuffer);
	}

	DxLib_End();												//DXライブラリ終了処理

	return 0;

}

bool GetChatName(char *ChatNameBuffer, int nSize) {
	unsigned long Size = nSize;
	char TmpName[MAX_PATH];
	GetUserName(TmpName, &Size);
	if (strcmp(TmpName, "newcomers")) {
		for (int i = 0; i < 13; i++)ChatNameBuffer[i] = TmpName[i];
		return false;
	}
	else {
		Size = nSize;
		GetComputerName(TmpName, &Size);
		for (int i = 0; i < 13; i++)ChatNameBuffer[i] = TmpName[i];
		return true;
	}
}

int SendStr(const char *SendString) {

	if (strlen(SendString) <= MAX_STRING_SIZE) {
		FILE *fp;
		fopen_s(&fp, ChatFilePath, "a");
		if (fp != NULL) {
			fprintf_s(fp, "MSG\t%s\t%s\n", ChatName, SendString);
			fclose(fp);
		}
		else {
			return -1;
		}
	}
	else {
		int SendCharNum = 0;
		char MSG[MAX_STRING_SIZE + 1];
		for (int i = 0; i < MAX_STRING_SIZE;) {
			switch (_mbclen((unsigned char*)(&SendString[i]))) {
			case 1:
				MSG[i] = SendString[i];
				i++;
				if (i >= MAX_STRING_SIZE) {
					MSG[i] = '\0';
					SendCharNum = i;
				}
				break;
			case 2:
				MSG[i] = SendString[i];
				MSG[i + 1] = SendString[i + 1];
				i += 2;
				if (i >= MAX_STRING_SIZE) {
					if (i > MAX_STRING_SIZE) {
						i -= 2;
					}
					MSG[i] = '\0';
					SendCharNum = i;
					i++;
				}
				break;
			}
		}
		FILE *fp;
		fopen_s(&fp, ChatFilePath, "a");
		if (fp != NULL) {

			fprintf_s(fp, "MSG\t%s\t%s\n", ChatName, MSG);			//1行目書き込み

			while (SendCharNum < (int)strlen(SendString)) {
				char CONT[MAX_STRING_SIZE + 1];
				int StartCharNum = SendCharNum;
				for (int i = 0; i < MAX_STRING_SIZE;) {
					switch (_mbclen((unsigned char*)(&SendString[StartCharNum + i]))) {
					case 1:
						CONT[i] = SendString[StartCharNum + i];
						i++;
						if (i >= MAX_STRING_SIZE) {
							CONT[i] = '\0';
							SendCharNum = StartCharNum + i;
						}
						break;
					case 2:
						CONT[i] = SendString[StartCharNum + i];
						CONT[i + 1] = SendString[StartCharNum + i + 1];
						i += 2;
						if (i >= MAX_STRING_SIZE) {
							if (i > MAX_STRING_SIZE) {
								i -= 2;
							}
							CONT[i] = '\0';
							SendCharNum = StartCharNum + i;
							i++;
						}
						break;
					}
				}
				fprintf_s(fp, "CONT\t%s\n", CONT);
			}
		}
		else {
			return -1;
		}

		fclose(fp);
	}

	return 0;
}

int SendFile(const char *FilePath) {
	char FileName[MAX_PATH];
	int PathLengh;

	//ファイル名取得
	PathLengh = strlen(FilePath);

	for (int i = PathLengh;; i--) {
		if (FilePath[i] == '\\') {
			for (int j = 0; j < PathLengh - i; j++) {
				FileName[j] = FilePath[i + j + 1];
			}
			break;
		}
	}

	//ファイルアップロード
	int FileNum = -1;
	char UpLoadFilePath[MAX_PATH];

	do {
		FileNum++;
		sprintf_s(UpLoadFilePath, "%s\\%d.dat", DateStr, FileNum);
	} while (PathFileExists(UpLoadFilePath));

	if (CopyFile(FilePath, UpLoadFilePath, TRUE) == 0) {
		return -1;
	}

	//コマンド書き込み
	FILE *fp;
	fopen_s(&fp, ChatFilePath, "a");
	if (fp != NULL) {
		fprintf_s(fp, "FILE\t%s\t%d\t%s\n", ChatName, FileNum, FileName);
		fclose(fp);
	}
	else {
		return -1;
	}

	return 0;
}

int CommandStrToInt(const char *Command) {
	if (strcmp(Command, "MSG") == 0)	return STR_MSG;
	if (strcmp(Command, "CONT") == 0)	return STR_CONT;
	if (strcmp(Command, "FILE") == 0)	return STR_FILE;
	if (strcmp(Command, "INFO") == 0)	return STR_INFO;
	return STR_ERR;
}

Chat* ReadChat(int *nBuffer) {
	Chat *ChatDatas = NULL;

	FILE *fp;
	fopen_s(&fp, ChatFilePath, "r");
	if (fp != NULL) {
		*nBuffer = 0;

		while (feof(fp) == 0) {
			char Line[MAX_PATH];
			char Command[MAX_PATH];

			if (fgets(Line, MAX_PATH, fp) == 0) {
				break;
			}

			sscanf_s(Line, "%s", Command, sizeof(Command));

			char NAME[MAX_PATH];
			char STRING[MAX_PATH];
			int FileNum;

			switch (CommandStrToInt(Command)) {
			case STR_MSG:
				STRING[0] = '\0';
				sscanf_s(Line, "%s\t%[^\t]\t%[^\n]", Command, sizeof(Command), NAME, sizeof(NAME), STRING, sizeof(STRING));
				(*nBuffer)++;
				ChatDatas = (Chat *)realloc(ChatDatas, sizeof(Chat) * *nBuffer);
				ChatDatas[*nBuffer - 1].StrAttribute = STR_MSG;
				strcpy_s(ChatDatas[*nBuffer - 1].UserName, NAME);
				strcpy_s(ChatDatas[*nBuffer - 1].DrawString, STRING);
				break;
			case STR_CONT:
				STRING[0] = '\0';
				sscanf_s(Line, "%s\t%[^\n]", Command, sizeof(Command), STRING, sizeof(STRING));
				(*nBuffer)++;
				ChatDatas = (Chat *)realloc(ChatDatas, sizeof(Chat) * *nBuffer);
				ChatDatas[*nBuffer - 1].StrAttribute = STR_CONT;
				strcpy_s(ChatDatas[*nBuffer - 1].DrawString, STRING);
				break;
			case STR_FILE:
				STRING[0] = '\0';
				sscanf_s(Line, "%s\t%[^\t]\t%d\t%[^\n]", Command, sizeof(Command), NAME, sizeof(NAME), &FileNum, STRING, sizeof(STRING));
				(*nBuffer)++;
				ChatDatas = (Chat *)realloc(ChatDatas, sizeof(Chat) * *nBuffer);
				ChatDatas[*nBuffer - 1].StrAttribute = STR_FILE;
				strcpy_s(ChatDatas[*nBuffer - 1].UserName, NAME);
				ChatDatas[*nBuffer - 1].FileNum = FileNum;
				strcpy_s(ChatDatas[*nBuffer - 1].DrawString, STRING);
				break;
			case STR_INFO:
				STRING[0] = '\0';
				sscanf_s(Line, "%s\t%[^\n]", Command, sizeof(Command), STRING, sizeof(STRING));
				(*nBuffer)++;
				ChatDatas = (Chat *)realloc(ChatDatas, sizeof(Chat) * *nBuffer);
				ChatDatas[*nBuffer - 1].StrAttribute = STR_INFO;
				strcpy_s(ChatDatas[*nBuffer - 1].DrawString, STRING);
				break;
			}
		}

		fclose(fp);

		return ChatDatas;
	}
	else {
		*nBuffer = -1;
		return NULL;
	}
}

int SendInfo(const char *InfoStr) {
	//送信
	FILE *fp;
	fopen_s(&fp, ChatFilePath, "a");
	if (fp != NULL) {
		fprintf_s(fp, "INFO\t%s\n", InfoStr);
		fclose(fp);
	}
	else {
		return -1;
	}
	return 0;
}