// Designer2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <sstream>

#define MAX_LOADSTRING 100


HWND designerHWnd;

bool savingDoc;

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
TCHAR designerClassName[] = _T("CinchDesigner");
 
void RegisterCinchDesigner()
{
    WNDCLASSEX wc;
	wc.cbSize         = sizeof(wc);
    wc.lpszClassName  = designerClassName;
    wc.hInstance      = GetModuleHandle(0);
	wc.lpfnWndProc    = CinchDesigner::WndProc;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon          = 0;
    wc.lpszMenuName   = 0;
	wc.hbrBackground  = (HBRUSH)CreateSolidBrush(DEFAULT_BACKGROUND_COLOR);
    wc.style          = 0;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hIconSm        = 0;
 
    RegisterClassEx(&wc);
}

HWND CinchDesigner::CreateCinchDesigner(HWND parent){
	RegisterCinchDesigner();

	HWND hWnd = CreateWindowEx(0,
		designerClassName,
		_T("Cinch Designer"),
		WS_VISIBLE | WS_CHILD,
		0, 0, 0, 0,
		parent,
		NULL, GetModuleHandle(0), NULL);

	designerHWnd = hWnd;

	return hWnd;
}

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
/*BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	INITCOMMONCONTROLSEX icex;

    // Ensure that the common control DLL is loaded. 
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_COOL_CLASSES | ICC_DATE_CLASSES | ICC_TAB_CLASSES;

	InitCommonControlsEx(&icex);

  
   hInst = hInstance; // Store instance handle in our global variable
   
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   SetupForm(hWnd, hInst);

   countNewFields = 0;

	return TRUE;
}
*/


/*void SetupForm(HWND hWnd, HINSTANCE hInst)
{
	
	//FormField color = FormField::createRadioGroup(hWnd, hInst, L"Is Occupied");
	//form.addField(color);

	form.show(hWnd, hInst);
}
*/


void loadVisibleTabLabels(CinchDesigner* des, Array tabs, HWND tabList){

	des->visibleTabDefinitions.clear();
	ListBox_ResetContent(tabList);

	for(unsigned i=0; i<tabs.size(); i++){
		Object config = tabs[i].getObject();
		string label = config["label"].getString();
		string field = config["field"].getString();
		wstring l =s2ws(label);
		
		Object doc = db.getDocument(field).getData().getObject();

		int pos = SendMessage(tabList, LB_ADDSTRING, 0, (LPARAM) l.c_str()); 
		
		des->visibleTabDefinitions.push_back(doc);
	}	
}


CinchDesigner::CinchDesigner(HWND _hWnd){
	hWnd = _hWnd;
	form = new Form();
}

CinchDesigner::~CinchDesigner(){
	delete form;
}

Form* CinchDesigner::getForm(){
	return form;
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
LRESULT CALLBACK CinchDesigner::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	CinchDesigner* self = (CinchDesigner *)GetWindowLong(hWnd, GWL_USERDATA);


	switch (message)
	{
	case WM_KEYUP:
		break;
	case WM_KILLFOCUS:
		OutputDebugStringW(TEXT("lost focus\n"));
		break;
	case WM_SETFOCUS:
		OutputDebugStringW(TEXT("set focus\n"));
		break;
	case WM_NCCREATE:
		{
		CREATESTRUCT* c = (CREATESTRUCT*)lParam;
		CinchDesigner *designer = new CinchDesigner(hWnd);
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)designer);
		return TRUE;
		}
	case WM_MOUSEMOVE:
		{
		int mouseYPos = GET_Y_LPARAM(lParam);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			POINT point;
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			HMENU hPopupMenu = CreatePopupMenu();
			InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EDIT_FIELDS, L"Edit Fields");
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EDIT_TABS, L"Edit Tabs");
            ClientToScreen(hWnd, &point);
			TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, hWnd, NULL);
		}
		break;
	case NM_KILLFOCUS:
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
        {
		case DTN_DATETIMECHANGE:
			self->getForm()->SaveDocument(((LPNMHDR)lParam)->idFrom);
			break;
		}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:

		//if ( wmEvent == BN_CLICKED ){
		//	self->getForm()->SaveDocument(wmId);
		//}
		if ( wmEvent == CBN_SELCHANGE || wmEvent == CBN_KILLFOCUS ){
			HWND w = GetDlgItem(hWnd, wmId);
			wchar_t name[80];
			GetClassName(w, name, 80);
			if ( wcscmp(name, TEXT("ComboBox")) == 0 ){
				self->getForm()->SaveDocument(wmId);
			}
		}

		if ( wmEvent == EN_KILLFOCUS || wmEvent == NM_KILLFOCUS ){
				self->getForm()->SaveDocument(wmId);
		} else {
			switch (wmId)
			{
			case IDD_EDIT_TABS:
				DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EDIT_TABS), hWnd, EditTabs);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case IDD_EDIT_FIELDS:
				DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EDIT_FIELDS), hWnd, EditFields);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		{
		CinchDesigner* designer = (CinchDesigner*)GetWindowLong(hWnd, GWL_USERDATA);
		delete designer;
		PostQuitMessage(0);
		}
		break;
	case WM_SIZE:
		self->getForm()->show(hWnd, GetModuleHandle(0));
		break;
	case WM_CTLCOLORSTATIC:
        {
        HDC hdcStatic = (HDC) wParam;
        SetBkColor(hdcStatic, RGB(0,0,0));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
        }
	case WM_GETMINMAXINFO:
		{
		MINMAXINFO * info = (MINMAXINFO *)lParam;
		info->ptMinTrackSize.x = self->getForm()->minWidth();
		info->ptMinTrackSize.y = self->getForm()->minHeight();
		return 0;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CinchDesigner::MoveField(HWND parent)
{
	RECT window;

	GetWindowRect(parent, &window);

	MoveWindow(edit, window.right-300, window.bottom-300, 200, 200, TRUE);

}



INT_PTR CALLBACK EditFields(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CinchDesigner* self = (CinchDesigner *)GetWindowLong(designerHWnd, GWL_USERDATA);
	HWND hWnd = designerHWnd;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{	
			HWND hiddenFields = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);
			
			ListBox_ResetContent(hiddenFields);
			ListBox_ResetContent(visibleFields);

			self->formWithUpdates = self->loadedForm;
			
			QueryOptions options;
			options.startKey = Value(self->getType());
			options.endKey = Value(self->getType());
			options.includeDocs = true;
			Object results = db.viewResults("all-templates", "by-target-type", options);
			
			int count = self->getForm()->getLayout()->getFieldCount();
			
			Array fields;

			if ( results["rows"].isArray() ){
				Array rows = results["rows"].getArray();
				if ( rows.size() > 0 ){
					Object templ = rows[0]["doc"].getObject();
					fields = templ["fields"].getArray();

					for(unsigned int i=0; i<fields.size(); i++){
						string id = fields[i].getString();

						Document d = db.getDocument(id);
						Object doc = d.getData().getObject();
					
					
						string label = doc["label"].getString();
						string name = id;

						int len = label.size() + sizeof(wchar_t);
						wchar_t* t = new wchar_t[len];

						wcscpy_s(t, len, s2ws(label).c_str());

						bool found = false;

						for(int i=0; i<count; i++){
							FormField* field = self->getForm()->getLayout()->getField(i);
							if ( field->getName().compare(name) == 0 ){
								int index = ListBox_AddString(visibleFields, t);
								ListBox_SetItemData(visibleFields, index, new Object(doc));
								found = true;
							}
						
						}
					}
				} else {
					fields = Array();
				}

				vector<string> attrs = collectAttributes(self->getType());

				for(unsigned int i=0; i<attrs.size(); i++){
					string name = attrs[i];
					bool found = false;
					for(unsigned int j=0; j<fields.size(); j++){
						if ( name.compare(fields[j].getString()) == 0 ){
							found = true;
						}
					}
					if ( !found ){
						string baseType = getBaseType(name);
						if ( baseType.compare("array") != 0  && baseType.compare("attachments") != 0 ){	
							Object doc = db.getDocument(name).getData().getObject();
							string label = doc["label"].getString();

							int len = label.size() + sizeof(wchar_t);
							wchar_t* t = new wchar_t[len];

							wcscpy_s(t, len, s2ws(label).c_str());

							int index = ListBox_AddString(hiddenFields, t);
							ListBox_SetItemData(hiddenFields, index, new Object(doc));
						}
					}
				}
			
				//SetWindowLongPtr(hiddenFields, GWL_USERDATA, (ULONG_PTR)new Array(rows));
			}

			

		
			return (INT_PTR)TRUE;
		}
	case WM_COMMAND:
		if ( LOWORD(wParam) == IDCANCEL ){
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{
			Array fields = Array();
			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);
			int count = ListBox_GetCount(visibleFields);
			for(int i=0; i<count; i++){
				ULONG_PTR data = ListBox_GetItemData(visibleFields, i);
				Object* doc = (Object*)data;

				int a =1;
				fields.push_back((*doc)["_id"].getString());
			}

			self->formWithUpdates["fields"] = fields;
			self->loadedForm = self->formWithUpdates;
			self->getForm()->deserializeForm(designerHWnd, Value(self->loadedForm));
			self->getForm()->ReloadDocument();
			//self->getForm()->show(hWnd, GetModuleHandle(0));
			//if( self->getForm()->getDelegate() != 0 ){

			//	
			//}
			self->SaveForm();
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if ( LOWORD(wParam) == IDC_ADD_TO_VISIBLE_FIELDS )
		{
			HWND hiddenFields = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

			int selected = ListBox_GetCurSel(hiddenFields);
			if( selected != LB_ERR ){
				wchar_t text[80];
				ListBox_GetText(hiddenFields, selected, text);
				ULONG_PTR data = ListBox_GetItemData(hiddenFields, selected);

				int pos = ListBox_AddString(visibleFields, text);
				ListBox_SetItemData(visibleFields, pos, data);
				
				ListBox_DeleteString(hiddenFields, selected);

			}
		} 
		else if ( LOWORD(wParam) == IDC_REMOVE_FROM_VISIBLE_FIELDS )
		{
			HWND hiddenFields = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visibleFields);
			if( selected != LB_ERR ){
				wchar_t text[80];
				ListBox_GetText(visibleFields, selected, text);
				ULONG_PTR data = ListBox_GetItemData(visibleFields, selected);

				int pos = ListBox_AddString(hiddenFields, text);
				ListBox_SetItemData(hiddenFields, pos, data);
				
				ListBox_DeleteString(visibleFields, selected);


			}
		}
		else if ( LOWORD(wParam) == IDC_FIELD_UP )
		{
			HWND hidden = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visible = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visible);
			if( selected != LB_ERR && selected > 0 ){
				wchar_t* text = new wchar_t[ListBox_GetTextLen(visible, selected)];
				ListBox_GetText(visible, selected, text);
				ULONG_PTR data = ListBox_GetItemData(visible, selected);

				ListBox_DeleteString(visible, selected);
				ListBox_InsertString(visible, selected-1, text);
				ListBox_SetItemData(visible, selected-1, data);
				//ListBox_InsertItemData(visible, selected-1, data);
				ListBox_SetCurSel(visible, selected-1);

			}
		}
		else if ( LOWORD(wParam) == IDC_FIELD_DOWN )
		{
			HWND hidden = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visible = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visible);
			int count = ListBox_GetCount(visible);
			if( selected != LB_ERR && selected < count -1 ){
				wchar_t* text = new wchar_t[ListBox_GetTextLen(visible, selected)];
				ListBox_GetText(visible, selected, text);
				ULONG_PTR data = ListBox_GetItemData(visible, selected);

				ListBox_DeleteString(visible, selected);
				ListBox_InsertString(visible, selected+1, text);
				ListBox_SetItemData(visible, selected+1, data);
				ListBox_SetCurSel(visible, selected+1);

				
			}

		} else if (LOWORD(wParam) == IDC_NEW_FIELD ){
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ADD_FIELD), hDlg, AddField);
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//void LoadViews(HWND hwnd, string emitsDocumentsOfType = "");

INT_PTR CALLBACK AddField(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmEvent;
	int wmId;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{

		vector<string>* v = new vector<string>();
		HWND combo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Short Text");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Medium Text");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Multi-line Text");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Yes/No");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Choose from a few values");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Choose from many values");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Date and Time");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Number");

		Database d = conn.getDatabase(DATABASE);
		Object obj = d.viewResults("all-document-types", "by-label");
		if ( obj["rows"].isArray() ){
			Array results = obj["rows"].getArray();
			for(unsigned int i=0; i<results.size(); i++){
				Object row = results[i].getObject();
				string key = row["key"].getString();
				wstring wkey =s2ws(key);
				Document doc = d.getDocument(row["id"].getString());
				Object obj = doc.getData().getObject();
				if ( obj["name"].isString() ){
					v->push_back(obj["name"].getString());
				} else {
					v->push_back("");
				}
				SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)wkey.c_str());
			}
		}

		SetWindowLong(combo, GWL_USERDATA, (ULONG_PTR)v);

		Edit_SetCueBannerText(GetDlgItem(hDlg, IDC_NEW_FIELD_NEW_OPTION), L"Add A New Option");
	
		ComboBox_SelectString(combo, 0, L"Medium Text");
		return (INT_PTR)TRUE;
		}
	case WM_COMMAND:
		wmEvent = HIWORD(wParam);
		wmId = LOWORD(wParam);
		if ( wmEvent == CBN_SELCHANGE ){
			HWND typeCombo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
			int type = SendMessage(typeCombo, CB_GETCURSEL, 0, 0);
			if ( type == 5 ){
				ShowWindow(GetDlgItem(hDlg, IDC_NEW_FIELD_OPTIONS), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_NEW_FIELD_NEW_OPTION), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_ADD_FIELD_CHOOSE_FROM_LABEL), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_NEW_FIELD_ADD_OPTION), SW_SHOW);
			} else {
				ShowWindow(GetDlgItem(hDlg, IDC_NEW_FIELD_OPTIONS), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_NEW_FIELD_NEW_OPTION), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_ADD_FIELD_CHOOSE_FROM_LABEL), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_NEW_FIELD_ADD_OPTION), SW_HIDE);
				
			}

			if ( type > 7 ){
				ShowWindow(GetDlgItem(hDlg, IDC_ADD_FIELD_VIEW_TREE), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_ADD_FIELD_CHOOSE_FROM_LABEL), SW_SHOW);
				
				HWND combo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
				vector<string>* v = (vector<string>*)GetWindowLong(combo, GWL_USERDATA);
				
				//LoadViews(GetDlgItem(hDlg, IDC_ADD_FIELD_VIEW_TREE), (*v)[type-8]);

			} else {
				ShowWindow(GetDlgItem(hDlg, IDC_ADD_FIELD_VIEW_TREE), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_ADD_FIELD_CHOOSE_FROM_LABEL), SW_HIDE);
			}
		}
		if ( wmId == IDC_NEW_FIELD_ADD_OPTION ){
			int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NEW_FIELD_NEW_OPTION)) + 1;
			wchar_t* option = new wchar_t[len];
			GetWindowText(GetDlgItem(hDlg, IDC_NEW_FIELD_NEW_OPTION), option, len);

			int count = ListBox_GetCount(GetDlgItem(hDlg, IDC_NEW_FIELD_OPTIONS));

			ListBox_InsertString(GetDlgItem(hDlg, IDC_NEW_FIELD_OPTIONS), count, option);
			
			SetWindowText(GetDlgItem(hDlg, IDC_NEW_FIELD_NEW_OPTION), L"");
		}
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NEW_FIELD_NAME));
			if ( len > 0 ){
				wchar_t* szNewFieldName = new wchar_t[80];
				wchar_t* szNewFieldLabel = new wchar_t[80];

				GetDlgItemText(hDlg, IDC_NEW_FIELD_NAME, szNewFieldName, 80);
				GetDlgItemText(hDlg, IDC_NEW_FIELD_LABEL, szNewFieldLabel, 80);
				HWND parent = GetParent(hDlg);
				HWND visibleFieldsList = GetDlgItem(parent, IDC_VISIBLE_FIELDS);
				int count = ListBox_GetCount(visibleFieldsList);

				ListBox_InsertString(visibleFieldsList, count, szNewFieldLabel);

				HWND typeCombo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
				int selected = SendMessage(GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE), CB_GETCURSEL, 0, 0);

				CinchDesigner* self = (CinchDesigner *)GetWindowLong(designerHWnd, GWL_USERDATA);
				int type = SendMessage(typeCombo, CB_GETCURSEL, 0, 0);
				FormField* field;
				switch(type){
				case 0:
					field = FormField::createEditField(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName), szNewFieldLabel);
					break;
				case 1:
					field = FormField::createEditField(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName),szNewFieldLabel);
					break;
				case 2:
					field = FormField::createMultilineText(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName),szNewFieldLabel);
					break;
				case 3:
					field = FormField::createYesNoField(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName),szNewFieldLabel);
					break;
				case 4:
					field = FormField::createRadioGroup(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName),szNewFieldLabel);
					break;
				case 5:
					{
					HWND options = GetDlgItem(hDlg, IDC_NEW_FIELD_OPTIONS);
					int count = ListBox_GetCount(options);
					Object config;
					Array pickFromList;
					for(int i=0; i<count; i++){
						int len = ListBox_GetTextLen(options, i) + 1;
						wchar_t* text = new wchar_t[len];

						ListBox_GetText(options, i, text);
						pickFromList.push_back(Value(ws2s(text)));

						free(text);

					}
					config["pick_from_list"] = pickFromList;

					field = FormField::createComboBox(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName), szNewFieldLabel, ws2s(szNewFieldName));
					}
					break;
				case 6:
					field = FormField::createDatePicker(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName), szNewFieldLabel);
					break;
				case 7:
					field = FormField::createNumberField(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName), szNewFieldLabel);
					break;
				default:
					{
						HWND tree = GetDlgItem(hDlg, IDC_ADD_FIELD_VIEW_TREE);

						HTREEITEM item = TreeView_GetSelection(tree);



						TVITEM tvitem;

						tvitem.hItem = item;
						tvitem.mask = TVIF_PARAM;
						TreeView_GetItem(tree, &tvitem);

						if ( tvitem.lParam != NULL ){
							ViewPair * v = (ViewPair *)tvitem.lParam;
							Object config;
							Object pickFrom;
							string design = v->design;
							string d = design.substr(design.find("/")+1);
							pickFrom["design"] = d;
							pickFrom["view"] = v->view;
							config["pick_from"] = pickFrom;

							field = FormField::createReferenceField(designerHWnd, GetModuleHandle(0), ws2s(szNewFieldName), szNewFieldLabel, Value(config));
					
							/* Create a relationship */
							Object rel = Object();
							rel["cinch_type"] = "relationship";
							
							CinchDesigner* self = (CinchDesigner *)GetWindowLong(designerHWnd, GWL_USERDATA);
							rel["source_document_type"] = self->getType();

							rel["destination_document_type"] = v->emitsDocsWithType;

							string name = ws2s(szNewFieldName);
							string label = ws2s(szNewFieldLabel);

							rel["name"] = name;
							rel["label"] = label;
							rel["source_document_property"] = name;

							;
							
							db.createDocument(Value(rel));
						}
						
					}
					break;
				}
				self->getForm()->addField(field);
			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}




INT_PTR CALLBACK AddTab(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	CinchDesigner* self = (CinchDesigner *)GetWindowLong(designerHWnd, GWL_USERDATA);
	
	int wmEvent;
	int wmId;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HWND combo = GetDlgItem(hDlg, IDC_CONTENT_COMBO);
			SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Text");
			SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Table");
			SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Related Documents");

		}
	case WM_COMMAND:
		wmEvent = HIWORD(wParam);
		wmId = LOWORD(wParam);

		switch(wmId){
		case IDC_CONTENT_COMBO:
			{
				if ( wmEvent == CBN_SELCHANGE ){
					int idx = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CONTENT_COMBO));
					if ( idx == 2 ){
						HWND relcombo = GetDlgItem(hDlg, IDC_RELATIONSHIP_COMBO);
						ShowWindow(relcombo, SW_SHOW);
						ShowWindow(GetDlgItem(hDlg, IDC_RELATIONSHIP_LABEL), SW_SHOW);
						ShowWindow(GetDlgItem(hDlg, IDC_CONDITIONS_LABEL), SW_SHOW);
						ComboBox_ResetContent(relcombo);
						
						QueryOptions options;
						options.startKey = Value(self->getType());
						options.endKey = Value(self->getType());
						Object results = db.viewResults("all-relationships", "by-destination-document-type", options);
				
						vector<string>* ids = new vector<string>();

						if( results["rows"].isArray() ){
							Array rows = results["rows"].getArray();

							for(unsigned i=0; i<rows.size(); i++){
								Object o = rows[i].getObject();
								string id = o["id"].getString();
							
								Document doc = db.getDocument(id);
								Object obj = doc.getData().getObject();
								//string label = obj["label"].getString();
								string source_document_type = obj["source_document_type"].getString();

								stringstream label;
								QueryOptions options;
								options.startKey = Value(source_document_type);
								options.endKey = Value(source_document_type);
								Object objectResults = db.viewResults("all-document-types", "by-name", options);
								if ( objectResults["rows"].isArray() ){
									Array rows = objectResults["rows"].getArray();
									if ( rows.size() > 0 ){
										Object o = rows[0].getObject();
										string id = o["id"].getString();
										Document doc = db.getDocument(id);
										Object data = doc.getData().getObject();
										label << data["plural"].getString();
										label << ": ";

									}
								}

								label << obj["label"].getString();

								wstring w = s2ws(label.str());
								SendMessage(relcombo, CB_ADDSTRING, 0, (LPARAM)w.c_str());
								ids->push_back(id);

							}
						}

						SetWindowLong(relcombo, GWL_USERDATA, (ULONG_PTR)ids);
					}
				}
			}
			break;
		case IDC_ADD_VIEW_SORT:
		case IDC_ADD_VIEW_FIELD:
			self->conditionManager->updateConditions(hDlg);
			self->conditionManager->arrangeWindowsInParent(hDlg, 135, 220);
			break;
		case IDC_RELATIONSHIP_COMBO:
			if ( wmEvent == CBN_SELCHANGE ){
				int idx = ComboBox_GetCurSel( GetDlgItem(hDlg, IDC_RELATIONSHIP_COMBO));
				vector<string>* ids = (vector<string>*)GetWindowLong(GetDlgItem(hDlg, IDC_RELATIONSHIP_COMBO), GWL_USERDATA);
				if ( idx >= 0 ){
					;
					
					Document d = db.getDocument((*ids)[idx]);
					Object obj = d.getData().getObject();
					string type = obj["source_document_type"].getString();
					self->conditionManager = new ConditionManager();
					//self->conditionManager->updateConditions("bug", hDlg);
					self->conditionManager->addEmptyCondition(type, hDlg);
					self->conditionManager->arrangeWindowsInParent(hDlg, 135, 220);
				}
				

			}
			break;

		case IDOK:
			{

			int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NEW_TAB_NAME))+1;
			wchar_t* tabname = new wchar_t[len];
			memset(tabname, 0, len);
			GetWindowText(GetDlgItem(hDlg, IDC_NEW_TAB_NAME), tabname, len);

			len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NEW_TAB_LABEL))+1;
			wchar_t* tablabel = new wchar_t[len];
			memset(tablabel, 0, len);
			GetWindowText(GetDlgItem(hDlg, IDC_NEW_TAB_LABEL), tablabel, len);

			Object newTab = Object();
			newTab["name"] = ws2s(tabname);
			newTab["label"] = ws2s(tablabel);

			HWND contentCombo = GetDlgItem(hDlg, IDC_CONTENT_COMBO);
			int idx = ComboBox_GetCurSel(contentCombo);
			if ( idx == 0 ){
				newTab["content"] = "Text";
			} else if ( idx == 1  ){
				newTab["content"] = "Table";
			} else if ( idx == 2 ){

				HWND relcombo = GetDlgItem(hDlg, IDC_RELATIONSHIP_COMBO);
				vector<string>* ids = (vector<string>*)GetWindowLong(relcombo, GWL_USERDATA);
				int relidx = ComboBox_GetCurSel(relcombo);

				string id = (*ids)[relidx];
				Document doc = db.getDocument(id);
				Object obj = doc.getData().getObject();


				/* Create a default view */
				char map[1024];
				memset(map, 0, 1024);

				string conditions = self->conditionManager->getJavascript();

				sprintf_s(map, 1024, 
					"function(doc){ if ( doc.cinch_type && doc.cinch_type == '%s' %s ) emit(doc.%s, null); }", 
					obj["source_document_type"].getString().c_str(),
					conditions.c_str(),
					obj["source_document_property"].getString().c_str());	

				Object design = Object();
				design["language"] = "javascript";
				Object view = Object();
				
					
				Object v = Object();
				v["map"] = map;

				string vname = CreateUUID();
				view[vname] = v;
			
				stringstream labelstream;
				design["views"] = view;

				stringstream design_id;
				design_id << "_design/";
				
				string design_uuid = CreateUUID();
				design_id << design_uuid;
				
				string design_id_str = design_id.str();
				
				db.createDocument(Value(design), design_id_str);
				
				newTab["content"] = "View";
				Object config = Object();
				config["design"] = design_uuid;
				config["view"] = vname;
				config["startkey_with_value_of"] = "_id";
                config["endkey_with_value_of"] = "_id";
				config["shows_docs_of_type"] = obj["source_document_type"].getString();
				newTab["config"] = config;
			}


			self->tabsForUpdate.push_back(newTab);

			HWND visibleTabs = GetDlgItem(GetParent(hDlg), IDC_VISIBLE_TABS);
			
			loadVisibleTabLabels(self, self->tabsForUpdate, visibleTabs);

			EndDialog(hDlg, LOWORD(wParam));
			break;
			}
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		break;
	default:
		break;
	}
	return (INT_PTR)FALSE;
}


void CinchDesigner::ChooseForm()
{
	
}

void CinchDesigner::SaveForm(){
	Object o = getForm()->serializeFormToObject(getLoadedForm());
	
	o["target_type"] = getType();
	Document newDoc = db.createDocument(Value(o));
	setLoadedForm(newDoc.getData().getObject());
}



INT_PTR CALLBACK EditTabs(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CinchDesigner* self = (CinchDesigner *)GetWindowLong(designerHWnd, GWL_USERDATA);
	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
		
			Object form = self->getLoadedForm();
			if ( form["tabs"].isArray() ){
				self->tabsForUpdate = form["tabs"].getArray();
			} else {
				self->tabsForUpdate = Array();
			}
		
			HWND visibleTabs = GetDlgItem(hDlg, IDC_VISIBLE_TABS);
			HWND hiddenTabs = GetDlgItem(hDlg, IDC_HIDDEN_TABS);
			
			loadVisibleTabLabels(self, self->tabsForUpdate, visibleTabs);

			QueryOptions options;
			options.startKey = Value(self->getType());
			options.endKey = Value(self->getType());
			options.includeDocs = true;
			Object results = db.viewResults("all-templates", "by-target-type", options);
			
			vector<string> attributes = collectAttributes(self->getType());

			int count = self->getForm()->getLayout()->getFieldCount();
					
			if ( results["rows"].isArray() ){
				Array rows = results["rows"].getArray();
				Object templ = rows[0]["doc"].getObject();
				Array tabs = templ["tabs"].getArray();

				vector<string> attributes = collectAttributes(self->getType());

				for(unsigned int i=0; i<attributes.size(); i++){
					string attribute = attributes[i];
					Object doc = db.getDocument(attribute).getData().getObject();
					string name = doc["_id"].getString();
					string label = doc["label"].getString();
					bool found = false;
					for(unsigned i=0; i<tabs.size(); i++){
						Object config = tabs[i].getObject();
						if ( name.compare(config["field"].getString()) == 0 ){
							found = true;
						}
					}
					if ( !found ){
						string baseType = getBaseType(name);
						if ( baseType.compare("array") == 0 || baseType.compare("attachments") == 0 ){
							
							int index = ListBox_AddString(hiddenTabs, s2ws(label).c_str());
							self->hiddenTabDefinitions.push_back(doc);

						}
					}
				}
			
				/* Now add views for documents which may reference this document */
				QueryOptions attributesQueryOptions;
				attributesQueryOptions.startKey = self->getType();
				attributesQueryOptions.endKey = self->getType();
				results = db.viewResults("all-attributes", "by-id", attributesQueryOptions);
				if ( results["rows"].isArray() ){
					Array rows = results["rows"].getArray();
			
					for(unsigned int i=0; i<rows.size(); i++){
						Object row = rows[i].getObject();
						Object value = row["value"].getObject();
						string id = value["_id"].getString();

						QueryOptions viewQueryOptions;
						viewQueryOptions.startKey = id;
						viewQueryOptions.endKey = id;
						viewQueryOptions.includeDocs = true;
						Object views = db.viewResults("all-view-definitions", "by-emitted-type", viewQueryOptions);
						if ( views["rows"].isArray() ){
							Array viewRows = views["rows"].getArray();

							for(unsigned int i=0; i<viewRows.size(); i++){
								Object view = viewRows[i].getObject();
								Object doc = view["doc"].getObject();
								string label = doc["label"].getString();
								string id = doc["_id"].getString();
								bool found = false;
								for(unsigned i=0; i<tabs.size(); i++){
									Object config = tabs[i].getObject();
									if ( id.compare(config["_id"].getString()) == 0 ){
										found = true;
									}
								}
								if ( !found ){
									int index = ListBox_AddString(hiddenTabs, s2ws(label).c_str());
									self->hiddenTabDefinitions.push_back(doc);	
								}
							}
						}

					}
				}
			}

			/*
			results = db.viewResults("all-view-definitions-emitting-ids", "by-emitted-type", Value(self->getType()), Value(self->getType()));
			if ( results["rows"].isArray() ){
				Array rows = results["rows"].getArray();
				for(unsigned int i=0; i<rows.size(); i++){
					Object row = rows[i].getObject();
					Object doc = row["value"].getObject();
					string label = doc["label"].getString();
					int len = label.size() + sizeof(wchar_t);
					wchar_t* t = new wchar_t[len];
					wcscpy_s(t, len, s2ws(label).c_str());

					int index = ListBox_AddString(hiddenTabs, t);
					ListBox_SetItemData(hiddenTabs, index, new Object(doc));
				}
			}

			*/

			
		return (INT_PTR)TRUE;
		}
	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK ){

			Object form = self->getLoadedForm();
			
			Array tabs = Array();
			HWND visibleTabs = GetDlgItem(hDlg, IDC_VISIBLE_TABS);
			int count = ListBox_GetCount(visibleTabs);
			for(int i=0; i<count; i++){
				Object obj = self->visibleTabDefinitions[i];
				if( obj["type"].getString().compare("array") == 0 ){
					string array_content_type = obj["array_contents"].getString();
					string baseType = getBaseType(array_content_type);

					if ( baseType.compare(DOCUMENT) == 0 ){
						Object tab;
						Object config;
						Array columns;
						Object column;
						column["field"] = array_content_type;
						column["label"] = "Label";
						columns.push_back(column);
						config["columns"] = columns;
						tab["config"] = config;
						tab["content"] = TABLE;
						tab["label"] = obj["label"];
						tabs.push_back(tab);
					} else {
						QueryOptions options;
						options.startKey = Value(array_content_type);
						options.endKey = Value(array_content_type);
						options.includeDocs = true;
						Object results = db.viewResults("all-attributes", "by-type", options);
						if ( results["rows"].isArray() ){
							Array rows = results["rows"].getArray();
							Array columns;
							for(unsigned int i=0; i<rows.size(); i++){
								Object row = rows[i].getObject();
								Object doc = row["doc"].getObject();
								int a = 1;
								Object column;
								column["field"] = doc["_id"];
								column["width"] = Value(250);
								columns.push_back(column);
							}

							Object config;
							config["columns"] = columns;
							Object tab;
							tab["config"] = config;
							tab["content"] = TABLE;
							tab["label"] = obj["label"];
							tab["field"] = obj["_id"];

							tabs.push_back(tab);
						}
					}
				} else if ( obj["type"].getString().compare("attachments") == 0 ){
					Object tab;
					tab["content"] = ATTACHMENTS_DETAIL;
					tab["label"] = "Attachments";
					tab["field"] = "attachments";
					tabs.push_back(tab);
				} else if ( obj["cinch_type"].getString().compare("view_definition") == 0 ){
					Object tab;
					Object config;
					Array columns;
					QueryOptions options;
					options.startKey = obj["emits"];
					options.endKey = obj["emits"];
					options.includeDocs = true;
					Object results = db.viewResults("all-attributes", "by-type", options);
					if ( results["rows"].isArray() ){
						Array rows = results["rows"].getArray();
						for(unsigned int i=0; i<rows.size(); i++){
							Object row = rows[i].getObject();
							Object doc = row["doc"].getObject();
							int a = 1;
							Object column;
							column["field"] = doc["_id"];
							column["width"] = Value(50);
							columns.push_back(column);
						}
					}

					/* Check if one of the already available views uses _id as a key */
					bool found = false;
					Array availableViews = obj["views"].getArray();
					for(unsigned int i=0; i<availableViews.size(); i++){
						Object view = availableViews[i].getObject();
						if ( view["key"].getString().compare(self->getType()) == 0 ){
							/* This one will work */
							found = true;
							config["design"] = obj["design_name"];
							config["view"] = view["view"];
						}
					}

					if ( !found ){
						/* We will need to create one */
						stringstream design;
						design << "_design/" << obj["design_name"].getString();
						string key = design.str();
						Object des = db.getDocument(design.str()).getData().getObject();

						Object views = des["views"].getObject();
							
						char map[1024];
						sprintf_s(map, 1024, "function(doc){ if ( doc.cinch_type && doc.cinch_type == '%s' ) emit(doc.%s, null); }", 
							obj["emits"].getString().c_str(), self->getType().c_str());

						stringstream viewname;
						viewname << "by-" << self->getType();

						views[viewname.str()]["map"] = map;

						des["views"] = views;

						
						config["design"] = obj["design_name"];
						config["view"] = viewname.str();

						Object newView;
						newView["key"] = "_id";
						newView["view"] = viewname.str();
						availableViews.push_back(newView);
						obj["views"] = availableViews;
						
						db.createDocument(obj);
						db.createDocument(des, design.str());
					}

					
					config["columns"] = columns;
					tab["content"] = VIEW_WITH_DOCUMENTS_DETAIL;
					tab["label"] = obj["label"];
					tab["view_id"] = obj["_id"];
					tab["config"] = config;

					tabs.push_back(tab);

				}
			}

			
			form["tabs"] = tabs;
			self->setLoadedForm(form);

			self->getForm()->deserializeForm(designerHWnd, Value(self->getLoadedForm()));
			self->getForm()->RefreshValues();
			self->SaveForm();

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		} 
		else if ( LOWORD(wParam) == IDC_ADD_TO_VISIBLE_TABS )
		{
			HWND hiddenTabs = GetDlgItem(hDlg, IDC_HIDDEN_TABS);
			HWND visibleTabs = GetDlgItem(hDlg, IDC_VISIBLE_TABS);

		
			int selected = ListBox_GetCurSel(hiddenTabs);
			if( selected != LB_ERR ){
				wchar_t text[80];
				ListBox_GetText(hiddenTabs, selected, text);
				
				int pos = (int)SendMessage(visibleTabs, LB_ADDSTRING, 0, (LPARAM)text); 

				self->visibleTabDefinitions.push_back(self->hiddenTabDefinitions[selected]);
				self->hiddenTabDefinitions.erase(self->hiddenTabDefinitions.begin()+selected);
				
				ListBox_DeleteString(hiddenTabs, selected);

				


			}
		} 
		else if ( LOWORD(wParam) == IDC_REMOVE_FROM_VISIBLE_TABS )
		{
			HWND hiddenTabs = GetDlgItem(hDlg, IDC_HIDDEN_TABS);
			HWND visibleTabs = GetDlgItem(hDlg, IDC_VISIBLE_TABS);

		
			int selected = ListBox_GetCurSel(visibleTabs);
			if( selected != LB_ERR ){
				wchar_t text[80];
				ListBox_GetText(visibleTabs, selected, text);
				
				int pos = (int)SendMessage(hiddenTabs, LB_ADDSTRING, 0, (LPARAM)text); 
				
				self->hiddenTabDefinitions.push_back(self->visibleTabDefinitions[selected]);
				self->visibleTabDefinitions.erase(self->visibleTabDefinitions.begin()+selected);
				
				ListBox_DeleteString(visibleTabs, selected);

			}
		}
		else if ( LOWORD(wParam) == IDC_VISIBLE_UP )
		{
			HWND hiddenTabs = GetDlgItem(hDlg, IDC_HIDDEN_TABS);
			HWND visibleTabs = GetDlgItem(hDlg, IDC_VISIBLE_TABS);

		
			int selected = ListBox_GetCurSel(visibleTabs);
			if( selected != LB_ERR && selected > 0 ){
				wchar_t text[80];
				ListBox_GetText(visibleTabs, selected, text);
				ListBox_DeleteString(visibleTabs, selected);
				ListBox_InsertString(visibleTabs, selected-1, text);

				swap(self->visibleTabDefinitions[selected], self->visibleTabDefinitions[selected-1]);

				ListBox_SetCurSel(visibleTabs, selected-1);
			}

			
		}
		else if ( LOWORD(wParam) == IDC_VISIBLE_DOWN )
		{
			HWND hiddenTabs = GetDlgItem(hDlg, IDC_HIDDEN_TABS);
			HWND visibleTabs = GetDlgItem(hDlg, IDC_VISIBLE_TABS);

		
			int selected = ListBox_GetCurSel(visibleTabs);
			int count = ListBox_GetCount(visibleTabs);
			if( selected != LB_ERR && selected < count -1 ){
				wchar_t text[80];
				ListBox_GetText(visibleTabs, selected, text);
				ListBox_DeleteString(visibleTabs, selected);
				ListBox_InsertString(visibleTabs, selected+1, text);

				swap(self->visibleTabDefinitions[selected], self->visibleTabDefinitions[selected+1]);

				ListBox_SetCurSel(visibleTabs, selected+1);
			}
		}
		else if ( LOWORD(wParam) == IDC_NEW_TAB )
		{
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ADD_TAB), hDlg, AddTab);
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void CinchDesigner::loadForm(string database, string t){
	if ( type.compare(t) == 0 ){
		/* skip loading the form, since we already have it */
		/* TODO: Check here if the form has been changed */
	} else {
		type = t;
		
		QueryOptions options;
		options.startKey = Value(type);
		options.endKey = Value(type);
		options.includeDocs = true;
		Object results = db.viewResults("all-templates", "by-target-type", options);
		Array rows = results["rows"].getArray();
		if ( rows.size() > 0 ){
			Object row = rows[0].getObject();
			Object doc = row["doc"].getObject();
			loadedForm = doc;
			form->deserializeForm(hWnd, doc);

		} else {
			loadedForm = Object();
			form->deserializeForm(hWnd, Value());
		}
	}
}

void CinchDesigner::NewDocument(string database, string type){
	loadForm(database, type);
	Object obj = Object();
	obj["cinch_type"] = Value(type);
	form->LoadDocument("", obj, false);
}

void CinchDesigner::LoadDocument(string database, string _id, Document doc, Object obj){
	/* First load the template for this doc */
	if ( obj["cinch_type"].isString() ){
		string t = obj["cinch_type"].getString();

		loadForm(database, t);
    }

	form->LoadDocument(_id, obj, false);
}

Object CinchDesigner::getLoadedForm(){
	return loadedForm;
}

void CinchDesigner::setLoadedForm(Object obj){
	loadedForm = obj;
}

string CinchDesigner::getType(){
	return type;
}
