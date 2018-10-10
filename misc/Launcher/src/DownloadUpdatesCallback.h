#pragma once
#include <windows.h>
#include <locale>

class DownloadUpdatesCallback : public IBindStatusCallback 
{
	enum {
		DOWNLOAD_FAILD = 0,
		DOWNLOAD_SUCCESS,
		DOWNLOAD_ABORTED,
		DOWNLOAD_IN_PROGRESS,
	};

public:
DownloadUpdatesCallback(HANDLE downloadFinishedEvent, function<bool(int, int)> clbk)
	: m_downloadFinishedEvent(downloadFinishedEvent)
	, m_state(DOWNLOAD_IN_PROGRESS)
	, _progressClbk(clbk)
{}

~DownloadUpdatesCallback() { }

// This one is called by URLDownloadToFile
STDMETHOD(OnProgress)(/* [in] */ ULONG ulProgress, /* [in] */ ULONG ulProgressMax, /* [in] */ ULONG ulStatusCode, /* [in] */ LPCWSTR wszStatusText)
{
//cout << "Downloaded " << ulProgress << " of " << ulProgressMax << " byte(s), " << " Status Code = " << ulStatusCode << endl;
	if (!_progressClbk(ulProgress, ulProgressMax))
	{
		m_state = DOWNLOAD_ABORTED;
		return S_FALSE;
	}
	if (ulStatusCode == BINDSTATUS_ENDDOWNLOADDATA)
	{
		if (ulProgressMax == 0 || (ulProgressMax != ulProgress))
			m_state = DOWNLOAD_FAILD;
		else
			m_state = DOWNLOAD_SUCCESS;
		SetEvent(m_downloadFinishedEvent);
	}	
	return S_OK;
}

// The rest don't do anything...
STDMETHOD(OnStartBinding)(/* [in] */ DWORD dwReserved, /* [in] */ IBinding __RPC_FAR *pib)
{ 
	return E_NOTIMPL; }

STDMETHOD(GetPriority)(/* [out] */ LONG __RPC_FAR *pnPriority)
{ return E_NOTIMPL; }

STDMETHOD(OnLowResource)(/* [in] */ DWORD reserved)
{ return E_NOTIMPL; }

STDMETHOD(OnStopBinding)(/* [in] */ HRESULT hresult, /* [unique][in] */ LPCWSTR szError)
{ return E_NOTIMPL; }

STDMETHOD(GetBindInfo)(/* [out] */ DWORD __RPC_FAR *grfBINDF, /* [unique][out][in] */ BINDINFO __RPC_FAR *pbindinfo)
{ 
	*grfBINDF |=  BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;
	return S_OK; 
}

STDMETHOD(OnDataAvailable)(/* [in] */ DWORD grfBSCF, /* [in] */ DWORD dwSize, /* [in] */ FORMATETC __RPC_FAR *pformatetc, /* [in] */ STGMEDIUM __RPC_FAR *pstgmed)
{ 
	return E_NOTIMPL; 
}

STDMETHOD(OnObjectAvailable)(/* [in] */ REFIID riid, /* [iid_is][in] */ IUnknown __RPC_FAR *punk)
{ return E_NOTIMPL; }

// IUnknown stuff
STDMETHOD_(ULONG,AddRef)()
{ return 0; }

STDMETHOD_(ULONG,Release)()
{ return 0; }

STDMETHOD(QueryInterface)(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{ return E_NOTIMPL; }

BOOL IsDownloadSuccess() {return (m_state == DOWNLOAD_SUCCESS) ? TRUE : FALSE;}

private:
	HANDLE		m_downloadFinishedEvent;
	long		m_state;
	function<bool(int, int)> _progressClbk;
};
