#pragma once


#define SYNC_TYPE_ERROR		-1
#define SYNC_TYPE_INVALID	0x0000
#define SYNC_TYPE_SLOW		0x0001
#define SYNC_TYPE_FAST		0x0003

struct TRANSMIT_ARGUMENT {
    volatile LONG bProcessStarted;
    //
    struct LocalConfig *pLocalConfig;
    struct ClientConfig *pClientConfig;
};

class CTRANSThread {
public:
    CTRANSThread(void);
    ~CTRANSThread(void);
private:
    struct TRANSMIT_ARGUMENT m_tTransArgu;
public:
    DWORD Initialize(struct LocalConfig *pLocalConfig);
    DWORD Finalize();
public:
    DWORD BeginTRANSThread();
};

extern CTRANSThread objTRANSThread;
