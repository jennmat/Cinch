
#include "stdafx.h"

void HandleExplorerRenames(HWND){
	SetWindowSubclass(hWnd, HandleExplorerRenamesProc, 0, 0);
}


LRESULT CALLBACK HandleExplorerRenamesProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	if ( message == WM_NOTIFY ){
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		LPNMTOOLBAR lpnmTB = (LPNMTOOLBAR)lParam;
		if ( pnmhdr->code == TVN_ENDLABELEDIT ){
			LPNMTVDISPINFO ptvdi = (LPNMTVDISPINFO) lParam ;
			if (ptvdi->item.pszText != NULL ){
				HWND tree = pnmhdr->hwndFrom;
				Explorer* explorer = (Explorer*)GetWindowLongPtr(tree, GWLP_USERDATA);
				explorer->saveChanges(tree);
		
				return TRUE;
			}
		}
	}

	return DefSubclassProc(hWnd, message, wParam, lParam);
}


