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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include regis_serv-Makefile.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/57abf465/addr_list.o \
	${OBJECTDIR}/_ext/57abf465/auth_ctrl.o \
	${OBJECTDIR}/_ext/57abf465/checksum.o \
	${OBJECTDIR}/_ext/57abf465/daemonize.o \
	${OBJECTDIR}/_ext/57abf465/encoding.o \
	${OBJECTDIR}/_ext/57abf465/fasd_list.o \
	${OBJECTDIR}/_ext/57abf465/file_chks.o \
	${OBJECTDIR}/_ext/57abf465/file_utility.o \
	${OBJECTDIR}/_ext/57abf465/hash_table_ext.o \
	${OBJECTDIR}/_ext/57abf465/linked_status.o \
	${OBJECTDIR}/_ext/57abf465/linux_os.o \
	${OBJECTDIR}/_ext/57abf465/recu_directory.o \
	${OBJECTDIR}/_ext/57abf465/recu_utility.o \
	${OBJECTDIR}/_ext/57abf465/regis_xml.o \
	${OBJECTDIR}/_ext/57abf465/sdtproto.o \
	${OBJECTDIR}/_ext/57abf465/server_list.o \
	${OBJECTDIR}/_ext/57abf465/settings_xml.o \
	${OBJECTDIR}/_ext/57abf465/string_utility.o \
	${OBJECTDIR}/_ext/40f48dc2/action_hmap.o \
	${OBJECTDIR}/_ext/40f48dc2/adler32.o \
	${OBJECTDIR}/_ext/40f48dc2/base64.o \
	${OBJECTDIR}/_ext/40f48dc2/cache_hmap.o \
	${OBJECTDIR}/_ext/40f48dc2/hash_map.o \
	${OBJECTDIR}/_ext/40f48dc2/hash_table.o \
	${OBJECTDIR}/_ext/40f48dc2/md5.o \
	${OBJECTDIR}/_ext/40f48dc2/strptime.o \
	${OBJECTDIR}/_ext/40f48dc2/tiny_pool.o \
	${OBJECTDIR}/_ext/f534dc3f/access_log.o \
	${OBJECTDIR}/_ext/f534dc3f/binary_logger.o \
	${OBJECTDIR}/_ext/f534dc3f/dmap64.o \
	${OBJECTDIR}/_ext/f534dc3f/logger.o \
	${OBJECTDIR}/_ext/f534dc3f/memory_utility.o \
	${OBJECTDIR}/_ext/f534dc3f/pmap64.o \
	${OBJECTDIR}/_ext/f534dc3f/tiny_map.o \
	${OBJECTDIR}/auth_bzl.o \
	${OBJECTDIR}/epoll_serv.o \
	${OBJECTDIR}/evd_list.o \
	${OBJECTDIR}/excep.o \
	${OBJECTDIR}/hand_epoll.o \
	${OBJECTDIR}/options.o \
	${OBJECTDIR}/parse_conf.o \
	${OBJECTDIR}/process_cycle.o \
	${OBJECTDIR}/regis_serv.o \
	${OBJECTDIR}/rserv_bzl.o \
	${OBJECTDIR}/rserv_object.o \
	${OBJECTDIR}/sdtp_utility.o \
	${OBJECTDIR}/session.o \
	${OBJECTDIR}/value_layer.o


# C Compiler Flags
CFLAGS=-Wall -D_REGIS_SERVER -D_DEBUG -D_LARGEFILE64_SOURCE

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/regis_serv

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/regis_serv: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/regis_serv ${OBJECTFILES} ${LDLIBSOPTIONS} -lxml2 -lstdc++ -lz -lpthread -lm -lc -lrt -lmysqlclient -L/usr/lib64/mysql -ldl

${OBJECTDIR}/_ext/57abf465/addr_list.o: ../serv_common/addr_list.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/addr_list.o ../serv_common/addr_list.c

${OBJECTDIR}/_ext/57abf465/auth_ctrl.o: ../serv_common/auth_ctrl.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/auth_ctrl.o ../serv_common/auth_ctrl.c

${OBJECTDIR}/_ext/57abf465/checksum.o: ../serv_common/checksum.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/checksum.o ../serv_common/checksum.c

${OBJECTDIR}/_ext/57abf465/daemonize.o: ../serv_common/daemonize.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/daemonize.o ../serv_common/daemonize.c

${OBJECTDIR}/_ext/57abf465/encoding.o: ../serv_common/encoding.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/encoding.o ../serv_common/encoding.c

${OBJECTDIR}/_ext/57abf465/fasd_list.o: ../serv_common/fasd_list.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/fasd_list.o ../serv_common/fasd_list.c

${OBJECTDIR}/_ext/57abf465/file_chks.o: ../serv_common/file_chks.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/file_chks.o ../serv_common/file_chks.c

${OBJECTDIR}/_ext/57abf465/file_utility.o: ../serv_common/file_utility.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/file_utility.o ../serv_common/file_utility.c

${OBJECTDIR}/_ext/57abf465/hash_table_ext.o: ../serv_common/hash_table_ext.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/hash_table_ext.o ../serv_common/hash_table_ext.c

${OBJECTDIR}/_ext/57abf465/linked_status.o: ../serv_common/linked_status.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/linked_status.o ../serv_common/linked_status.c

${OBJECTDIR}/_ext/57abf465/linux_os.o: ../serv_common/linux_os.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/linux_os.o ../serv_common/linux_os.c

${OBJECTDIR}/_ext/57abf465/recu_directory.o: ../serv_common/recu_directory.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/recu_directory.o ../serv_common/recu_directory.c

${OBJECTDIR}/_ext/57abf465/recu_utility.o: ../serv_common/recu_utility.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/recu_utility.o ../serv_common/recu_utility.c

${OBJECTDIR}/_ext/57abf465/regis_xml.o: ../serv_common/regis_xml.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/regis_xml.o ../serv_common/regis_xml.c

${OBJECTDIR}/_ext/57abf465/sdtproto.o: ../serv_common/sdtproto.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/sdtproto.o ../serv_common/sdtproto.c

${OBJECTDIR}/_ext/57abf465/server_list.o: ../serv_common/server_list.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/server_list.o ../serv_common/server_list.c

${OBJECTDIR}/_ext/57abf465/settings_xml.o: ../serv_common/settings_xml.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/settings_xml.o ../serv_common/settings_xml.c

${OBJECTDIR}/_ext/57abf465/string_utility.o: ../serv_common/string_utility.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/57abf465
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/57abf465/string_utility.o ../serv_common/string_utility.c

${OBJECTDIR}/_ext/40f48dc2/action_hmap.o: ../serv_common/utility/action_hmap.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/action_hmap.o ../serv_common/utility/action_hmap.c

${OBJECTDIR}/_ext/40f48dc2/adler32.o: ../serv_common/utility/adler32.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/adler32.o ../serv_common/utility/adler32.c

${OBJECTDIR}/_ext/40f48dc2/base64.o: ../serv_common/utility/base64.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/base64.o ../serv_common/utility/base64.c

${OBJECTDIR}/_ext/40f48dc2/cache_hmap.o: ../serv_common/utility/cache_hmap.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/cache_hmap.o ../serv_common/utility/cache_hmap.c

${OBJECTDIR}/_ext/40f48dc2/hash_map.o: ../serv_common/utility/hash_map.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/hash_map.o ../serv_common/utility/hash_map.c

${OBJECTDIR}/_ext/40f48dc2/hash_table.o: ../serv_common/utility/hash_table.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/hash_table.o ../serv_common/utility/hash_table.c

${OBJECTDIR}/_ext/40f48dc2/md5.o: ../serv_common/utility/md5.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/md5.o ../serv_common/utility/md5.c

${OBJECTDIR}/_ext/40f48dc2/strptime.o: ../serv_common/utility/strptime.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/strptime.o ../serv_common/utility/strptime.c

${OBJECTDIR}/_ext/40f48dc2/tiny_pool.o: ../serv_common/utility/tiny_pool.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/40f48dc2
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40f48dc2/tiny_pool.o ../serv_common/utility/tiny_pool.c

${OBJECTDIR}/_ext/f534dc3f/access_log.o: ../third_party/access_log.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/access_log.o ../third_party/access_log.c

${OBJECTDIR}/_ext/f534dc3f/binary_logger.o: ../third_party/binary_logger.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/binary_logger.o ../third_party/binary_logger.c

${OBJECTDIR}/_ext/f534dc3f/dmap64.o: ../third_party/dmap64.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/dmap64.o ../third_party/dmap64.c

${OBJECTDIR}/_ext/f534dc3f/logger.o: ../third_party/logger.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/logger.o ../third_party/logger.c

${OBJECTDIR}/_ext/f534dc3f/memory_utility.o: ../third_party/memory_utility.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/memory_utility.o ../third_party/memory_utility.c

${OBJECTDIR}/_ext/f534dc3f/pmap64.o: ../third_party/pmap64.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/pmap64.o ../third_party/pmap64.c

${OBJECTDIR}/_ext/f534dc3f/tiny_map.o: ../third_party/tiny_map.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/tiny_map.o ../third_party/tiny_map.c

${OBJECTDIR}/auth_bzl.o: auth_bzl.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/auth_bzl.o auth_bzl.c

${OBJECTDIR}/epoll_serv.o: epoll_serv.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/epoll_serv.o epoll_serv.c

${OBJECTDIR}/evd_list.o: evd_list.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/evd_list.o evd_list.c

${OBJECTDIR}/excep.o: excep.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/excep.o excep.c

${OBJECTDIR}/hand_epoll.o: hand_epoll.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/hand_epoll.o hand_epoll.c

${OBJECTDIR}/options.o: options.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/options.o options.c

${OBJECTDIR}/parse_conf.o: parse_conf.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/parse_conf.o parse_conf.c

${OBJECTDIR}/process_cycle.o: process_cycle.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/process_cycle.o process_cycle.c

${OBJECTDIR}/regis_serv.o: regis_serv.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/regis_serv.o regis_serv.c

${OBJECTDIR}/rserv_bzl.o: rserv_bzl.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/rserv_bzl.o rserv_bzl.c

${OBJECTDIR}/rserv_object.o: rserv_object.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/rserv_object.o rserv_object.c

${OBJECTDIR}/sdtp_utility.o: sdtp_utility.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sdtp_utility.o sdtp_utility.c

${OBJECTDIR}/session.o: session.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/session.o session.c

${OBJECTDIR}/value_layer.o: value_layer.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I. -I../serv_common -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/value_layer.o value_layer.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/regis_serv

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
