#ifndef _UPDATECOM_H_
#define _UPDATECOM_H_

// System interface
//
#include "common_macro.h"
#include "lupdate_macro.h"
#include "StringUtility.h"
#include "ProxyConnect.h"
#include "FileUtility.h"

// Program interface
//
#include "ConfigurationBzl.h"
#include "ParseConfig.h"

// Sync interface
//

// Programe to Pro interface
//

// #ifdef SWD_APPLICATION
// #include "client_macro.h"
// #endif
// #ifdef LONGIN_WIZARD
// #include "lwizard_macro.h"
// #endif
#ifdef LIVE_UPDATE
#include "lupdate_macro.h"
#endif

#endif /* _UPDATECOM_H_ */