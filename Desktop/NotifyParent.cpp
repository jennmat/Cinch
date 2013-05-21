
#include "stdafx.h"

void NotifyParentControl(HWND hWnd){
	SetWindowSubclass(hWnd, NotifyParentProc, 0, 0);
}

LRESULT CALLBACK NotifyParentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData){

	if ( message == WM_COMMAND ){
		PostMessage(GetParent(hWnd), message, wParam, lParam);
	}
	if ( message == WM_NOTIFY ){
		PostMessage(GetParent(hWnd), message, wParam, lParam);
	}
	return DefSubclassProc(hWnd, message, wParam, lParam);
}



void NotifyParentOfEnterKey(HWND hWnd){
	SetWindowSubclass(hWnd, NotifyParentOfEnterKeyProc, 0, 0);
}

LRESULT CALLBACK NotifyParentOfEnterKeyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	if ( message == WM_KEYUP && wParam == VK_RETURN ){
		PostMessage(GetParent(hWnd), message, wParam, lParam);
	}

	return DefSubclassProc(hWnd, message, wParam, lParam);
}





void PreserveTabInMultilineEdit(HWND hWnd){
	SetWindowSubclass(hWnd, PreserveTabInMultilineEditProc, 0, 0);
}

LRESULT CALLBACK PreserveTabInMultilineEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData){
	if(message == WM_GETDLGCODE) {
		if(wParam != VK_RETURN){
		// Escape and Tab still used for dialog navigation        
        return DLGC_WANTARROWS|DLGC_WANTCHARS;
        }
    }

	return DefSubclassProc(hWnd, message, wParam, lParam);
}
