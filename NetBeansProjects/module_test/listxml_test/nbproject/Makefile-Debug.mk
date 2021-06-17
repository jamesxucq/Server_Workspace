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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/dires_xml.o \
	${OBJECTDIR}/fatt_xml.o \
	${OBJECTDIR}/listxml.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/recu_directory.o \
	${OBJECTDIR}/recu_utility.o


# C Compiler Flags
CFLAGS=-D_LARGEFILE64_SOURCE

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/listxml_test

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/listxml_test: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/listxml_test ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/dires_xml.o: dires_xml.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -D_LARGEFILE64_SOURCE -MMD -MP -MF $@.d -o ${OBJECTDIR}/dires_xml.o dires_xml.c

${OBJECTDIR}/fatt_xml.o: fatt_xml.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -D_LARGEFILE64_SOURCE -MMD -MP -MF $@.d -o ${OBJECTDIR}/fatt_xml.o fatt_xml.c

${OBJECTDIR}/listxml.o: listxml.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -D_LARGEFILE64_SOURCE -MMD -MP -MF $@.d -o ${OBJECTDIR}/listxml.o listxml.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -D_LARGEFILE64_SOURCE -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/recu_directory.o: recu_directory.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -D_LARGEFILE64_SOURCE -MMD -MP -MF $@.d -o ${OBJECTDIR}/recu_directory.o recu_directory.c

${OBJECTDIR}/recu_utility.o: recu_utility.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -D_LARGEFILE64_SOURCE -MMD -MP -MF $@.d -o ${OBJECTDIR}/recu_utility.o recu_utility.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/listxml_test

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
