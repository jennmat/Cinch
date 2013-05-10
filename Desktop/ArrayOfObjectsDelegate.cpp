
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;


ArrayOfObjectsDelegate::ArrayOfObjectsDelegate(Detail * _d, int _fieldId){
	detail = _d;
	fieldId = _fieldId;
}

int ArrayOfObjectsDelegate::totalRows()
{
	return rowCount;
}

void ArrayOfObjectsDelegate::addColumn(std::string field, std::wstring title, string editorType, Value config){
	fields.push_back(field);
	titles.push_back(title);
	widths.push_back(250);
	editorTypes.push_back(editorType);
	editorConfigs.push_back(config);
}

void ArrayOfObjectsDelegate::setData(Array array){
	data = array;
	rowCount = array.size();
}

int ArrayOfObjectsDelegate::totalColumns(){
	int a = fields.size();
	return a;
}

int ArrayOfObjectsDelegate::columnWidth(int col){
	return widths[col];
}

int ArrayOfObjectsDelegate::rowHeight(){
	return 25;
}

wchar_t* ArrayOfObjectsDelegate::headerContent(int col)
{
	wchar_t* result;

	wstring title = titles[col];
	int size = title.size() + sizeof(wchar_t);
	result = new wchar_t[size];
	wcscpy_s(result, size, title.c_str());
	return result;
}

const wchar_t* ArrayOfObjectsDelegate::cellContent(int row, int col)
{
	wchar_t* result;

	if ( data[row].isObject() ){
		Object o = data[row].getObject();
		string field = fields[col];
		editorForColumn(col, parent, GetModuleHandle(0));

		if ( data[row].isObject() ){
			string s = editors[col]->toString(data[row].getObject());
			wstring ws = s2ws(s);
			int len = ws.length() + sizeof(wchar_t);
			result = new wchar_t[len];
			wcscpy_s(result, len, ws.c_str());
			return result;
		}
	}
	return L"";
}

bool ArrayOfObjectsDelegate::stickyHeaders(){
	return false;
}

bool ArrayOfObjectsDelegate::drawHorizontalGridlines(){
	return true;
}

bool ArrayOfObjectsDelegate::drawVerticalGridlines(){
	return true;
}

bool ArrayOfObjectsDelegate::rowSelection(){
	return false;
}


void ArrayOfObjectsDelegate::setupEditorForCell(HWND editor, int row, int col){
	if ( row >= rowCount ){
		SetWindowText(editor, L"");
	} else {
		editors[col]->loadValue(data[row].getObject());

		/*editors[col]->loadValue();

		if (editorTypes[col].compare(DATEPICKER) == 0 ){
			const wchar_t* timeStr = this->cellContent(row, col);
			if (timeStr == NULL ) return;

			SYSTEMTIME time;
			int month, day, year;
			GetLocalTime(&time);

			if ( wcslen(timeStr) > 0 ){
				swscanf_s(timeStr, TEXT("%d-%d-%d"), &year, &month, &day);
				time.wMonth = month;
				time.wDay = day;
				time.wYear = year;
			}

			DateTime_SetSystemtime(editor, GDT_VALID, &time);
			return;

		} else {
			const wchar_t* c = cellContent(row, col);
			SetWindowText(editor, c);
		}*/
	}
}

bool ArrayOfObjectsDelegate::allowEditing(int col){
	return true;
}


bool ArrayOfObjectsDelegate::allowHeaderTitleEditing(int col){
	return true;
}

HWND ArrayOfObjectsDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	if ( editors[col] == NULL ){
		FormField* formField;
		string type = editorTypes[col];

		if ( type.compare(DATEPICKER) == 0 ){
			formField = FormField::createDatePicker(parent, GetModuleHandle(0), fields[col], L"", true);
		} else if ( type.compare(RADIO) == 0 ){
			formField = FormField::createRadioGroup(parent, GetModuleHandle(0), fields[col], L"", true);
		} else if ( type.compare(NUMBER) == 0 ){
			formField = FormField::createNumberField(parent, GetModuleHandle(0), fields[col], L"", true);
		} else if ( type.compare(YESNO) == 0 ){
			formField = FormField::createYesNoField(parent, GetModuleHandle(0), fields[col], L"", true);
		} else if ( type.compare(MULTILINE) == 0 ){
			formField = FormField::createMultilineText(parent, GetModuleHandle(0), fields[col], L"", true);
		} else if ( type.compare(EDIT) == 0 ){
			formField = FormField::createEditField(parent, GetModuleHandle(0), fields[col], L"", true);
		} else if ( type.compare(COMBO) == 0 ){
			formField = FormField::createComboBox(parent, GetModuleHandle(0), fields[col], L"", editorConfigs[col], true);
		} else if ( type.compare(REFERENCE) == 0 ){
			formField = FormField::createReferenceField(parent, GetModuleHandle(0), fields[col], L"", editorConfigs[col], true);
		} else {
			formField = FormField::createEditField(parent, GetModuleHandle(0), fields[col], L"", true);
		}

		editors[col] = formField;
	}
	return editors[col]->getControl();
}

void ArrayOfObjectsDelegate::editingFinished(HWND hwnd, int row, int col){
	if ( row < 0 ) return;
	
	data[row] = editors[col]->storeValue(data[row].getObject());

}

void ArrayOfObjectsDelegate::willLoseFocus(){
	detail->getForm()->SaveDocument(fieldId);
}

bool ArrayOfObjectsDelegate::allowNewRows(){
	return true;
}

bool ArrayOfObjectsDelegate::allowNewColumns(){
	return false;
}

void ArrayOfObjectsDelegate::prepareNewRow(int row){
	data.push_back(Value());
	rowCount++;
}

HFONT ArrayOfObjectsDelegate::getFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}

HFONT ArrayOfObjectsDelegate::getEditFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}



Array ArrayOfObjectsDelegate::storeValuesToArray(Array a){
	/*unsigned int i = 0;
	for(i=0; i<a.size(); i++){
		Object obj = a[i].getObject();
		vector<wstring> rowData = data[i];
		for(unsigned int j=0; j<fields.size(); j++){
			obj[fields[j]] =ws2s(rowData[j]);
		}

		a[i] = obj;
	}

	for(; i<data.size(); i++){
		Object obj;
		vector<wstring> rowData = data[i];
		for(unsigned int j=0; j<fields.size(); j++){
			obj[fields[j]] =ws2s(rowData[j]);
		}
		a.push_back(obj);
	}

	return a;*/
	return data;
}

void ArrayOfObjectsDelegate::deserializeUIElements(HWND _parent, Object obj){
	parent = _parent;
	fields.clear();
	titles.clear();
	widths.clear();
	editorConfigs.clear();
	editorTypes.clear();
	editors.clear();
	if( obj["columns"].isArray() ){
		Array columns = obj["columns"].getArray();
		for(unsigned int i=0; i<columns.size(); i++){
			Object col = columns[i].getObject();
			fields.push_back(col["name"].getString());
			editorTypes.push_back(col["cinch_type"].getString());
			editors.push_back(NULL);
			editorConfigs.push_back(col["config"]);
			titles.push_back(s2ws(col["label"].getString()));
			if ( col["width"].isInteger() && col["width"].getInt() > 0 ){
				widths.push_back(col["width"].getInt());
			} else {
				widths.push_back(250);
			}
		}
	}
}

Object ArrayOfObjectsDelegate::serializeUIElements(){
	Object o;
	Array columns;
	for(unsigned int i=0; i<fields.size(); i++){
		Object col;
		col["name"] = Value(fields[i]);
		col["cinch_type"] = editorTypes[i];
		col["label"] =ws2s(titles[i]);
		col["width"] = Value(250);
		columns.push_back(col);
	}
	o["columns"] = columns;
	return o;
}

void ArrayOfObjectsDelegate::headerContextClick(HWND grid, int x, int y){
	HMENU hPopupMenu = CreatePopupMenu();
	InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EDIT_COLUMNS, L"Edit Columns...");
	POINT point;
	point.x = x;
	point.y = y;
    ClientToScreen(grid, &point);
	TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, detail->getDetailHwnd(), NULL);
}

void ArrayOfObjectsDelegate::willReloadData(){
}

void ArrayOfObjectsDelegate::didReloadData(){
}

void ArrayOfObjectsDelegate::didSelectRow(int){
}

void ArrayOfObjectsDelegate::setGrid(CinchGrid*){
}