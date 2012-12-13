// Uploader.h : Declaration of the CUploader

#pragma once
#include "resource.h"       // main symbols



#include "HttpUploader_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

#include "WinhttpUploader.h"
#include "MsgWnd.h"

#include <boost/thread.hpp>

struct PostField {
  std::wstring key;
  std::wstring value;
};

/*
*/

namespace state {
enum {
  kStateLeisure        = 0,
  kStateUploading      = 1,
  kStateStop           = 2,
  kStateUploadComplete = 3,
  kStateError          = 4,
  kStateConnected      = 5,
  kStateMd5Working     = 6,
  kStateMd5Complete    = 7,
};
}

/*
** "0": "���ӷ���������"
** "1": "�������ݴ���"
** "2": "�������ݴ���"
** "3": "δ���ñ����ļ�"
** "4": "�����ļ�������"
** "5": "�򿪱����ļ�����"
** "6": "���ܶ�ȡ�����ļ�"
** "7": "��˾δ��Ȩ"
** "8": "δ����IP"
** "9": "����δ��Ȩ"
** "10": "�ļ���С��������"//Ĭ��Ϊ2G
** //md5
** "200": "�޷����ļ�"
** "201": "�ļ���СΪ0"
*/
namespace err {
enum {
  kConnectError          = 0,
  kSendDataError         = 1,
  kRecvDateError         = 2,
  kLocalFileEmpty        = 3,
  kLocalFileAbsent       = 4,
  kFileOpenError         = 5,
  kFileReadError         = 6,
  kCompanyUnauth         = 7,
  kPostUrlEmpty          = 8,
  kDomainUnauth          = 9,
  kFileSizeExceed        = 10,
  kMd5FileOpenError      = 200,
  kMd5FileSizeZero       = 201,
};
}

// CUploader

class ATL_NO_VTABLE CUploader :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUploader, &CLSID_Uploader>,
	public IObjectWithSiteImpl<CUploader>,
	public IDispatchImpl<IUploader, &IID_IUploader, &LIBID_HttpUploaderLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CUploader, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA> {
public:
	CUploader(void);

DECLARE_REGISTRY_RESOURCEID(IDR_UPLOADER)

DECLARE_NOT_AGGREGATABLE(CUploader)

BEGIN_COM_MAP(CUploader)
	COM_INTERFACE_ENTRY(IUploader)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() {
		return S_OK;
	}

	void FinalRelease()	{
	}

//�����ڻص��ĺ���
public:

  void SetState(LONG state);
  void OnPostTimer(void);
  void OnPostCallback(ULONGLONG speed, ULONGLONG posted, USHORT percent, DWORD left_time);

private:

  //Ŀǰ��Ϊ�߳�ʹ�ã�ע��ͬ��
  void CalcMd5(const std::wstring& file);
  void DoPost(ULONGLONG start_pos);
  void SetError(LONG error_code);

  //ie related
  CComQIPtr<IWebBrowser2> pwebbrowser_;
  CComQIPtr<IHTMLDocument2> phtmldoc_;
  HWND hwnd_browser_;

  //property
  std::wstring md5_;
  std::wstring post_url_;
  std::wstring encode_type_;
  std::wstring local_file_;
  std::wstring company_lisensed_;
  ULONG file_id_;
  ULONGLONG posted_length_;
  ULONGLONG file_size_limit_;
  ULONGLONG range_size_;
  ULONGLONG file_size_;
  USHORT md5_percent_;
  LONG error_code_;
  std::wstring error_msg_;

  DWORD begin_post_time_;
  ULONGLONG begin_post_cursor_;

  CComQIPtr<IDispatch> object_;
  CComQIPtr<IDispatch> on_post_;
  CComQIPtr<IDispatch> on_state_changed_;

  MsgWnd msgwnd_;
  std::vector<PostField> post_fields_;
  bool stop_;

  boost::mutex mutex_calcmd5_;
  boost::mutex mutex_uploader_;

  ULONGLONG test_;

public:

  // IObjectWithSite
  STDMETHOD(SetSite)(IUnknown* punksite);

  STDMETHOD(get_MD5)(BSTR* pVal);
  STDMETHOD(get_PostedLength)(ULONGLONG* pVal);
  STDMETHOD(get_PostUrl)(BSTR* pVal);
  STDMETHOD(put_PostUrl)(BSTR newVal);
  STDMETHOD(get_EncodeType)(BSTR* pVal);
  STDMETHOD(put_EncodeType)(BSTR newVal);
  STDMETHOD(get_OnPost)(IDispatch** pVal);
  STDMETHOD(put_OnPost)(IDispatch* newVal);
  STDMETHOD(get_OnStateChanged)(IDispatch** pVal);
  STDMETHOD(put_OnStateChanged)(IDispatch* newVal);
  STDMETHOD(get_LocalFile)(BSTR* pVal);
  STDMETHOD(put_LocalFile)(BSTR newVal);
  STDMETHOD(get_FileSizeLimit)(ULONGLONG* pVal);
  STDMETHOD(put_FileSizeLimit)(ULONGLONG newVal);
  STDMETHOD(get_RangeSize)(ULONGLONG* pVal);
  STDMETHOD(put_RangeSize)(ULONGLONG newVal);
  STDMETHOD(get_CompanyLicensed)(BSTR* pVal);
  STDMETHOD(put_CompanyLicensed)(BSTR newVal);
  STDMETHOD(get_FileID)(ULONG* pVal);
  STDMETHOD(put_FileID)(ULONG newVal);
  STDMETHOD(ClearFields)(BYTE* result);
  STDMETHOD(AddField)(BSTR key, BSTR value, BYTE* result);
  STDMETHOD(Post)(BYTE* result);
  STDMETHOD(CheckFile)(BYTE* result);
  STDMETHOD(Stop)(BYTE* result);
  STDMETHOD(get_Object)(IDispatch** pVal);
  STDMETHOD(put_Object)(IDispatch* newVal);
  STDMETHOD(get_Md5Percent)(USHORT* pVal);
  STDMETHOD(get_ErrorCode)(LONG* pVal);
  STDMETHOD(get_FileSize)(ULONGLONG* pVal);
  STDMETHOD(get_ErrorMsg)(BSTR* pVal);
  STDMETHOD(PostFromPosition)(ULONGLONG position, BYTE* result);
  STDMETHOD(get_Test)(DOUBLE* pVal);
  STDMETHOD(put_Test)(DOUBLE newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(Uploader), CUploader)
