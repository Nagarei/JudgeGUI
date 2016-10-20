#include <time.h>
#include <direct.h>
#include "Shlwapi.h"
#include "DxLib.h"
#include "resource.h"

#pragma comment(lib, "shlwapi.lib")

#define DATA_DIR_NAME "\\\\SERVER-PC\\Server\\WorkSpace\\CombChat"	//�f�[�^�Ǘ��p�t�H���_

#define WIN_SIZE_X		640							//����
#define WIN_SIZE_Y		480							//���c
#define MAX_STRING_SIZE	54							//�����̍ő�
#define DISP_SENTENCE	28

#define STR_ERR		-1								//�G���[
#define STR_MSG		1								//�����񂪃��b�Z�[�W
#define STR_CONT	2								//�����񂪑O�̕��̑���
#define STR_FILE	3								//�����񂪃t�@�C��
#define STR_INFO	4								//�����񂪏��

#define BUFFER_TYPE_STRING	1						//�o�b�t�@�[��������
#define BUFFER_TYPE_FILE	2						//�o�b�t�@�[���t�@�C��
#define BUFFER_TYPE_INFO	3						//�o�b�t�@�[�����

struct Chat {										//�`���b�g�����f�[�^
	int StrAttribute;
	char UserName[MAX_PATH];
	char DrawString[MAX_PATH];
	int FileNum;
};

struct SendBuffer {
	int BufferType;				//�o�b�t�@�[�̎��
	char BufferStr[1024];		//�o�b�t�@�[�̓��e
};

bool GetChatName(char *ChatNameBuffer, int nSize);	//�`���b�g�ł̖��O���擾
int SendStr(const char *SendString);				//������o��
int SendFile(const char *FilePath);					//�t�@�C���A�b�v���[�h&��������
int CommandStrToInt(const char *Command);			//�R�}���h�𐔒l�ɕϊ�
Chat* ReadChat(int *nBuffer);						//�R�}���h�ǂݍ���
int SendInfo(const char *InfoStr);					//��񑗐M

char ChatFilePath[MAX_PATH];
char ChatName[MAX_PATH];
char DateStr[MAX_PATH];

void ShutDownCallback(void *Data)
{
	char Logout[MAX_PATH];

	sprintf_s(Logout, "%s���ޏo���܂����B", ChatName);

	SendInfo(Logout);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	SetOutApplicationLogValidFlag(FALSE);				//���O���o���Ȃ�
	ChangeWindowMode(TRUE);								//�E�B���h�E���[�h�ɂ���
	SetAlwaysRunFlag(TRUE);								//��ɓ���
	SetGraphMode(WIN_SIZE_X, WIN_SIZE_Y, 16);			//�𑜓x�ύX
	SetMainWindowText("���z�`���b�g");					//�^�C�g���ύX
	SetWindowIconID(IDI_ICON1);							//�A�C�R���ύX

	//�V���b�g�_�E�����Ɏ��s
	SetShutdownCallbackFunction(ShutDownCallback, NULL, "Exiting...");

	//�E�B���h�E��\��
	SetWindowVisibleFlag(FALSE);

	if (DxLib_Init() == -1) return -1;					//DX���C�u��������������

	SetWindowUserCloseEnableFlag(FALSE);				//���閳��
	SetDragFileValidFlag(TRUE);							//D&D��L��
	SetDrawScreen(DX_SCREEN_BACK);						//�`����ʂ𗠂ɂ���
	SetFontSize(16);									//�t�H���g�T�C�Y�w��

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

	SetFontSize(28);											//�t�H���g�T�C�Y�w��
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_8X8);				//�t�H���g�^�C�v�w��

	int FontHandle = CreateFontToHandle(NULL, 16, -1);

	char Login[MAX_PATH];

	sprintf_s(Login, "%s���������܂����B", ChatName);

	if (SendInfo(Login) == -1) {
		BufferNum++;
		SBuffer = (SendBuffer *)realloc(SBuffer, sizeof(SendBuffer)*BufferNum);
		SBuffer[BufferNum - 1].BufferType = BUFFER_TYPE_INFO;
		strcpy_s(SBuffer[BufferNum - 1].BufferStr, Login);
	}

	//�E�B���h�E�\��
	SetWindowVisibleFlag(TRUE);

	while (ProcessMessage() == 0) {

		ClearDrawScreen();										//��ʂ̃N���A

		/*********************����*********************/
		MouseState = GetMouseInput();
		GetMousePoint(&MouseX, &MouseY);

		//���b�t�@�C���m�F
		if (GetNowCount() >= LastReadTime + 100) {
			SetDragFileValidFlag(TRUE);							//D&D��L��
			HANDLE hFile = CreateFile(ChatFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE) {
				GetFileTime(hFile, NULL, NULL, &LastWriteTime_Now);
				if (LastWriteTime_Now.dwHighDateTime != LastWriteTime_Last.dwHighDateTime || LastWriteTime_Now.dwLowDateTime != LastWriteTime_Last.dwLowDateTime) {
					//�t�@�C���ǂݍ���
					Chat *TmpChatDatas = NULL;
					int TmpChatDataNum;
					TmpChatDatas = ReadChat(&TmpChatDataNum);

					if (TmpChatDataNum != -1) {
						ChatDataNum = TmpChatDataNum;
						TopSentence = ChatDataNum - DISP_SENTENCE;
						if (ChatDataNum > 28) {
							Scroll = true;							//28�s�ȏ�ŃX�N���[��
						}
						ChatDatas = TmpChatDatas;
						//�Ō�ɓǂݍ��񂾎��Ԃƃt�@�C���X�V�����L�^
						LastWriteTime_Last = LastWriteTime_Now;
						LastReadTime = GetNowCount();

						if (GetWindowActiveFlag() == FALSE) {
							FlashWindow(GetMainWindowHandle(), TRUE);		//��A�N�e�B�u���ɍX�V���ꂽ��A�C�R���_��
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

			while (GetDragFileNum() > 0) {								//�t�@�C��D&D����
				char FilePath[MAX_PATH];

				GetDragFilePath(FilePath);							//D&D�t�@�C�����擾

				if (SendFile(FilePath) == -1) {						//�t�@�C���A�b�v���[�h&��������
					BufferNum++;
					SBuffer = (SendBuffer *)realloc(SBuffer, sizeof(SendBuffer)*BufferNum);
					SBuffer[BufferNum - 1].BufferType = BUFFER_TYPE_FILE;
					strcpy_s(SBuffer[BufferNum - 1].BufferStr, FilePath);
				}
			}

		//����ꂽ��ŏ���
		if (GetWindowUserCloseFlag(TRUE)) {
			ShowWindow(GetMainWindowHandle(), SW_MINIMIZE);
		}

		//�����񑗐M
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

		//�o�b�t�@�[����
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

		//�X�N���[��
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
		//�␳
		if (TopSentence > ChatDataNum - DISP_SENTENCE) {
			TopSentence = ChatDataNum - DISP_SENTENCE;
		}
		if (TopSentence < 0) {
			TopSentence = 0;
		}

		//�t�@�C���N���b�N
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
		sprintf_s(DLPath, "%s\\Documents\\���z�`���b�g", DLPath);

		if ((MouseState&MOUSE_INPUT_LEFT) && SentenceClicking == false) {
			SentenceClicking = true;
			if (SelectingNum != -1) {
				if (ChatDatas[SelectingNum].StrAttribute == STR_FILE) {
					//�t�@�C�����N���b�N���ꂽ
					_mkdir(DLPath);
					char UpLoadFilePath[MAX_PATH];
					char LocalFilePath[MAX_PATH];

					sprintf_s(UpLoadFilePath, "%s\\%d.dat", DateStr, ChatDatas[SelectingNum].FileNum);
					sprintf_s(LocalFilePath, "%s\\%s", DLPath, ChatDatas[SelectingNum].DrawString);

					CopyFile(UpLoadFilePath, LocalFilePath, FALSE);

					ShellExecute(NULL, "explore", DLPath, NULL, NULL, SW_SHOWNORMAL);
				}
				else if (ChatDatas[SelectingNum].StrAttribute == STR_MSG){
					//�����񂪃N���b�N���ꂽ
					//�N���b�v�{�[�h�փR�s�[
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

		/*********************�`��*********************/

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

		//�X�N���[���o�[
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

		//�L���o�[
		DrawBox(0, WIN_SIZE_Y - 30, WIN_SIZE_X - 17, WIN_SIZE_Y, GetColor(128, 128, 128), TRUE);
		SetDrawArea(0, WIN_SIZE_Y - 30, WIN_SIZE_X - 17, WIN_SIZE_Y);

		DrawKeyInputString(0, WIN_SIZE_Y - 30, KeyInputHandle);
		SetDrawArea(0, 0, WIN_SIZE_X, WIN_SIZE_Y);

		ScreenFlip();											//����ʔ��f

		if (GetWindowCloseFlag() == TRUE){
			break;
		}

	}

	char Logout[MAX_PATH];

	sprintf_s(Logout, "%s���ޏo���܂����B", ChatName);

	SendInfo(Logout);

	//�������J��
	if (ChatDatas != NULL) {
		free(ChatDatas);
	}
	if (SBuffer != NULL) {
		free(SBuffer);
	}

	DxLib_End();												//DX���C�u�����I������

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

			fprintf_s(fp, "MSG\t%s\t%s\n", ChatName, MSG);			//1�s�ڏ�������

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

	//�t�@�C�����擾
	PathLengh = strlen(FilePath);

	for (int i = PathLengh;; i--) {
		if (FilePath[i] == '\\') {
			for (int j = 0; j < PathLengh - i; j++) {
				FileName[j] = FilePath[i + j + 1];
			}
			break;
		}
	}

	//�t�@�C���A�b�v���[�h
	int FileNum = -1;
	char UpLoadFilePath[MAX_PATH];

	do {
		FileNum++;
		sprintf_s(UpLoadFilePath, "%s\\%d.dat", DateStr, FileNum);
	} while (PathFileExists(UpLoadFilePath));

	if (CopyFile(FilePath, UpLoadFilePath, TRUE) == 0) {
		return -1;
	}

	//�R�}���h��������
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
	//���M
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