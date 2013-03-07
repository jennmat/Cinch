// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <WindowsX.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#include <CommCtrl.h>

#include "Resource.h"
#include "JsonBox.h"
#include "util.h"

// TODO: reference additional headers your program requires here
class Form;
class FormDelegate;

#include "CinchGrid.h"
#include "CinchDesigner.h"

#include "FormField.h"
#include "FormLayout.h"
#include "FormDelegate.h"
#include "Detail.h"
#include "Form.h"
#include "ReadOnlyDelegate.h"
#include "BlankDelegate.h"
#include "ArrayOfObjectsDelegate.h"
#include "util.h"
#include "MaskEdit.h"
#include "NotifyParent.h"

#include <CommCtrl.h>
#include <Windows.h>

#define ROW_HEIGHT 25
#define LABEL_WIDTH 150
#define LABEL_HEIGHT 25
#define ROW_MARGIN 7
#define CONTROL_WIDTH 200
#define CONTROL_HEIGHT 25
#define MARGIN 10
#define LAYOUT_MARGIN 15

#define IDM_TABULAR		34009
#define IDM_TEXTAREA	34010

#define DATABASE "bugs"