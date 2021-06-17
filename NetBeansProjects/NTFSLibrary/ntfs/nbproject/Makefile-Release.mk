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
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/dir.o \
	${OBJECTDIR}/attrlist.o \
	${OBJECTDIR}/bitmap.o \
	${OBJECTDIR}/logging.o \
	${OBJECTDIR}/misc.o \
	${OBJECTDIR}/compress.o \
	${OBJECTDIR}/win32_io.o \
	${OBJECTDIR}/debug.o \
	${OBJECTDIR}/security.o \
	${OBJECTDIR}/collate.o \
	${OBJECTDIR}/inode.o \
	${OBJECTDIR}/mst.o \
	${OBJECTDIR}/bootsect.o \
	${OBJECTDIR}/crypto.o \
	${OBJECTDIR}/version.o \
	${OBJECTDIR}/compat.o \
	${OBJECTDIR}/logfile.o \
	${OBJECTDIR}/mft.o \
	${OBJECTDIR}/attrib.o \
	${OBJECTDIR}/device.o \
	${OBJECTDIR}/device_io.o \
	${OBJECTDIR}/volume.o \
	${OBJECTDIR}/runlist.o \
	${OBJECTDIR}/index.o \
	${OBJECTDIR}/unistr.o \
	${OBJECTDIR}/lcnalloc.o


# C Compiler Flags
CFLAGS=-DHAVE_CONFIG_H

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libntfs.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libntfs.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libntfs.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libntfs.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libntfs.a

${OBJECTDIR}/dir.o: dir.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/dir.o dir.c

${OBJECTDIR}/attrlist.o: attrlist.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/attrlist.o attrlist.c

${OBJECTDIR}/bitmap.o: bitmap.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/bitmap.o bitmap.c

${OBJECTDIR}/logging.o: logging.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/logging.o logging.c

${OBJECTDIR}/misc.o: misc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/misc.o misc.c

${OBJECTDIR}/compress.o: compress.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/compress.o compress.c

${OBJECTDIR}/win32_io.o: win32_io.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/win32_io.o win32_io.c

${OBJECTDIR}/debug.o: debug.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/debug.o debug.c

${OBJECTDIR}/security.o: security.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/security.o security.c

${OBJECTDIR}/collate.o: collate.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/collate.o collate.c

${OBJECTDIR}/inode.o: inode.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/inode.o inode.c

${OBJECTDIR}/mst.o: mst.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/mst.o mst.c

${OBJECTDIR}/bootsect.o: bootsect.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/bootsect.o bootsect.c

${OBJECTDIR}/crypto.o: crypto.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/crypto.o crypto.c

${OBJECTDIR}/version.o: version.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/version.o version.c

${OBJECTDIR}/compat.o: compat.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/compat.o compat.c

${OBJECTDIR}/logfile.o: logfile.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/logfile.o logfile.c

${OBJECTDIR}/mft.o: mft.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/mft.o mft.c

${OBJECTDIR}/attrib.o: attrib.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/attrib.o attrib.c

${OBJECTDIR}/device.o: device.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/device.o device.c

${OBJECTDIR}/device_io.o: device_io.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/device_io.o device_io.c

${OBJECTDIR}/volume.o: volume.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/volume.o volume.c

${OBJECTDIR}/runlist.o: runlist.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/runlist.o runlist.c

${OBJECTDIR}/index.o: index.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/index.o index.c

${OBJECTDIR}/unistr.o: unistr.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/unistr.o unistr.c

${OBJECTDIR}/lcnalloc.o: lcnalloc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -I/C/MinGW/msys/1.0/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lcnalloc.o lcnalloc.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libntfs.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
