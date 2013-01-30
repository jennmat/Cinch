#pragma once

#include "resource.h"

#define WM_NEW_DATA_ARRIVED WM_USER+140

CouchViewDelegate* delegate;

class Desktop : public FormDelegate {
	void formModified();
};

HWND hWnd; //Main window
HWND grid;
HWND tree;
HWND designer;
HWND toolbar;

Desktop desktop;

CouchDB::Connection conn;

DWORD WINAPI ChangesListener(LPVOID lParam);
