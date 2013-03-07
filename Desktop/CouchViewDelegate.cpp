
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;


CouchViewDelegate::CouchViewDelegate(Connection& _conn) : conn(_conn)
{
	rowCount = 0;
	viewInitialized = false;
}

void CouchViewDelegate::setView(const wstring& _design, const wstring& _view)
{
	view = _view;
	design = _design;
	viewInitialized = true;
	loadViewResults();
}

void CouchViewDelegate::loadViewResults(){

	if ( viewInitialized == false ) return;

	Database db = conn.getDatabase(DATABASE);

	string s = ws2s(design);
	int ix = s.find("/");
	s = s.substr(ix);


	viewResults = db.viewResults(s, ws2s(view), Value(""), 25);
	
	rowCount = viewResults["total_rows"].getInt();
	data = new wchar_t*[rowCount];
	memset(data, NULL, rowCount);

	int i = 0;
	if ( viewResults["total_rows"].getInt() > 0 ){
	   Array rows = viewResults["rows"].getArray();
	   for(unsigned int j=0; j<rows.size(); j++){
		   Object o = rows[j].getObject();
		   wstring w = s2ws(o["key"].getString());

			data[i] = new wchar_t[80];
			wcscpy_s(data[i], 80, w.c_str());
			i++;
			lastRead = o;
		}
	}
}

int CouchViewDelegate::totalRows()
{
	return rowCount;
}


int CouchViewDelegate::totalColumns(){
	return 1;
}


int CouchViewDelegate::columnWidth(int col){
	return 200;
}


int CouchViewDelegate::rowHeight(){
	return 25;
}

wchar_t* CouchViewDelegate::headerContent(int col)
{
	return L"Name";
}

const wchar_t* CouchViewDelegate::cellContent(int row, int col)
{
	if( data[row] != NULL ){
		return data[row];
	} else {
		/* Load some more documents 
		Object obj = db.viewResulsFromStartDocId("friends", "by-name", lastRead["key"], lastRead["id"].getString(), 25);
		int i = row;
		if ( obj["total_rows"].getInt() > 0 ){
			Array rows = obj["rows"].getArray();
			for(unsigned int j=0; j<rows.size(); j++){
				Object o = rows[j].getObject();
				wstring w = s2ws(o["key"].getString());

				data[i] = new wchar_t[80];
				wcscpy_s(data[i], 80, w.c_str());
				i++;
				lastRead = o;
			}
		}

		return data[row];*/
		return TEXT("");
	}
}

bool CouchViewDelegate::stickyHeaders(){
	return false;
}

bool CouchViewDelegate::drawHorizontalGridlines(){
	return true;
}

bool CouchViewDelegate::drawVerticalGridlines(){
	return true;
}

bool CouchViewDelegate::rowSelection(){
	return true;
}


void CouchViewDelegate::setupEditorForCell(HWND editor, int row, int col)
{
}

bool CouchViewDelegate::allowEditing(int col){
	return false;
}


bool CouchViewDelegate::allowHeaderTitleEditing(int col){
	return true;
}

HWND CouchViewDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	return NULL;
}

void CouchViewDelegate::editingFinished(HWND hwnd, int row, int col){
}

void CouchViewDelegate::willLoseFocus(){
}

bool CouchViewDelegate::allowNewRows(){
	return false;
}

bool CouchViewDelegate::allowNewColumns(){
	return false;
}

void CouchViewDelegate::prepareNewRow(int row){
}

HFONT CouchViewDelegate::getFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}

HFONT CouchViewDelegate::getEditFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}


string CouchViewDelegate::getDocumentIdForRow(int row){
	if( row < 0 ) return "";
	if (viewResults["total_rows"].getInt() > 0 ){
	   Array rows = viewResults["rows"].getArray();
	   Object o = rows[row].getObject();
	   return o["id"].getString();
	}

	return "";
}


void CouchViewDelegate::headerContextClick(HWND grid, int x, int y){
}