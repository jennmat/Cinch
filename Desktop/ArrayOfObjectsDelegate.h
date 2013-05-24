
#include <stdafx.h>
#include "CinchGrid.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class ArrayOfObjectsDelegate: public GridDelegate {

private:
	HWND parent;
	int rowCount;
	wstring view;
	Array data;
	vector<string> fields;
	vector<wstring> titles;
	vector<FormField*> editors;
	vector<string> editorTypes;
	vector<Value> editorConfigs;
	vector<int> widths;
	Detail * detail;
	CinchGrid* grid;
	int fieldId;
public:
	ArrayOfObjectsDelegate(Detail *, int);
	void setData(Array array);
	void addColumn(string field, wstring label, string editorType, Value config);

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

	void deserializeUIElements(HWND hwnd, Object obj);
	Object serializeUIElements();

	void willReloadData();
	void didReloadData();
	void didSelectRow(int);

	void didChangeColumnWidth(int, int);

	void setGrid(CinchGrid *);
};
