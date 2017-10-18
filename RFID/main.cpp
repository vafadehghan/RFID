#pragma once
#include "Header.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:		main.cpp - The main entry point of the program that houses all functions.
--
-- PROGRAM:			RFID
--
-- FUNCTIONS:		int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspzCmdParam, int nCmdShow)
--					LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--					void createConnectThread()
--					DWORD WINAPI connect(LPVOID i)
--					DWORD WINAPI search(LPVOID i)
--					unsigned char tagCallBack(LPSKYETEK_TAG lpTag, void *user)
--
--
-- DATE:			October 17, 2017
--
--
-- DESIGNER:		Vafa Dehghan Saei, William Murphy
--
-- PROGRAMMER:		Vafa Dehghan Saei, William Murphy
--
-- NOTES:			This file is the entry point of RFID, it creates the window that stores the terminal.
--					Once the user is connected to a RFID reader they can view the tags that are detected by the reader.
--					The tag info is displayed in a list in the terminal window.
----------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	WinMain
--
-- DATE:		October 17, 2017
--
--
-- DESIGNER:	Vafa Dehghan Saei, William Murphy
--
-- PROGRAMMER:	Vafa Dehghan Saei, William Murphy
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
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspzCmdParam, int nCmdShow) 
{

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
		CloseHandle(threadPool[i]);
	}
	return Msg.wParam;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	WndProc
--
-- DATE:		October 17, 2017
--
--
-- DESIGNER:	Vafa Dehghan Saei, William Murphy
--
-- PROGRAMMER:	Vafa Dehghan Saei, William Murphy
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
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) 
{

	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_CONNECT:
			isStop = 0;
			threadPool[0] = CreateThread(NULL, 0, connect, NULL, 0, NULL);
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	createConnectThread()
--
-- DATE:		October 17, 2017
--
--
-- DESIGNER:	Vafa Dehghan Saei, William Murphy
--
-- PROGRAMMER:	Vafa Dehghan Saei, William Murphy
--
-- INTERFACE:	void createConnectThread()
--
--
-- RETURNS:		Void
--
-- NOTES:		This function is a wrapper function for the connect() function, it launces connect() in a new thread.
--
----------------------------------------------------------------------------------------------------------------------*/
void createConnectThread()
{
	EnableMenuItem(hMenu, ID_FILE_SEARCH, MF_GRAYED);
	threadPool[1] = CreateThread(NULL, 0, search, NULL, 0, NULL);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	connect()
--
-- DATE:		October 17, 2017
--
--
-- DESIGNER:	Vafa Dehghan Saei, William Murphy
--
-- PROGRAMMER:	Vafa Dehghan Saei, William Murphy
--
-- INTERFACE:	DWORD WINAPI connect(LPVOID i)
--				LPVOID i: The arguments of this thread
--
-- RETURNS:		If the function succeeds it returns a 1, else returns a 0
--
-- NOTES:		This function looks for the RFID devices connected to the computer, if a reader was found the user
--				can connect to read mode.
--
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI connect(LPVOID i)
{
	HDC hdc = GetDC(hwnd);

	numDevices = SkyeTek_DiscoverDevices(&devices);
	TextOut(hdc, 1, 1, "                         ", 26);
	TextOut(hdc, 1, 1, "Searching...", 13);
	if (numDevices == 0)
	{
		MessageBox(hwnd, "No Devices found", "", MB_OK);
		TextOut(hdc, 1, 1, "                         ", 26);
		return 0;
	}
	else
	{
		numReaders = SkyeTek_DiscoverReaders(devices, numDevices, &readers);
		if (numReaders == 0) {
			MessageBox(hwnd, "No Readers found", "", MB_OK);
			TextOut(hdc, 1, 1, "                         ", 26);
			return 0;
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	search()
--
-- DATE:		October 17, 2017
--
--
-- DESIGNER:	Vafa Dehghan Saei, William Murphy
--
-- PROGRAMMER:	Vafa Dehghan Saei, William Murphy
--
-- INTERFACE:	DWORD WINAPI search(LPVOID i)
--				LPVOID i: The arguments of this thread
--
-- RETURNS:		If the function succeeds it returns a 1, else returns a 0
--
-- NOTES:		This function searches for the tags in the RFID readers range and sends the tag to the callback.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI search(LPVOID i)
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	tagCallBack()
--
-- DATE:		October 17, 2017
--
--
-- DESIGNER:	Vafa Dehghan Saei, William Murphy
--
-- PROGRAMMER:	Vafa Dehghan Saei, William Murphy
--
-- INTERFACE:	unsigned char tagCallBack(LPSKYETEK_TAG lpTag, void *user)
--				LPSKYETEK_TAG lpTag: The tag that was detected by the RFID reader
--				void *user:	The user data, NULL for this program
--
--
-- RETURNS:		If the function if the function is looping it returns a 1, else a 0.
--
-- NOTES:		This function is called when the RFID reader detects a tag, it prints the tag info to the screen.
----------------------------------------------------------------------------------------------------------------------*/
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