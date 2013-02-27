

#include "stdafx.h"


void FormField::setLabel(HWND label)
{
	this->label = label;
}

void FormField::setControl(HWND control)
{
	this->control = control;
}

HWND FormField::getControl()
{
	return this->control;
}

HWND FormField::getLabel()
{
	return this->label;
}

char* FormField::getControlType()
{
	return this->controlType;
}

Value FormField::getConfig(){
	return config;
}

FormField* FormField::createEditField(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label)
{
	static int fieldId = 12002;

	FormField* field = new EditField();

	field->controlChildId = fieldId++;

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT
	SendMessage(field->label, WM_SETFONT, (WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Edit";
	field->name = name;
	field->config = Value();
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, (HMENU)field->controlChildId, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	return field;
}

FormField* FormField::createReferenceField(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label, Value config)
{
	static int fieldId = 9870;

	FormField* field = new ReferenceField();

	field->controlChildId = fieldId++;

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	vector<string>* ids = new vector<string>();


	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, 200, parent, NULL, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	field->controlType = "Reference";
	field->name = name;
	field->config = config;

	if ( config.isObject() ){
		if ( config["pick_from"].isObject() ){
			Object pick = config["pick_from"].getObject();
			string design, view;
			design = pick["design"].getString();
			view = pick["view"].getString();

			if ( design.length() > 0 && view.length() > 0 ){
				Connection conn;
				Database db = conn.getDatabase("bugs");
				Object results = db.viewResults(design, view, Value(), 10);

				Array rows = results["rows"].getArray();
				for(unsigned int i=0; i<rows.size(); i++){
					Object row = rows[i].getObject();
					string key = row["key"].getString();

					wstring wkey = Designer::s2ws(key);
					SendMessage(field->control,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)wkey.c_str()); 
					ids->push_back(row["id"].getString());
				}
			}
		}
	}

	SetWindowLong(field->control, GWL_USERDATA, (ULONG_PTR)ids);

	return field;
}



FormField* FormField::createNumberField(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label)
{
	static int fieldId = 13001;

	FormField* field = new NumberField();

	field->controlChildId = fieldId++;
	field->config = Value();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Number";
	field->name = name;
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, (HMENU)field->controlChildId, hInst, NULL);

	MaskEditControl(field->control, "0123456789\b", TRUE);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	return field;
}

const wchar_t* FormField::getName(){
	return name;
}


FormField* FormField::createComboBox(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | WS_TABSTOP,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Combo";
	field->name = label;
	field->config = Value();

	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, 200, parent, NULL, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);


	TCHAR Planets[9][10] =  
	{
		TEXT("Mercury"), TEXT("Venus"), TEXT("Terra"), TEXT("Mars"), 
		TEXT("Jupiter"), TEXT("Saturn"), TEXT("Uranus"), TEXT("Neptune"), 
		TEXT("Pluto??") 
	};
       
	TCHAR A[16]; 
	int  k = 0; 

	memset(&A,0,sizeof(A));       
	for (k = 0; k <= 8; k += 1)
	{
		wcscpy_s(A, sizeof(A)/sizeof(TCHAR),  (TCHAR*)Planets[k]);

		// Add string to combobox.
		SendMessage(field->control,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) A); 
	}
  
	// Send the CB_SETCURSEL message to display an initial item 
	//  in the selection field  
	SendMessage(field->control, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

	return field;

}


FormField* FormField::createDatePicker(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label)
{
	static int fieldId = 130010;
	FormField* field = new DatePickerField();

	field->controlChildId = fieldId++;
	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "DatePicker";
	field->name = name;
	
	field->control = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, (HMENU)field->controlChildId, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);

	return field;
}


FormField* FormField::createCheckBox(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);
	field->config = Value();
	HFONT hFont=DEFAULT_FONT
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Checkbox";
	field->name = name;
	
	field->control = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, NULL, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);


	return field;
}


FormField* FormField::createRadioGroup(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);
	field->config = Value();
	HFONT hFont=DEFAULT_FONT
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Radio";
	field->name = name;
	
	field->control = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_GROUPBOX,
		0, 0, CONTROL_WIDTH, 60, parent, NULL, hInst, NULL);

	HWND option1 = CreateWindowEx(0, L"BUTTON", L"Yes", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP,
		10, 10, CONTROL_WIDTH, CONTROL_HEIGHT, field->control, NULL, hInst, NULL);
	HWND option2 = CreateWindowEx(0, L"BUTTON", L"No", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP,
		10, 30, CONTROL_WIDTH, CONTROL_HEIGHT, field->control, NULL, hInst, NULL);
	
	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(option1, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(option2, WM_SETFONT,(WPARAM)hFont,0);
	return field;
}


FormField* FormField::createYesNoField(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label)
{
	static int fieldId = 7022;
	
	FormField* field = new YesNoField();
	field->controlChildId = fieldId++;
	field->config = Value();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "YesNo";
	field->name = name;
	
	field->control = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_GROUPBOX,
		0, 0, CONTROL_WIDTH, 60, parent, (HMENU)field->controlChildId, hInst, NULL);

	NotifyParentControl(field->control);

	HWND option1 = CreateWindowEx(0, L"BUTTON", L"Yes", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP,
		10, 10, CONTROL_WIDTH, CONTROL_HEIGHT, field->control, (HMENU)YES_RADIO, hInst, NULL);
	HWND option2 = CreateWindowEx(0, L"BUTTON", L"No", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP,
		10, 30, CONTROL_WIDTH, CONTROL_HEIGHT, field->control, (HMENU)NO_RADIO, hInst, NULL);
	
	
	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(option1, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(option2, WM_SETFONT,(WPARAM)hFont,0);
	return field;
}



FormField* FormField::createMultilineText(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont = DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT, (WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Multiline";
	field->name = name;
	field->config = Value();
	
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
		0, 0, CONTROL_WIDTH, 75, parent, NULL, hInst, NULL);
	
	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	return field;
}

void EditField::loadValue(Object obj){
	const wchar_t* name = getName();
	string n = Designer::ws2s(name);
	if ( obj[n.c_str()].isString() ){
		string val = obj[n.c_str()].getString();
		wstring valw = Designer::s2ws(val);
		LPCWSTR r = valw.c_str();
		SetWindowText(getControl(), r); 
	}
}

void EditField::clearValue(){
	SetWindowText(getControl(), L"");
}

Object EditField::storeValue(Object obj){
	int len = GetWindowTextLength(getControl()) + 1;
	LPWSTR str = new wchar_t[len];
	
	GetWindowText(getControl(), str, len);
	string key = Designer::ws2s(getName());
	string value = Designer::ws2s(str);
	obj[key.c_str()] = value;	
	return obj;
}




void DatePickerField::loadValue(Object obj){
	const wchar_t* name = getName();
	string n = Designer::ws2s(name);
	if ( obj[n.c_str()].isInteger() ){
		int val = obj[n.c_str()].getInt();
		FILETIME ft;
		SYSTEMTIME time;
		time_t t = val;
		Designer::UnixTimeToFileTime(t, &ft);
		FileTimeToSystemTime(&ft, &time);
		DateTime_SetSystemtime(getControl(), GDT_VALID, &time);
	} else if ( obj[n.c_str()].isString() ){
		SYSTEMTIME time;
		int month, day, year;
		sscanf_s(obj[n.c_str()].getString().c_str(), "%d-%d-%d", &year, &month, &day);
		time.wYear = year;
		time.wMonth = month;
		time.wDay = day;
		DateTime_SetSystemtime(getControl(), GDT_VALID, &time);
	}
}


void DatePickerField::clearValue(){
}

Object DatePickerField::storeValue(Object obj){
	SYSTEMTIME time;
	DateTime_GetSystemtime(getControl(), &time);
	wchar_t* date = new wchar_t[80];
	GetDateFormat(LOCALE_INVARIANT, 0, &time, L"yyyy-MM-dd", date, 80);

	LPWSTR str = new wchar_t[80];
	string key = Designer::ws2s(getName());
	string value = Designer::ws2s(date);
	obj[key.c_str()] = value;	
	return obj;
	
}

void NumberField::loadValue(Object obj){
	const wchar_t* name = getName();
	string n = Designer::ws2s(name);
	if ( obj[n.c_str()].isInteger() ){
		int val = obj[n.c_str()].getInt();
		wchar_t* w = new wchar_t[80];
		memset(w, 0, 80);
		_itow_s(val, w, 80, 10);
		SetWindowText(getControl(), w); 
	}
}

void NumberField::clearValue(){
	SetWindowText(getControl(), L"");
}

Object NumberField::storeValue(Object obj){
	LPWSTR str = new wchar_t[80];
	string key = Designer::ws2s(getName());
	if ( GetWindowTextLength(getControl()) > 0 ){
		GetWindowText(getControl(), str, 80);
		long val = _wtol(str);
		
		obj[key.c_str()] = val;	
	} else {
		obj[key.c_str()] = Value();
	}
	return obj;
}


void YesNoField::loadValue(Object obj){
	const wchar_t* name = getName();
	string n = Designer::ws2s(name);
	if ( obj[n.c_str()].isBoolean() ){
		bool val = obj[n.c_str()].getBoolean();
		if ( val ){
			CheckDlgButton(getControl(), YES_RADIO, BST_CHECKED);
			CheckDlgButton(getControl(), NO_RADIO, BST_UNCHECKED);
		} else {
			CheckDlgButton(getControl(), NO_RADIO, BST_CHECKED);
			CheckDlgButton(getControl(), YES_RADIO, BST_UNCHECKED);
		}
	}
}

void YesNoField::clearValue(){
	CheckDlgButton(getControl(), YES_RADIO, BST_UNCHECKED);
	CheckDlgButton(getControl(), NO_RADIO, BST_UNCHECKED);	
}

Object YesNoField::storeValue(Object obj){
	LPWSTR str = new wchar_t[80];
	string key = Designer::ws2s(getName());
	
	if ( IsDlgButtonChecked(getControl(), YES_RADIO) ){
		obj[key.c_str()] = true;	
	} else if ( IsDlgButtonChecked(getControl(), NO_RADIO ) ) {
		obj[key.c_str()] = false;	
	}
	
	return obj;
}




void ReferenceField::clearValue(){
	ComboBox_SetCurSel(getControl(), -1);
}

Object ReferenceField::storeValue(Object obj){

	vector<string>* ids = (vector<string>*)GetWindowLong(getControl(), GWL_USERDATA);
	int idx = ComboBox_GetCurSel(getControl());

	string key = Designer::ws2s(getName());

	if ( idx >= 0 ){
		string id = (*ids)[idx];
		obj[key.c_str()] = id;	
	} else {
		obj[key.c_str()] = Value();
	}

	return obj;

}

void ReferenceField::loadValue(Object obj){

	vector<string>* ids = (vector<string>*)GetWindowLong(getControl(), GWL_USERDATA);
	
	const wchar_t* name = getName();
	string n = Designer::ws2s(name);

	if ( obj[n.c_str()].isString() ){
		string id = obj[n.c_str()].getString();
		for(unsigned int i=0; i<ids->size(); i++){
			if( (*ids)[i].compare(id) == 0 ){
				ComboBox_SetCurSel(getControl(), i);
			}
		}
	}
}

