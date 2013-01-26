#pragma once

#include "resource.h"

#define WM_NEW_DATA_ARRIVED WM_USER+140

CouchViewDelegate* delegate;

HWND hWnd; //Main window
HWND grid;
HWND tree;
HWND designer;
HWND toolbar;

CouchDB::Connection conn;

DWORD WINAPI ChangesListener(LPVOID lParam);
