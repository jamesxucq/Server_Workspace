//clientcom.h
#ifndef _CLIENTCOM_H_
#define _CLIENTCOM_H_


#include "AnchorCache.h"
//System interface
#include "NTFSLibrary.h"
/////////////////////////////////
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
#include "FileCsums.h"
#include "FileHandler.h"

//Program interface
#include "Session.h"
#include "ReplySessionBzl.h"
#include "TRANSSessionBzl.h"

#include "FilesAttribXml.h"
#include "FilesVec.h"
#include "TasksListObject.h"
#include "ParseConfig.h"
#include "FilesDenied.h"
#include "DiresVec.h"
#include "DiresXml.h"
#include "VolumeUtility.h"
#include "ReplyCommand.h"
#include "Anchor.h"
#include "ForceControl.h"
#include "FlushThread.h"
#include "ShellLinker.h"
#include "TRANSCache.h"
//Sync interface
#include "BuildUtility.h"
#include "SDTProtocol.h"
#include "SDTPUtility.h"
#include "TRANSHandler.h"
#include "ReplyHandler.h"
//#include "Anchor.h"

//Programe to Pro interface
#include "ShellInterface.h"


#ifdef SD_APPLICATION
#include "client_macro.h"
#endif
#ifdef LONGIN_WIZARD
#include "lwizard_macro.h"
#endif

#endif /* _CLIENTCOM_H_ */