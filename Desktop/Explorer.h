
#include "stdafx.h"


/* RenameItemHandler.cpp */
void HandleExplorerRenames(HWND);
LRESULT CALLBACK HandleExplorerRenamesProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

/*DragDropHandler.cpp */
void HandleDragDrop(HWND);
LRESULT CALLBACK HandleDragDropProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	

/*ContextMenuHandler.cpp */
void HandleContextMenu(HWND);
LRESULT CALLBACK HandleContextMenuProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
INT_PTR CALLBACK HandleAddExistingDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void EnumerateChildrenRecursively(HWND tree, HTREEITEM parent, std::function<void (HWND,HTREEITEM)> func);

auto CleanupItemData = [](HWND tree, HTREEITEM hitem){
	if ( hitem == NULL ) return;
	TVITEMEX item;
	item.mask = TVIF_PARAM;
	item.hItem = hitem;
	TreeView_GetItem(tree, &item);

	Object* obj = (Object *)item.lParam;
	delete obj;
	
};

/* Explorer.cpp */
void CreateApplicationExplorer(HWND tree);
void DestroyApplicationExplorer(HWND tree);

class Explorer {
private:
	Object doc;
	string id;
	string rev;
	string perspective;
	void AddMenuItems(HWND tree, HTREEITEM parent, const Array& items, int level);

public:
	Explorer();
	~Explorer();
	bool isInitialized();
	void buildExplorer(HWND tree, const string& perspective);
	bool saveChanges(HWND tree);
	void deleteRoot();
	void addRoot(char* label);
	Object* getRoot();
};
