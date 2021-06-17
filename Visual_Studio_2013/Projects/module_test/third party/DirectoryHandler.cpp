#include "StdAfx.h"
#include "DirectoryHandler.h"
// #include ".//utility//ulog.h"


#define WM_DIRECTORY_NOTIFICATION (WM_APP+1024)

HINSTANCE GetInstanceHandle() {
	return (HINSTANCE)GetModuleHandle(NULL);
}
static inline bool IsEmptyString(LPCTSTR sz) {
	return (bool)(sz==NULL || *sz == 0);
}

//////////////////////////////////////////////////////////////////////////
//
//CDirChangeNotification member functions:
//
ChangeNotification::ChangeNotification(CDirectoryHandler * pDirectoryHandler)
:eFunction(eFunctionNotDefined)
,szFileName1(NULL)
,szFileName2(NULL)
,dwError(0UL)
,pDirectoryHandler( pDirectoryHandler )
{
	ASSERT( pDirectoryHandler );
}

ChangeNotification::~ChangeNotification() {
	if( szFileName1 ) free(szFileName1);
	if( szFileName2 ) free(szFileName2);
}

static LRESULT CALLBACK DelayedNotificationWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if( message == WM_DIRECTORY_NOTIFICATION ) {
		ChangeNotification * pNotification = reinterpret_cast<ChangeNotification*>(lParam);
		if( pNotification ) {
// logger(_T("c:\\post1.log"), _T("%s\r\n"), pNotification->szFileName1);
			pNotification->pDirectoryHandler->DispatchNotificationFunction(pNotification);
// logger(_T("c:\\post2.log"), _T("%s\r\n"), pNotification->szFileName1);
			delete pNotification;
		}
		return 0UL;
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//CDelayedNotifier static member vars:
long CDelayedNotifier::s_nReferenceCount = 0L;
HWND CDelayedNotifier::s_hWnd = NULL;
BOOL CDelayedNotifier::s_bRegisterWindow = FALSE;
//
long CDelayedNotifier::AddReference() { //creates window for first time if necessary
	if(InterlockedIncrement(&s_nReferenceCount)==1 || !::IsWindow(s_hWnd)) {
		TRACE(_T("CDelayedNotifier -- Creating the notification window\n"));
		VERIFY( CreateNotificationWindow() );
	}
	return s_nReferenceCount;
}

long CDelayedNotifier::Release()//destroys window for last time if necessary
{
	long nReferenceCount = -1;
	if( (nReferenceCount=InterlockedDecrement(&s_nReferenceCount)) == 0 ) { //no body else using the window so destroy it?
		TRACE(_T("CDelayedNotifier -- Destroying the notification window\n"));
		DestroyWindow( s_hWnd );
		s_hWnd = NULL;
	}
	return s_nReferenceCount;
}

BOOL CDelayedNotifier::RegisterWindowClass(LPCTSTR szClassName)
{
	WNDCLASS wc = {0};

	wc.style = 0;
	wc.hInstance		= GetInstanceHandle();
	wc.lpszClassName	= szClassName;
	wc.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	wc.lpfnWndProc		= DelayedNotificationWndProc;

	ATOM ant = RegisterClass( &wc );
	if( ant == NULL ) {
		TRACE(_T("CDirChangeNotification::RegisterWindowClass - RegisterClass failed: %d\n"), GetLastError());
	}
	return (BOOL)(ant!= NULL);
}

BOOL CDelayedNotifier::CreateNotificationWindow()
{
	TCHAR szClassName[] = _T("Delayed_Message_Sender");
	if( !s_bRegisterWindow ) s_bRegisterWindow = RegisterWindowClass(szClassName);
	s_hWnd 	= CreateWindowEx(0, szClassName, _T("DelayedWnd"),0,0,0,0,0, NULL, 0, GetInstanceHandle(), NULL);
	if( s_hWnd == NULL ) {
		TRACE(_T("Unable to create notification window! GetLastError(): %d\n"), GetLastError());
		TRACE(_T("File: %s Line: %d\n"), _T(__FILE__), __LINE__);
	}

	return (BOOL)(s_hWnd != NULL);
}

void CDelayedNotifier::PostNotification(ChangeNotification * pNotification) {
// logger(_T("c:\\post3.log"), _T("%s\r\n"), pNotification->szFileName1);
	PostMessage(s_hWnd, WM_DIRECTORY_NOTIFICATION, 0, reinterpret_cast<LPARAM>( pNotification ));
}

CDirectoryHandler::CDirectoryHandler() 
:m_nReferenceCount( 1 ) 
{
}

CDirectoryHandler::~CDirectoryHandler() {
}

long CDirectoryHandler::AddReference() {
	return InterlockedIncrement(&m_nReferenceCount);	
}

long CDirectoryHandler::Release() {  
	long nReferenceCount = -1;
	if( (nReferenceCount = InterlockedDecrement(&m_nReferenceCount)) == 0 )
		delete this;
	return nReferenceCount;
}
long CDirectoryHandler::ReferenceCount()const {
	return m_nReferenceCount;
}

//	Default implmentations for CDirectoryHandler's virtual functions.
void CDirectoryHandler::On_FileAdded(const TCHAR *szFileName) { 
	TRACE(_T("The following file was added: %s\n"), szFileName);
}

void CDirectoryHandler::On_FileRemoved(const TCHAR *szFileName) {
	TRACE(_T("The following file was removed: %s\n"), szFileName);
}

void CDirectoryHandler::On_FileModified(const TCHAR *szFileName) {
	TRACE(_T("The following file was modified: %s\n"), szFileName);
}

void CDirectoryHandler::On_FileNameChanged(const TCHAR *szExistsFileName, const TCHAR *szNewFileName) {
	TRACE(_T("The file %s was RENAMED to %s\n"), szExistsFileName, szNewFileName);
}
void CDirectoryHandler::On_ReadDirectoryChangesError(DWORD dwError, const TCHAR *szDirectoryName) {
	TRACE(_T("WARNING!!!!!\n") );
	TRACE(_T("An error has occurred on a watched directory!\n"));
	TRACE(_T("This directory has become unwatched! -- %s \n"), szDirectoryName);
	TRACE(_T("ReadDirectoryChangesW has failed! %d"), dwError);
}

void CDirectoryHandler::On_WatchStarted(DWORD dwError, const TCHAR *szDirectoryName) {	
	if( dwError == 0 ) TRACE(_T("A watch has begun on the following directory: %s\n"), szDirectoryName);
	else TRACE(_T("A watch failed to start on the following directory: (Error: %d) %s\n"),dwError, szDirectoryName);
}

void CDirectoryHandler::On_WatchStopped(const TCHAR *szDirectoryName) {
	TRACE(_T("The watch on the following directory has stopped: %s\n"), szDirectoryName);
}

void CDirectoryHandler::DispatchNotificationFunction(ChangeNotification *pNotification) {
	CDirectoryHandler *pDirectoryHandler = pNotification->pDirectoryHandler;
	if(pDirectoryHandler) {
		switch(pNotification->eFunction) {
		case ChangeNotification::eFileAdded:
			pDirectoryHandler->On_FileAdded( pNotification->szFileName1 ); 
			break;
		case ChangeNotification::eFileRemoved:
			pDirectoryHandler->On_FileRemoved( pNotification->szFileName1 );
			break;
		case ChangeNotification::eFileModified:
			pDirectoryHandler->On_FileModified( pNotification->szFileName1 );
			break;
		case ChangeNotification::eFileNameChanged:
			pDirectoryHandler->On_FileNameChanged( pNotification->szFileName1, pNotification->szFileName2 );
			break;
		case ChangeNotification::eReadDirectoryChangesError:
			pDirectoryHandler->On_ReadDirectoryChangesError( pNotification->dwError, pNotification->szFileName1 );
			break;
		case ChangeNotification::eWatchStarted:
			pDirectoryHandler->On_WatchStarted(pNotification->dwError, pNotification->szFileName1);
			break;
		case ChangeNotification::eWatchStopped:
			pDirectoryHandler->On_WatchStopped(pNotification->szFileName1);
			break;
		case ChangeNotification::eFunctionNotDefined:
		default:
			break;
		}
	}
}


CDelayedSender::CDelayedSender(CDirectoryHandler *pDirectoryHandler) 
:m_pDelayNotifier( NULL )
,m_pDirectoryHandler( pDirectoryHandler )
,m_hWatchStoppedDispatchedEvent(NULL)
{	
	//AUTO-RESET, not initially signalled
	m_hWatchStoppedDispatchedEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pDelayNotifier = new CDelayedNotifier();
}

CDelayedSender::~CDelayedSender() {
	if( m_pDelayNotifier ) delete m_pDelayNotifier, m_pDelayNotifier = NULL;

	if( m_hWatchStoppedDispatchedEvent )
		CloseHandle(m_hWatchStoppedDispatchedEvent), m_hWatchStoppedDispatchedEvent = NULL;
}

//	Maybe in future I'll keep a pool of these objects around to increase performance...
//	using objects from a cache will be fatteer than allocated and destroying a new one each time.
ChangeNotification * CDelayedSender::GetNotificationObject() {
	//helps support FILTERS_CHECK_PARTIAL_PATH
	return new ChangeNotification(m_pDirectoryHandler);
}

void CDelayedSender::DisposeOfNotification(ChangeNotification * pNotification) {
	delete pNotification;
}

void CDelayedSender::On_FileAdded(const TCHAR *szFileName) {
	ChangeNotification *pNotification = GetNotificationObject();
	if( pNotification ) {
		pNotification->eFunction = ChangeNotification::eFileAdded;
		pNotification->szFileName1 = _tcsdup(szFileName);

		m_pDelayNotifier->PostNotification( pNotification );
	}
}

void CDelayedSender::On_FileRemoved(const TCHAR *szFileName) {
	ChangeNotification *pNotification = GetNotificationObject();
	if( pNotification ) {
		pNotification->eFunction = ChangeNotification::eFileRemoved;
		pNotification->szFileName1 = _tcsdup(szFileName);

		m_pDelayNotifier->PostNotification( pNotification );
	}
}

void CDelayedSender::On_FileModified(const TCHAR *szFileName) {
	ChangeNotification *pNotification = GetNotificationObject();
	if( pNotification ) {
		pNotification->eFunction = ChangeNotification::eFileModified;
		pNotification->szFileName1 = _tcsdup(szFileName);

		m_pDelayNotifier->PostNotification( pNotification );
	}
}

void CDelayedSender::On_FileNameChanged(const TCHAR *szExistsFileName, const TCHAR *szNewFileName) {
	ChangeNotification *pNotification = GetNotificationObject();
	if( pNotification ) {
		pNotification->eFunction = ChangeNotification::eFileNameChanged;
		pNotification->szFileName1 = _tcsdup(szExistsFileName);
		pNotification->szFileName2 = _tcsdup(szNewFileName);

		m_pDelayNotifier->PostNotification( pNotification );
	}
}

void CDelayedSender::On_ReadDirectoryChangesError(DWORD dwError, const TCHAR *szDirectoryName) {
	ChangeNotification *pNotification = GetNotificationObject();
	if( pNotification ) {
		pNotification->eFunction = ChangeNotification::eReadDirectoryChangesError;
		pNotification->szFileName1 = _tcsdup(szDirectoryName);
		pNotification->dwError = dwError;

		m_pDelayNotifier->PostNotification( pNotification );
	}
}

void CDelayedSender::On_WatchStarted(DWORD dwError, const TCHAR *szDirectoryName) {
	ChangeNotification *pNotification = GetNotificationObject();
	if( pNotification ) {
		pNotification->eFunction = ChangeNotification::eWatchStarted;
		pNotification->szFileName1 = _tcsdup(szDirectoryName);
		pNotification->dwError = dwError;

		m_pDelayNotifier->PostNotification( pNotification );
	}
}

void CDelayedSender::On_WatchStopped(const TCHAR *szDirectoryName) {
	ChangeNotification *pNotification = GetNotificationObject();
	if( pNotification ) {
		pNotification->eFunction = ChangeNotification::eWatchStopped;
		pNotification->szFileName1 = _tcsdup(szDirectoryName);

		m_pDelayNotifier->PostNotification( pNotification );
	}
}
