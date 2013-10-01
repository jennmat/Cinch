
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
	void LoadSegment(int start_row, int len, wchar_t*** data, int*, int*);
	void CleanupSegment(int rows, int cols, wchar_t*** data);
	void setupEditorForCell(HWND editor, int row, int col, wchar_t*** data);
	void editingFinished(HWND hwnd, int row, int col, wchar_t*** data);
	void prepareNewRow(int row);
	
	bool allowEditing(int col);
	bool allowHeaderTitleEditing(int col);
	HWND editorForColumn(int col, HWND parent, HINSTANCE hInst);
	void willLoseFocus();
	bool allowNewRows();
	bool allowNewColumns();

};