// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "stdafx.h"

// Static method to create an instance of the object.
__checkReturn HRESULT CCommandHandler::CreateInstance(__deref_out IUICommandHandler **ppCommandHandler)
{
    if (!ppCommandHandler)
    {
        return E_POINTER;
    }

    *ppCommandHandler = NULL;

    HRESULT hr = S_OK;
   
    CCommandHandler* pCommandHandler = new CCommandHandler();

    if (pCommandHandler != NULL)
    {
        *ppCommandHandler = static_cast<IUICommandHandler *>(pCommandHandler);
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

// IUnknown method implementations.
STDMETHODIMP_(ULONG) CCommandHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CCommandHandler::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

STDMETHODIMP CCommandHandler::QueryInterface(REFIID iid, void** ppv)
{
    if (iid == __uuidof(IUnknown))
    {
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (iid == __uuidof(IUICommandHandler))
    {
        *ppv = static_cast<IUICommandHandler*>(this);
    }
    else 
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//
//  FUNCTION: UpdateProperty()
//
//  PURPOSE: Called by the Ribbon framework when a command property (PKEY) needs to be updated.
//
//  COMMENTS:
//
//    This function is used to provide new command property values, such as labels, icons, or
//    tooltip information, when requested by the Ribbon framework.  
//    
//    In this SimpleRibbon sample, the method is not implemented.  
//
STDMETHODIMP CCommandHandler::UpdateProperty(
    UINT nCmdID,
    __in REFPROPERTYKEY key,
    __in_opt const PROPVARIANT* ppropvarCurrentValue,
    __out PROPVARIANT* ppropvarNewValue)
{
    UNREFERENCED_PARAMETER(nCmdID);
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(ppropvarCurrentValue);
    UNREFERENCED_PARAMETER(ppropvarNewValue);

     HRESULT hr = E_FAIL;
 
     if (key == UI_PKEY_Categories)
     {
         IUICollection* pCollection;
         hr = ppropvarCurrentValue->punkVal->QueryInterface(
                                                           IID_PPV_ARGS(&pCollection));
 
         // Create a property set for the Size category.
         CPropertySet *pSize;
         hr = CPropertySet::CreateInstance(&pSize);
 
         // Load the label for the Size category from the resource file.
         WCHAR wszSizeLabel[MAX_RESOURCE_LENGTH];
         
         // Initialize the property set with the label that was just loaded and 
         // a category id of 0.
         pSize->InitializeCategoryProperties(wszSizeLabel, 0);
 
         // Add the newly-created property set to the collection supplied 
         // by the framework.
         pCollection->Add(pSize);
 
         pSize->Release();
 
 
         // Create a property set for the Color category.
         CPropertySet *pColor;
         hr = CPropertySet::CreateInstance(&pColor);
 
         // Load the label for the Color category from the resource file.
         WCHAR wszColorLabel[MAX_RESOURCE_LENGTH];
		 wcscpy_s(wszColorLabel, MAX_RESOURCE_LENGTH, L"Test");
 
         // Initialize the property set with the label that was just loaded and 
         // a category id of 1.
         pColor->InitializeCategoryProperties(wszColorLabel, 1);
         
         // Add the newly-created property set to the collection supplied 
         // by the framework.
         pCollection->Add(pColor);
 
         pColor->Release();
         pCollection->Release();
 
         hr = S_OK;
     }
     else if (key == UI_PKEY_ItemsSource)
     {
         IUICollection* pCollection;
         hr = ppropvarCurrentValue->punkVal->QueryInterface(
                                                           IID_PPV_ARGS(&pCollection));
 
       
        // Create a new property set for each item.
        CPropertySet* pCommand;
        hr = CPropertySet::CreateInstance(&pCommand);
 
        // Initialize the property set with the appropriate command id and 
        // category id and type Boolean (which makes these appear as 
        // ToggleButtons).
        pCommand->InitializeCommandProperties(0, 0, UI_COMMANDTYPE_BOOLEAN);
 
        // Add the newly-created property set to the collection supplied 
        // by the framework.
        pCollection->Add(pCommand);
 
        pCommand->Release();
       
		pCollection->Release();
        hr = S_OK;
     }
     return hr;
}

//
//  FUNCTION: Execute()
//
//  PURPOSE: Called by the Ribbon framework when a command is executed by the user.  For example, when
//           a button is pressed.
//
STDMETHODIMP CCommandHandler::Execute(
    UINT nCmdID,
    UI_EXECUTIONVERB verb,
    __in_opt const PROPERTYKEY* key,
    __in_opt const PROPVARIANT* ppropvarValue,
    __in_opt IUISimplePropertySet* pCommandExecutionProperties)
{
    UNREFERENCED_PARAMETER(pCommandExecutionProperties);
    UNREFERENCED_PARAMETER(ppropvarValue);
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(verb);
    UNREFERENCED_PARAMETER(nCmdID);

	if ( verb == UI_EXECUTIONVERB_EXECUTE ){
		switch (nCmdID) {
		case IDR_CMD_NEW_VIEW:
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ADD_VIEW), hWnd, NewView);
			break;
		case IDR_CMD_DEBUGOUTPUT:
			stu::Console::Create(500, 200);
			break;
		case IDR_CMD_NEW_PERSPECTIVE:
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_CREATE_PERSPECTIVE), hWnd, NewPerspective);
			break;
		}
	}
    return S_OK;
}
