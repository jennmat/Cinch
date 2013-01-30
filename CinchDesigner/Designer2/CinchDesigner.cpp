// Designer2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CinchDesigner.h"
#include "Windowsx.h"
#include <Windows.h>
#include <CommDlg.h>
#include "JsonBox.h"


#define MAX_LOADSTRING 100


int newFieldTypes[MAX_FIELDS];
int countNewFields;

HWND designerHWnd;

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
			InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EDIT_FIELDS, L"Edit Fields");
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EDIT_TABS, L"Edit Tabs");
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
		if ( wmEvent == EN_KILLFOCUS || wmEvent == NM_KILLFOCUS || wmEvent == BN_KILLFOCUS){
			_this->getForm()->SaveDocument(wmId);
		} else {
			switch (wmId)
			{
			case IDM_EDIT_TABS:
				DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EDIT_TABS), hWnd, EditTabs);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case IDM_EDIT_FIELDS:
				DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EDIT_FIELDS), hWnd, EditFields);
				break;
			case IDM_SAVE_FORM:
				_this->SaveForm();
				break;
			case IDM_OPEN_FORM:
				_this->ChooseForm();
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
				wchar_t title[80];
				GetWindowText(label, title, 80);
				SendMessage(visibleFields, LB_ADDSTRING, i+1, (LPARAM) title); 
		
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
			HWND fields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);
			int count = ListBox_GetCount(fields);
			_this->getForm()->removeAllFields();
			for(int i=0; i<count; i++){
				wchar_t szFieldName[80];
				ListBox_GetText(fields, i, szFieldName);
				FormField* field;

				switch(newFieldTypes[i]){
					case 0:
						field = FormField::createEditField(hWnd, GetModuleHandle(0), szFieldName);
						break;
					case 1:
						field = FormField::createEditField(hWnd, GetModuleHandle(0), szFieldName);
						break;
					case 2:
						field = FormField::createMultilineText(hWnd, GetModuleHandle(0), szFieldName);
						break;
					case 3:
						field = FormField::createYesNoField(hWnd, GetModuleHandle(0), szFieldName);
						break;
					case 4:
						field = FormField::createRadioGroup(hWnd, GetModuleHandle(0), szFieldName);
						break;
					case 5:
						field = FormField::createComboBox(hWnd, GetModuleHandle(0), szFieldName);
						break;
					case 6:
						field = FormField::createDatePicker(hWnd, GetModuleHandle(0), szFieldName);
						break;

				}
				_this->getForm()->addField(field);
			}
				
			/*
				HWND typeCombo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
				int selected = SendMessage(typeCombo, CB_GETCURSEL, 0, 0);
				if( selected != CB_ERR ){
					FormField field;
					switch(selected){
					
					}
					
					form.addField(field);
						
				}	
				
				RECT window;
				GetWindowRect(hWnd, &window);

				form.show(hWnd, hInst);
				*/


			RECT window;
			GetWindowRect(hWnd, &window);

			_this->getForm()->show(hWnd, GetModuleHandle(0));

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

				int temp;
				temp = newFieldTypes[selected];
				newFieldTypes[selected] = newFieldTypes[selected-1];
				newFieldTypes[selected-1] = temp;
				
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
			}

		} else if (LOWORD(wParam) == IDC_NEW_FIELD ){
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ADD_FIELD), hDlg, AddField);
		}
		break;
	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK AddField(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
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
	
		ComboBox_SelectString(combo, 0, L"Medium Text");
		return (INT_PTR)TRUE;
		}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NEW_FIELD_NAME));
			if ( len > 0 ){
				wchar_t szNewFieldName[80];
				GetDlgItemText(hDlg, IDC_NEW_FIELD_NAME, szNewFieldName, 80);

				HWND parent = GetParent(hDlg);
				HWND visibleFieldsList = GetDlgItem(parent, IDC_VISIBLE_FIELDS);
				int count = ListBox_GetCount(visibleFieldsList);

				ListBox_InsertString(visibleFieldsList, count, szNewFieldName);

				HWND typeCombo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
				int selected = SendMessage(GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE), CB_GETCURSEL, 0, 0);

				newFieldTypes[countNewFields] = selected;
				countNewFields++;
			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void CinchDesigner::ChooseForm()
{
	OPENFILENAME ofn;       // common dialog box structure
	wchar_t szFile[260];       // buffer for file name
	
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn)==TRUE) {
		form->open(hWnd, ofn.lpstrFile);
	}
}

void CinchDesigner::SaveForm(){
	OPENFILENAME ofn;       // common dialog box structure
	wchar_t szFile[260];       // buffer for file name
	
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetSaveFileName(&ofn)==TRUE) {
		form->save(ofn.lpstrFile);
	}
}


// Message handler for about box.
INT_PTR CALLBACK EditTabs(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CinchDesigner* _this = (CinchDesigner *)GetWindowLong(designerHWnd, GWL_USERDATA);
	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
		HWND visiblePages = GetDlgItem(hDlg, IDC_VISIBLE_TABS);
		Detail* detail = _this->getForm()->getDetail();

		for(int i=0; i<detail->getDetailPageCount(); i++){
			wchar_t title[80];
			detail->getDetailPageTitle(i, title);
			SendMessage(visiblePages, LB_ADDSTRING, 0, (LPARAM) title); 
		
		}
		
		return (INT_PTR)TRUE;
		}
	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK ){

			_this->getForm()->removeAllDetailPages();

			HWND tabs = GetDlgItem(hDlg, IDC_VISIBLE_TABS);
			int count = ListBox_GetCount(tabs);
			for(int i=0; i<count; i++){
				wchar_t szTabName[80];
				ListBox_GetText(tabs, i, szTabName);

				_this->getForm()->addDetail(szTabName);
			}

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
		else if ( LOWORD(wParam) == IDC_ADD_NEW_TAB_BUTTON )
		{
			int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NEW_TAB_NAME));
			if ( len > 0 ){
				wchar_t szNewTabName[80];
				GetDlgItemText(hDlg, IDC_NEW_TAB_NAME, szNewTabName, 80);
				HWND visiblePages = GetDlgItem(hDlg, IDC_VISIBLE_TABS);
				int pos = (int)SendMessage(visiblePages, LB_ADDSTRING, 0, (LPARAM) szNewTabName); 
				SendMessage(visiblePages, LB_SETITEMDATA, 0, (LPARAM)pos);

				SetDlgItemText(hDlg, IDC_NEW_TAB_NAME, L"");
			}
			
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
