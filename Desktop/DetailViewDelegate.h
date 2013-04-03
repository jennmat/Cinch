
#include <stdafx.h>
#include "CinchGrid.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class DetailViewDelegate: public GridDelegate {

private:
	DetailViewDelegate();
	int rowCount;
	string database;
	string design;
	string view;
	string startkey_from;
	string endkey_from;
	string shows_docs_of_type;
	Object* obj;
	vector<vector<wstring> > data;
	vector<string> fields;
	vector<wstring> titles;
	vector<HWND> editors;
	vector<string> editorTypes;
	vector<int> widths;
	int fieldId;
public:
	DetailViewDelegate(string design, string view, string startkey_from, string endkey_from, string docs_of_type);
	void LoadDocument(string database, Object obj);
	void addColumn(string field, wstring label, string editorType);

	int totalRows();
	int totalColumns();
	int columnWidth(int column);
	int rowHeight();

	wchar_t* headerContent(int);
	const wchar_t* cellContent(int, int);

	bool stickyHeaders();
	
	bool rowSelection();

	bool drawHorizontalGridlines();
	bool drawVerticalGridlines();

	bool allowEditing(int);
	bool allowHeaderTitleEditing(int);
	void setupEditorForCell(HWND editor, int row, int col);
	HWND editorForColumn(int, HWND parent, HINSTANCE hInst) ;
	void editingFinished(HWND editor, int row, int col);
	void willLoseFocus();

	bool allowNewRows();
	bool allowNewColumns();
	void prepareNewRow(int row);

	HFONT getFont();
	HFONT getEditFont();

	Array storeValuesToArray(Array obj);

	void headerContextClick(HWND grid, int x, int y);

	void deserializeUIElements(Object obj);
	Object serializeUIElements();

	void willReloadData();
	void didReloadData();

	void didSelectRow(int row);

	void setGrid(CinchGrid*);
};
