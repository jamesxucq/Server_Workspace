#ifndef _LWIZARDCOM_H_
#define _LWIZARDCOM_H_

#include "AnchorCache.h"
#include "./utility/ulog.h"

// System interface
//
#include "NTFSLibrary.h"
#include "common_macro.h"
#include "Encoding.h"
#include "ErrorCode.h"
#include "ErrPrint.h"
#include "logger_ansi.h"
// #include "DebugPrinter.h"
#include "TRANSSockets.h"
#include "ProxySockets.h"
#include "StringUtility.h"
#include "FileUtility.h"
#include "FileChks.h"
#include "Privilege.h"

// Program interface
//
#include "ConfigurationBzl.h"
#include "Session.h"
#include "ReplySeionBzl.h"
#include "TRANSSeionBzl.h"

#include "SlowAttXml.h"
#include "iattb_type.h"
#include "FilesVec.h"
#include "TaskListObj.h"
#include "ParseConfig.h"
#include "FileForbid.h"
#include "DiresVec.h"
#include "DiresXml.h"
#include "VolumeUtility.h"
#include "ReplyComand.h"
#include "Anchor.h"
#include "FlushThread.h"
#include "ShellLinker.h"
#include "TRANSCache.h"

// Sync interface
//
#include "BuildUtility.h"
#include "SDTProtocol.h"
#include "SDTPUtility.h"
#include "TRANSHandler.h"
#include "ReplyHandler.h"
//#include "Anchor.h"

// Programe to Pro interface
//
#include "ShellInterface.h"

// #ifdef SWD_APPLICATION
// #include "client_macro.h"
// #endif
#ifdef LONGIN_WIZARD
#include "lwizard_macro.h"
#endif
// #ifdef LIVE_UPDATE
// #include "lupdate_macro.h"
// #endif

#endif /* _LWIZARDCOM_H_ */