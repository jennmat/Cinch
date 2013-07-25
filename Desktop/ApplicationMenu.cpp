
#include "stdafx.h"

HTREEITEM AddItemToTree(HWND hwndTV, LPWSTR lpszItem, LPARAM data, int nLevel)
{ 
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
    static HTREEITEM hPrevRootItem = NULL; 
    static HTREEITEM hPrevLev2Item = NULL; 

	tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	// Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 
	tvi.iImage = 0; 
    tvi.iSelectedImage = 0;
        
	tvi.lParam = data;
    tvins.item = tvi; 
    tvins.hInsertAfter = hPrev; 
	
    // Set the parent item based on the specified level. 
    if (nLevel == 1) 
        tvins.hParent = TVI_ROOT; 
    else if (nLevel == 2) 
        tvins.hParent = hPrevRootItem; 
    else 
        tvins.hParent = hPrevLev2Item; 

    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
        0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

    if (hPrev == NULL)
        return NULL;

	// Save the handle to the item. 
    if (nLevel == 1) 
        hPrevRootItem = hPrev; 
    else if (nLevel == 2) 
        hPrevLev2Item = hPrev; 

    // The new item is a child item. Give the parent item a 
    // closed folder bitmap to indicate it now has child items. 
    /*if (nLevel > 1)
    { 
        hti = TreeView_GetParent(hwndTV, hPrev); 
	    tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        tvi.hItem = hti; 
		tvi.iImage = 0; 
        tvi.iSelectedImage = 0;
        TreeView_SetItem(hwndTV, &tvi); 
		*/
    return hPrev; 
} 

BOOL InitTreeViewImageLists(HWND hwndTV) 
{ 
    HIMAGELIST himl;  // handle to image list 
    HBITMAP hbmp;     // handle to bitmap 

    // Create the image list. 
    if ((himl = ImageList_Create(16, 
                                 16,
								 ILC_COLOR32 | ILC_MASK, 
                                 1, 0)) == NULL) 
        return FALSE; 

    // Add the open file, closed file, and document bitmaps. 
	hbmp = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(IDB_FOLDER)); 
	
    int g_nOpen = ImageList_Add(himl, hbmp, (HBITMAP)NULL); 
	DeleteObject(hbmp); 

    // Fail if not all of the images were added. 
    if (ImageList_GetImageCount(himl) < 1) 
        return FALSE; 

    // Associate the image list with the tree-view control. 
    TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL); 

    return TRUE; 
} 

Explorer::Explorer(){
}

Explorer::~Explorer(){
}

void Explorer::AddMenuItems(HWND tree, const Array& items, int level){
	for(unsigned int i=0; i<items.size(); i++){
		Object item = items[i].getObject();
		const Object& constItem = items[i].getObject();
		if ( item["type"].isString() ) {
			if ( item["type"].getString().compare("folder") == 0 ){

				string folder = item["label"].getString();
				wstring wl = s2ws(folder);
				int len = folder.length() + sizeof(wchar_t);
				wchar_t* clabel = new wchar_t[len];
				wcscpy_s(clabel, len, wl.c_str());

				ULONG_PTR ptr = (ULONG_PTR)&constItem;
				HTREEITEM hitem = AddItemToTree(tree, clabel, ptr, level);	
				
				Object::const_iterator it = constItem.find("items");
				if ( it != constItem.end() ){
					AddMenuItems(tree, it->second.getArray(), level+1);
				}

			} else if (  item["type"].getString().compare("view") == 0 ){
				string viewId = item["view"].getString();
				Object view = db.getDocument(viewId).getData().getObject();

				string label = view["label"].getString();
				wstring wl = s2ws(label);
				HTREEITEM hitem = AddItemToTree(tree, (LPWSTR)wl.c_str(), (LPARAM)&constItem, level);
			}
		}
	}
}

void buildData(HWND tree, HTREEITEM parent, Object& data){
	Array items;
	//HTREEITEM sib = TreeView_GetNextSibling(tree, item);
	HTREEITEM item = parent;

	while ( item != NULL ){
		Object o;
		TVITEMEX tvi;
		tvi.mask = TVIF_TEXT;
		tvi.cchTextMax = 80;
		tvi.pszText = new WCHAR[80];
		tvi.hItem = item;
		TreeView_GetItem(tree, &tvi);
		o["label"] = ws2s(wstring(tvi.pszText));
		
		delete tvi.pszText;

		Array children;
		HTREEITEM child = TreeView_GetChild(tree, parent);
		if ( child != NULL ){
			buildData(tree, child, o);
			
		}

		items.push_back(o);

		item = TreeView_GetNextSibling(tree, item);
	}
	data["items"] = items;

}

bool Explorer::saveChanges(){
	try {
		Document updated = db.createDocument(Value(doc), id);
		doc["_rev"] = updated.getRevision();
	}catch(CouchDB::Exception ex){
		return false;
	}
	return true;
}


void Explorer::buildExplorer(HWND tree){
	Object results = db.viewResults("all-menu-definitions", "by-perspective", 25, 0, true);
	if ( results["rows"].isArray() ){
		Array rows = results["rows"].getArray();
		Object row = rows[0].getObject();
		if ( row["doc"].isObject() ){
			doc = row["doc"].getObject();
			id = row["id"].getString();
			rev = doc["_rev"].getString();

			
			if( doc["items"].isArray() ){
				const Array& items = doc["items"].getArray();
				AddMenuItems(tree, items, 1);
			}
		}
	}


	HandleExplorerRenames(tree);
	HandleDragDrop(tree);
}

void CreateApplicationExplorer(HWND tree){
	TreeView_DeleteAllItems(tree);
	InitTreeViewImageLists(tree);
	
	Explorer* explorer = new Explorer;
	SetWindowLongPtr(tree, GWLP_USERDATA, (ULONG_PTR)explorer);

	explorer->buildExplorer(tree);
}

void DestroyApplicationExplorer(HWND tree){
	Explorer* explorer = (Explorer *)GetWindowLongPtr(tree, GWLP_USERDATA);
	delete explorer;
}