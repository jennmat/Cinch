// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <Shlwapi.h>

#include <stdint.h>

#include "Windowsx.h"

#include <strsafe.h>

#include <CommCtrl.h>
#include <string>

#include <Wincrypt.h>
#include <unordered_set>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG   
#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
#endif
#endif  // _DEBUG



#include <UIRibbon.h>
#include "Application.h"
#include "CommandHandler.h"
#include "RibbonFramework.h"
#include "PropertySet.h"
#include <uiribbonpropertyhelpers.h>

using namespace std;

class Form;
class FormDelegate;
class FormField;

struct ViewPair {
	string emitsDocsWithType;
	string design;
	string view;
};

struct FileWatchController {
	wchar_t* directory;
	bool pause;
	bool exit;
};

#include "Constants.h"
#include "Resource.h"

#include "Toolbar.h"
#include "CinchGrid.h"
#include "CouchViewDelegate.h"
// Comment out if you don't want replication 
//#include "Replication.h"
#include "CouchDB.hpp"
#include "util.h"
#include "ConditionManager.h"
// TODO: reference additional headers your program requires here

#include "JsonBox.h"
#include "util.h"


#include <objbase.h>
#include <shlobj.h>             // shell stuff
#include <sstream>
#include <functional>


//#define DATABASE "cinch-customer-contact-plugin-starter"
//#define DATABASE "cinch-business-starter"
//#define DATABASE "cinch-inventory-starter"
//#define DATABASE "cinch-starter"
#define DATABASE "cinch-real-estate-starter"
//#define DATABASE "hvac"
//#define DATABASE "zebra"

// TODO: reference additional headers your program requires here
#include "util.h"
#include "base64.h"
#include "FormField.h"
#include "FormLayout.h"
#include "FormDelegate.h"
#include "Detail.h"
#include "Form.h"
#include "ReadOnlyDelegate.h"
#include "BlankDelegate.h"
#include "ArrayOfObjectsDelegate.h"
#include "MaskEdit.h"
#include "NotifyParent.h"
#include "CinchDesigner.h"
#include "DetailViewDelegate.h"
#include "ViewAutocompleteSource.h"

#include "Explorer.h"

#include "AttachmentViewer.h"
#include "SimpleRibbon.h"
#include "NewDocumentHandler.h"
#include "SwitchPerspectiveHandler.h"

#include "console.h"

#include <conio.h>
#include <limits>	// For SHRT_MAX
#include <cassert>
#include <iostream>



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


#define DATEPICKER "DatePicker"
#define NUMBER "Number"
#define YESNO "YesNo"
#define RADIO "Radio"
#define EDIT "Edit"
#define REFERENCE "Reference"
#define COMBO "Combo"

#define MULTILINE "multiline"
#define STRING "string"
#define DOCUMENT "document"
#define DATETYPE "date"
#define CODEDVALUE "coded_value"
#define NUMBERTYPE	"number"
#define ID		"id"
#define BOOLEAN "boolean"



#define VIEW "View"
#define VIEW_WITH_DOCUMENTS_DETAIL "ViewWithDocuments"
#define TABLE "Table"
#define TEXT_DETAIL "Text"
#define ATTACHMENTS_DETAIL "Attachments"


#define WM_NEW_DATA_ARRIVED WM_USER+140
#define WM_ATTACHMENTS_UPLOADED WM_USER+141


#define DEFAULT_FONT CreateFont(-12,0,0,0,400,0,0,0,1,0,0,0,0,TEXT("Segoe UI"))

#define DEFAULT_BACKGROUND_COLOR RGB(240,240,240)


#define BUFSIZE 1024
#define MD5LEN  16

extern Connection conn;
extern Database db;