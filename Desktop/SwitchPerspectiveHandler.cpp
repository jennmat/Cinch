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

extern Explorer* explorer;
extern HWND tree;

STDMETHODIMP CSwitchPerspectiveHandler::Execute(UINT nCmdID,
                   UI_EXECUTIONVERB verb, 
                   __in_opt const PROPERTYKEY* key,
                   __in_opt const PROPVARIANT* ppropvarValue,
                   __in_opt IUISimplePropertySet* pCommandExecutionProperties)
{
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(ppropvarValue);
    UNREFERENCED_PARAMETER(pCommandExecutionProperties);

	if ( verb == UI_EXECUTIONVERB_EXECUTE ){
		Object perspective = perspectiveDefinitions[ppropvarValue->intVal];
		explorer->buildExplorer(tree, perspective["_id"].getString());
	}

   	
	return S_OK;
}

// Factory method to create IUIImages from resource identifiers.
HRESULT CSwitchPerspectiveHandler::CreateUIImageFromBitmapResource(
                         LPCTSTR pszResource, __out IUIImage **ppimg)
{
    HRESULT hr = E_FAIL;

    *ppimg = NULL;

    if (NULL == m_pifbFactory)
    {
        hr = CoCreateInstance(
               CLSID_UIRibbonImageFromBitmapFactory, 
               NULL, 
               CLSCTX_ALL, 
               IID_PPV_ARGS(&m_pifbFactory));
        if (FAILED(hr))
        {
            return hr;
        }
    }

    // Load the bitmap from the resource file.
    HBITMAP hbm = NULL;
    hbm = (HBITMAP) LoadImage(
                      GetModuleHandle(NULL), 
                      pszResource, 
                      IMAGE_BITMAP, 
                      0, 
                      0, 
                      LR_CREATEDIBSECTION);

    if (NULL == hbm)
    {
      return HRESULT_FROM_WIN32(GetLastError());
    }

    // Use the factory implemented by the framework to produce an IUIImage.
    hr = m_pifbFactory->CreateImage(hbm, UI_OWNERSHIP_TRANSFER, ppimg);
    if (FAILED(hr))
    {
      DeleteObject(hbm);
    }
    return hr;
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
STDMETHODIMP CSwitchPerspectiveHandler::UpdateProperty(UINT nCmdID,
                              __in REFPROPERTYKEY key,
                              __in_opt const PROPVARIANT* ppropvarCurrentValue,
                              __out PROPVARIANT* ppropvarNewValue)
{
    HRESULT hr = S_FALSE;
	
    if (key == UI_PKEY_ItemsSource)
    {
		perspectiveDefinitions.clear();
	    IUICollection* pCollection;
        hr = ppropvarCurrentValue->punkVal->QueryInterface( 
                                                      IID_PPV_ARGS(&pCollection));
     

		
		Object r = db.viewResults("all-perspectives", "by-role", 100, 0, true);
		Array rows = r["rows"].getArray();
		unsigned int i = 0;
		for(; i<rows.size(); i++){
			Object row = rows[i].getObject();
			Object doc = row["doc"].getObject();
			wstring label = s2ws(doc["label"].getString());
			perspectiveDefinitions.push_back(doc);

			// Create a new property set for each item.
            CPropertySet* pItem;
            hr = CPropertySet::CreateInstance(&pItem);
              
            
			IUIImage* pImg;
			MAKEINTRESOURCE(IDR_SMALL_ADD_BITMAP);

            HRESULT a = CreateUIImageFromBitmapResource(MAKEINTRESOURCE(IDR_SMALL_ADD_BITMAP), &pImg);


			// Initialize the property set with no image, the label that was just
            // loaded, and no category.
            pItem->InitializeItemProperties(pImg, label.c_str(), UI_COLLECTION_INVALIDINDEX);

            pCollection->Add(pItem);

			pItem->Release();
        }
        
     
        pCollection->Release();
        hr = S_OK;
    }
    
    return hr;
}

HRESULT CSwitchPerspectiveHandler::CreateInstance(__deref_out CSwitchPerspectiveHandler **ppHandler)
{
    if (!ppHandler)
    {
        return E_POINTER;
    }

    *ppHandler = NULL;

    HRESULT hr = S_OK;

    CSwitchPerspectiveHandler* pHandler = new CSwitchPerspectiveHandler();

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
STDMETHODIMP_(ULONG) CSwitchPerspectiveHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CSwitchPerspectiveHandler::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

STDMETHODIMP CSwitchPerspectiveHandler::QueryInterface(REFIID iid, void** ppv)
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