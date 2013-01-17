

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

FormField* FormField::createEditField(HWND parent, HINSTANCE hInst, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Edit";
	field->name = label;
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, NULL, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	return field;
}

const wchar_t* FormField::getName(){
	return name;
}




FormField* FormField::createComboBox(HWND parent, HINSTANCE hInst, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | WS_TABSTOP,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Combo";
	field->name = label;
	
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


FormField* FormField::createDatePicker(HWND parent, HINSTANCE hInst, const wchar_t * label)
{
	FormField* field = new DatePickerField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "DatePicker";
	field->name = label;
	
	field->control = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, NULL, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);

	return field;
}


FormField* FormField::createCheckBox(HWND parent, HINSTANCE hInst, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Checkbox";
	field->name = label;
	
	field->control = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, NULL, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);

	return field;
}


FormField* FormField::createRadioGroup(HWND parent, HINSTANCE hInst, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Radio";
	field->name = label;
	
	field->control = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_GROUPBOX,
		0, 0, CONTROL_WIDTH, 45, parent, NULL, hInst, NULL);

	HWND option1 = CreateWindowEx(0, L"BUTTON", L"Yes", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, field->control, NULL, hInst, NULL);
	HWND option2 = CreateWindowEx(0, L"BUTTON", L"No", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP,
		0, 20, CONTROL_WIDTH, CONTROL_HEIGHT, field->control, NULL, hInst, NULL);
	
	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(option1, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(option2, WM_SETFONT,(WPARAM)hFont,0);
	return field;
}


FormField* FormField::createMultilineText(HWND parent, HINSTANCE hInst, const wchar_t * label)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	SendMessage(field->label, WM_SETFONT, (WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Multiline";
	field->name = label;
	
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL,
		0, 0, CONTROL_WIDTH, 200, parent, NULL, hInst, NULL);
	
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

	} else {
		SYSTEMTIME time;
		GetLocalTime(&time);
		DateTime_SetSystemtime(getControl(), GDT_VALID, &time);

	}
}



