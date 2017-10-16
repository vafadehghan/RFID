#pragma once

#include "windows.h"
#include "stdafx.h"
#include "resource.h"
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI connect(LPVOID i);
void search();
void createConnectThread();
unsigned char tagCallBack(LPSKYETEK_TAG lpTag, void *user);


HWND hwnd;
static TCHAR Name[] = TEXT("RFID");
unsigned int numDevices;
LPSKYETEK_DEVICE *devices = NULL;
LPSKYETEK_READER *readers = NULL;
unsigned int numReaders;
SKYETEK_STATUS st;
LPSKYETEK_TAG *lpTags = NULL;
unsigned short count;
unsigned char isStop;
HMENU hMenu;
