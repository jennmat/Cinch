

#include "stdafx.h"
#include <sstream>


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

FormField::~FormField(){
}

FormField* FormField::createEditField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare)
{
	static int fieldId = 12002;

	FormField* field = new EditField();

	field->controlChildId = fieldId++;

	HFONT hFont=DEFAULT_FONT;
	
	if ( !bare ){
		field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
			0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);
		SendMessage(field->label, WM_SETFONT, (WPARAM)hFont,0);
		SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);
	}

	
	field->controlType = "Edit";
	field->name = name;
	field->config = Value();
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, (HMENU)field->controlChildId, hInst, NULL);

	//NotifyParentOfEnterKey(field->control);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	return field;
}

FormField* FormField::createAutocompletingEditField(HWND parent, HINSTANCE hInst, string enclosingType, string name, const wchar_t * label, bool bare)
{
	static int fieldId = 48900;

	EditField* field = new EditField();

	field->controlChildId = fieldId++;

	HFONT hFont=DEFAULT_FONT;
	
	if ( !bare ){
		field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
			0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);
		SendMessage(field->label, WM_SETFONT, (WPARAM)hFont,0);
		SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);
	}

	
	field->controlType = "Edit";
	field->name = name;
	field->config = Value();
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_OVERLAPPED | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, (HMENU)field->controlChildId, hInst, NULL);

	
	IAutoComplete2 *pac;

	HRESULT hr = CoCreateInstance(CLSID_AutoComplete, 
                                NULL, 
                              CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&pac));


	IUnknown *punkSource;

	hr = CoCreateInstance(CLSID_ACListISF, 
                      NULL, 
                      CLSCTX_INPROC_SERVER,
                      IID_PPV_ARGS(&punkSource));

	string design;
	string viewstr;
	
	/* Find an appropriate view */
	QueryOptions options;
	options.startKey = Value(name);
	options.endKey = Value(name);
	Object results = db.viewResults("all-grouping-view-definitions", "by-grouped-field", options);
	if ( results["rows"].isArray() ){
		Array rows = results["rows"].getArray();
		if ( rows.size() == 0 ){
			/* None exists.  Lets create one */
			Object view;
			stringstream id;
			stringstream designname;
			designname << "all-" << enclosingType;
			id << "_design/" << designname.str();
			view["_id"] = id.str();
			view["language"] = "javascript";
			stringstream map;
			stringstream viewname;
			viewname << "by-" << name;
			map << "function(doc) {if ( doc.cinch_type && doc.cinch_type == '" << enclosingType << "' ){emit(doc." << name << ", null);}}";
			view["views"][viewname.str()]["map"] = map.str();
			view["views"][viewname.str()]["reduce"] = "function(key, value){ return true; }";
			
			Object designDefinition;
			designDefinition["cinch_type"] = "view_definition";
			designDefinition["design_name"] = designname.str();
			//viewDefinition["view_name"] = viewname.str();
			designDefinition["emits"] = enclosingType;
			//viewDefinition["key_field"] =  name;
			Object viewDefinition;
			viewDefinition["key"] = name;
			viewDefinition["view"] = viewname.str();
			viewDefinition["groups"] = true;
			viewDefinition["reduces"] = true;
			Array viewDefinitions = Array();
			viewDefinitions.push_back(viewDefinition);
			designDefinition["views"] = viewDefinitions;

			db.createDocument(Value(designDefinition));
			db.createDocument(Value(view));

			design = designname.str();
			viewstr = viewname.str();
		} else {
			Object row = rows[0].getObject();
			Object doc = row["value"].getObject();

			design = doc["design"].getString();
			viewstr = doc["view"] .getString();
		}
	}

	ViewAutocompleteSource *pcacs = new ViewAutocompleteSource();
	pcacs->design = design;
	pcacs->view = viewstr;
	pcacs->setup();


	hr = pcacs->QueryInterface(IID_PPV_ARGS(&punkSource));
	if(SUCCEEDED(hr))
	{
		hr = pac->Init(field->control, punkSource, NULL, NULL);
		pac->SetOptions(ACO_AUTOSUGGEST | ACO_AUTOAPPEND | ACO_UPDOWNKEYDROPSLIST);
		field->autoCompleteSource = pcacs;
	}

	pcacs->Release();
	
	//SHAutoComplete(field->control, SHACF_FILESYSTEM);
	
	//NotifyParentOfEnterKey(field->control);
	
	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	return field;
}


FormField* FormField::createIdentifierField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare)
{
	static int fieldId = 1199;

	FormField* field = new IdentifierField();

	field->controlChildId = fieldId++;

	HFONT hFont=DEFAULT_FONT;
	
	if ( !bare ){
		field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
			0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);
		SendMessage(field->label, WM_SETFONT, (WPARAM)hFont,0);
		SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);
	}

	
	field->controlType = "Edit";
	field->name = name;
	field->config = Value();
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, (HMENU)field->controlChildId, hInst, NULL);

	NotifyParentOfEnterKey(field->control);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	return field;
}

void ReferenceField::setupValues(){
	vector<string>* ptr = (vector<string>*) GetWindowLongPtr(getControl(), GWLP_USERDATA);
	if ( ptr != NULL ){
		ptr->clear();
		delete ptr;
	}
	vector<string>* ids = new vector<string>();

	if ( config.isObject() ){
		if ( config["pick_from"].isObject() ){
			Object pick = config["pick_from"].getObject();
			string design, view;
			design = pick["design"].getString();
			view = pick["view"].getString();

			if ( design.length() > 0 && view.length() > 0 ){
				Object results = db.viewResults(design, view);

				ComboBox_ResetContent(getControl());

				Array rows = results["rows"].getArray();
				for(unsigned int i=0; i<rows.size(); i++){
					Object row = rows[i].getObject();
					string key = row["key"].getString();

					wstring wkey =s2ws(key);
					SendMessage(getControl(),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)wkey.c_str()); 
					ids->push_back(row["id"].getString());
				}
			}
		}
	}

	SetWindowLongPtr(getControl(), GWLP_USERDATA, (ULONG_PTR)ids);

}

FormField* FormField::createReferenceField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, Value config, bool bare)
{
	static int fieldId = 9870;

	ReferenceField* field = new ReferenceField();

	field->controlChildId = fieldId++;

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"COMBOBOX", L"", WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, 200, parent, (HMENU)field->controlChildId, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	field->controlType = "Reference";
	field->name = name;
	field->config = config;

	field->setupValues();
	
	return field;
}



FormField* FormField::createNumberField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare)
{
	static int fieldId = 13001;

	FormField* field = new NumberField();

	field->controlChildId = fieldId++;
	field->config = Value();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Number";
	field->name = name;
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, (HMENU)field->controlChildId, hInst, NULL);

	MaskEditControl(field->control, "0123456789.\b", TRUE);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	
	return field;
}

string FormField::getName(){
	return name;
}

string FormField::toString(Object obj){
	return obj[getName()].getString();
}

string ReferenceField::toString(Object obj){
	string id = obj[getName()].getString();

	Object o = db.getDocument(id).getData().getObject();
	return o["label"].getString();
}

FormField* FormField::createComboBox(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, string type, bool bare)
{
	static int fieldId = 651;

	FormField* field = new ComboBoxField();

	field->controlChildId = fieldId++;

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE | WS_TABSTOP,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Combo";
	field->name = name;
	
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"COMBOBOX", L"", WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, 200, parent, (HMENU)field->controlChildId, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);

	
	Object obj = db.getDocument(type).getData().getObject();

	if ( obj["allowed_codes"].isArray() ){
		Array values = obj["allowed_codes"].getArray();
		for(unsigned int i=0; i<values.size(); i++){
			Object code = values[i].getObject();
			string value = code["value"].getString();
			string label = code["label"].getString();
			wstring wlabel = s2ws(label);
			int index = ComboBox_AddString(field->control, wlabel.c_str());
			ComboBox_SetItemData(field->control, index, new string(value));

		}
	}

	return field;

}


FormField* FormField::createDatePicker(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare)
{
	static int fieldId = 130010;
	FormField* field = new DatePickerField();

	field->controlChildId = fieldId++;
	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "DatePicker";
	field->name = name;
	
	field->control = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, (HMENU)field->controlChildId, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);

	return field;
}


FormField* FormField::createCheckBox(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);
	field->config = Value();
	HFONT hFont=DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Checkbox";
	field->name = name;
	
	field->control = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD|BS_AUTOCHECKBOX|WS_TABSTOP,
		0, 0, CONTROL_WIDTH, CONTROL_HEIGHT, parent, NULL, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);


	return field;
}


FormField* FormField::createRadioGroup(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare)
{
	FormField* field = new EditField();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);
	field->config = Value();
	HFONT hFont=DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Radio";
	field->name = name;
	
	field->control = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD | WS_TABSTOP|BS_GROUPBOX,
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


FormField* FormField::createYesNoField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare)
{
	static int fieldId = 7022;
	
	FormField* field = new YesNoField();
	field->controlChildId = fieldId++;
	field->config = Value();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont=DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "YesNo";
	field->name = name;
	
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"COMBOBOX", L"", WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP,
		0, 0, CONTROL_WIDTH, 200, parent,  (HMENU)field->controlChildId, hInst, NULL);

	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);

	SendMessage(field->control,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)L"Yes"); 
	SendMessage(field->control,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)L"No"); 
	
	return field;
}



FormField* FormField::createMultilineText(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare)
{
	static int fieldId = 62388;
	
	FormField* field = new EditField();
	field->controlChildId = fieldId++;
	field->config = Value();

	field->label = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTERIMAGE,
		0, 0, LABEL_WIDTH, LABEL_HEIGHT, parent, NULL, hInst, NULL);

	HFONT hFont = DEFAULT_FONT;
	SendMessage(field->label, WM_SETFONT, (WPARAM)hFont,0);
	SendMessage(field->label, WM_SETTEXT, 0, (LPARAM)label);

	field->controlType = "Multiline";
	field->name = name;
	field->config = Value();
	
	field->control = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
		0, 0, CONTROL_WIDTH, 75, parent,  (HMENU)field->controlChildId, hInst, NULL);
	
	PreserveTabInMultilineEdit(field->control);
	SendMessage(field->control, WM_SETFONT,(WPARAM)hFont,0);
	return field;
}


void EditField::loadValue(Object obj){
	string n = getName();
	if ( obj[n.c_str()].isString() ){
		string val = obj[n.c_str()].getString();
		wstring valw =s2ws(val);
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
	string key = getName();
	string value =ws2s(str);
	obj[key.c_str()] = value;	
	delete str;
	return obj;
}

string EditField::serializeForJS(){
	int len = GetWindowTextLength(getControl()) + 1;
	LPWSTR str = new wchar_t[len];
	
	GetWindowText(getControl(), str, len);
	string key = getName();
	string value =ws2s(str);
	stringstream rc;
	rc << "'" << value << "'";
	delete str;
	return rc.str();

}

EditField::EditField(){
	autoCompleteSource = NULL;
}

EditField::~EditField(){
	if( autoCompleteSource != NULL )
		autoCompleteSource->Release();
}


void IdentifierField::loadValue(Object obj){
	string n = "_id";
	if ( obj[n.c_str()].isString() ){
		string val = obj[n.c_str()].getString();
		wstring valw =s2ws(val);
		LPCWSTR r = valw.c_str();
		SetWindowText(getControl(), r); 
	}
}

void IdentifierField::clearValue(){
	SetWindowText(getControl(), L"");
}

Object IdentifierField::storeValue(Object obj){
	int len = GetWindowTextLength(getControl()) + 1;
	LPWSTR str = new wchar_t[len];
	
	GetWindowText(getControl(), str, len);
	string key = "_id";
	string value =ws2s(str);
	obj[key.c_str()] = value;	
	delete str;
	return obj;
}

string IdentifierField::serializeForJS(){
	int len = GetWindowTextLength(getControl()) + 1;
	LPWSTR str = new wchar_t[len];
	
	GetWindowText(getControl(), str, len);
	string key = "_id";
	string value =ws2s(str);
	stringstream rc;
	rc << "'" << value << "'";
	delete str;
	return rc.str();

}

IdentifierField::~IdentifierField(){
}


void DatePickerField::loadValue(Object obj){
	string n = getName();
	if ( obj[n.c_str()].isInteger() ){
		int val = obj[n.c_str()].getInt();
		FILETIME ft;
		SYSTEMTIME time;
		time_t t = val;
		UnixTimeToFileTime(t, &ft);
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
	SYSTEMTIME st;
    
    GetSystemTime(&st);
	DateTime_SetSystemtime(getControl(), GDT_VALID, &st);
}

Object DatePickerField::storeValue(Object obj){
	SYSTEMTIME time;
	DateTime_GetSystemtime(getControl(), &time);
	wchar_t* date = new wchar_t[80];
	GetDateFormat(LOCALE_INVARIANT, 0, &time, L"yyyy-MM-dd", date, 80);

	LPWSTR str = new wchar_t[80];
	string key = getName();
	string value =ws2s(date);
	obj[key.c_str()] = value;
	delete date;
	return obj;
	
}

string DatePickerField::serializeForJS(){
	SYSTEMTIME time;
	DateTime_GetSystemtime(getControl(), &time);
	wchar_t* date = new wchar_t[80];
	GetDateFormat(LOCALE_INVARIANT, 0, &time, L"yyyy-MM-dd", date, 80);

	LPWSTR str = new wchar_t[80];
	string key = getName();
	string value =ws2s(date);

	stringstream rc;
	rc << "'" << value << "'";
	delete date;
	return rc.str();
}

DatePickerField::~DatePickerField(){
}

string NumberField::toString(Object obj){
	string n = getName();
	if ( obj[n.c_str()].isDouble() ){
		return convertToString(obj[n.c_str()].getDouble());
	} else if ( obj[n.c_str()].isInteger() ){
		int val = obj[n.c_str()].getInt();
		return convertToString(val);
	}

	return "NaN";
}

void NumberField::loadValue(Object obj){
	string n = getName();
	if ( obj[n.c_str()].isDouble() ){
		double val = obj[n.c_str()].getDouble();
		int decimal, sign;
		char* buffer = new char[_CVTBUFSIZE];
		int precision = 2;
		_fcvt_s(buffer, _CVTBUFSIZE, val, precision, &decimal, &sign);
		wstring w =s2ws(string(buffer));
		w.insert(decimal, L".");
		SetWindowText(getControl(), w.c_str());
	} else if ( obj[n.c_str()].isInteger() ){
		int val = obj[n.c_str()].getInt();
		wchar_t* buf = new wchar_t[100];
		memset(buf, 0, 100);
		_itow_s(val, buf, 100, 10);

		SetWindowText(getControl(), buf);
		delete buf;
	}
}

void NumberField::clearValue(){
	SetWindowText(getControl(), L"");
}

Object NumberField::storeValue(Object obj){
	int len = GetWindowTextLength(getControl()) + 1;
	LPWSTR str = new wchar_t[len];
	string key = getName();
	if ( GetWindowTextLength(getControl()) > 0 ){
		GetWindowText(getControl(), str, len);
		double val = _wtof(str);
		
		obj[key.c_str()] = val;	
	} else {
		obj[key.c_str()] = Value();
	}
	
	free(str);

	return obj;
}

string NumberField::serializeForJS(){
	int len = GetWindowTextLength(getControl()) + 1;
	if ( GetWindowTextLength(getControl()) > 0 ){
		LPWSTR str = new wchar_t[len];
		GetWindowText(getControl(), str, len);

		string rc =ws2s(str);
		free(str);
		return rc;
	}

	return "0";

}

NumberField::~NumberField(){
}


void YesNoField::loadValue(Object obj){
	string n = getName();
	if ( obj[n.c_str()].isBoolean() ){
		bool val = obj[n.c_str()].getBoolean();
		if ( val ){
			ComboBox_SetCurSel(getControl(), 0);
		} else {
			ComboBox_SetCurSel(getControl(), 1);
		}
	}
}

void YesNoField::clearValue(){
	ComboBox_SetCurSel(getControl(), -1);
}

Object YesNoField::storeValue(Object obj){
	int idx = ComboBox_GetCurSel(getControl());
	string key = getName();


	if ( idx == 0 ){
		obj[key.c_str()] = true;	
	} else {
		obj[key.c_str()] = false;	
	}
	
	return obj;
}

string YesNoField::serializeForJS(){
	int idx = ComboBox_GetCurSel(getControl());
	if ( idx == 0 ){
		return "true";
	} else {
		return "false";
	}
}

YesNoField::~YesNoField(){
}


void ReferenceField::clearValue(){
	ComboBox_SetCurSel(getControl(), -1);
}

Object ReferenceField::storeValue(Object obj){

	vector<string>* ids = (vector<string>*)GetWindowLong(getControl(), GWL_USERDATA);
	int idx = ComboBox_GetCurSel(getControl());

	string key = getName();

	if ( idx >= 0 ){
		string id = (*ids)[idx];
		obj[key.c_str()] = id;	
	} else {
		obj[key.c_str()] = Value();
	}

	return obj;

}

void ReferenceField::loadValue(Object obj){

	setupValues();

	vector<string>* ids = (vector<string>*)GetWindowLong(getControl(), GWL_USERDATA);
	
	string n = getName();

	if ( obj[n.c_str()].isString() ){
		string id = obj[n.c_str()].getString();
		for(unsigned int i=0; i<ids->size(); i++){
			if( (*ids)[i].compare(id) == 0 ){
				ComboBox_SetCurSel(getControl(), i);
			}
		}
	}
}


string ReferenceField::serializeForJS(){
	vector<string>* ids = (vector<string>*)GetWindowLong(getControl(), GWL_USERDATA);
	int idx = ComboBox_GetCurSel(getControl());

	string key = getName();

	if ( idx >= 0 ){
		string id = (*ids)[idx];

		stringstream rc;
		rc << "'" << id << "'";
		return rc.str();
	}
	return "";
}

ReferenceField::~ReferenceField(){
	vector<string>* ptr = (vector<string>*) GetWindowLongPtr(getControl(), GWLP_USERDATA);
	if ( ptr != NULL ){
		ptr->clear();
		delete ptr;
	}
}

void ComboBoxField::clearValue(){
	ComboBox_SetCurSel(getControl(), -1);
}

Object ComboBoxField::storeValue(Object obj){

	int idx = ComboBox_GetCurSel(getControl());
	
	ULONG_PTR data = ComboBox_GetItemData(getControl(), idx);
	string s = *(string *)data;
			
	string key = getName();

	if ( idx >= 0 ){
		obj[key.c_str()] = s;
	} else {
		obj[key.c_str()] = Value();
	}

	return obj;

}

string ComboBoxField::serializeForJS(){
	int idx = ComboBox_GetCurSel(getControl());
	
	ULONG_PTR data = ComboBox_GetItemData(getControl(), idx);
	string s = *(string *)data;
			

	if ( idx >= 0 ){
		stringstream rc;
		rc << "'" << s << "'";
		return rc.str();
	} else {
		return "";
	}
}

void ComboBoxField::loadValue(Object obj){

	string n = getName();

	if ( obj[n.c_str()].isString() ){
		string value = obj[n.c_str()].getString();
		wstring val = s2ws(value);
		int count = ComboBox_GetCount(getControl());
		for(int i=0; i<count; i++){
			ULONG_PTR data = ComboBox_GetItemData(getControl(), i);
			string s = *(string *)data;
			if ( s.compare(value) == 0 ){
				ComboBox_SetCurSel(getControl(), i);
			}
		}
	}
}

ComboBoxField::~ComboBoxField(){
}