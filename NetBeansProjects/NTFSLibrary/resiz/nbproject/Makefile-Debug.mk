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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ntfsinfo.o \
	${OBJECTDIR}/makedev.o \
	${OBJECTDIR}/ntfsresiz.o


# C Compiler Flags
CFLAGS=-DHAVE_CONFIG_H -DCLEAN_EXIT -DBUILDING_DLL

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../ntfs/dist/Debug/MinGW-Windows/libntfs.a -liconv -lintl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libresiz.dll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libresiz.dll: ../ntfs/dist/Debug/MinGW-Windows/libntfs.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libresiz.dll: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -shared -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libresiz.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/ntfsinfo.o: ntfsinfo.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -I/C/MinGW/msys/1.0/usr/include -I../ntfs  -MMD -MP -MF $@.d -o ${OBJECTDIR}/ntfsinfo.o ntfsinfo.c

${OBJECTDIR}/makedev.o: makedev.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -I/C/MinGW/msys/1.0/usr/include -I../ntfs  -MMD -MP -MF $@.d -o ${OBJECTDIR}/makedev.o makedev.c

${OBJECTDIR}/ntfsresiz.o: ntfsresiz.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -I/C/MinGW/msys/1.0/usr/include -I../ntfs  -MMD -MP -MF $@.d -o ${OBJECTDIR}/ntfsresiz.o ntfsresiz.c

# Subprojects
.build-subprojects:
	cd ../ntfs && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libresiz.dll

# Subprojects
.clean-subprojects:
	cd ../ntfs && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
