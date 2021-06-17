#ifndef _COMMON_HEADER_H_
#define _COMMON_HEADER_H_

#ifdef	__cplusplus
extern "C" {
#endif
//
#include "utility/utlist.h"
#include "utility/adler32.h"
#include "utility/hash_table.h"
#include "utility/hash_map.h"
#include "utility/md5.h"
#include "utility/tiny_pool.h"
#include "utility/strptime.h"

//
#include "common_macro.h"
#include "error_code.h"
#include "linux_os.h"
#include "daemonize.h"
#include "recu_directory.h"
#include "file_utility.h"
#include "string_utility.h"
#include "checksum.h"
#include "encoding.h"
#include "hash_table_ext.h"
#include "sdtproto.h"

//
#ifdef _AUTH_CACHE
#include "author_macro.h"
#include "auth_cache.h"
#include "list_xml.h"
#include "status_xml.h"
#include "query_xml.h" 
#include "server_list.h"  
#include "addr_list.h"  
#endif /* _AUTH_CACHE */

//
#ifdef _REGIS_SERVER
#include "regis_macro.h"
#include "regis_xml.h"
#include "settings_xml.h"
#include "linked_status.h"
#endif /* _REGIS_SERVER */

//
#ifdef _SYNC_SERVER
#include "server_macro.h"
#include "text_value.h"
#include "auth_stru.h"
#include "auth_cache.h"
#include "auth_socks.h"
#include "anchor.h"
#include "anchor_cache.h"
#include "action.h"
#include "ope_list.h"
#include "capacity.h"
#include "iattb_type.h"
#include "fasd_list.h"
#include "iattb_xml.h"
#include "file_chks.h"
#include "dires_xml.h"
#include "anchor_xml.h"
#include "query_xml.h"
#include "status_xml.h"
#endif /* _SYNC_SERVER */

#ifdef	__cplusplus
}
#endif
    
#endif /* _COMMON_HEADER_H_ */

