
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;


DetailViewDelegate::DetailViewDelegate(Detail * d, int i, const string& _design, const string& _view, const string& _viewDefId, const string& _docs_of_type)
	: BaseDelegate(d, i) {
	obj = nullptr;

	design = _design;
	view = _view;
	viewDefId = _viewDefId;
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
	source_document_type = o["cinch_type"].getString();
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

/*
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
}*/

int DetailViewDelegate::LoadSegment(int start_row, int len, wchar_t*** data){
	if ( (*obj)["rows"].isArray() ){
		Array rows = (*obj)["rows"].getArray();
		int data_index = 0;
		for(UINT row_index=start_row; row_index<min(len, totalRows()); row_index++){
			Object row = rows[row_index].getObject();

			Object doc = row["doc"].getObject();
			for(int col = 0; col<totalColumns(); col++){
				string name = fields[col];
				string value = serializeForDisplay(doc[name], name);
				wstring w = s2ws(value);
				int len = w.length() + sizeof(wchar_t);
				data[data_index][col] = new wchar_t[w.length()+sizeof(wchar_t)];
				wcscpy_s(data[data_index][col], len, w.c_str());

			}

			data_index++;
		}
		return data_index;
	}
	return 0;
}

void DetailViewDelegate::CleanupSegment(int len, wchar_t*** data){
	for(int i=0; i<len; i++){
		for(int col=0; col<totalColumns(); col++){
			delete data[i][col];
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
	o["view_definition_id"] = viewDefId;
	BaseDelegate::serializeUIElements(o);
}



bool DetailViewDelegate::allowSorting(int col){
	return true;
}

void DetailViewDelegate::sortByCol(int col){
	/* Find a view that sorts by id and the desired column */
	QueryOptions options;
	Array key;
	key.push_back(source_document_type);
	key.push_back(fields[col]);
	options.startKey = key;
	options.endKey = key;
	Object results = db.viewResults("all-views", "by-key", options);

	string d, v;

	if ( results["rows"].isArray() && results["rows"].getArray().size() > 0 ){
		Array rows = results["rows"].getArray();
		Object row = rows[0].getObject();
		Object value = row["value"].getObject();

		d = value["design"].getString();
		v = value["view"].getString();
	} else {
		/* Need to create a new view */
		Object def = db.getDocument(viewDefId).getData().getObject();
		stringstream design;
		design << "_design/" << def["design_name"].getString();
		string key = design.str();
		Object des = db.getDocument(design.str()).getData().getObject();

		Object views = des["views"].getObject();
							
		string map = def["map_template"].getString();
		stringstream key_str;
		key_str << "[doc." << source_document_type << ", doc." << fields[col] << "]";
		map = map.replace(map.find("__KEY__"), 7, key_str.str());

		stringstream viewname;

		viewname << "by-" << source_document_type << "-and-" << fields[col];

		views[viewname.str()]["map"] = map;

		des["views"] = views;

		db.createDocument(des, design.str());
		


		Object newView;
		Array viewKey;
		viewKey.push_back(source_document_type);
		viewKey.push_back(fields[col]);
		newView["key"] = viewKey;
		newView["view"] = viewname.str();
		Array currentViews = def["views"].getArray();
		currentViews.push_back(newView);
		def["views"] = currentViews;
					
		db.createDocument(def, viewDefId);

		d = def["design_name"].getString();
		v = viewname.str();
	}

	delete obj;

	QueryOptions viewQueryOptions;
	viewQueryOptions.includeDocs = true;	
	Array start;
	start.push_back(source_document_id);
	Array end;
	end.push_back(source_document_id);
	end.push_back(Object());
	if ( descending ){
		viewQueryOptions.startKey = end;
		viewQueryOptions.endKey = start;
	} else {
		viewQueryOptions.startKey = start;
		viewQueryOptions.endKey = end;
	}
	viewQueryOptions.descending = descending;
	obj = new Object(db.viewResults(d, v, viewQueryOptions));
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