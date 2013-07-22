// Desktop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Desktop.h"
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;

#define MAX_LOADSTRING 100

#define TREE_WIDTH 200
#define LIST_WIDTH 200
#define TOP_MARGIN 5
#define INNER_MARGIN 5

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
void				DestroyInstance();
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

	HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        return FALSE;
    }

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
		try {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg) && !IsDialogMessage(designer, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}catch(CouchDB::Exception ex){
			wstring m = s2ws(ex.what());
			MessageBox(msg.hwnd, m.c_str(), L"", 0);
		}
	}

	CoUninitialize();

	DestroyInstance();

	_CrtDumpMemoryLeaks();

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
	wcex.hbrBackground	= CreateSolidBrush(DEFAULT_BACKGROUND_COLOR);
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



/*

void LoadViews(HWND hwnd, string emitsDocumentsOfType = ""){

	TreeView_DeleteAllItems(hwnd);

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

		   bool hasCinchView = false;

		   Object views = doc["views"].getObject();
		   Object::const_iterator it = views.begin();

		   for(it=views.begin(); it != views.end(); it++){
				pair<string, Value> p = *it;
				Object o = p.second.getObject();
				if ( o["cinch_view"].isBoolean() && o["cinch_view"].getBoolean() == true && (emitsDocumentsOfType.length() == 0 || ( o["emits_docs_with_type"].isString() && o["emits_docs_with_type"].getString().compare(emitsDocumentsOfType) == 0 ) ) ){
					hasCinchView = true;
				}
		   }

		   if ( hasCinchView ){
				AddItemToTree(hwnd, str, NULL, 1);
		   }
	
		   for(it=views.begin(); it != views.end(); it++){
			   pair<string, Value> p = *it;
				wstring view = s2ws(p.first);
				wstring name = s2ws(p.first);
				Object o = p.second.getObject();
				if ( o["cinch_view"].isBoolean() && o["cinch_view"].getBoolean() == true && (emitsDocumentsOfType.length() == 0 || ( o["emits_docs_with_type"].isString() && o["emits_docs_with_type"].getString().compare(emitsDocumentsOfType) == 0 ) ) ){
					if ( o["label"].isString() ){
						name = s2ws(o["label"].getString());
					}
					LPWSTR str = new wchar_t[80];
					wcscpy_s(str, 80, name.c_str());

					int dlen = id.length() + sizeof(wchar_t);
					ViewPair* v = new ViewPair;
					v->design = row["id"].getString();
					v->view = p.first;
					v->emitsDocsWithType = o["emits_docs_with_type"].getString();
					AddItemToTree(hwnd, str, (LPARAM)v, 2);
				}
		
		   }
		   //for(unsigned int j = 0; j<views.size(); j++){
			 //  Object view = views[j].getObject();
			  // int a = 1;
			//}

	   }
   }
}

*/

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
	   WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_NOTOOLTIPS | TVS_EDITLABELS,
	   0, 0, TREE_WIDTH, client.bottom,
	   hWnd, (HMENU) IDC_VIEW_TREE, hInst, 0);

   TreeView_SetBkColor(tree, DEFAULT_BACKGROUND_COLOR);

	HIMAGELIST hImageList=ImageList_Create(16,16,ILC_COLOR16,3,10);					  // Macro: 16x16:16bit with 2 pics [array]
	HBITMAP hBitMap=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_TREE));					  // load the picture from the resource
	
	ImageList_Add(hImageList,hBitMap,NULL);								      // Macro: Attach the image, to the image list
	DeleteObject(hBitMap);													  // no need it after loading the bitmap
	//SendMessage(hWnd,IDC_TREE1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control
	//TreeView_SetImageList(tree, hImageList, 0);
  
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	delegate = new CouchViewDelegate(conn);
	//delegate->setView(s2ws(string("_design/properties")), s2ws(string("by-nickname")));

	grid = CinchGrid::CreateCinchGrid(hWnd, delegate);
	designer = CinchDesigner::CreateCinchDesigner(hWnd);
   
	
   
	CinchDesigner* d = (CinchDesigner *)GetWindowLong(designer, GWL_USERDATA);
   
	d->getForm()->setDelegate(&desktop);
   
   
	ShowWindow(grid, SW_SHOW);
	//ShowWindow(designer, SW_SHOW);

	CreateApplicationExplorer(tree);
  
	
	ShowWindow(tree, SW_SHOW);
	SizeWindows(hWnd);

	DWORD threadId;
	CreateThread(NULL, 0, ChangesListener, NULL, 0, &threadId); 

#ifdef REPLICATION
	db.startReplication(DESTINATION_HOST, DESTINATION_DATABASE, DESTINATION_USERNAME, DESTINATION_PASSWORD);
#endif 
   return TRUE;
}


void DestroyInstance() {
	//CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(grid, GWL_USERDATA);
	DestroyApplicationExplorer(tree);
}


void SizeWindows(HWND hWnd)
{
	RECT client;
	
	GetClientRect(hWnd, &client);
	
	UINT ribbonHeight;
	GetRibbonHeight(&ribbonHeight);
	
	SetWindowPos(grid, HWND_TOP, TREE_WIDTH + INNER_MARGIN, ribbonHeight+TOP_MARGIN, LIST_WIDTH, client.bottom - ribbonHeight - TOP_MARGIN - TOP_MARGIN, 0);
	SetWindowPos(tree, HWND_TOP, 0, ribbonHeight+TOP_MARGIN, TREE_WIDTH, client.bottom - ribbonHeight - TOP_MARGIN, 0);
	SetWindowPos(designer, HWND_TOP, TREE_WIDTH + LIST_WIDTH + INNER_MARGIN + INNER_MARGIN, ribbonHeight + TOP_MARGIN, client.right - TREE_WIDTH - LIST_WIDTH - INNER_MARGIN - INNER_MARGIN - INNER_MARGIN, client.bottom - ribbonHeight - TOP_MARGIN - TOP_MARGIN, 0);

	
	//SetWindowPos(toolbar, HWND_TOP, TREE_WIDTH + LIST_WIDTH, 0, client.right - TREE_WIDTH - LIST_WIDTH, TOOLBAR_HEIGHT, 0);
	//SetWindowPos(toolbar, HWND_TOP, TREE_WIDTH+LIST_WIDTH, 10, client.right, TOOLBAR_HEIGHT, 0);
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
	bool initSuccess;
	switch (message)
	{
	case WM_CREATE:
    // Initializes the Ribbon framework.
    initSuccess = InitializeFramework(hWnd);
    if (!initSuccess) 
    {
        return -1;
    }
	break;
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
			
					delegate->setView(s2ws(v->design), s2ws(v->view));
					CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(grid, GWL_USERDATA);
					gridcontrol->reloadData();
				}

			}
			break;
		case TVN_ENDLABELEDIT:
			{
				
			}
		}
		}
		break;
	case WM_ERASEBKGND:
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			UINT rheight;
			GetRibbonHeight(&rheight);
			rect.top = rheight;
			HDC dc = (HDC)wParam;
			FillRect(dc, &rect, CreateSolidBrush(DEFAULT_BACKGROUND_COLOR));

		}
		return 1;
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
		
		Document d = db.getDocument(str);
		Value v = d.getData();
		designercontrol->LoadDocument(DATABASE, d.getID(), d, v.getObject());
		break;
		}
	//case WM_MENUCOMMAND:
		//DialogBox(hInst, MAKEINTRESOURCE(IDD_ADD_DOCUMENT_TYPE), hWnd, AddDocumentType);
//		break;
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
		

		HWND typeCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_DOC_TYPE);
		
		Object r = db.viewResults("all-document-types", "by-label", 100, 0);
	
		vector<string>* ids = new vector<string>();

		Array rows = r["rows"].getArray();
		unsigned int i = 0;
		for(; i<rows.size(); i++){
			Object row = rows[i].getObject();
			string key = row["key"].getString();
			wstring wkey = s2ws(key);
			Document doc = db.getDocument(row["id"].getString());
			Object obj = doc.getData().getObject();
			SendMessage(typeCombo, CB_ADDSTRING, 0, (LPARAM)wkey.c_str());
			ids->push_back(obj["_id"].getString());
        }

		SetWindowLong(typeCombo, GWL_USERDATA, (ULONG_PTR)ids);
		
		ConditionManager* manager = new ConditionManager();
		SetWindowLong(hDlg, GWL_USERDATA, (ULONG_PTR)manager);
		

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
					string type = (*ids)[idx];

				
					ConditionManager* manager = (ConditionManager*)GetWindowLong(hDlg, GWL_USERDATA);
						
					manager->addEmptyCondition(type, hDlg);
					manager->arrangeWindowsInParent(hDlg, 35, 75);


					/* Setup the sort combo */

 					;
					
					vector<Object>* fieldsVector = new vector<Object>();

					Object results = db.viewResults("all-attributes", "by-type", Value(type), Value(type), true);
					Array rows = results["rows"].getArray();
					for(unsigned i=0; i<rows.size(); i++){
						Object row = rows[i].getObject();
						Object doc = row["doc"].getObject();

						string name = doc["_id"].getString();
						string label = doc["label"].getString();

						wstring wlabel = s2ws(label);

						fieldsVector->push_back(doc);

						SendMessage(sortCombo, CB_ADDSTRING, 0, (LPARAM)wlabel.c_str());
				

					}
					SetWindowLong(sortCombo, GWL_USERDATA, (ULONG_PTR)fieldsVector);


					
						
				}
				else if ( wmId == IDC_ADD_VIEW_FIELD || IDC_ADD_VIEW_SORT ){

					HWND typeCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_DOC_TYPE);
					vector<string>* ids = (vector<string>*)GetWindowLong(typeCombo, GWL_USERDATA);
					int idx = ComboBox_GetCurSel(typeCombo);
					string type = (*ids)[idx];

					ConditionManager* manager = (ConditionManager*)GetWindowLong(hDlg, GWL_USERDATA);
					manager->updateConditions(hDlg);
					manager->arrangeWindowsInParent(hDlg, 35, 75);
				}
			}
			break;
		}
		if ( wmId == IDOK ){
			/* Create the view */
			HWND typeCombo = GetDlgItem(hDlg, IDC_ADD_VIEW_DOC_TYPE);
			HWND sortCombo = GetDlgItem(hDlg, IDC_NEW_VIEW_SORT_BY);
			HWND newViewNameEdit = GetDlgItem(hDlg, IDC_NEW_VIEW_NAME);

			char map[1024];
			memset(map, 0, 1024);
			vector<string>* ids = (vector<string>*)GetWindowLong(typeCombo, GWL_USERDATA);
			int idx = ComboBox_GetCurSel(typeCombo);
			string type = (*ids)[idx];

			int sortIdx = ComboBox_GetCurSel(sortCombo);
			vector<Object>* sortFieldsVector = (vector<Object>*)GetWindowLong(sortCombo, GWL_USERDATA);
			Object sortFields = (*sortFieldsVector)[sortIdx];

			string sortby = sortFields["_id"].getString();
			string sortbylabel = sortFields["label"].getString();

			int namelen = GetWindowTextLength(newViewNameEdit) + 1;
			wchar_t* wname = new wchar_t[namelen];
			GetWindowText(newViewNameEdit, wname, namelen);

			string name = ws2s(wname);

			string viewnamesanitized = name;
			for(unsigned i=0; i<viewnamesanitized.size(); i++){
				viewnamesanitized[i] = tolower(viewnamesanitized[i]);
				if ( viewnamesanitized[i] == ' ' ){
					viewnamesanitized[i] = '-';
				}
			}


			ConditionManager* manager = (ConditionManager*)GetWindowLong(hDlg, GWL_USERDATA);
			string conditionsJs = manager->getJavascript();


			sprintf_s(map, 1024, "function(doc){ if ( doc.cinch_type && doc.cinch_type == '%s' %s ) emit(doc.%s, null); }", 
				type.c_str(), conditionsJs.c_str(), sortby.c_str());	


			

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
			v["cinch_view"] = true;
			v["emits_docs_with_type"] = type.c_str();

			view[viewname.str()] = v;
			
			stringstream labelstream;
			labelstream << name;
			design["label"] = labelstream.str();
			design["views"] = view;

			stringstream design_id;
			design_id << "_design/";
			design_id << viewnamesanitized;

			db.createDocument(Value(design), design_id.str());

			//LoadViews(tree);

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




INT_PTR CALLBACK AddDocumentType(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
		HWND combo = GetDlgItem(hDlg, IDC_FIELD_TYPE_COMBO);
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Text");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Date and Time");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Number");

		ComboBox_SetCurSel(combo, 0);

		return (INT_PTR)TRUE;
		}
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
			definition["plural"] = splurallabel;
			definition["first_field_name"] = sfname;
			definition["first_field_label"] = sflabel;
			definition["cinch_type"] = "object-definition";

			
			/* Create a default view */
			char map[1024];
			memset(map, 0, 1024);

			sprintf_s(map, 1024, "function(doc){ if ( doc.cinch_type && doc.cinch_type == '%s' ) emit(doc.%s, null); }", sname.c_str(), sfname.c_str());	

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
			v["cinch_view"] = true;
			v["emits_docs_with_type"] = sname.c_str();

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
			HWND combo = GetDlgItem(hDlg, IDC_FIELD_TYPE_COMBO);
			int idx = ComboBox_GetCurSel(combo);
			switch(idx){
			case 0:
				field["cinch_type"] = EDIT;
				break;
			case 1:
				field["cinch_type"] = DATEPICKER;
				break;
			case 2:
				field["cinch_type"] = NUMBER;
				break;
			}

			fields.push_back(field);
			_template["fields"] = fields;
			_template["cinch_type"] = "template";


			db.createDocument(Value(definition));

			db.createDocument(Value(design), id.str());

			stringstream template_id;
			template_id << "template/";
			template_id << sname;

			db.createDocument(Value(_template), template_id.str());

			//LoadViews(tree);
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
	conn.setTimeout(0);
	Database db = conn.getDatabase(DATABASE);
	db.listenForChanges(changesArrived);

	return 0;
}

void Desktop::formModified(){
	CinchDesigner* d = (CinchDesigner *)GetWindowLong(designer, GWL_USERDATA);
   
	d->SaveForm();
}
