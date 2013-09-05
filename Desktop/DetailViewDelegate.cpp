
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;


DetailViewDelegate::DetailViewDelegate(Detail * d, int i, string _design, string _view, string _startkey_from, string _endkey_from, string _docs_of_type)
	: BaseDelegate(d, i) {
	obj = nullptr;

	design = _design;
	view = _view;
	startkey_from = _startkey_from;
	endkey_from = _endkey_from;
	shows_docs_of_type = _docs_of_type;

	includeDocs = false;
	allowEdit = false;
	descending = false;
}

void DetailViewDelegate::setIncludeDocs(bool _includeDocs){
	includeDocs = _includeDocs;
}


int DetailViewDelegate::totalRows()
{
	if ( obj == NULL ) return 0;
	if ( (*obj)["rows"].isArray() ) return (*obj)["rows"].getArray().size();
	return 0;
}

void DetailViewDelegate::LoadDocument(const string& database, Object& o){
	delete obj;
	source_document_id = o["_id"].getString();

	QueryOptions options;
	options.includeDocs = true;
	options.startKey = source_document_id;
	options.endKey = source_document_id;
	obj = new Object(db.viewResults(design, view, options));
	
}

DetailViewDelegate::~DetailViewDelegate(){
	delete obj;
}

void DetailViewDelegate::setConfig(Object _config){
	config = _config;
}


void DetailViewDelegate::cellContent(int row, int col, wstring &content)
{
	content = L"";

	if ( (*obj)["rows"].isArray() ){
		Array rows = (*obj)["rows"].getArray();
		if ( (unsigned)row < rows.size() ){
			Object r = rows[row].getObject();
			string name = fields[col];	
				
			string value;
			Object data = r["doc"].getObject();
			value = serializeForDisplay(data[name], name);

			content = s2ws(value);
			
		}
	}
	
	
}





void DetailViewDelegate::setupEditorForCell(HWND editor, int row, int col){
}

bool DetailViewDelegate::allowEditing(int col){
	return allowEdit;
}


bool DetailViewDelegate::allowHeaderTitleEditing(int col){
	return false;
}

HWND DetailViewDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	return NULL;
}

void DetailViewDelegate::willLoseFocus(){
}

bool DetailViewDelegate::allowNewRows(){
	return false;
}

bool DetailViewDelegate::allowNewColumns(){
	return false;
}

void DetailViewDelegate::prepareNewRow(int row){
}

void DetailViewDelegate::editingFinished(HWND hwnd, int row, int col){
}

void DetailViewDelegate::serializeUIElements(Object& o){
	o["design"] = design;
	o["view"] = view;

	BaseDelegate::serializeUIElements(o);
}



bool DetailViewDelegate::allowSorting(int col){
	return true;
}

void DetailViewDelegate::sortByCol(int col){
		/* Find a view that sorts by id and the desired column */
	QueryOptions options;
	Array key;
	key.push_back("_id");
	key.push_back(fields[col]);
	options.startKey = key;
	options.endKey = key;
	Object results = db.viewResults("all-views", "by-key", options);

	if ( results["rows"].isArray() && results["rows"].getArray().size() > 0 ){
		Array rows = results["rows"].getArray();
		Object row = rows[0].getObject();
		Object value = row["value"].getObject();

		string d = value["design"].getString();
		string v = value["view"].getString();

		delete obj;

		QueryOptions options;
		options.includeDocs = true;	
		Array start;
		start.push_back(source_document_id);
		Array end;
		end.push_back(source_document_id);
		end.push_back(Object());
		if ( descending ){
			options.startKey = end;
			options.endKey = start;
		} else {
			options.startKey = start;
			options.endKey = end;
		}
		options.descending = descending;
		obj = new Object(db.viewResults(d, v, options));
	} else {
		/* Need to create a new view */

	}
}

void DetailViewDelegate::sortAscending(int col){
	descending = false;
	sortByCol(col);
}

void DetailViewDelegate::sortDescending(int col){
	descending = true;
	sortByCol(col);
}

void DetailViewDelegate::sortOff(int col){
}