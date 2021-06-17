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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/dmap64.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/memory_utility.o \
	${OBJECTDIR}/pmap64.o \
	${OBJECTDIR}/rivfs/creat_river.o \
	${OBJECTDIR}/rivfs/riv_md5.o \
	${OBJECTDIR}/rivfs/riv_sha1.o \
	${OBJECTDIR}/rivfs/river_cache.o \
	${OBJECTDIR}/rivfs/river_entity.o \
	${OBJECTDIR}/rivfs/river_utility.o \
	${OBJECTDIR}/rivfs/rivfs.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/river_test

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/river_test: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/river_test ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/dmap64.o: dmap64.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/dmap64.o dmap64.c

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/memory_utility.o: memory_utility.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/memory_utility.o memory_utility.c

${OBJECTDIR}/pmap64.o: pmap64.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/pmap64.o pmap64.c

${OBJECTDIR}/rivfs/creat_river.o: rivfs/creat_river.cpp 
	${MKDIR} -p ${OBJECTDIR}/rivfs
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/rivfs/creat_river.o rivfs/creat_river.cpp

${OBJECTDIR}/rivfs/riv_md5.o: rivfs/riv_md5.c 
	${MKDIR} -p ${OBJECTDIR}/rivfs
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/rivfs/riv_md5.o rivfs/riv_md5.c

${OBJECTDIR}/rivfs/riv_sha1.o: rivfs/riv_sha1.c 
	${MKDIR} -p ${OBJECTDIR}/rivfs
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/rivfs/riv_sha1.o rivfs/riv_sha1.c

${OBJECTDIR}/rivfs/river_cache.o: rivfs/river_cache.cpp 
	${MKDIR} -p ${OBJECTDIR}/rivfs
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/rivfs/river_cache.o rivfs/river_cache.cpp

${OBJECTDIR}/rivfs/river_entity.o: rivfs/river_entity.cpp 
	${MKDIR} -p ${OBJECTDIR}/rivfs
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/rivfs/river_entity.o rivfs/river_entity.cpp

${OBJECTDIR}/rivfs/river_utility.o: rivfs/river_utility.cpp 
	${MKDIR} -p ${OBJECTDIR}/rivfs
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/rivfs/river_utility.o rivfs/river_utility.cpp

${OBJECTDIR}/rivfs/rivfs.o: rivfs/rivfs.cpp 
	${MKDIR} -p ${OBJECTDIR}/rivfs
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/rivfs/rivfs.o rivfs/rivfs.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/river_test

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
