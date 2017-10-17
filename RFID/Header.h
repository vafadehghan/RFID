#pragma once

#include "windows.h"
#include "stdafx.h"
#include "RFID/resource.h"
#include "../stapi/SkyeTekAPI.h"
#include "../stapi/SkyeTekProtocol.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI connect(LPVOID i);
DWORD WINAPI search(LPVOID i);
void createConnectThread();
unsigned char tagCallBack(LPSKYETEK_TAG lpTag, void *user);

// constants
constexpr unsigned int MAX_THREADS = 2;

HWND hwnd;
HANDLE threadPool[MAX_THREADS];
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
