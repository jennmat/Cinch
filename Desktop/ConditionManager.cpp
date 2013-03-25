
#include "stdafx.h"
#include <sstream>

#define CONDITION_FIELD_WIDTH 100
#define CONDITION_COMPARE_WIDTH 100
#define CONDITION_VALUE_WIDTH 175
#define CONDITION_MARGIN 10
#define CONDITION_ROW_HEIGHT 30
#define CONDITION_EDITOR_HEIGHT 24

ConditionManager::ConditionManager(){
	conditions = new vector<Condition*>();
}

ConditionManager::~ConditionManager(){
	free(conditions);
}

void ConditionManager::arrangeWindowsInParent(HWND parent, int startX, int startY){
	int x = startX;
	int y = startY;
	for(unsigned i=0; i<conditions->size(); i++){
		Condition* c = (*conditions)[i];
		SetWindowPos(c->fieldCombo, HWND_TOP, x, y, CONDITION_FIELD_WIDTH, 15, 0);
		SetWindowPos(c->compareCombo, HWND_TOP, x+CONDITION_FIELD_WIDTH+CONDITION_MARGIN, y, CONDITION_COMPARE_WIDTH, CONDITION_EDITOR_HEIGHT, 0);
		
		ShowWindow(c->fieldCombo, SW_SHOW);
		ShowWindow(c->compareCombo, SW_SHOW);
		
		if ( c->value ) {
			SetWindowPos(c->value->getControl(), HWND_TOP, x+CONDITION_FIELD_WIDTH+CONDITION_MARGIN+CONDITION_COMPARE_WIDTH+CONDITION_MARGIN, y, CONDITION_VALUE_WIDTH, CONDITION_EDITOR_HEIGHT, 0);
			ShowWindow(c->value->getControl(), SW_SHOW);
		}

		y += CONDITION_ROW_HEIGHT;

	}
}

void ConditionManager::updateConditions(string type, HWND parent){
	bool hasEmptyCondition = false;

	for(unsigned i=0; i<conditions->size(); i++){
		Condition* c = (*conditions)[i];

		int fieldIdx = ComboBox_GetCurSel(c->fieldCombo);
		
		if ( fieldIdx < 0 ) hasEmptyCondition = true;

		if ( fieldIdx >= 0 ){
			vector<Object>* fieldsVector = (vector<Object>*)GetWindowLong(c->fieldCombo, GWL_USERDATA);
			Object field = (*fieldsVector)[fieldIdx];
			string name = field["name"].getString();


			if ( c->value == NULL || name.compare(c->setupForField) != 0 ){
			
				if ( c->value != NULL ){
					DestroyWindow(c->value->getControl());
					DestroyWindow(c->value->getLabel());
					free(c->value);
				}
			
				string type = field["cinch_type"].getString();
				Value config = Value();
				if ( field["config"].isObject() ){
					config = field["config"].getObject();
				}
					
				FormField* formField;
					
				if ( type.compare(DATEPICKER) == 0 ){
					formField = FormField::createDatePicker(parent, GetModuleHandle(0), L"", L"");
				} else if ( type.compare(RADIO) == 0 ){
					formField = FormField::createRadioGroup(parent, GetModuleHandle(0), L"", L"");
				} else if ( type.compare(NUMBER) == 0 ){
					formField = FormField::createNumberField(parent, GetModuleHandle(0), L"", L"");
				} else if ( type.compare(YESNO) == 0 ){
					formField = FormField::createYesNoField(parent, GetModuleHandle(0), L"", L"");
				} else if ( type.compare(MULTILINE) == 0 ){
					formField = FormField::createMultilineText(parent, GetModuleHandle(0), L"", L"");
				} else if ( type.compare(EDIT) == 0 ){
					formField = FormField::createEditField(parent, GetModuleHandle(0), L"", L"");
				} else if ( type.compare(COMBO) == 0 ){
					formField = FormField::createComboBox(parent, GetModuleHandle(0), L"", L"", config);
				} else if ( type.compare(REFERENCE) == 0 ){
					formField = FormField::createReferenceField(parent, GetModuleHandle(0), L"", L"", config);
				} else {
					formField = FormField::createEditField(parent, GetModuleHandle(0), L"", L"");
				}

				c->value = formField;
				c->setupForField = field["name"].getString();

			}
		}
	}

	if ( !hasEmptyCondition ){
		addEmptyCondition(type, parent);
	}
}

void ConditionManager::addEmptyCondition(string type, HWND parent){
	
	Connection conn;
	Database db = conn.getDatabase(DATABASE);

	stringstream template_id;
	template_id << "template/" << type;
	Document doc = db.getDocument(template_id.str());
	Value v = doc.getData();
	if ( v.isObject() ){
		Object templ = v.getObject();
		if ( templ["fields"].isArray() ){
			Array fields = templ["fields"].getArray();
			vector<Object>* fieldsVector = new vector<Object>();

			HWND fieldCombo = CreateWindowEx(WS_EX_CLIENTEDGE, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP,
				0, 0, 0, 0, parent, (HMENU)IDC_ADD_VIEW_FIELD, GetModuleHandle(0), NULL);

			HWND compareCombo = CreateWindowEx(WS_EX_CLIENTEDGE, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP,
				0, 0, 0, 0, parent, NULL, GetModuleHandle(0), NULL);

			ShowWindow(fieldCombo, SW_HIDE);
			ShowWindow(compareCombo, SW_HIDE);
			SetWindowFont(fieldCombo, DEFAULT_FONT, false);
			SetWindowFont(compareCombo, DEFAULT_FONT, false);

			for(unsigned i=0; i<fields.size(); i++){
				Object field = fields[i].getObject();
				string name = field["name"].getString();
				string label = field["label"].getString();

				wstring wlabel = s2ws(label);

				fieldsVector->push_back(field);

				SendMessage(fieldCombo, CB_ADDSTRING, 0, (LPARAM)wlabel.c_str());
				
				SetWindowLong(fieldCombo, GWL_USERDATA, (ULONG_PTR)fieldsVector);

			}

			vector<string>* compareOperators = new vector<string>();
			SendMessage(compareCombo, CB_ADDSTRING, 0, (LPARAM)L"is equal to");
			compareOperators->push_back("==");
			SendMessage(compareCombo, CB_ADDSTRING, 0, (LPARAM)L"is not equal to");
			compareOperators->push_back("!=");
			SendMessage(compareCombo, CB_ADDSTRING, 0, (LPARAM)L"is greater than");
			compareOperators->push_back(">");
			SendMessage(compareCombo, CB_ADDSTRING, 0, (LPARAM)L"is less than");
			compareOperators->push_back("<");
		
			SetWindowLong(compareCombo, GWL_USERDATA, (ULONG_PTR)compareOperators);

			Condition* c = new Condition;
			c->fieldCombo = fieldCombo;
			c->compareCombo = compareCombo;
			c->value = NULL;
			int a = conditions->size();

			conditions->push_back(c);
		}
	}
}

vector<Condition *>* ConditionManager::getConditions(){
	return conditions;
}
