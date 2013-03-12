// Designer2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"


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
	wc.hbrBackground  = (HBRUSH)CreateSolidBrush(RGB(240,240,240));
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
		WS_VISIBLE | WS_CHILD | WS_BORDER,
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


void loadTabLabels(Array tabs, HWND tabList){

	for(unsigned i=0; i<tabs.size(); i++){
		Object config = tabs[i].getObject();
		string label = config["label"].getString();
		wstring l =s2ws(label);

		SendMessage(tabList, LB_ADDSTRING, 0, (LPARAM) l.c_str()); 
	
	}	
}


CinchDesigner::CinchDesigner(HWND _hWnd){
	hWnd = _hWnd;
	form = new Form();
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

	CinchDesigner* _this = (CinchDesigner *)GetWindowLong(hWnd, GWL_USERDATA);


	switch (message)
	{
	case WM_KEYUP:
		OutputDebugStringW(TEXT("Key pressed"));
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
		CinchDesigner * designer = new CinchDesigner(hWnd);
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
			_this->getForm()->SaveDocument(((LPNMHDR)lParam)->idFrom);
			break;
		}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:

		if ( wmEvent == BN_CLICKED ){
			_this->getForm()->SaveDocument(wmId);
		}
		if ( wmEvent == EN_KILLFOCUS || wmEvent == NM_KILLFOCUS || wmEvent == CBN_SELCHANGE ){
			_this->getForm()->SaveDocument(wmId);
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
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		_this->getForm()->show(hWnd, GetModuleHandle(0));
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
		info->ptMinTrackSize.x = _this->getForm()->minWidth();
		info->ptMinTrackSize.y = _this->getForm()->minHeight();
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
	
	CinchDesigner* _this = (CinchDesigner *)GetWindowLong(designerHWnd, GWL_USERDATA);
	HWND hWnd = designerHWnd;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{

			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);
			int count = _this->getForm()->getLayout()->getFieldCount();
			for(int i=0; i<count; i++){
				FormField* field = _this->getForm()->getLayout()->getField(i);
				HWND label = field->getLabel();
				wchar_t* title = new wchar_t[80];
				memset(title, 0, 80);
				GetWindowText(label, title, 80);
				SendMessage(visibleFields, LB_ADDSTRING, 0, (LPARAM) title); 
		
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
			
			_this->getForm()->show(hWnd, GetModuleHandle(0));
			if( _this->getForm()->getDelegate() != 0 ){

				_this->SaveForm();
			}
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
				
				int pos = (int)SendMessage(visibleFields, LB_ADDSTRING, 0, (LPARAM)text); 
				_this->getForm()->addField(FormField::createEditField(hWnd, GetModuleHandle(0), text, text));
				ListBox_DeleteString(hiddenFields, selected);

			}
		} 
		else if ( LOWORD(wParam) == IDC_REMOVE_FROM_VISIBLE_FIELDS )
		{
			HWND hidden = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visible = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visible);
			if( selected != LB_ERR ){
				wchar_t text[80];
				ListBox_GetText(visible, selected, text);
				
				int pos = (int)SendMessage(hidden, LB_ADDSTRING, 0, (LPARAM)text); 

				ListBox_DeleteString(visible, selected);
				_this->getForm()->getLayout()->removeField(selected);
			}
		}
		else if ( LOWORD(wParam) == IDC_FIELD_UP )
		{
			HWND hidden = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visible = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visible);
			if( selected != LB_ERR && selected > 0 ){
				wchar_t text[80];
				ListBox_GetText(visible, selected, text);
				ListBox_DeleteString(visible, selected);
				ListBox_InsertString(visible, selected-1, text);
				ListBox_SetCurSel(visible, selected-1);

				_this->getForm()->getLayout()->swapFields(selected, selected-1);
				
				
			}
		}
		else if ( LOWORD(wParam) == IDC_FIELD_DOWN )
		{
			HWND hidden = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visible = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visible);
			int count = ListBox_GetCount(visible);
			if( selected != LB_ERR && selected < count -1 ){
				wchar_t text[80];
				ListBox_GetText(visible, selected, text);
				ListBox_DeleteString(visible, selected);
				ListBox_InsertString(visible, selected+1, text);
				ListBox_SetCurSel(visible, selected+1);

				_this->getForm()->getLayout()->swapFields(selected, selected+1);
				
			}

		} else if (LOWORD(wParam) == IDC_NEW_FIELD ){
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ADD_FIELD), hDlg, AddField);
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void LoadViews(HWND hwnd);


INT_PTR CALLBACK AddField(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmEvent;
	int wmId;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
		HWND combo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Short Text");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Medium Text");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Multi-line Text");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Yes/No");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Choose from a few values");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Choose from many values");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Date and Time");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Number");

		LoadViews(GetDlgItem(hDlg, IDC_ADD_FIELD_VIEW_TREE));

		Connection conn;
		Database d = conn.getDatabase(DATABASE);
		Object obj = d.viewResults("all-objects", "by-label", Value(), 10);
		if ( obj["rows"].isArray() ){
			Array results = obj["rows"].getArray();
			for(unsigned int i=0; i<results.size(); i++){
				Object row = results[i].getObject();
				string key = row["key"].getString();
				wstring wkey =s2ws(key);
				SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)wkey.c_str());
			}
		}

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

				CinchDesigner* _this = (CinchDesigner *)GetWindowLong(designerHWnd, GWL_USERDATA);
				int type = SendMessage(typeCombo, CB_GETCURSEL, 0, 0);
				FormField* field;
				switch(type){
				case 0:
					field = FormField::createEditField(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel);
					break;
				case 1:
					field = FormField::createEditField(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel);
					break;
				case 2:
					field = FormField::createMultilineText(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel);
					break;
				case 3:
					field = FormField::createYesNoField(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel);
					break;
				case 4:
					field = FormField::createRadioGroup(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel);
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

					field = FormField::createComboBox(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel, config);
					}
					break;
				case 6:
					field = FormField::createDatePicker(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel);
					break;
				case 7:
					field = FormField::createNumberField(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel);
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
							DViewPair * v = (DViewPair *)tvitem.lParam;
							Object config;
							Object pickFrom;
							string design =ws2s(v->design);
							string d = design.substr(design.find("/")+1);
							pickFrom["design"] = d;
							pickFrom["view"] =ws2s(v->view);
							config["pick_from"] = pickFrom;

							field = FormField::createReferenceField(designerHWnd, GetModuleHandle(0), szNewFieldName,szNewFieldLabel, Value(config));
					
						}
						
					}
					break;
				}
				_this->getForm()->addField(field);
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
		}
	case WM_COMMAND:
		wmEvent = HIWORD(wParam);
		wmId = LOWORD(wParam);
		switch(wmId){
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
			newTab["name"] =ws2s(tabname);
			newTab["label"] =ws2s(tablabel);
			newTab["content"] = "Text";
			self->tabsForUpdate.push_back(newTab);

			HWND visibleTabs = GetDlgItem(GetParent(hDlg), IDC_VISIBLE_TABS);
			
			loadTabLabels(self->tabsForUpdate, visibleTabs);

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

	Connection conn;
	Database db = conn.getDatabase(DATABASE);

	Document newDoc = db.createDocument(Value(o), "template/"+ getType());
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
			
			loadTabLabels(self->tabsForUpdate, visibleTabs);
			
		return (INT_PTR)TRUE;
		}
	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK ){

			Object form = self->getLoadedForm();
			form["tabs"] = self->tabsForUpdate;
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
		Connection conn;
		Database db = conn.getDatabase(database);
		try {
			Document doc = db.getDocument("template/" + type);
			Value v = doc.getData();
			loadedForm = v.getObject();
			form->deserializeForm(hWnd, v);
		}catch(Exception e){
		}
	}
}

void CinchDesigner::NewDocument(string database, string type){
	loadForm(database, type);
	Object* obj = new Object();
	(*obj)["cinch_type"] = Value(type);
	form->LoadDocument("", *obj);
}

void CinchDesigner::LoadDocument(string database, string _id, Object obj){
	/* First load the template for this doc */
	if ( obj["cinch_type"].isString() ){
		string t = obj["cinch_type"].getString();

		loadForm(database, t);
    }

	form->LoadDocument(_id, obj);
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
