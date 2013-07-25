
#include "stdafx.h"

extern HWND tree;
extern HWND hWnd;

void HandleDragDrop(HWND hwnd){
	SetWindowSubclass(GetParent(hwnd), HandleDragDropProc, 0, 0);
}

bool DraggingInExplorer = false;
HTREEITEM Selected;

TVHITTESTINFO tvht; 
HTREEITEM hitTarget;
POINTS Pos;


LRESULT CALLBACK HandleDragDropProc(HWND h, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (message) {
	case WM_NOTIFY:
		{
		 
			if(((LPNMHDR)lParam)->code == TVN_BEGINDRAG)
			{
				HIMAGELIST hImg;
				LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lParam;
				hImg=TreeView_CreateDragImage(tree, lpnmtv->itemNew.hItem);
				ImageList_BeginDrag(hImg, 0, 0, 0);
				ImageList_DragEnter(tree,lpnmtv->ptDrag.x,lpnmtv->ptDrag.y);
				//ShowCursor(FALSE); 
				SetCapture(h); 
				DraggingInExplorer = TRUE;	
			}

		}
		break;

		case WM_MOUSEMOVE:
		{
			if (DraggingInExplorer) 
			{ 
				Pos = MAKEPOINTS(lParam);
				MapWindowPoints(hWnd, tree, (LPPOINT)&Pos, 1);
				ImageList_DragMove(Pos.x-32, Pos.y-25); // where to draw the drag from
				ImageList_DragShowNolock(FALSE);
				tvht.pt.x = Pos.x-20; // the highlight items should be as the same points as the drag
				tvht.pt.y = Pos.y-20; //
				if(hitTarget=(HTREEITEM)SendMessage(tree,TVM_HITTEST,NULL,(LPARAM)&tvht)) // if there is a hit
					SendMessage(tree,TVM_SELECTITEM,TVGN_DROPHILITE,(LPARAM)hitTarget);   // highlight it
			
			    ImageList_DragShowNolock(TRUE); 
			} 
		}
		break;

        case WM_LBUTTONUP:
        {
            if (DraggingInExplorer) 
            {
                ImageList_DragLeave(tree);
                ImageList_EndDrag();
                Selected=(HTREEITEM)SendMessage(tree,TVM_GETNEXTITEM,TVGN_DROPHILITE,0);
                SendMessage(tree,TVM_SELECTITEM,TVGN_CARET,(LPARAM)Selected);
                SendMessage(tree,TVM_SELECTITEM,TVGN_DROPHILITE,0);
                ReleaseCapture();
                ShowCursor(TRUE); 
                DraggingInExplorer = FALSE;
            }
        }
        break;
	}
	return DefSubclassProc(h, message, wParam, lParam);
}


