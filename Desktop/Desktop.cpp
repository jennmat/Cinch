// Desktop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Desktop.h"
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;

#define MAX_LOADSTRING 100

#define TREE_WIDTH 200
#define LIST_WIDTH 200
#define TOOLBAR_HEIGHT 54

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	NewView(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK AddDocumentType(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DESKTOP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DESKTOP));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg) && !IsDialogMessage(designer, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DESKTOP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DESKTOP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


// Adds items to a tree-view control. 
// Returns the handle to the newly added item. 
// hwndTV - handle to the tree-view control. 
// lpszItem - text of the item to add. 
// nLevel - level at which to add the item. 
//
// g_nClosed, and g_nDocument - global indexes of the images.

HTREEITEM AddItemToTree(HWND hwndTV, LPWSTR lpszItem, LPARAM data, int nLevel)
{ 
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
    static HTREEITEM hPrevRootItem = NULL; 
    static HTREEITEM hPrevLev2Item = NULL; 
    HTREEITEM hti; 

	tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    // Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 
	tvi.iImage = 2;
	tvi.iSelectedImage = 2;
   
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
    if (nLevel > 1)
    { 
        hti = TreeView_GetParent(hwndTV, hPrev); 
	    tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        tvi.hItem = hti; 
		tvi.iImage = 0; 
        tvi.iSelectedImage = 0;
        TreeView_SetItem(hwndTV, &tvi); 
    } 

    return hPrev; 
} 

void LoadViews(HWND hwnd){

	TreeView_DeleteAllItems(hwnd);

	Connection conn;
	Database db = conn.getDatabase("bugs");
	Object views = db.listViews();

   if ( views["total_rows"].getInt() > 0 ){
	   Array rows = views["rows"].getArray();
	   for(unsigned int i=0; i<rows.size(); i++){
		   Object row = rows[i].getObject();
			
		   Object doc = row["doc"].getObject();
		   if ( doc["system_view"].isBoolean() && doc["system_view"].getBoolean() == true ){
			   continue;
		   }
		   wstring name;
		   wstring id = s2ws(row["id"].getString());
		   if ( doc["label"].isString() ){
				name = s2ws(doc["label"].getString());
		   } else {
				name = s2ws(row["id"].getString());
		   }
		   LPWSTR str = new wchar_t[80];
		   wcscpy_s(str, 80, name.c_str());

		   AddItemToTree(hwnd, str, NULL, 1);
	
		   Object views = doc["views"].getObject();
		   Object::const_iterator it = views.begin();
		   for(it=views.begin(); it != views.end(); it++){
				pair<string, Value> p = *it;
				wstring view = s2ws(p.first);
				wstring name = s2ws(p.first);
				Object o = p.second.getObject();
				if ( o["label"].isString() ){
					name = s2ws(o["label"].getString());
				}
				LPWSTR str = new wchar_t[80];
				wcscpy_s(str, 80, name.c_str());

				int dlen = id.length() + sizeof(wchar_t);
				ViewPair* v = new ViewPair;
				v->design = new wchar_t[dlen];
				wcscpy_s(v->design, dlen, id.c_str());

				int vlen = view.length() + sizeof(wchar_t);
				v->view = new wchar_t[vlen];
				wcscpy_s(v->view, vlen, view.c_str());
				
				AddItemToTree(hwnd, str, (LPARAM)v, 2);
		
		   }
		   //for(unsigned int j = 0; j<views.size(); j++){
			 //  Object view = views[j].getObject();
			  // int a = 1;
			//}

	   }
   }
}

void SizeWindows(HWND hWnd);

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1000, 700, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   
   RECT client;
   GetClientRect(hWnd, &client);

   InitCommonControls();

   tree = CreateWindowEx(0, WC_TREEVIEW, TEXT("Tree View"),
	   WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
	   0, 0, TREE_WIDTH, client.bottom,
	   hWnd, (HMENU) IDC_VIEW_TREE, hInst, 0);

	HIMAGELIST hImageList=ImageList_Create(16,16,ILC_COLOR16,3,10);					  // Macro: 16x16:16bit with 2 pics [array]
	HBITMAP hBitMap=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_TREE));					  // load the picture from the resource
	
	ImageList_Add(hImageList,hBitMap,NULL);								      // Macro: Attach the image, to the image list
	DeleteObject(hBitMap);													  // no need it after loading the bitmap
	//SendMessage(hWnd,IDC_TREE1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control
	//TreeView_SetImageList(tree, hImageList, 0);
  
	toolbar = CreateCinchToolbar(hWnd);
	ShowWindow(toolbar, SW_SHOW);
	UpdateWindow(toolbar);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	delegate = new CouchViewDelegate(conn);
	//delegate->setView(s2ws(string("_design/properties")), s2ws(string("by-nickname")));

   grid = CinchGrid::CreateCinchGrid(hWnd, delegate);
   designer = CinchDesigner::CreateCinchDesigner(hWnd);
   
   Database db = conn.getDatabase("bugs");
   
   CinchDesigner* d = (CinchDesigner *)GetWindowLong(designer, GWL_USERDATA);
   
   d->getForm()->setDelegate(&desktop);
   /*d->getForm()->addField(FormField::createEditField(designer, hInst, TEXT("nickname")));
   d->getForm()->addField(FormField::createEditField(designer, hInst, TEXT("address")));
   d->getForm()->addField(FormField::createNumberField(designer, hInst, TEXT("price")));
   d->getForm()->addField(FormField::createDatePicker(designer, hInst, TEXT("datePurchased")));
   d->getForm()->addField(FormField::createNumberField(designer, hInst, TEXT("purchaseAmount")));
   d->getForm()->addField(FormField::createNumberField(designer, hInst, TEXT("lastAppraisedValue")));
   d->getForm()->addField(FormField::createNumberField(designer, hInst, TEXT("mortgagePayment")));
   d->getForm()->addField(FormField::createNumberField(designer, hInst, TEXT("minimumRent")));
   d->getForm()->addField(FormField::createYesNoField(designer, hInst, TEXT("occupied")));
   d->getForm()->addDetail(TEXT("Notes"));
   d->getForm()->addDetail(TEXT("Inspections"));
   d->getForm()->getDetail()->CreateTextareaForPage(L"notes", 0);
   d->getForm()->getDetail()->CreateTableForPage(L"inspections", 1);*/

   try {
	/*Document doc = db.getDocument("template/property");
	Value v = doc.getData();
	desktop.loadedForm = v.getObject();
	d->getForm()->deserializeForm(designer, v);*/
   }catch(Exception e){
   }


   ShowWindow(grid, SW_SHOW);
   ShowWindow(designer, SW_SHOW);

	LoadViews(tree);
  

   ShowWindow(tree, SW_SHOW);
   SizeWindows(hWnd);

   DWORD threadId;
   CreateThread(NULL, 0, ChangesListener, NULL, 0, &threadId); 

#ifdef REPLICATION
   db.startReplication(DESTINATION_HOST, DESTINATION_DATABASE, DESTINATION_USERNAME, DESTINATION_PASSWORD);
#endif 
   return TRUE;
}

void SizeWindows(HWND hWnd)
{
	RECT client;
	GetClientRect(hWnd, &client);
	SetWindowPos(grid, HWND_TOP, TREE_WIDTH, TOOLBAR_HEIGHT, LIST_WIDTH, client.bottom - TOOLBAR_HEIGHT, 0);
	SetWindowPos(tree, HWND_TOP, 0, TOOLBAR_HEIGHT, TREE_WIDTH, client.bottom - TOOLBAR_HEIGHT, 0);
	SetWindowPos(designer, HWND_TOP, TREE_WIDTH + LIST_WIDTH, TOOLBAR_HEIGHT, client.right - TREE_WIDTH - LIST_WIDTH, client.bottom - TOOLBAR_HEIGHT, 0);
	//SetWindowPos(toolbar, HWND_TOP, TREE_WIDTH + LIST_WIDTH, 0, client.right - TREE_WIDTH - LIST_WIDTH, TOOLBAR_HEIGHT, 0);
	SetWindowPos(toolbar, HWND_TOP, TREE_WIDTH+LIST_WIDTH, 10, client.right, TOOLBAR_HEIGHT, 0);
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_NOTIFY:
		{
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		LPNMTOOLBAR lpnmTB = (LPNMTOOLBAR)lParam;

		switch(pnmhdr->code){
		case TVN_SELCHANGED:
			if  (pnmhdr->idFrom == IDC_VIEW_TREE && pnmhdr->code == TVN_SELCHANGED ){
				HTREEITEM item = ((LPNM_TREEVIEW)pnmhdr)->itemNew.hItem;

				TVITEM tvitem;

				tvitem.hItem = item;
				tvitem.mask = TVIF_PARAM;
				TreeView_GetItem(tree, &tvitem);

				if ( tvitem.lParam != NULL ){
					ViewPair * v = (ViewPair *)tvitem.lParam;
			
					delegate->setView(v->design, v->view);
					CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(grid, GWL_USERDATA);
					gridcontrol->reloadData();
				}

			}


			break;
		case TBN_DROPDOWN:
			// Get the coordinates of the button.
            RECT rc;
            SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmTB->iItem, (LPARAM)&rc);

            // Convert to screen coordinates.            
            MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);                         
        
            // Get the menu.
			//HMENU hMenuLoaded = LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(IDR_POPUP)); 
         
            // Get the submenu for the first menu item.
            //HMENU hPopupMenu = GetSubMenu(hMenuLoaded, 0);


			HMENU hPopupMenu = CreatePopupMenu();

			objectTypes.clear();

			Connection conn;
			Database db = conn.getDatabase("bugs");
			Object r = db.viewResults("all-objects", "by-label", Value(), 100);
			Array rows = r["rows"].getArray();
			unsigned int i = 0;
			for(; i<rows.size(); i++){
				Object row = rows[i].getObject();
				string key = row["key"].getString();
				wstring wkey = s2ws(key);
				
				Document doc = db.getDocument(row["id"].getString());

				objectTypes.push_back(doc.getData().getObject());

				InsertMenu(hPopupMenu, i, MF_BYPOSITION | MF_STRING, IDD_ADD_OBJECT, wkey.c_str());
            }
			

			InsertMenu(hPopupMenu, i+1, MF_BYPOSITION | MF_STRING, IDC_ADD_DOCUMENT_TYPE, L"Document Type");

			MENUINFO mi;
			memset(&mi, 0, sizeof(mi));
			mi.cbSize = sizeof(mi);
			mi.fMask = MIM_STYLE;
			mi.dwStyle = MNS_NOTIFYBYPOS;
			SetMenuInfo(hPopupMenu, &mi);

            // Set up the pop-up menu.
            // In case the toolbar is too close to the bottom of the screen, 
            // set rcExclude equal to the button rectangle and the menu will appear above 
            // the button, and not below it.
         
            TPMPARAMS tpm;
         
            tpm.cbSize    = sizeof(TPMPARAMS);
            tpm.rcExclude = rc;
         
            // Show the menu and wait for input. 
            // If the user selects an item, its WM_COMMAND is sent.
         
            TrackPopupMenuEx(hPopupMenu, 
                             TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
                             rc.left, rc.bottom, hWnd, &tpm);

            DestroyMenu(hPopupMenu);
         
        return (FALSE);
			break;
		}
		
		break;
		}
	case WM_NEW_DATA_ARRIVED:
		{
		CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(grid, GWL_USERDATA);
		delegate->loadViewResults();			
		gridcontrol->reloadData();
		}
		break;
	case CINCHGRID_ROW_SELECTED:
		{
		CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(grid, GWL_USERDATA);
		CinchDesigner* designercontrol = (CinchDesigner *)GetWindowLong(designer, GWL_USERDATA);
		int row = gridcontrol->GetActiveRow();
		string str = delegate->getDocumentIdForRow(row);
		Database db = conn.getDatabase("bugs");
		Document d = db.getDocument(str);
		Value v = d.getData();
		designercontrol->LoadDocument("bugs", d.getID(), v.getObject());
		break;
		}
	case WM_MENUCOMMAND:
		{
		unsigned int idx = wParam;
		if ( idx >= objectTypes.size() ){
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ADD_DOCUMENT_TYPE), hWnd, AddDocumentType);
		} else {
			Object objectDefinition = objectTypes[idx];
			CinchDesigner* designercontrol = (CinchDesigner *)GetWindowLong(designer, GWL_USERDATA);
			designercontrol->NewDocument("bugs", objectDefinition["name"].getString());
		}
		
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		if ( wmEvent == EN_KILLFOCUS ){
			int a = 1;
		}
		switch (wmId)
		{
		case IDM_NEW_VIEW:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ADD_VIEW), hWnd, NewView);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		SizeWindows(hWnd);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK NewView(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	int wmEvent;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
		vector<string>* compareOperators = new vector<string>();
		HWND compareCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_COMPARE);
		SendMessage(compareCombo, CB_ADDSTRING, 0, (LPARAM)L"is equal to");
		compareOperators->push_back("==");
		SendMessage(compareCombo, CB_ADDSTRING, 0, (LPARAM)L"is not equal to");
		compareOperators->push_back("!=");
		SendMessage(compareCombo, CB_ADDSTRING, 0, (LPARAM)L"is greater than");
		compareOperators->push_back(">");
		SendMessage(compareCombo, CB_ADDSTRING, 0, (LPARAM)L"is less than");
		compareOperators->push_back("<");
		
		SetWindowLong(compareCombo, GWL_USERDATA, (ULONG_PTR)compareOperators);

		HWND typeCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_DOC_TYPE);

		Connection conn;
		Database db = conn.getDatabase("bugs");
		Object r = db.viewResults("all-objects", "by-label", Value(), 100);
	
		vector<string>* ids = new vector<string>();

		Array rows = r["rows"].getArray();
		unsigned int i = 0;
		for(; i<rows.size(); i++){
			Object row = rows[i].getObject();
			string key = row["key"].getString();
			wstring wkey = s2ws(key);
				
			SendMessage(typeCombo, CB_ADDSTRING, 0, (LPARAM)wkey.c_str());
			ids->push_back(row["id"].getString());

        }

		SetWindowLong(typeCombo, GWL_USERDATA, (ULONG_PTR)ids);

		return (INT_PTR)TRUE;

		}
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch( wmEvent ){
		case CBN_SELCHANGE:
			{
				if ( wmId == IDC_ADD_VIEW_DOC_TYPE ){

					HWND fieldCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_FIELD);
					HWND sortCombo = GetDlgItem(hDlg, IDC_NEW_VIEW_SORT_BY);
					ComboBox_ResetContent(fieldCombo);
					ComboBox_ResetContent(sortCombo);

					HWND typeCombo = GetDlgItem(hDlg, wmId);
					vector<string>* ids = (vector<string>*)GetWindowLong(typeCombo, GWL_USERDATA);
					int idx = ComboBox_GetCurSel(typeCombo);
					string id = (*ids)[idx];

					Connection conn;
					Database db = conn.getDatabase("bugs");
					Document doc = db.getDocument(id);
					Object obj = doc.getData().getObject();

					if (obj["name"].isString() ){
						string name = obj["name"].getString();

						stringstream template_id;
						template_id << "template/" << name;
						Document doc = db.getDocument(template_id.str());
						Value v = doc.getData();
						if ( v.isObject() ){
							Object templ = v.getObject();
							if ( templ["fields"].isArray() ){
								Array fields = templ["fields"].getArray();
								vector<Object>* fieldsVector = new vector<Object>();
								for(unsigned i=0; i<fields.size(); i++){
									Object field = fields[i].getObject();
									string name = field["name"].getString();
									string label = field["label"].getString();

									wstring wlabel = s2ws(label);

									fieldsVector->push_back(field);
									SendMessage(fieldCombo, CB_ADDSTRING, 0, (LPARAM)wlabel.c_str());
									SendMessage(sortCombo, CB_ADDSTRING, 0, (LPARAM)wlabel.c_str());
								}

								SetWindowLong(fieldCombo, GWL_USERDATA, (ULONG_PTR)fieldsVector);
								SetWindowLong(sortCombo, GWL_USERDATA, (ULONG_PTR)fieldsVector);
							}
						}
					}
				}
				else if ( wmId == IDC_ADD_VIEW_FIELD ){
					

				}
			}
			break;
		}
		if ( wmId == IDOK ){
			/* Create the view */
			HWND typeCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_DOC_TYPE);
			HWND fieldCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_FIELD);
			HWND compareCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_COMPARE);
			HWND valueEdit = GetDlgItem(hDlg, IDC_COMPARE_TO_VALUE);	
			HWND sortCombo = GetDlgItem(hDlg, IDC_NEW_VIEW_SORT_BY);
			HWND newViewNameEdit = GetDlgItem(hDlg, IDC_NEW_VIEW_NAME);

			char map[1024];
			memset(map, 0, 1024);
			vector<string>* ids = (vector<string>*)GetWindowLong(typeCombo, GWL_USERDATA);
			int idx = ComboBox_GetCurSel(typeCombo);
			string id = (*ids)[idx];

			Connection conn;
			Database db = conn.getDatabase("bugs");
			Document doc = db.getDocument(id);
			Object obj = doc.getData().getObject();

			string type;
			if (obj["name"].isString() ){
				type = obj["name"].getString();
			}

			int fieldIdx = ComboBox_GetCurSel(fieldCombo);
			vector<Object>* fieldsVector = (vector<Object>*)GetWindowLong(fieldCombo, GWL_USERDATA);
			Object field = (*fieldsVector)[fieldIdx];

			string fieldforcomparison = field["name"].getString();

			vector<string>* operatorVector = (vector<string>*)GetWindowLong(compareCombo, GWL_USERDATA);
			int compareIdx = ComboBox_GetCurSel(compareCombo);
			string comparison = (*operatorVector)[compareIdx];

			int len = GetWindowTextLength(valueEdit) + 1;
			wchar_t* val = new wchar_t[len];
			GetWindowText(valueEdit, val, len);

			string valueforcomparison = ws2s(val);

			int sortIdx = ComboBox_GetCurSel(sortCombo);
			vector<Object>* sortFieldsVector = (vector<Object>*)GetWindowLong(sortCombo, GWL_USERDATA);
			Object sortFields = (*sortFieldsVector)[sortIdx];

			string sortby = sortFields["name"].getString();
			string sortbylabel = sortFields["label"].getString();

			int namelen = GetWindowTextLength(newViewNameEdit) + 1;
			wchar_t* wname = new wchar_t[namelen];
			GetWindowText(newViewNameEdit, wname, namelen);

			string name = ws2s(wname);

			string viewnamesanitized = name;

			sprintf_s(map, 1024, "function(doc){ if ( doc.type && doc.type == '%s' && doc.%s && doc.%s %s '%s' ) emit(doc.%s, null); }", 
				type.c_str(), fieldforcomparison.c_str(), fieldforcomparison.c_str(), comparison.c_str(), valueforcomparison.c_str(), sortby.c_str());	

			Object design = Object();
			design["language"] = "javascript";
			Object view = Object();
			stringstream viewname;
			viewname << "by-";
			viewname << sortby;

			stringstream viewlabelstream;
			viewlabelstream << "By ";
			viewlabelstream << sortbylabel;

			Object v = Object();
			v["label"] = viewlabelstream.str();
			v["map"] = map;

			view[viewname.str()] = v;
			
			stringstream labelstream;
			labelstream << name;
			design["label"] = labelstream.str();
			design["views"] = view;

			stringstream design_id;
			design_id << "_design/";
			design_id << viewnamesanitized;

			db.createDocument(Value(design), design_id.str());

			LoadViews(tree);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



// Message handler for about box.
INT_PTR CALLBACK AddDocumentType(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK ){
			int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_TYPE_LABEL)) + 1;
			wchar_t * label = new wchar_t[len];

			GetWindowText(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_TYPE_LABEL), label, len);

			len = GetWindowTextLength(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_TYPE_PLURAL_LABEL)) + 1;
			wchar_t * plurallabel = new wchar_t[len];

			GetWindowText(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_TYPE_PLURAL_LABEL), plurallabel, len);


			len = GetWindowTextLength(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_TYPE_NAME)) + 1;
			wchar_t * name = new wchar_t[len];

			GetWindowText(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_TYPE_NAME), name, len);

			len = GetWindowTextLength(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_FIRST_FIELD_NAME)) + 1;
			wchar_t * fname = new wchar_t[len];

			GetWindowText(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_FIRST_FIELD_NAME), fname, len);

			len = GetWindowTextLength(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_FIRST_FIELD_LABEL)) + 1;
			wchar_t * flabel = new wchar_t[len];

			GetWindowText(GetDlgItem(hDlg, IDC_ADD_DOCUMENT_FIRST_FIELD_LABEL), flabel, len);


			string sname = ws2s(name);
			string slabel = ws2s(label);
			string splurallabel = ws2s(plurallabel);

			string sfname = ws2s(fname);
			string sflabel = ws2s(flabel);

			Object definition = Object();
			definition["name"] = sname;
			definition["label"] = slabel;
			definition["type"] = "object-definition";

			
			/* Create a default view */
			char map[1024];
			memset(map, 0, 1024);

			sprintf_s(map, 1024, "function(doc){ if ( doc.type && doc.type == '%s' ) emit(doc.%s, null); }", sname.c_str(), sfname.c_str());	

			Object design = Object();
			design["language"] = "javascript";
			Object view = Object();
			stringstream viewname;
			viewname << "by-";
			viewname << sfname;

			stringstream viewlabelstream;
			viewlabelstream << "By ";
			viewlabelstream << sflabel;

			Object v = Object();
			v["label"] = viewlabelstream.str();
			v["map"] = map;

			view[viewname.str()] = v;
			
			stringstream labelstream;
			labelstream << "All ";
			labelstream << splurallabel;
			design["label"] = labelstream.str();
			design["views"] = view;

			stringstream id;
			id << "_design/";
			id << sname;


			/* Create a starter template */
			Object _template = Object();
			Array fields = Array();
			Object field = Object();

			field["name"] = sfname;
			field["label"] = sflabel;
			field["type"] = EDIT;

			fields.push_back(field);
			_template["fields"] = fields;


			Connection conn;
			Database db = conn.getDatabase("bugs");

			db.createDocument(Value(definition));

			db.createDocument(Value(design), id.str());

			stringstream template_id;
			template_id << "template/";
			template_id << sname;

			db.createDocument(Value(_template), template_id.str());

			LoadViews(tree);
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



void changesArrived(){
	PostMessage(hWnd, WM_NEW_DATA_ARRIVED, 0, 0);
}

DWORD WINAPI ChangesListener(LPVOID lParam){

	Connection conn;
	Database db = conn.getDatabase("bugs");
	db.listenForChanges(changesArrived);

	return 0;
}

void Desktop::formModified(){
	CinchDesigner* d = (CinchDesigner *)GetWindowLong(designer, GWL_USERDATA);
   
	Object o = d->getForm()->serializeFormToObject(loadedForm);

	Connection conn;
	Database db = conn.getDatabase("bugs");

	db.createDocument(Value(o), "template/property");
}



