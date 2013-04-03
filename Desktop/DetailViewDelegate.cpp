
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;

DetailViewDelegate::DetailViewDelegate(){
	obj = NULL;
}

DetailViewDelegate::DetailViewDelegate(string _design, string _view, string _startkey_from, string _endkey_from, string _docs_of_type){
	obj = NULL;

	design = _design;
	view = _view;
	startkey_from = _startkey_from;
	endkey_from = _endkey_from;
	shows_docs_of_type = _docs_of_type;

	
}

int DetailViewDelegate::totalRows()
{
	if ( obj == NULL ) return 0;

	return (*obj)["total_rows"].isInteger() ? (*obj)["total_rows"].getInt() : 0;
}

void DetailViewDelegate::LoadDocument(string database, Object o){
	Connection conn;
	Database db = conn.getDatabase(database);
	Value startkey = o[startkey_from];
	Value endkey = o[endkey_from];

	obj = new Object(db.viewResults(design, view, startkey, endkey));
	
}

void DetailViewDelegate::addColumn(std::string field, std::wstring title, string editorType){
	fields.push_back(field);
	titles.push_back(title);
	widths.push_back(250);
	editorTypes.push_back(editorType);
}


int DetailViewDelegate::totalColumns(){
	return fields.size();
}

int DetailViewDelegate::columnWidth(int col){
	return widths[col];
}

int DetailViewDelegate::rowHeight(){
	return 25;
}

wchar_t* DetailViewDelegate::headerContent(int col)
{
	wchar_t* result;

	wstring title = titles[col];
	int size = title.size() + sizeof(wchar_t);
	result = new wchar_t[size];
	wcscpy_s(result, size, title.c_str());
	return result;
}

const wchar_t* DetailViewDelegate::cellContent(int row, int col)
{

	if ( (*obj)["rows"].isArray() ){
		Array rows = (*obj)["rows"].getArray();
		if ( (unsigned)row < rows.size() ){
			Object r = rows[row].getObject();
			string id = r["id"].getString();

			Connection conn;
			Database d = conn.getDatabase(DATABASE);
			Document doc = d.getDocument(id);
			Object data = doc.getData().getObject();

			string field = fields[col];
			string value = data[field].getString();

			wstring val = s2ws(value);
			int size = val.size() + sizeof(wchar_t);
			wchar_t* wval = new wchar_t[size];
			wcscpy_s(wval, size, val.c_str());
			return wval;
		}
	}
	
	return L"";
}

bool DetailViewDelegate::stickyHeaders(){
	return false;
}

bool DetailViewDelegate::drawHorizontalGridlines(){
	return true;
}

bool DetailViewDelegate::drawVerticalGridlines(){
	return true;
}

bool DetailViewDelegate::rowSelection(){
	return true;
}


void DetailViewDelegate::setupEditorForCell(HWND editor, int row, int col){
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

bool DetailViewDelegate::allowEditing(int col){
	return false;
}


bool DetailViewDelegate::allowHeaderTitleEditing(int col){
	return false;
}

HWND DetailViewDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
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

void DetailViewDelegate::editingFinished(HWND hwnd, int row, int col){
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

void DetailViewDelegate::willLoseFocus(){
}

bool DetailViewDelegate::allowNewRows(){
	return true;
}

bool DetailViewDelegate::allowNewColumns(){
	return false;
}

void DetailViewDelegate::prepareNewRow(int row){
	vector<wstring> rowData;
	for(unsigned int i=0; i<fields.size(); i++){
		rowData.push_back(L"");
	}
	data.push_back(rowData);
	rowCount++;
}

HFONT DetailViewDelegate::getFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}

HFONT DetailViewDelegate::getEditFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}


void DetailViewDelegate::deserializeUIElements(Object obj){
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

Object DetailViewDelegate::serializeUIElements(){
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

	o["design"] = design;
	o["view"] = view;
	o["startkey_with_value_of"] = startkey_from;
	o["endkey_with_value_of"] = endkey_from;
	o["shows_docs_of_type"] = shows_docs_of_type;
	return o;

}

void DetailViewDelegate::headerContextClick(HWND grid, int x, int y){
	HMENU hPopupMenu = CreatePopupMenu();
	InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EDIT_COLUMNS, L"Edit Columns...");
	POINT point;
	point.x = x;
	point.y = y;
    ClientToScreen(grid, &point);
	//TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, detail->getDetailHwnd(), NULL);
}


void DetailViewDelegate::willReloadData(){
}

void DetailViewDelegate::didReloadData(){
}

void DetailViewDelegate::didSelectRow(int row){
}

void DetailViewDelegate::setGrid(CinchGrid *){
}