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
STDMETHODIMP CNewDocumentHandler::Execute(UINT nCmdID,
                   UI_EXECUTIONVERB verb, 
                   __in_opt const PROPERTYKEY* key,
                   __in_opt const PROPVARIANT* ppropvarValue,
                   __in_opt IUISimplePropertySet* pCommandExecutionProperties)
{
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(ppropvarValue);
    UNREFERENCED_PARAMETER(pCommandExecutionProperties);

	if ( verb == UI_EXECUTIONVERB_EXECUTE ){
		Object objectDefinition = objectTypes[ppropvarValue->intVal];
		CinchDesigner* designercontrol = (CinchDesigner *)GetWindowLong(designer, GWL_USERDATA);
		designercontrol->NewDocument(DATABASE, objectDefinition["_id"].getString());
	}
   	
	return S_OK;
}

// Factory method to create IUIImages from resource identifiers.
HRESULT CNewDocumentHandler::CreateUIImageFromBitmapResource(
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
STDMETHODIMP CNewDocumentHandler::UpdateProperty(UINT nCmdID,
                              __in REFPROPERTYKEY key,
                              __in_opt const PROPVARIANT* ppropvarCurrentValue,
                              __out PROPVARIANT* ppropvarNewValue)
{
    HRESULT hr = S_FALSE;
	
    if (key == UI_PKEY_ItemsSource)
    {
		objectTypes.clear();
	    IUICollection* pCollection;
        hr = ppropvarCurrentValue->punkVal->QueryInterface( 
                                                      IID_PPV_ARGS(&pCollection));
        
		pCollection->Clear();

		Array docTypes = getAllConcreteDocumentTypes();
		for(int i=0; i<docTypes.size(); i++){
			Object doc = docTypes[i].getObject();
			objectTypes.push_back(doc);
			// Create a new property set for each item.
			CPropertySet* pItem;
			hr = CPropertySet::CreateInstance(&pItem);
        
			string key = doc["label"].getString();
			wstring wkey = s2ws(key);
		
			// Load the label from the resource file.
			WCHAR wszLabel[MAX_RESOURCE_LENGTH];
			wcscpy_s(wszLabel, MAX_RESOURCE_LENGTH, wkey.c_str());
            
			IUIImage* pImg;
			MAKEINTRESOURCE(IDR_SMALL_ADD_BITMAP);

			HRESULT a = CreateUIImageFromBitmapResource(MAKEINTRESOURCE(IDR_SMALL_ADD_BITMAP), &pImg);


			// Initialize the property set with no image, the label that was just
			// loaded, and no category.
			pItem->InitializeItemProperties(pImg, wszLabel, UI_COLLECTION_INVALIDINDEX);

			pCollection->Add(pItem);
        }
        
     
        pCollection->Release();
        hr = S_OK;
    }
    
    return hr;
}

HRESULT CNewDocumentHandler::CreateInstance(__deref_out CNewDocumentHandler **ppHandler)
{
    if (!ppHandler)
    {
        return E_POINTER;
    }

    *ppHandler = NULL;

    HRESULT hr = S_OK;

    CNewDocumentHandler* pHandler = new CNewDocumentHandler();

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
STDMETHODIMP_(ULONG) CNewDocumentHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CNewDocumentHandler::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

STDMETHODIMP CNewDocumentHandler::QueryInterface(REFIID iid, void** ppv)
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