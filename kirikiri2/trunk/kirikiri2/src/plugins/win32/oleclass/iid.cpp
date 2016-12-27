/**
 * ƒfƒtƒHƒ‹ƒg‚Ì IID ‚ð’T‚·
 * @param pitf –¼‘O
 * @param piid Žæ“¾‚µ‚½IID‚ÌŠi”[æ
 * @param ppTypeInfo ŠÖ˜A‚·‚é•ûî•ñ
 */

HRESULT findDefaultIID(IDispatch *pDispatch, IID *piid, ITypeInfo **ppTypeInfo)
{
	HRESULT hr;
	
	IProvideClassInfo2 *pProvideClassInfo2;
	hr = pDispatch->QueryInterface(IID_IProvideClassInfo2, (void**)&pProvideClassInfo2);
	if (SUCCEEDED(hr)) {
		hr = pProvideClassInfo2->GetGUID(GUIDKIND_DEFAULT_SOURCE_DISP_IID, piid);
		pProvideClassInfo2->Release();
		ITypeInfo *pTypeInfo;
		if (SUCCEEDED(hr = pDispatch->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &pTypeInfo))) {
			ITypeLib *pTypeLib;
			unsigned int index;
			if (SUCCEEDED(hr = pTypeInfo->GetContainingTypeLib(&pTypeLib, &index))) {
				hr = pTypeLib->GetTypeInfoOfGuid(*piid, ppTypeInfo);
			}
		}
		return hr;
	}
	
	IProvideClassInfo *pProvideClassInfo;
	if (SUCCEEDED(hr = pDispatch->QueryInterface(IID_IProvideClassInfo, (void**)&pProvideClassInfo))) {
		ITypeInfo *pTypeInfo;
		if (SUCCEEDED(hr = pProvideClassInfo->GetClassInfo(&pTypeInfo))) {
			
			TYPEATTR *pTypeAttr;
			if (SUCCEEDED(hr = pTypeInfo->GetTypeAttr(&pTypeAttr))) {
				int i;
				for (i = 0; i < pTypeAttr->cImplTypes; i++) {
					int iFlags;
					if (SUCCEEDED(hr = pTypeInfo->GetImplTypeFlags(i, &iFlags))) {
						if ((iFlags & IMPLTYPEFLAG_FDEFAULT) &&	(iFlags & IMPLTYPEFLAG_FSOURCE)) {
							HREFTYPE hRefType;
							if (SUCCEEDED(hr = pTypeInfo->GetRefTypeOfImplType(i, &hRefType))) {
								if (SUCCEEDED(hr = pTypeInfo->GetRefTypeInfo(hRefType, ppTypeInfo))) {
									break;
								}
							}
						}
					}
				}
				pTypeInfo->ReleaseTypeAttr(pTypeAttr);
			}
			pTypeInfo->Release();
		}
		pProvideClassInfo->Release();
	}
	
	if (!*ppTypeInfo) {
		if (SUCCEEDED(hr)) {
			hr = E_UNEXPECTED;
		}
	} else {
		TYPEATTR *pTypeAttr;
		hr = (*ppTypeInfo)->GetTypeAttr(&pTypeAttr);
		if (SUCCEEDED(hr)) {
			*piid = pTypeAttr->guid;
			(*ppTypeInfo)->ReleaseTypeAttr(pTypeAttr);
		} else {
			(*ppTypeInfo)->Release();
			*ppTypeInfo = NULL;
		}
	}
	return hr;
}

/**
 * IID ‚ð’T‚·
 * @param pitf –¼‘O
 * @param piid Žæ“¾‚µ‚½IID‚ÌŠi”[æ
 * @param ppTypeInfo ŠÖ˜A‚·‚é•ûî•ñ
 */
HRESULT findIID(IDispatch *pDispatch, const tjs_char *pitf, IID *piid, ITypeInfo **ppTypeInfo)
{
	if (pitf == NULL) {
		return findDefaultIID(piid, ppTypeInfo);
	}
	
	HRESULT hr;
	ITypeInfo *pTypeInfo;
	if (SUCCEEDED(hr = pDispatch->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &pTypeInfo))) {
		ITypeLib *pTypeLib;
		unsigned int index;
		if (SUCCEEDED(hr = pTypeInfo->GetContainingTypeLib(&pTypeLib, &index))) {
			bool found = false;
			unsigned int count = pTypeLib->GetTypeInfoCount();
			for (index = 0; index < count; index++) {
				ITypeInfo *pTypeInfo;
				if (SUCCEEDED(pTypeLib->GetTypeInfo(index, &pTypeInfo))) {
					TYPEATTR *pTypeAttr;
					if (SUCCEEDED(pTypeInfo->GetTypeAttr(&pTypeAttr))) {
						if (pTypeAttr->typekind == TKIND_COCLASS) {
							int type;
							for (type = 0; !found && type < pTypeAttr->cImplTypes; type++) {
								HREFTYPE RefType;
								if (SUCCEEDED(pTypeInfo->GetRefTypeOfImplType(type, &RefType))) {
									ITypeInfo *pImplTypeInfo;
									if (SUCCEEDED(pTypeInfo->GetRefTypeInfo(RefType, &pImplTypeInfo))) {
										BSTR bstr = NULL;
										if (SUCCEEDED(pImplTypeInfo->GetDocumentation(-1, &bstr, NULL, NULL, NULL))) {
											if (wcscmp(pitf, bstr) == 0) {
												TYPEATTR *pImplTypeAttr;
												if (SUCCEEDED(pImplTypeInfo->GetTypeAttr(&pImplTypeAttr))) {
													found = true;
													*piid = pImplTypeAttr->guid;
													if (ppTypeInfo) {
														*ppTypeInfo = pImplTypeInfo;
														(*ppTypeInfo)->AddRef();
													}
													pImplTypeInfo->ReleaseTypeAttr(pImplTypeAttr);
												}
											}
											SysFreeString(bstr);
										}
										pImplTypeInfo->Release();
									}
								}
							}
						}
						pTypeInfo->ReleaseTypeAttr(pTypeAttr);
					}
					pTypeInfo->Release();
				}
				if (found) {
					break;
				}
			}
			if (!found) {
				hr = E_NOINTERFACE;
			}
			pTypeLib->Release();
		}
		pTypeInfo->Release();
	}
	return hr;
}
