// clientcom.h
#ifndef _CLIENTCOM_H_
#define _CLIENTCOM_H_

#include "AnchorCache.h"
#include "utility/hash_table.h"
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
#include "RecursiveUtility.h"
#include "TVUtility.h"
#include "AdvaTView.h"
#include "Privilege.h"

// Program interface
//
#include "ConfigurationBzl.h"
#include "Session.h"
#include "ReplySeionBzl.h"
#include "TRANSSeionBzl.h"

#include "ActionHandler.h"
#include "SlowAttXml.h"
#include "FastActXml.h"
#include "iattb_type.h"
#include "FilesVec.h"
#include "TaskListObj.h"
#include "ListVObj.h"
// #include "NodeUtility.h"
#include "ParseConfig.h"
#include "FileForbid.h"
#include "FileConfli.h"
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
#include "ResVector.h"
#include "BuildUtility.h"
#include "SDTProtocol.h"
#include "SDTPUtility.h"
#include "TRANSHandler.h"
#include "ReplyHandler.h"
// #include "Anchor.h"

// Programe to Pro interface
//
#include "ShellUtility.h"
#include "ShellInterface.h"

#ifdef SWD_APPLICATION
#include "client_macro.h"
#endif
// #ifdef LONGIN_WIZARD
// #include "lwizard_macro.h"
// #endif
// #ifdef LIVE_UPDATE
// #include "lupdate_macro.h"
// #endif

#endif /* _CLIENTCOM_H_ */