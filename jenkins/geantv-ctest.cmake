####################################################################
# Before run should be exported next variables:
# $CTEST_BUILD_OPTIONS // CMake flags for Geant-V build
# $CMAKE_SOURCE_DIR    // CMake source directory
# $CMAKE_BINARY_DIR    // CMake binary directory
# $CMAKE_BUILD_TYPE    // CMake build type: Debug, Release
# $CMAKE_INSTALL_PREFIX // Installation prefix for CMake (Jenkins trigger)
# CC and CXX (In Jenkins this step has been done authomaticly)
# export $LD_LIBRARY_PATH=$WORKSPACE/lib:$LD_LIBRARY_PATH (for GeantV libraries)
# Enviroment for name of build for CERN CDash:
# $LABEL                // Name of node (Jenkins trigger)
# Name of $BACKEND     // Backend for Geant-V (VecGeom/ROOT or CUDA)

cmake_minimum_required(VERSION 2.8)
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS "1000")
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS "1000")
set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE "50000")
###################################################################
macro(CheckExitCode)
  if(NOT ${ExitCode} EQUAL 0)
    return(${ExitCode})
 endif()
endmacro(CheckExitCode)

####################################################################
# Build name settings
find_program(UNAME NAMES uname)
macro(getuname name flag)
  exec_program("${UNAME}" ARGS "${flag}" OUTPUT_VARIABLE "${name}")
endmacro(getuname)

getuname(osname -s)
getuname(osrel  -r)
getuname(cpu    -m)

if (DEFINED ENV{BACKEND})
  set(CTEST_BUILD_NAME "$ENV{OPTION}-${cpu}+$ENV{BACKEND}-$ENV{LABEL}-$ENV{COMPILER}-$ENV{CMAKE_BUILD_TYPE}")
else()
  set(CTEST_BUILD_NAME "$ENV{OPTION}-${cpu}-$ENV{LABEL}-$ENV{COMPILER}-$ENV{CMAKE_BUILD_TYPE}")
endif()
if(DEFINED ENV{gitlabMergedByUser} AND DEFINED ENV{gitlabMergeRequestIid})
  set(CTEST_BUILD_NAME "$ENV{gitlabMergedByUser}#$ENV{gitlabMergeRequestIid}-${CTEST_BUILD_NAME}")
endif()

message("CTEST name: ${CTEST_BUILD_NAME}")

find_program(HOSTNAME_CMD NAMES hostname)
exec_program(${HOSTNAME_CMD} ARGS OUTPUT_VARIABLE HOSTNAME)
IF(NOT DEFINED CTEST_SITE)
  SET(CTEST_SITE "${HOSTNAME}")
ENDIF(NOT DEFINED CTEST_SITE)

#######################################################
# Build dashboard model setup

#------------ Cdash---------------------------------
if("$ENV{MODE}" STREQUAL "")
  set(CTEST_MODE Experimental)
else()
  set(CTEST_MODE "$ENV{MODE}")
endif()

if(${CTEST_MODE} MATCHES nightly)
  SET(MODEL Nightly)
elseif(${CTEST_MODE} MATCHES continuous)
  SET(MODEL Continuous)
elseif(${CTEST_MODE} MATCHES memory)
  SET(MODEL NightlyMemoryCheck)
elseif(${CTEST_MODE} MATCHES benchmarks)
  SET(MODEL Benchmarks)
else()
  SET(MODEL Experimental)
endif()

find_program(CTEST_COMMAND_BIN NAMES ctest)
SET (CTEST_COMMAND
    "$CTEST_COMMAND_BIN -D ${MODEL}")

#######################################################
if(MODEL STREQUAL NightlyMemoryCheck)
  set(WITH_MEMCHECK TRUE)
#  find_package(Valgrind)
#  find_package(ROOT REQUIRED)
  if(ROOT)
    set(MEMORYCHECK_SUPPRESSIONS_FILE "${ROOTSYS}/etc/valgrind-root.supp")
  endif()
  set(CTEST_MEMORYCHECK_COMMAND ${VALGRIND_PROGRAM})
else()
  set(WITH_MEMCHECK FALSE)
endif()

######################################################
set(WITH_COVERAGE FALSE)

#######################################################
# CTest/CMake settings

set(CTEST_TEST_TIMEOUT 3600)
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS "2000")
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS "2000")
set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE "5000")
set(CTEST_BUILD_CONFIGURATION "$ENV{CMAKE_BUILD_TYPE}")
set(CTEST_INSTALL_PREFIX "$ENV{CMAKE_INSTALL_PREFIX}")
set(CTEST_SOURCE_DIRECTORY "$ENV{CMAKE_SOURCE_DIR}")
set(CTEST_BINARY_DIRECTORY "$ENV{CMAKE_BINARY_DIR}")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_OPTIONS "$ENV{CTEST_BUILD_OPTIONS}")

set(config_options -DCMAKE_INSTALL_PREFIX=${CTEST_INSTALL_PREFIX} 
                   -DCMAKE_BUILD_TYPE=${CTEST_BUILD_CONFIGURATION} 
                   -DUSE_ROOT=ON 
                   -DCTEST=ON
                   -DBUILD_REAL_PHYSICS_TESTS=ON
                   $ENV{ExtraCMakeOptions})

if("$ENV{COMPILER}" MATCHES gcc7)
  list(APPEND config_options -DCMAKE_CXX_STANDARD=17)
else()
  list(APPEND config_options -DCMAKE_CXX_STANDARD=14)
endif()

if("$ENV{OPTION}" STREQUAL "NUMA")
  list(APPEND config_options -DUSE_NUMA=ON)
elseif("$ENV{OPTION}" STREQUAL "TBB")
  list(APPEND config_options -DUSE_TBB=ON)
endif()
 
ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

#########################################################
# git command configuration

find_program(CTEST_GIT_COMMAND NAMES git)
if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}")
  set(CTEST_CHECKOUT_COMMAND "${CTEST_GIT_COMMAND} clone https://gitlab.cern.ch/GeantV/geant.git ${CTEST_SOURCE_DIRECTORY}")
endif()
set(CTEST_GIT_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")
if(${MODEL} MATCHES Nightly OR Experimental)
    if(NOT "$ENV{GIT_COMMIT}" STREQUAL "")
        set(CTEST_CHECKOUT_COMMAND "cmake -E chdir ${CTEST_SOURCE_DIRECTORY} ${CTEST_GIT_COMMAND} checkout -f $ENV{GIT_PREVIOUS_COMMIT}")
        set(CTEST_GIT_UPDATE_CUSTOM  ${CTEST_GIT_COMMAND} checkout -f $ENV{GIT_COMMIT})
    endif()
else()
    if(NOT "$ENV{GIT_COMMIT}" STREQUAL "")
        set(CTEST_CHECKOUT_COMMAND "cmake -E chdir ${CTEST_SOURCE_DIRECTORY} ${CTEST_GIT_COMMAND} checkout -f $ENV{GIT_COMMIT}")
        set(CTEST_GIT_UPDATE_CUSTOM  ${CTEST_GIT_COMMAND} checkout -f $ENV{GIT_COMMIT})
    endif()
endif()

#########################################################
## Output language
set($ENV{LC_MESSAGES}  "en_EN")

#########################################################
# Use multiple CPU cores to build
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
  if(NOT WIN32)
    set(CTEST_BUILD_FLAGS -j${N})
  endif()
  set(ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${N})
endif()

##########################################################
# Print summary information.
foreach(v
    CTEST_SITE
    CTEST_BUILD_NAME
    CTEST_SOURCE_DIRECTORY
    CTEST_BINARY_DIRECTORY
    CTEST_CMAKE_GENERATOR
    CTEST_BUILD_CONFIGURATION
    CTEST_GIT_COMMAND
    CTEST_CONFIGURE_COMMAND
    CTEST_SCRIPT_DIRECTORY
    CTEST_BUILD_FLAGS
    WITH_MEMCHECK
    WITH_COVERAGE
  )
  set(vars "${vars}  ${v}=[${${v}}]\n")
endforeach(v)
message("Dashboard script configuration (check if everything is declared correctly):\n${vars}\n")

#######################################################
# Test custom update with a dashboard script.
message("Running CTest Dashboard Script (custom update)...")
include("${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake")

#ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

ctest_start(${MODEL} TRACK ${MODEL})

ctest_update(SOURCE ${CTEST_SOURCE_DIRECTORY})

ctest_configure(BUILD   ${CTEST_BINARY_DIRECTORY}
                SOURCE  ${CTEST_SOURCE_DIRECTORY}
                OPTIONS "${config_options}"
                APPEND)
ctest_submit(PARTS Update Configure Notes)

ctest_build(BUILD ${CTEST_BINARY_DIRECTORY} TARGET install APPEND)
ctest_submit(PARTS Build)

ctest_test(BUILD ${CTEST_BINARY_DIRECTORY} APPEND)
ctest_submit(PARTS Test)

if(${MODEL} MATCHES NightlyMemoryCheck)
  ctest_submit(PARTS MemCheck)
endif()
