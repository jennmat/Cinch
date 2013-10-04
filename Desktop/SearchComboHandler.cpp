// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "stdafx.h"

//
//  FUNCTION: Execute()
//
//  PURPOSE: Called by the Ribbon framework when the user executes an action on the size and color gallery.
//
//  COMMENTS:
//    When a button in a Commands gallery is clicked, the Execute events are sent to the handler for the 
//    button, not the handler for the gallery. This function will never actually be called.
//
//
STDMETHODIMP CSearchComboHandler::Execute(UINT nCmdID,
                   UI_EXECUTIONVERB verb, 
                   __in_opt const PROPERTYKEY* key,
                   __in_opt const PROPVARIANT* ppropvarValue,
                   __in_opt IUISimplePropertySet* pCommandExecutionProperties)
{
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(ppropvarValue);
    UNREFERENCED_PARAMETER(pCommandExecutionProperties);

	if ( verb == UI_EXECUTIONVERB_EXECUTE ){
		SetupSearching();
	}


   	
	return S_OK;
}

//
//  FUNCTION: UpdateProperty()
//
//  PURPOSE: Called by the Ribbon framework when a command property (PKEY) needs to be updated.
//
//  COMMENTS:
//
//    This function is used to populate the gallery.
//
//
STDMETHODIMP CSearchComboHandler::UpdateProperty(UINT nCmdID,
                              __in REFPROPERTYKEY key,
                              __in_opt const PROPVARIANT* ppropvarCurrentValue,
                              __out PROPVARIANT* ppropvarNewValue)
{
    HRESULT hr = S_FALSE;
	
	if (key == UI_PKEY_ItemsSource)
    {
	    /*IUICollection* pCollection;
        hr = ppropvarCurrentValue->punkVal->QueryInterface( 
                                                      IID_PPV_ARGS(&pCollection));
        
		// Create a new property set for each item.
        CPropertySet* pItem;
        hr = CPropertySet::CreateInstance(&pItem);
              
        // Load the label from the resource file.
        WCHAR wszLabel[MAX_RESOURCE_LENGTH];
		wcscpy_s(wszLabel, MAX_RESOURCE_LENGTH, L"documents");
            
		// Initialize the property set with no image, the label that was just
        // loaded, and no category.
        pItem->InitializeItemProperties(NULL, wszLabel, UI_COLLECTION_INVALIDINDEX);

        pCollection->Add(pItem);
		pCollection->Add(pItem);


        pCollection->Release();
        hr = S_OK;
		*/
        hr = S_OK;
    }
    
    return hr;
}

HRESULT CSearchComboHandler::CreateInstance(__deref_out CSearchComboHandler **ppHandler)
{
    if (!ppHandler)
    {
        return E_POINTER;
    }

    *ppHandler = NULL;

    HRESULT hr = S_OK;

    CSearchComboHandler* pHandler = new CSearchComboHandler();

    if (pHandler != NULL)
    {
        *ppHandler = pHandler;
        
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

// IUnknown methods.
STDMETHODIMP_(ULONG) CSearchComboHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CSearchComboHandler::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

STDMETHODIMP CSearchComboHandler::QueryInterface(REFIID iid, void** ppv)
{
    if (!ppv)
    {
        return E_POINTER;
    }

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