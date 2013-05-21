
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;

#define PAGESIZE 200

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


	viewResults = db.viewResults(s, ws2s(view), PAGESIZE, 0);
	
	rowCount = viewResults["total_rows"].getInt();
	data = new wchar_t*[PAGESIZE];
	rownums = new int[PAGESIZE];
	docids = new string[PAGESIZE];

	memset(data, NULL, PAGESIZE);

	int i = 0;
	if ( viewResults["total_rows"].getInt() > 0 ){
	   Array rows = viewResults["rows"].getArray();
	   for(unsigned int j=0; j<rows.size(); j++){
		   Object o = rows[j].getObject();
		   wstring w = s2ws(o["key"].getString());

			data[i] = new wchar_t[80];
			rownums[i] = i;
			docids[i] = o["id"].getString();

			wcscpy_s(data[i], 80, w.c_str());
			i++;
			
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

void CouchViewDelegate::loadPage(int row){
	/* Load some more documents  */
	Database db = conn.getDatabase(DATABASE);
		
	string s = ws2s(design);
	int ix = s.find("/");
	s = s.substr(ix);

	int j = row - 1;
	int skip = 1;
	while ( j>=0 && rownums[j % PAGESIZE] != j ){
		j--;
		skip++;
	}


	Object obj;
	int i;
	if ( j < 0 ){
		obj = db.viewResults(s, ws2s(view), PAGESIZE, skip-1, false);
		i = skip-1;
	} else {
		obj = db.viewResultsFromStartDocId(s, ws2s(view), Value(ws2s(data[j%PAGESIZE])), docids[j%PAGESIZE], PAGESIZE, skip);
		i = rownums[j%PAGESIZE] + skip;
	}


	if ( obj["total_rows"].getInt() > 0 ){
		Array rows = obj["rows"].getArray();
		for(unsigned int j=0; j<rows.size(); j++){
			Object o = rows[j].getObject();
			wstring w = s2ws(o["key"].getString());
			delete data[i%PAGESIZE];
			data[i%PAGESIZE] = new wchar_t[80];
			wcscpy_s(data[i%PAGESIZE], 80, w.c_str());
			rownums[i%PAGESIZE] = i;
			docids[i%PAGESIZE] = o["id"].getString();
			i++;
		}
	}
}

const wchar_t* CouchViewDelegate::cellContent(int row, int col)
{
	if ( rownums[row % PAGESIZE] != row ){
		loadPage(row);
	}

		
	return data[row % PAGESIZE];

}

bool CouchViewDelegate::stickyHeaders(){
	return true;
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
	if ( rownums[row % PAGESIZE] != row ){
		loadPage(row);
	}

	if (viewResults["total_rows"].getInt() > 0 ){
	   return docids[row % PAGESIZE];
	}

	return "";
}


void CouchViewDelegate::headerContextClick(HWND grid, int x, int y){
}


void CouchViewDelegate::willReloadData(){
}

void CouchViewDelegate::didReloadData(){
	if ( viewInitialized == false ) return;
	if ( selectedDocId.length() == 0 ) return;
	bool found = false;
	for(int i=0; i<PAGESIZE && !found; i++){
		if ( docids[i].compare(selectedDocId) == 0 ){
			grid->SetActiveRow(i+1, true);
			grid->ScrollRowIntoView(i+1);
			found = true;
		}
	}
	if ( !found ){
		if ( rowCount > 0 ){
			grid->SetActiveRow(1);
			grid->ScrollRowIntoView(1);
		}
	}
}

void CouchViewDelegate::didSelectRow(int row){
	if ( rownums[row % PAGESIZE] != row ){
		loadPage(row);
	}
	selectedDocId = docids[row];
}


void CouchViewDelegate::setGrid(CinchGrid* g){
	grid = g;
}