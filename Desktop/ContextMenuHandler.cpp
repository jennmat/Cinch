#include "stdafx.h"

#define IDD_EXPLORER_NEW_FOLDER 15065
#define IDD_EXPLORER_DELETE_FOLDER 15066
#define IDD_EXPLORER_ADD_VIEW 15067

extern HWND tree;
extern Explorer* explorer;

void HandleContextMenu(HWND hwnd) {
	SetWindowSubclass(hwnd, HandleContextMenuProc, 0, 0);
}

LRESULT CALLBACK HandleContextMenuProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData){

	if ( message == WM_RBUTTONDOWN ){
		if ( explorer->isInitialized() ){
			POINT point;
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			HMENU hPopupMenu = CreatePopupMenu();
			InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EXPLORER_DELETE_FOLDER, L"Delete");
			InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EXPLORER_NEW_FOLDER, L"New Folder");
			InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EXPLORER_ADD_VIEW, L"Add View");
		
			ClientToScreen(hWnd, &point);
			TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, hWnd, NULL);
			DestroyMenu(hPopupMenu);
		}
	}

	if ( message == WM_COMMAND ) {
		int wmId, wmEvent;

		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
			case IDD_EXPLORER_ADD_VIEW:
			{
				DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ADD_EXISTING_VIEW), hWnd, HandleAddExistingDialogFunc);
				break;
			}
			case IDD_EXPLORER_NEW_FOLDER:
			{
				HTREEITEM current = TreeView_GetSelection(tree);
				
				Object newFolder;
				newFolder["type"] = "folder";
				newFolder["label"] = "New Folder";
				
				TVITEM tvi; 
				TVINSERTSTRUCT tvins; 
    
				tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
				// Set the text of the item. 
				tvi.pszText = L"New Folder"; 
				tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 
				tvi.iImage = 0; 
				tvi.lParam = (LPARAM)new Object(newFolder);
				tvi.iSelectedImage = 0;
        
			    tvins.item = tvi; 
				tvins.hInsertAfter = TVI_LAST; 
				tvins.hParent = current;
				TreeView_InsertItem(tree, &tvins);

				TreeView_Expand(tree, current, TVE_EXPAND);

				return explorer->saveChanges(tree);
			}
			case IDD_EXPLORER_DELETE_FOLDER:
			{
				HTREEITEM current = TreeView_GetSelection(tree);
				TVITEMEX current_item;
				current_item.mask = TVIF_PARAM;
				current_item.hItem = current;
				TreeView_GetItem(tree, &current_item);

				EnumerateChildrenRecursively(tree, current, CleanupItemData);
				CleanupItemData(tree, current);

				TreeView_DeleteItem(tree, current);
				return explorer->saveChanges(tree);
			}
		}
	}


	return DefSubclassProc(hWnd, message, wParam, lParam);
}



INT_PTR CALLBACK HandleAddExistingDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HWND combo = GetDlgItem(hDlg, IDC_PICK_VIEW_COMBO);
			Object results = db.viewResults("all-view-definitions", "by-label", false, 100, 0);
			Object* storedPtr = new Object(results);
			SetWindowLongPtr(hDlg, GWLP_USERDATA, (ULONG_PTR)storedPtr);
			if ( results["rows"].isArray() ){
				Array rows = results["rows"].getArray();
				for(unsigned int i=0; i<rows.size(); i++){
					Object row = rows[i].getObject();
					if ( row["key"].isString() ){
						const string& key = row["key"].getString();
						ComboBox_AddString(combo, s2ws(key).c_str());
					}
				}
			}
			return (INT_PTR)TRUE;
		}
	case WM_DESTROY:
		{
		Object* ptr = (Object *)GetWindowLongPtr(hDlg, GWLP_USERDATA);
		delete ptr;
		break;
		}
	case WM_COMMAND:
		if ( LOWORD(wParam == IDOK ) ){
			HWND combo = GetDlgItem(hDlg, IDC_PICK_VIEW_COMBO);
			Object* storedPtr = (Object*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
			Object views = *storedPtr;

			int idx = ComboBox_GetCurSel(combo);
			if ( idx >= 0 ){
				Array rows = views["rows"].getArray();
				Object selected = rows[idx].getObject();
				string id = selected["id"].getString();

				Object viewDefinition = db.getDocument(id).getData().getObject();

				HTREEITEM current = TreeView_GetSelection(tree);

				Object newView = Object();
				newView["type"] = "view";
				newView["design"] = viewDefinition["design_name"];
				newView["view"] = viewDefinition["default_view"];
				newView["label"] = viewDefinition["label"];

				TVITEM tvi; 
				TVINSERTSTRUCT tvins; 
    
				tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
				// Set the text of the item. 
				wstring wlabel = s2ws(viewDefinition["label"].getString());
				int len = wlabel.length()+1;
				wchar_t* label = new wchar_t[len];
				wcscpy_s(label, len, wlabel.c_str());
				tvi.pszText = label;
				tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 
				tvi.iImage = 0; 
				tvi.lParam = (ULONG_PTR)new Object(newView);
				tvi.iSelectedImage = 0;
        
			    tvins.item = tvi; 
				tvins.hInsertAfter = TVI_LAST; 
				tvins.hParent = current;
				TreeView_InsertItem(tree, &tvins);

				delete label;
				
				explorer->saveChanges(tree);
			}
		}
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}