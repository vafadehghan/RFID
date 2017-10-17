
#pragma once
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:		Application.cpp - The main entry point of the program that houses WinMain and WndProc.
--
-- PROGRAM:			DummyTerminal
--
-- FUNCTIONS:		int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspzCmdParam, int nCmdShow)
--					LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--					INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
--
--
-- DATE:			October 3, 2017
--
--
-- DESIGNER:		Vafa Dehghan Saei
--
-- PROGRAMMER:		Vafa Dehghan Saei
--
-- NOTES:			This file is the entry point of DummyTerminal the creates the window that stores the terminal.
--					Once a key is setup is completed then users can communicate with another computer through the serial port.
----------------------------------------------------------------------------------------------------------------------*/
#include "Header.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	WinMain
--
-- DATE:		October 3, 2017
--
--
-- DESIGNER:	Vafa Dehghan Saei
--
-- PROGRAMMER:	Vafa Dehghan Saei
--
-- INTERFACE:	int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspzCmdParam, int nCmdShow)
--					HINSTANCE hInst: A handle to the current instance of the application
--					HINSTANCE hprevInstance: A handle to the previous instance of the application.
--					LPSTR lspzCmdParam: The command line for the application.
--					int nCmdShow: Controls how the windows should be shown.
--
--
-- RETURNS:		If the function succeeds, and terminated successfully with WM_QUIT, it returns the exit value contained in WM_QUITs wParam. If terminated before the message loop, return a 0
--
-- NOTES:		This function creates the window and is the application entry point.
--
----------------------------------------------------------------------------------------------------------------------*/
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspzCmdParam, int nCmdShow) {

	MSG Msg;
	WNDCLASSEX Wcl;

	// Define a Window class
	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = 0; // default style
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	Wcl.lpfnWndProc = WndProc; // window function
	Wcl.hInstance = hInst; // handle to this instance
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //white background
	Wcl.lpszClassName = Name; // window class name

	Wcl.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); // no class menu 
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0;

	// Register the class
	if (!RegisterClassEx(&Wcl))
		return 0;

	hwnd = CreateWindow(
		Name, // name of window class
		Name, // title 
		WS_OVERLAPPEDWINDOW, // window style - normal
		CW_USEDEFAULT,	// X coord
		CW_USEDEFAULT, // Y coord
		CW_USEDEFAULT, // width
		CW_USEDEFAULT, // height
		NULL, // no parent window
		NULL, // no menu
		hInst, // instance handle
		NULL // no additional arguments
	);
	hMenu = GetMenu(hwnd);
	EnableMenuItem(hMenu, ID_FILE_DISCONNECT, MF_GRAYED);
	EnableMenuItem(hMenu, ID_FILE_SEARCH, MF_GRAYED);
	// Display the window
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	// Create the message loop
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg); // translate keybpard messages
		DispatchMessage(&Msg); // dispatch message and return control to windows
	}
	for (unsigned int i = 0; i < MAX_THREADS; ++i)
	{
		printf("close handle");
		CloseHandle(threadPool[i]);
	}
	return Msg.wParam;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	WndProc
--
-- DATE:		October 3, 2017
--
--
-- DESIGNER:	Vafa Dehghan Saei
--
-- PROGRAMMER:	Vafa Dehghan Saei
--
-- INTERFACE:	LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--					HWND hwnd: A handle to the window.
--					UINT Message: The message.
--					WPARAM wParam: Additional message information.
--					LPARAM lParam: Additional message information.
--
--
-- RETURNS:		The result of the message processing which depends on the message sent.
--
-- NOTES:		This function processes the messages sent to a window, and takes appropriate actions which is defined by the user.
--
----------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_CONNECT:
			isStop = 0;
			threadPool[0] = CreateThread(NULL, 0, search, NULL, 0, NULL);
			break;
		case ID_FILE_DISCONNECT:
			isStop = 1;
			SkyeTek_FreeDevice(*devices);
			EnableMenuItem(hMenu, ID_FILE_DISCONNECT, MF_GRAYED);
			EnableMenuItem(hMenu, ID_FILE_SEARCH, MF_GRAYED);
			EnableMenuItem(hMenu, ID_FILE_CONNECT, MF_ENABLED);
			break;
		case ID_FILE_EXIT:
			PostQuitMessage(0);
			break;
		case ID_FILE_SEARCH:
			createConnectThread();
			break;
		}
		break;
	case WM_CHAR:
		break;

	case WM_DESTROY:		// message to terminate the program
		PostQuitMessage(0);
		break;

	default: // Let Win32 process all other messages
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

void createConnectThread()
{
	EnableMenuItem(hMenu, ID_FILE_SEARCH, MF_GRAYED);
	threadPool[1] = CreateThread(NULL, 0, connect, NULL, 0, NULL);
}

DWORD WINAPI search(LPVOID i)
{
	HDC hdc = GetDC(hwnd);

	numDevices = SkyeTek_DiscoverDevices(&devices);
	TextOut(hdc, 1, 1, "                      ", 23);
	TextOut(hdc, 1, 1, "Searching...", 13);
	if (numDevices == 0)
	{
		MessageBox(hwnd, "No Devices found", "", MB_OK);
	}
	else
	{
		numReaders = SkyeTek_DiscoverReaders(devices, numDevices, &readers);
		if (numReaders == 0) {
			MessageBox(hwnd, "No Readers found", "", MB_OK);
		}
		else
		{
			TextOut(hdc, 1, 2, readers[0]->friendly, 256);
			EnableMenuItem(hMenu, ID_FILE_CONNECT, MF_GRAYED);
			EnableMenuItem(hMenu, ID_FILE_DISCONNECT, MF_ENABLED);
			EnableMenuItem(hMenu, ID_FILE_SEARCH, MF_ENABLED);
		}
	}
	return 1;
}

unsigned char tagCallBack(LPSKYETEK_TAG lpTag, void *user)
{
	SKYETEK_ADDRESS addr;
	LPSKYETEK_DATA lpData = NULL;
	char *str = NULL;
	HDC hdc = GetDC(hwnd);
	addr.start = 10;
	addr.blocks = 1;
	if (lpTag != NULL && !isStop)
	{
		if ((lpTag->type & 0xFFF0) != EM4X22_AUTO)
		{
			SkyeTek_GetTagTypeNameFromType(lpTag->type);
			TextOut(hdc, 1, 1, lpTag->friendly, 256);
			SkyeTek_FreeTag(lpTag);

		}
	}
	else
	{
		TextOut(hdc, 1, 1, "                                                                                                   ", 100);
		TextOut(hdc, 1, 15, "                                                                                                   ", 100);

	}
	if (isStop) {
		TextOut(hdc, 1, 1, "                                                                                                   ", 100);
		TextOut(hdc, 1, 1, "Device Stopped Reading", 23);
		TextOut(hdc, 1, 15, "                                                                                                   ", 100);

	}

	return !isStop;
}

DWORD WINAPI connect(LPVOID i)
{
	HDC hdc = GetDC(hwnd);

	lpTags = NULL;
	st = SkyeTek_SelectTags(readers[0], AUTO_DETECT, tagCallBack, 0, 1, NULL);
	if (st == SKYETEK_TIMEOUT)
	{
		MessageBox(hwnd, "Get Tags Timed out", "", MB_OK);
		return 0;
	}
	else if (st != SKYETEK_SUCCESS)
	{
		MessageBox(hwnd, "Get Tags Failed", "", MB_OK);
		return 0;
	}

	return 1;
}