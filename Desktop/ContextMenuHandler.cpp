#include "stdafx.h"


void HandleContextMenu(HWND hwnd) {
	SetWindowSubclass(hwnd, HandleContextMenuProc, 0, 0);
}


LRESULT CALLBACK HandleContextMenuProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData){

	if ( message == WM_RBUTTONDOWN ){
		POINT point;
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		HMENU hPopupMenu = CreatePopupMenu();
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EDIT_FIELDS, L"New Folder");
        ClientToScreen(hWnd, &point);
		TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, hWnd, NULL);
	}

	return DefSubclassProc(hWnd, message, wParam, lParam);
}
