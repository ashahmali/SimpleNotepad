#include <windows.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";
HWND hEdit;

// Function to read file for edit
BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
					   OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize;

		dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize != 0xFFFFFFFF) {
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if (pszFileText != NULL) {
				DWORD dwRead;

				if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL)) {
					pszFileText[dwFileSize] = 0; // Add null terminator
					if (SetWindowText(hEdit, pszFileText))
						bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

// Writing to file
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
					   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD dwTextLength;

		dwTextLength = GetWindowTextLength(hEdit);
		// No need to bother if there's no text.
		if (dwTextLength > 0) {
			LPSTR pszText;
			DWORD dwBufferSize = dwTextLength + 1;

			pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if (pszText != NULL) {
				if (GetWindowText(hEdit, pszText, dwBufferSize)) {
					DWORD dwWritten;

					if (WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
						bSuccess = TRUE;
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

// for dialog box
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		case WM_INITDIALOG:

		return TRUE;
		case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDOK:
			EndDialog(hwnd, IDOK);
			break;
			case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		break;
		default:
		return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";
	switch (Message) {
		case WM_CREATE:
		{
			HMENU hMenu, hSubMenu;
			hMenu = CreateMenu();
			hSubMenu = CreatePopupMenu();
			AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, "O&pen");
			AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVE, "S&ave");
			AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "E&xit");
			AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");

			hSubMenu = CreatePopupMenu();
			AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, "&Go");
			AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Stuff");

			SetMenu(hwnd, hMenu);

			HFONT hfDefault;
			

			hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
								   WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
								   0, 0, 600, 400, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if (hEdit == NULL)
				MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		}
		break;
		case WM_SIZE:
		/*REQRESIZE rcClient;
		GetClientRect(hwnd, &rcClient);

		hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);*/
		
		break;
		case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case ID_STUFF_GO:
			{
				int ret = DialogBox(GetModuleHandle(NULL),
									MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
				if (ret == IDOK) {
					MessageBox(hwnd, "Dialog exited with IDOK.", "Notice",
							   MB_OK | MB_ICONINFORMATION);
				} else if (ret == IDCANCEL) {
					MessageBox(hwnd, "Dialog exited with IDCANCEL.", "Notice",
							   MB_OK | MB_ICONINFORMATION);
				} else if (ret == -1) {
					MessageBox(hwnd, "Dialog failed!", "Error",
							   MB_OK | MB_ICONINFORMATION);
				}
			}
			break;
			case ID_FILE_OPEN:
			// Struct to store information fro open and save fle predefined dialog
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
				ofn.hwndOwner = hwnd;
				// this a double null terminated string and is broken by the NULLs in the string
				// the first part is Text Files (*.txt), then *.txt, All Files (*.*), *.*
				ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
				ofn.lpstrFile = szFileName;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.lpstrDefExt = "txt";

				// function to open the file
				if (GetOpenFileName(&ofn)) {
					if (LoadTextFileToEdit(hEdit, ofn.lpstrFile)) {
						MessageBox(hwnd, "File Opened", "Notice", MB_OK);
					} else {
						MessageBox(hwnd, "File could not be opened", "Warning", MB_OK);
					}
				}


				break;
			case ID_FILE_SAVE:
				// Struct to store information fro open and save fle predefined dialog
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
				ofn.hwndOwner = hwnd;
				// this a double null terminated string and is broken by the NULLs in the string
				// the first part is Text Files (*.txt), then *.txt, All Files (*.*), *.*
				ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
				ofn.lpstrFile = szFileName;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				ofn.lpstrDefExt = "txt";

				if (GetSaveFileName(&ofn)) {
					// Do something usefull with the filename stored in szFileName
					if (SaveTextFileFromEdit(hEdit, ofn.lpstrFile))
						MessageBox(hwnd, "File has been saved! Congratulations", "Notice", MB_OK);
				}


			break;
			case ID_FILE_EXIT:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
			// Other menu commands...
		}
		break;

		case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
		PostQuitMessage(0);
		break;
		default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}





int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
	wc.lpszClassName = g_szClassName;
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
	wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"The title of my window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 450,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}