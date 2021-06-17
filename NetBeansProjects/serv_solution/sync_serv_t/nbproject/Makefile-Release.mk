#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=None-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/57abf465/action.o \
	${OBJECTDIR}/_ext/57abf465/addr_list.o \
	${OBJECTDIR}/_ext/57abf465/anchor.o \
	${OBJECTDIR}/_ext/57abf465/anchor_cache.o \
	${OBJECTDIR}/_ext/57abf465/anchor_xml.o \
	${OBJECTDIR}/_ext/57abf465/auth_cache.o \
	${OBJECTDIR}/_ext/57abf465/auth_ctrl.o \
	${OBJECTDIR}/_ext/57abf465/auth_reply.o \
	${OBJECTDIR}/_ext/57abf465/auth_socks.o \
	${OBJECTDIR}/_ext/57abf465/capacity.o \
	${OBJECTDIR}/_ext/57abf465/checksum.o \
	${OBJECTDIR}/_ext/57abf465/chks_cache.o \
	${OBJECTDIR}/_ext/57abf465/daemonize.o \
	${OBJECTDIR}/_ext/57abf465/dires_xml.o \
	${OBJECTDIR}/_ext/57abf465/encoding.o \
	${OBJECTDIR}/_ext/57abf465/fasd_list.o \
	${OBJECTDIR}/_ext/57abf465/file_chks.o \
	${OBJECTDIR}/_ext/57abf465/file_utility.o \
	${OBJECTDIR}/_ext/57abf465/hash_table_ext.o \
	${OBJECTDIR}/_ext/57abf465/iattb_xml.o \
	${OBJECTDIR}/_ext/57abf465/linux_os.o \
	${OBJECTDIR}/_ext/57abf465/ope_list.o \
	${OBJECTDIR}/_ext/57abf465/query_xml.o \
	${OBJECTDIR}/_ext/57abf465/recu_directory.o \
	${OBJECTDIR}/_ext/57abf465/recu_utility.o \
	${OBJECTDIR}/_ext/57abf465/sdtproto.o \
	${OBJECTDIR}/_ext/57abf465/status_xml.o \
	${OBJECTDIR}/_ext/57abf465/string_utility.o \
	${OBJECTDIR}/_ext/57abf465/text_value.o \
	${OBJECTDIR}/_ext/40f48dc2/action_hmap.o \
	${OBJECTDIR}/_ext/40f48dc2/adler32.o \
	${OBJECTDIR}/_ext/40f48dc2/base64.o \
	${OBJECTDIR}/_ext/40f48dc2/cache_hmap.o \
	${OBJECTDIR}/_ext/40f48dc2/hash_map.o \
	${OBJECTDIR}/_ext/40f48dc2/hash_table.o \
	${OBJECTDIR}/_ext/40f48dc2/md5.o \
	${OBJECTDIR}/_ext/40f48dc2/sha1.o \
	${OBJECTDIR}/_ext/40f48dc2/strptime.o \
	${OBJECTDIR}/_ext/40f48dc2/tiny_pool.o \
	${OBJECTDIR}/_ext/f534dc3f/access_log.o \
	${OBJECTDIR}/_ext/f534dc3f/binary_logger.o \
	${OBJECTDIR}/_ext/f534dc3f/dmap64.o \
	${OBJECTDIR}/_ext/f534dc3f/logger.o \
	${OBJECTDIR}/_ext/f534dc3f/memory_utility.o \
	${OBJECTDIR}/_ext/f534dc3f/pmap64.o \
	${OBJECTDIR}/_ext/7474305c/creat_river.o \
	${OBJECTDIR}/_ext/7474305c/riv_md5.o \
	${OBJECTDIR}/_ext/7474305c/riv_sha1.o \
	${OBJECTDIR}/_ext/7474305c/river_cache.o \
	${OBJECTDIR}/_ext/7474305c/river_entry.o \
	${OBJECTDIR}/_ext/7474305c/river_utility.o \
	${OBJECTDIR}/_ext/7474305c/rivfs.o \
	${OBJECTDIR}/_ext/f534dc3f/tiny_map.o \
	${OBJECTDIR}/auth_bzl.o \
	${OBJECTDIR}/build_utility.o \
	${OBJECTDIR}/chks_bzl.o \
	${OBJECTDIR}/dseion.o \
	${OBJECTDIR}/epoll_serv.o \
	${OBJECTDIR}/evd_list.o \
	${OBJECTDIR}/excep.o \
	${OBJECTDIR}/gseion.o \
	${OBJECTDIR}/hand_epoll.o \
	${OBJECTDIR}/list_cache.o \
	${OBJECTDIR}/listxml.o \
	${OBJECTDIR}/match_utility.o \
	${OBJECTDIR}/opera_bzl.o \
	${OBJECTDIR}/options.o \
	${OBJECTDIR}/parse_conf.o \
	${OBJECTDIR}/process_cycle.o \
	${OBJECTDIR}/sdtp_utility.o \
	${OBJECTDIR}/session.o \
	${OBJECTDIR}/speed_utility.o \
	${OBJECTDIR}/sserv_bzl.o \
	${OBJECTDIR}/sserv_object.o \
	${OBJECTDIR}/sync_serv.o \
	${OBJECTDIR}/value_layer.o \
	${OBJECTDIR}/versi_utility.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sync_serv_t

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sync_serv_t: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sync_serv_t ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/57abf465/action.o: ../serv_common/action.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/action.o ../serv_common/action.c

${OBJECTDIR}/_ext/57abf465/addr_list.o: ../serv_common/addr_list.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/addr_list.o ../serv_common/addr_list.c

${OBJECTDIR}/_ext/57abf465/anchor.o: ../serv_common/anchor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/anchor.o ../serv_common/anchor.c

${OBJECTDIR}/_ext/57abf465/anchor_cache.o: ../serv_common/anchor_cache.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/anchor_cache.o ../serv_common/anchor_cache.c

${OBJECTDIR}/_ext/57abf465/anchor_xml.o: ../serv_common/anchor_xml.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/anchor_xml.o ../serv_common/anchor_xml.c

${OBJECTDIR}/_ext/57abf465/auth_cache.o: ../serv_common/auth_cache.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/auth_cache.o ../serv_common/auth_cache.c

${OBJECTDIR}/_ext/57abf465/auth_ctrl.o: ../serv_common/auth_ctrl.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/auth_ctrl.o ../serv_common/auth_ctrl.c

${OBJECTDIR}/_ext/57abf465/auth_reply.o: ../serv_common/auth_reply.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/auth_reply.o ../serv_common/auth_reply.c

${OBJECTDIR}/_ext/57abf465/auth_socks.o: ../serv_common/auth_socks.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/auth_socks.o ../serv_common/auth_socks.c

${OBJECTDIR}/_ext/57abf465/capacity.o: ../serv_common/capacity.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/capacity.o ../serv_common/capacity.c

${OBJECTDIR}/_ext/57abf465/checksum.o: ../serv_common/checksum.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/checksum.o ../serv_common/checksum.c

${OBJECTDIR}/_ext/57abf465/chks_cache.o: ../serv_common/chks_cache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/chks_cache.o ../serv_common/chks_cache.cpp

${OBJECTDIR}/_ext/57abf465/daemonize.o: ../serv_common/daemonize.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/daemonize.o ../serv_common/daemonize.c

${OBJECTDIR}/_ext/57abf465/dires_xml.o: ../serv_common/dires_xml.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/dires_xml.o ../serv_common/dires_xml.c

${OBJECTDIR}/_ext/57abf465/encoding.o: ../serv_common/encoding.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/encoding.o ../serv_common/encoding.c

${OBJECTDIR}/_ext/57abf465/fasd_list.o: ../serv_common/fasd_list.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/fasd_list.o ../serv_common/fasd_list.c

${OBJECTDIR}/_ext/57abf465/file_chks.o: ../serv_common/file_chks.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/file_chks.o ../serv_common/file_chks.c

${OBJECTDIR}/_ext/57abf465/file_utility.o: ../serv_common/file_utility.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/file_utility.o ../serv_common/file_utility.c

${OBJECTDIR}/_ext/57abf465/hash_table_ext.o: ../serv_common/hash_table_ext.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/hash_table_ext.o ../serv_common/hash_table_ext.c

${OBJECTDIR}/_ext/57abf465/iattb_xml.o: ../serv_common/iattb_xml.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/iattb_xml.o ../serv_common/iattb_xml.c

${OBJECTDIR}/_ext/57abf465/linux_os.o: ../serv_common/linux_os.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/linux_os.o ../serv_common/linux_os.c

${OBJECTDIR}/_ext/57abf465/ope_list.o: ../serv_common/ope_list.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/ope_list.o ../serv_common/ope_list.c

${OBJECTDIR}/_ext/57abf465/query_xml.o: ../serv_common/query_xml.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/query_xml.o ../serv_common/query_xml.c

${OBJECTDIR}/_ext/57abf465/recu_directory.o: ../serv_common/recu_directory.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/recu_directory.o ../serv_common/recu_directory.c

${OBJECTDIR}/_ext/57abf465/recu_utility.o: ../serv_common/recu_utility.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/recu_utility.o ../serv_common/recu_utility.c

${OBJECTDIR}/_ext/57abf465/sdtproto.o: ../serv_common/sdtproto.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/sdtproto.o ../serv_common/sdtproto.c

${OBJECTDIR}/_ext/57abf465/status_xml.o: ../serv_common/status_xml.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/status_xml.o ../serv_common/status_xml.c

${OBJECTDIR}/_ext/57abf465/string_utility.o: ../serv_common/string_utility.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/string_utility.o ../serv_common/string_utility.c

${OBJECTDIR}/_ext/57abf465/text_value.o: ../serv_common/text_value.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/text_value.o ../serv_common/text_value.c

${OBJECTDIR}/_ext/40f48dc2/action_hmap.o: ../serv_common/utility/action_hmap.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/action_hmap.o ../serv_common/utility/action_hmap.c

${OBJECTDIR}/_ext/40f48dc2/adler32.o: ../serv_common/utility/adler32.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/adler32.o ../serv_common/utility/adler32.c

${OBJECTDIR}/_ext/40f48dc2/base64.o: ../serv_common/utility/base64.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/base64.o ../serv_common/utility/base64.c

${OBJECTDIR}/_ext/40f48dc2/cache_hmap.o: ../serv_common/utility/cache_hmap.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/cache_hmap.o ../serv_common/utility/cache_hmap.c

${OBJECTDIR}/_ext/40f48dc2/hash_map.o: ../serv_common/utility/hash_map.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/hash_map.o ../serv_common/utility/hash_map.c

${OBJECTDIR}/_ext/40f48dc2/hash_table.o: ../serv_common/utility/hash_table.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/hash_table.o ../serv_common/utility/hash_table.c

${OBJECTDIR}/_ext/40f48dc2/md5.o: ../serv_common/utility/md5.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/md5.o ../serv_common/utility/md5.c

${OBJECTDIR}/_ext/40f48dc2/sha1.o: ../serv_common/utility/sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/sha1.o ../serv_common/utility/sha1.c

${OBJECTDIR}/_ext/40f48dc2/strptime.o: ../serv_common/utility/strptime.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/strptime.o ../serv_common/utility/strptime.c

${OBJECTDIR}/_ext/40f48dc2/tiny_pool.o: ../serv_common/utility/tiny_pool.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/tiny_pool.o ../serv_common/utility/tiny_pool.c

${OBJECTDIR}/_ext/f534dc3f/access_log.o: ../third_party/access_log.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/access_log.o ../third_party/access_log.c

${OBJECTDIR}/_ext/f534dc3f/binary_logger.o: ../third_party/binary_logger.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/binary_logger.o ../third_party/binary_logger.c

${OBJECTDIR}/_ext/f534dc3f/dmap64.o: ../third_party/dmap64.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/dmap64.o ../third_party/dmap64.c

${OBJECTDIR}/_ext/f534dc3f/logger.o: ../third_party/logger.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/logger.o ../third_party/logger.c

${OBJECTDIR}/_ext/f534dc3f/memory_utility.o: ../third_party/memory_utility.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/memory_utility.o ../third_party/memory_utility.c

${OBJECTDIR}/_ext/f534dc3f/pmap64.o: ../third_party/pmap64.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/pmap64.o ../third_party/pmap64.c

${OBJECTDIR}/_ext/7474305c/creat_river.o: ../third_party/rivfs/creat_river.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7474305c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7474305c/creat_river.o ../third_party/rivfs/creat_river.cpp

${OBJECTDIR}/_ext/7474305c/riv_md5.o: ../third_party/rivfs/riv_md5.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/7474305c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7474305c/riv_md5.o ../third_party/rivfs/riv_md5.c

${OBJECTDIR}/_ext/7474305c/riv_sha1.o: ../third_party/rivfs/riv_sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/7474305c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7474305c/riv_sha1.o ../third_party/rivfs/riv_sha1.c

${OBJECTDIR}/_ext/7474305c/river_cache.o: ../third_party/rivfs/river_cache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7474305c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7474305c/river_cache.o ../third_party/rivfs/river_cache.cpp

${OBJECTDIR}/_ext/7474305c/river_entry.o: ../third_party/rivfs/river_entry.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7474305c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7474305c/river_entry.o ../third_party/rivfs/river_entry.cpp

${OBJECTDIR}/_ext/7474305c/river_utility.o: ../third_party/rivfs/river_utility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7474305c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7474305c/river_utility.o ../third_party/rivfs/river_utility.cpp

${OBJECTDIR}/_ext/7474305c/rivfs.o: ../third_party/rivfs/rivfs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7474305c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7474305c/rivfs.o ../third_party/rivfs/rivfs.cpp

${OBJECTDIR}/_ext/f534dc3f/tiny_map.o: ../third_party/tiny_map.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/tiny_map.o ../third_party/tiny_map.c

${OBJECTDIR}/auth_bzl.o: auth_bzl.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/auth_bzl.o auth_bzl.c

${OBJECTDIR}/build_utility.o: build_utility.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/build_utility.o build_utility.c

${OBJECTDIR}/chks_bzl.o: chks_bzl.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chks_bzl.o chks_bzl.cpp

${OBJECTDIR}/dseion.o: dseion.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dseion.o dseion.cpp

${OBJECTDIR}/epoll_serv.o: epoll_serv.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/epoll_serv.o epoll_serv.cpp

${OBJECTDIR}/evd_list.o: evd_list.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/evd_list.o evd_list.cpp

${OBJECTDIR}/excep.o: excep.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/excep.o excep.cpp

${OBJECTDIR}/gseion.o: gseion.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gseion.o gseion.cpp

${OBJECTDIR}/hand_epoll.o: hand_epoll.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/hand_epoll.o hand_epoll.cpp

${OBJECTDIR}/list_cache.o: list_cache.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/list_cache.o list_cache.c

${OBJECTDIR}/listxml.o: listxml.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/listxml.o listxml.c

${OBJECTDIR}/match_utility.o: match_utility.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/match_utility.o match_utility.c

${OBJECTDIR}/opera_bzl.o: opera_bzl.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/opera_bzl.o opera_bzl.c

${OBJECTDIR}/options.o: options.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/options.o options.c

${OBJECTDIR}/parse_conf.o: parse_conf.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/parse_conf.o parse_conf.c

${OBJECTDIR}/process_cycle.o: process_cycle.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/process_cycle.o process_cycle.cpp

${OBJECTDIR}/sdtp_utility.o: sdtp_utility.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sdtp_utility.o sdtp_utility.cpp

${OBJECTDIR}/session.o: session.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/session.o session.cpp

${OBJECTDIR}/speed_utility.o: speed_utility.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/speed_utility.o speed_utility.c

${OBJECTDIR}/sserv_bzl.o: sserv_bzl.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sserv_bzl.o sserv_bzl.cpp

${OBJECTDIR}/sserv_object.o: sserv_object.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sserv_object.o sserv_object.cpp

${OBJECTDIR}/sync_serv.o: sync_serv.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sync_serv.o sync_serv.cpp

${OBJECTDIR}/value_layer.o: value_layer.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/value_layer.o value_layer.c

${OBJECTDIR}/versi_utility.o: versi_utility.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/versi_utility.o versi_utility.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sync_serv_t

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
