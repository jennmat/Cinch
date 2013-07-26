
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


/* Explorer.cpp */
void CreateApplicationExplorer(HWND tree);
void DestroyApplicationExplorer(HWND tree);

class Explorer {
private:
	Object doc;
	string id;
	string rev;
	void AddMenuItems(HWND tree, const Array& items, int level);

public:
	Explorer();
	~Explorer();
	void buildExplorer(HWND tree);
	bool saveChanges();
};
