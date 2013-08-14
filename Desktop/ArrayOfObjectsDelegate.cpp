
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

ArrayOfObjectsDelegate::~ArrayOfObjectsDelegate(){
	for(int i=0; i<totalColumns(); i++){
		if ( editors[i] != NULL ){
			delete editors[i];
		}
	}
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

void ArrayOfObjectsDelegate::headerContent(int col, wstring &content)
{
	content = titles[col];

}

void ArrayOfObjectsDelegate::cellContent(int row, int col, wstring &content)
{
	if ( data[row].isObject() ){
		Object o = data[row].getObject();
		string field = fields[col];
		editorForColumn(col, parent, GetModuleHandle(0));

		if ( data[row].isObject() ){
			string s = editors[col]->toString(data[row].getObject());
			content = s2ws(s);
		}
	}
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

		formField = createFieldForType(parent, "", type, true);

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
			string field = col["field"].getString();
			Object fieldConfig = getTypeDefinition(field);
			
			fields.push_back(fieldConfig["_id"].getString());
			editorTypes.push_back(field);
			editors.push_back(NULL);
			editorConfigs.push_back(col["config"]);
			titles.push_back(s2ws(fieldConfig["label"].getString()));
			if ( col["width"].isInteger() && col["width"].getInt() > 0 ){
				widths.push_back(col["width"].getInt());
			} else {
				widths.push_back(250);
			}
		}
	}

	grid->reloadData();

}

Object ArrayOfObjectsDelegate::serializeUIElements(){
	Object o;
	Array columns;
	for(unsigned int i=0; i<fields.size(); i++){
		Object col;
		col["field"] = Value(fields[i]);
		col["width"] = Value(widths[i]);
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

void ArrayOfObjectsDelegate::setGrid(CinchGrid* g){
	grid = g;
}

void ArrayOfObjectsDelegate::didChangeColumnWidth(int col, int newWidth){
	widths[col] = newWidth;
	detail->getForm()->getDelegate()->formModified();
}


bool ArrayOfObjectsDelegate::allowSorting(int col){
	return false;
}

void ArrayOfObjectsDelegate::sortAscending(int col){
}

void ArrayOfObjectsDelegate::sortDescending(int col){
}

void ArrayOfObjectsDelegate::sortOff(int col){
}