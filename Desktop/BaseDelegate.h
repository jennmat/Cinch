
#include <stdafx.h>
#include "CinchGrid.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class BaseDelegate : public GridDelegate {

protected:
	HWND parent;
	wstring view;
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
	BaseDelegate(Detail *, int);
	~BaseDelegate();
	void setData(Array array);
	void addColumn(string field, wstring label, string editorType, Value config);

	int totalColumns();
	int columnWidth(int column);
	int rowHeight();

	void headerContent(int, wstring &);

	bool stickyHeaders();
	
	bool rowSelection();

	bool drawHorizontalGridlines();
	bool drawVerticalGridlines();

	
	HFONT getFont();
	HFONT getEditFont();

	
	void headerContextClick(HWND grid, int x, int y);

	void deserializeUIElements(HWND hwnd, Object obj);
	void serializeUIElements(Object&);

	void willReloadData();
	void didReloadData();
	void didSelectRow(int);

	void didChangeColumnWidth(int, int);

	void setGrid(CinchGrid *);

	bool allowSorting(int col);
	void sortAscending(int col);
	void sortDescending(int col);
	void sortOff(int col);
};
