
#include "stdafx.h"

void HandleExplorerRenames(HWND){
	SetWindowSubclass(hWnd, HandleExplorerRenamesProc, 0, 0);
}


LRESULT CALLBACK HandleExplorerRenamesProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	if ( message == WM_NOTIFY ){
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		LPNMTOOLBAR lpnmTB = (LPNMTOOLBAR)lParam;
		if ( pnmhdr->code == TVN_BEGINLABELEDIT ){
			LPNMTVDISPINFO ptvdi = (LPNMTVDISPINFO) lParam ;
			HWND tree = pnmhdr->hwndFrom;
				
			TVITEMEX tvi;
			tvi.mask = TVIF_PARAM;
			tvi.hItem = ptvdi->item.hItem;
			TreeView_GetItem(tree, &tvi);


			Object* obj = (Object *)tvi.lParam;
			if ((*obj)["type"].isString() && (*obj)["type"].getString().compare("view") == 0 ){
				return TRUE;
			}
		}
		if ( pnmhdr->code == TVN_ENDLABELEDIT ){
			LPNMTVDISPINFO ptvdi = (LPNMTVDISPINFO) lParam ;
			if (ptvdi->item.pszText != NULL ){
				HWND tree = pnmhdr->hwndFrom;
				
				TVITEMEX tvi;
				tvi.mask = TVIF_PARAM;
				tvi.hItem = ptvdi->item.hItem;
				TreeView_GetItem(tree, &tvi);


				Object* obj = (Object *)tvi.lParam;

				string label = ws2s(wstring(ptvdi->item.pszText));
				(*obj)["label"] = label;

				Explorer* explorer = (Explorer*)GetWindowLongPtr(tree, GWLP_USERDATA);
				return explorer->saveChanges();
		
			}
		}
	}

	return DefSubclassProc(hWnd, message, wParam, lParam);
}


