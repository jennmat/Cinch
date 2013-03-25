
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

void ArrayOfObjectsDelegate::addColumn(std::string field, std::wstring title, string editorType){
	fields.push_back(field);
	titles.push_back(title);
	widths.push_back(250);
	editorTypes.push_back(editorType);
}

void ArrayOfObjectsDelegate::setData(Array array){
	data.clear();
	rowCount = array.size();
	for(unsigned int i=0; i<array.size(); i++){
		Object o = array[i].getObject();
		vector<wstring> rowData;
		for(unsigned int j=0; j<fields.size(); j++){
			string data = o[fields[j]].getString();
			wstring w = s2ws(data);
			rowData.push_back(w);	
		}
		data.push_back(rowData);
	} 

	for(unsigned int i=0; i<data.size(); i++){
		vector<wstring> row = data[i];
		for(unsigned int j=0; j<row.size(); j++){
			wstring jk = row[j];
			int a = 1;
		}
	}
}

int ArrayOfObjectsDelegate::totalColumns(){
	return fields.size();
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

    vector<wstring> rowData = data[row];
	wstring data = L"";
	if ( col < (int)rowData.size() ) {
		data = rowData[col];
	}
	
	result = new wchar_t[80];
	wcscpy_s(result, 80, data.c_str());
	return result;
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
		}
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
		if ( editorTypes[col].compare(DATEPICKER) == 0 ){
			editors[col] =  CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				0, 0, 0, 0, parent, NULL, hInst, NULL);
			DateTime_SetFormat(editors[col], L"yyyy-MM-dd");
		} else {
			editors[col] = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				0, 0, 0, 0, parent, NULL, hInst, NULL);
		}
	}
	return editors[col];
}

void ArrayOfObjectsDelegate::editingFinished(HWND hwnd, int row, int col){
	if ( row < 0 ) return;
	string field = fields[col];

	if ( editorTypes[col].compare(DATEPICKER) == 0 ){
		SYSTEMTIME time;
		DateTime_GetSystemtime(hwnd, &time);
		wchar_t* text = new wchar_t[20];
		memset(text, 0, 20);
		GetDateFormatEx(LOCALE_NAME_INVARIANT, 0, &time, L"yyyy-MM-dd", text, 20, NULL);

		data[row][col] = text;

	} else {
		int len = GetWindowTextLength(hwnd);
		len += sizeof(wchar_t);

		wchar_t* text = new wchar_t[len];
		GetWindowText(hwnd, text, len);
		data[row][col] = text;
	}
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
	vector<wstring> rowData;
	for(unsigned int i=0; i<fields.size(); i++){
		rowData.push_back(L"");
	}
	data.push_back(rowData);
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
	unsigned int i = 0;
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

	return a;
}

void ArrayOfObjectsDelegate::deserializeUIElements(Object obj){
	fields.clear();
	titles.clear();
	widths.clear();
	if( obj["columns"].isArray() ){
		Array columns = obj["columns"].getArray();
		for(unsigned int i=0; i<columns.size(); i++){
			Object col = columns[i].getObject();
			fields.push_back(col["name"].getString());
			editorTypes.push_back(col["cinch_type"].getString());
			editors.push_back(NULL);
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