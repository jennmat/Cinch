
#include "stdafx.h"


void NotifyParentControl(HWND);
LRESULT CALLBACK NotifyParentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);


void NotifyParentOfEnterKey(HWND);
LRESULT CALLBACK NotifyParentOfEnterKeyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);


void PreserveTabInMultilineEdit(HWND);
LRESULT CALLBACK PreserveTabInMultilineEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);


