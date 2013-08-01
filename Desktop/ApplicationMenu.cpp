
#include "stdafx.h"

HTREEITEM AddItemToTree(HWND hwndTV, HTREEITEM parent, LPWSTR lpszItem, LPARAM data, int nLevel)
{ 
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
  
	tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	// Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 
	tvi.iImage = 0; 
    tvi.iSelectedImage = 0;
        
	tvi.lParam = data;
    tvins.item = tvi; 
    tvins.hInsertAfter = TVI_LAST; 
	
	if ( parent == NULL ){
		tvins.hParent = TVI_ROOT; 
	} else {
		tvins.hParent = parent;
	}
    
    // Add the item to the tree-view control. 
    return (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
        0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

} 


void EnumerateChildrenRecursively(HWND tree, HTREEITEM parent, std::function<void (HWND,HTREEITEM)> func) {
	Array items;
	HTREEITEM item = parent;

	HTREEITEM child = TreeView_GetChild(tree, parent);
	while ( child != NULL ){
		func(tree, child);
		EnumerateChildrenRecursively(tree, child, func);
		child = TreeView_GetNextSibling(tree, child);
	}
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

void Explorer::AddMenuItems(HWND tree, HTREEITEM parent, const Array& items, int level){
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

				Object obj = Object();
				obj["type"] = item["type"];
				obj["label"] = item["label"];
				HTREEITEM hitem = AddItemToTree(tree, parent, clabel, (LPARAM)new Object(obj), level);	
				
				delete clabel;
				
				Object::const_iterator it = constItem.find("items");
				if ( it != constItem.end() ){
					AddMenuItems(tree, hitem, it->second.getArray(), level+1);
				}

			} else if (  item["type"].getString().compare("view") == 0 ){
				string viewId = item["view"].getString();
				Object view = db.getDocument(viewId).getData().getObject();

				string label = view["label"].getString();
				wstring wl = s2ws(label);
				Object obj = Object();
				obj["type"] = item["type"];
				obj["view"] = item["view"];
				hitem = AddItemToTree(tree, parent, (LPWSTR)wl.c_str(), (LPARAM)new Object(obj), level);
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
		tvi.mask = TVIF_PARAM;
		tvi.hItem = item;
		TreeView_GetItem(tree, &tvi);
		Object* objPtr = (Object *)tvi.lParam;
		o = Object(*objPtr);
		
		
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

bool Explorer::saveChanges(HWND tree){
	try {
		Object obj;
		buildData(tree, TreeView_GetRoot(tree), obj);
		cout << obj;
		obj["_rev"] = rev;
		obj["cinch_type"] = "menu_definition";
		obj["target_perspective"] = "abc";
		Document updated = db.createDocument(Value(obj), id);
		rev = updated.getRevision();
	}catch(CouchDB::Exception ex){
		return false;
	}
	return true;
}

void Explorer::deleteRoot(){
	if ( doc["items"].isArray() ){
		doc["items"] = Array();
	}
}

void Explorer::addRoot(char* label){
	if( !doc["items"].isArray() ){
		doc["items"] = Array();
	}
	Object newRootObj = Object();
	newRootObj["type"] = "folder";
	newRootObj["label"] = label;
	doc["items"].push(newRootObj);
}

Object* Explorer::getRoot(){
	return &doc;
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
				AddMenuItems(tree, TVI_ROOT, items, 1);
			}
		}
	}


	HandleExplorerRenames(tree);
	HandleDragDrop(tree);
	HandleContextMenu(tree);
}

void CreateApplicationExplorer(HWND tree){
	DestroyApplicationExplorer(tree);
	InitTreeViewImageLists(tree);
	
	Explorer* explorer = new Explorer;
	SetWindowLongPtr(tree, GWLP_USERDATA, (ULONG_PTR)explorer);

	explorer->buildExplorer(tree);
}

void DestroyApplicationExplorer(HWND tree){
	
	HTREEITEM root = TreeView_GetRoot(tree);
	EnumerateChildrenRecursively(tree, root, CleanupItemData);
	CleanupItemData(tree, root);
	TreeView_DeleteAllItems(tree);
	
	
	Explorer* explorer = (Explorer *)GetWindowLongPtr(tree, GWLP_USERDATA);
	delete explorer;
}