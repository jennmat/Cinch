
#include <stdafx.h>
#include "CinchGrid.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class ArrayOfObjectsDelegate: public GridDelegate {

private:
	int rowCount;
	wstring view;
	vector<vector<wstring> > data;
	vector<string> fields;
	vector<wstring> titles;
	Detail * detail;
	int fieldId;
public:
	ArrayOfObjectsDelegate(Detail *, int);
	void setData(Array array);
	void addColumn(string field, wstring title);

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
};
