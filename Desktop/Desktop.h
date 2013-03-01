#pragma once

#include "resource.h"

#define WM_NEW_DATA_ARRIVED WM_USER+140

CouchViewDelegate* delegate;

class Desktop : public FormDelegate {
public:
	Object loadedForm;
	void formModified();

};

HWND hWnd; //Main window
HWND grid;
HWND tree;
HWND designer;
HWND toolbar;

vector<Object> objectTypes;

Desktop desktop;

CouchDB::Connection conn;

DWORD WINAPI ChangesListener(LPVOID lParam);

struct ViewPair {
	wchar_t* design;
	wchar_t* view;
};

