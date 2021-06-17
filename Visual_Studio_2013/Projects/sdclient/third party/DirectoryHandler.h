#pragma once
#include "DirectoryChanges.h"

class CDirectoryHandler;

struct ChangeNotification {
    explicit ChangeNotification(CDirectoryHandler *pDirectoryHandler);
    ~ChangeNotification();
    //
    enum FunctionToDispatch { eFunctionNotDefined = -1, eFileAdded = FILE_ACTION_ADDED,
                              eFileRemoved = FILE_ACTION_REMOVED, eFileModified = FILE_ACTION_MODIFIED, eFileNameChanged = FILE_ACTION_RENAMED_OLD_NAME,
                              eReadDirectoryChangesError, eWatchStarted, eWatchStopped
                            };
    FunctionToDispatch eFunction;
    // Notification Data:
    TCHAR *szFileName1;// <-- is the szFileName parameter to On_FileAdded(),On_FileRemoved,On_FileModified(), and is szExistFileName to On_FileNameChanged(). Is also szDirectoryName to On_ReadDirectoryChangesError(), On_WatchStarted(), and On_WatchStopped()
    TCHAR *szFileName2;// <-- is the szNewFileName parameter to On_FileNameChanged()
    DWORD ulTimestamp;
    DWORD dwError;// <-- is the dwError parameter to On_WatchStarted(), and On_ReadDirectoryChangesError()
    // Handle the notification
    CDirectoryHandler *pDirectoryHandler;
};

class CDelayedNotifier {
public:
    CDelayedNotifier() {
        AddReference();
    }
    virtual ~CDelayedNotifier() {
        Release();
    }

    void PostNotification(ChangeNotification *pNotification);
private:
    long AddReference();		// the window handle is reference counted
    long Release();		//

    static long s_nReferenceCount;
    static BOOL s_bRegisterWindow;
    static HWND s_hWnd;
    BOOL RegisterWindowClass(LPCTSTR szClassName);
    BOOL CreateNotificationWindow();
};

class CDirectoryHandler {
public:
    CDirectoryHandler();
    virtual ~CDirectoryHandler();
private:
    long m_nReferenceCount;
public:
    // this class is reference counted
    long AddReference();
    long Release();
    long ReferenceCount()const;
private:
    virtual void On_FileAdded(const TCHAR *szFileName, DWORD ulTimestamp);
    virtual void On_FileRemoved(const TCHAR *szFileName, DWORD ulTimestamp);
    virtual void On_FileNameChanged(const TCHAR *szExistFileName, const TCHAR *szNewFileName, DWORD ulTimestamp);
    virtual void On_FileModified(const TCHAR *szFileName, DWORD ulTimestamp);
    virtual void On_ReadDirectoryChangesError(DWORD dwError, const TCHAR *szDirectoryName);
    virtual void On_WatchStarted(DWORD dwError, const TCHAR *szDirectoryName);
    virtual void On_WatchStopped(const TCHAR *szDirectoryName);
public:
    void DispatchNotificationFunction(ChangeNotification *pNotification);
};

class CDelayedSender {
public:
    CDelayedSender(CDirectoryHandler * pDirectoryHandler);
    ~CDelayedSender();
private:
    CDelayedNotifier * m_pDelayNotifier;
    CDirectoryHandler * m_pDirectoryHandler;
    HANDLE m_hWatchStoppedDispatchedEvent;// supports WaitForOnWatchStoppedDispatched()
public:
    CDirectoryHandler * GetChangeHandler()const {
        return m_pDirectoryHandler;
    }
    void SetChangeHandler(CDirectoryHandler *pChangeHandler) {
        m_pDirectoryHandler = pChangeHandler;
    }
public:
    void On_FileAdded(const TCHAR *szFileName, DWORD ulTimestamp);
    void On_FileRemoved(const TCHAR *szFileName, DWORD ulTimestamp);
    void On_FileModified(const TCHAR *szFileName, DWORD ulTimestamp);
    void On_FileNameChanged(const TCHAR *szExistFileName, const TCHAR *szNewFileName, DWORD ulTimestamp);
    void On_ReadDirectoryChangesError(DWORD dwError, const TCHAR *szDirectoryName);
    void On_WatchStarted(DWORD dwError, const TCHAR *szDirectoryName);
    void On_WatchStopped(const TCHAR *szDirectoryName);
private:
    ChangeNotification *GetNotificationObject();
    void DisposeOfNotification(ChangeNotification * pNotification);
};
