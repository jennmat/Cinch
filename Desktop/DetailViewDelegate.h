
#include <stdafx.h>
#include "CinchGrid.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class DetailViewDelegate: public BaseDelegate {

private:
	DetailViewDelegate();
	int rowCount;
	string database;
	string design;
	string view;
	string viewDefId;
	string referencingField;
	bool descending;
	string shows_docs_of_type;
	string source_document_id;
	string source_document_type;
	Object* obj;
	bool includeDocs;
	bool allowEdit;
	vector<vector<wstring> > data;
	vector<HWND> editors;
	Object config;
	int fieldId;

	void sortByCol(int col);
	
public:
	DetailViewDelegate(Detail*, int, const string& design, const string& view, const string& viewDefId, const string& docs_of_type, const string& referencingField);
	~DetailViewDelegate();
	void LoadDocument(const string& database, Object& obj);
	void setConfig(Object);
	void setIncludeDocs(bool);
	void serializeUIElements(Object& obj);
	
	int totalRows();
	int LoadSegment(int start_row, int len, wchar_t*** data);
	void CleanupSegment(int len, wchar_t*** data);
	void setupEditorForCell(HWND editor, int row, int col, wchar_t*** data);
	void editingFinished(HWND hwnd, int row, int col, wchar_t*** data);
	void prepareNewRow(int row);
	
	bool allowEditing(int col);
	bool allowHeaderTitleEditing(int col);
	HWND editorForColumn(int col, HWND parent, HINSTANCE hInst);
	void willLoseFocus();
	bool allowNewRows();
	bool allowNewColumns();

	bool allowSorting(int col);
	void sortAscending(int col);
	void sortDescending(int col);
	void sortOff(int col);
};
