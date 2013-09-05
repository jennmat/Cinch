
#include "stdafx.h"


class ArrayDelegate : public BaseDelegate {
private:
	Array data;
	int rowCount;
	
public:
	ArrayDelegate(Detail* d, int i) : BaseDelegate(d, i) {}
	Array storeValuesToArray(Array a);
	void setData(Array array);

	int totalRows();
	void cellContent(int row, int col, wstring &content);
	void setupEditorForCell(HWND editor, int row, int col);
	void editingFinished(HWND hwnd, int row, int col);
	void prepareNewRow(int row);
	
	bool allowEditing(int col);
	bool allowHeaderTitleEditing(int col);
	HWND editorForColumn(int col, HWND parent, HINSTANCE hInst);
	void willLoseFocus();
	bool allowNewRows();
	bool allowNewColumns();

};