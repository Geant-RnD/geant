include(cmake/modules/RecordCmdLine.cmake)

# uninstall target
configure_file(
    "cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

include(cmake/modules/DoxygenSetup.cmake)

#----------------------------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.7.0 FATAL_ERROR)
project(GEANTV)
enable_language(CXX)
set(GEANTV_VERSION pre-beta-2)

if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
# Fix bug on APPLE, this is the default everywhere
  set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
  set(CMAKE_MACOSX_RPATH ON)
endif()

#----------------------------------------------------------------------------------------------
# Additional options
#
option(KNC "Enable compilation for KNC." OFF)

option(KNL "Enable compilation for KNL." OFF)

option(CUDA "Enable compilation for CUDA." OFF)

option(DATA_DOWNLOAD "Enable data download" OFF)

option(DISABLE_WEFFC "Disable Weffc++ flag" ON)

message(STATUS "DISABLE_WEFFC=" ${DISABLE_WEFFC})

option(GPERFTOOLS "Enable profiling using Google performance tools" OFF)

option(MAGIC_DEBUG "Debug xsec serialisation with magic number" ON)
if(MAGIC_DEBUG)
  add_definitions(-DMAGIC_DEBUG)
endif()

set(CMAKE_MACOSX_RPATH ON)

option(GEANT_INSTALL_RPATH "Enable RPATH injection for installed binaries" ON)

option(WITH_GEANT4 "Enable libraries and test using the Geant4 libraries" ON)
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)

option(WITH_NUDY "Enable building Nudy neutron library" ON)

#----------------------------------------------------------------------------------------------
# Use absolute RPATH when linking to avoid needing to export LD_LIBRARY_PATH
#
if (GEANT_INSTALL_RPATH)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  # RPATH to be used when installing, but only if it's not a system directory
  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  endif("${isSystemDir}" STREQUAL "-1")
endif()

set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)

#----------------------------------------------------------------------------------------------
# Process various options
#
option(CTEST "Enable CTest when building." OFF)
set(CTEST_LEVEL 1 CACHE STRING "Select the amount of test including in CTEST (1 for minimun, 99 for maximum)")
if (CTEST)
  set (DATA_DOWNLOAD ON)
endif()

option(USE_VECPHYS "Enable activation of vector physics." OFF)
if(USE_VECPHYS)
  add_definitions(-DUSE_VECPHYS)
  message(STATUS "USE_VECPHYS ON  : Vector physics is active !")
else()
  message(STATUS "USE_VECPHYS OFF : Vector physics is NOT active !")
endif()

option(VECTORIZED_GEOMETRY "Enable vectorized geometry" OFF)
if(VECTORIZED_GEOMETRY)
  add_definitions(-DVECTORIZED_GEOMETRY)
  message(STATUS "VECTORIZED_GEOMETRY ON  : Vectorized geometry ON !")
else()
  message(STATUS "VECTORIZED_GEOMETRY OFF : Vectorized geometry OFF !")
endif()

option(USE_CALLGRIND_CONTROL "Enable callgrind profiling" OFF)
if(USE_CALLGRIND_CONTROL)
  add_definitions(-DUSE_CALLGRIND_CONTROL)
  message(STATUS "USE_CALLGRIND_CONTROL ON  : Callgrind profiling enabled")
else()
  message(STATUS "USE_CALLGRIND_CONTROL OFF : Callgrind profiling disabled")
endif()

option(BUG_HUNT "Enable crosschecks for finding bugs." OFF)
option(USE_ROOT "Use ROOT" ON)
option(USE_NUMA "Use NUMA" ON)
option(USE_TBB "Use threading building blocks" OFF)

option(BUILD_REAL_PHYSICS_TESTS "Build real-physics GeantV and the corresponding Geant4 test applications." OFF)
if(BUILD_REAL_PHYSICS_TESTS)
  message (STATUS "BUILD_PHYSICS_TESTS ON  : Physics tests will be built.")
  add_definitions(-DBUILD_REAL_PHYSICS_TESTS)
else()
  message (STATUS "BUILD_PHYSICS_TESTS OFF : Physics tests won't be built.")
endif()

option(USE_VECGEOM_CONTAINERS "Replace std::vector, std::map with vecgeom::Vector, vecgeom::map in the material lib." ON)
if (USE_VECGEOM_CONTAINERS)
  message(STATUS "USE_VECGEOM_CONTAINERS ON : Material library will be built with vecgeom::Vector and vecgeom::map.")
  add_definitions(-DUSE_VECGEOM_CONTAINERS)
else()
  message(STATUS "USE_VECGEOM_CONTAINERS OFF : Material library will be built with std::vector and std::map.")
endif()

#----------------------------------------------------------------------------------------------
# Find packages
#
find_package(VecMath 0.1.0 REQUIRED COMPONENTS Vc)
#Temporary solution is to use VecMath_INCLUDES/LIBRARIES variables since GV does not support modern cmake targets
include_directories(${VecMath_INCLUDE_DIRS})
add_definitions(${VecMath_DEFINITIONS})

find_package(HepMC REQUIRED)
if(HepMC_FOUND)
  message(STATUS "HepMC includes: ${HEPMC_INCLUDE_DIR}")
  include_directories(AFTER SYSTEM ${HEPMC_INCLUDE_DIR})
endif()

if (KNC)
  if (USE_ROOT)
    message (WARNING "Root has to be disabled in order to compile for KNC, setting USE_ROOT=OFF")
    set (USE_ROOT OFF CACHE BOOL "Use ROOT" FORCE)
  endif()
  if (WITH_GEANT4)
    message (WARNING "Disabling Geant4 in order to compile for KNC, setting WITH_GEANT4=OFF")
    set (WITH_GEANT4 OFF CACHE BOOL "Enable libraries and test using the Geant4 libraries" FORCE)
  endif()
  if (CUDA)
    message (FATAL "CUDA and KNC are incompatible flags")
  endif()
endif()

#----------------------------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
if(WITH_GEANT4)
  if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
  else()
    find_package(Geant4 REQUIRED)
  endif()
endif()

#----------------------------------------------------------------------------------------------
# Set pathes where to put the libraries, executables and headers
#
if( GEANT_OUTPUT_DIR )
    set(OUTDIR ${GEANT_OUTPUT_DIR})
else()
    set(OUTDIR $ENV{GEANT_OUTPUT_DIR})
endif()
if(NOT OUTDIR)
  set(OUTDIR ${PROJECT_BINARY_DIR})
endif()
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTDIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTDIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTDIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/modules)

#----------------------------------------------------------------------------------------------
# Find VecGeom geometry headers library
#
set(VECGEOM_DIR "" CACHE DIRECTORY "Location of VecGeom")
if(VECGEOM_DIR)
  list(APPEND CMAKE_PREFIX_PATH ${VECGEOM_DIR})
endif()
if($ENV{VECGEOM_DIR})
  list(APPEND CMAKE_PREFIX_PATH $ENV{VECGEOM_DIR})
endif()

find_package(VecGeom REQUIRED)
if( VECGEOM_FOUND )
   message(STATUS "VecGeom libraries ${VECGEOM_LIBRARIES}")
   find_package(Vc REQUIRED)
   add_definitions( "-DUSE_VECGEOM_NAVIGATOR" )
   #---ensure that GeantV is compiled with same architecture flags as VecGeom
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VECGEOM_CXX_FLAGS}")
   include_directories(${VECGEOM_INCLUDE_DIRS})
   set ( VECGEOM_ROOT OFF )
   if ( VECGEOM_DEFINITIONS MATCHES VECGEOM_ROOT )
     set ( VECGEOM_ROOT ON )
   endif()
   #---- For the moment USE_ROOT must match VECGEOM_ROOT ----
   if (NOT ${USE_ROOT} MATCHES ${VECGEOM_ROOT})
      message (STATUS "VecGeom was compiled with VECGEOM_ROOT=" ${VECGEOM_ROOT} " while USE_ROOT=" ${USE_ROOT})
      if ( USE_ROOT )
         message(WARNING "USE_ROOT was disabled")
         set (USE_ROOT OFF CACHE BOOL "Use ROOT" FORCE)
      else()
         message(WARNING "USE_ROOT was enabled")
         set (USE_ROOT ON CACHE BOOL "Use ROOT" FORCE)
      endif()
   endif()

else()
   message(FATAL_ERROR "Required VecGeom library not found")
endif()

#----------------------------------------------------------------------------------------------
# Find ROOT and setup the libraries
#

if(USE_ROOT)
# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
   list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
   find_package(ROOT REQUIRED COMPONENTS EG Gui Geom TreePlayer)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
   include(${ROOT_USE_FILE})
   include_directories(AFTER SYSTEM ${ROOT_INCLUDE_DIRS})

   add_definitions("-DUSE_ROOT")
else()
#----------------------------------------------------------------------------------------------
# ROOT adds some of these flags. If we do not use ROOT, we have to add them by hand
#
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -pipe")
endif()

#----------------------------------------------------------------------------
#Find TBB and setup the libraries
#
set (TBB_LIBRARIES)
if(USE_TBB)
  find_package(TBB)
  if( TBB_FOUND )
    add_definitions("-DGEANT_TBB")
    message(STATUS "USE_TBB ON  : TBB enabled")
  else()
    message(STATUS "TBB requested but not found")
    set(USE_TBB OFF CACHE BOOL "" FORCE)
  endif()
else()
  message(STATUS "USE_TBB OFF  : TBB disabled")
endif()

#----------------------------------------------------------------------------
# Find if the system is NUMA aware
#----------------------------------------------------------------------------
set(NUMA_ALL_LIBRARIES GeantNuma)
if (USE_NUMA AND APPLE)
  # Disable NUMA for APPLE
  set(USE_NUMA OFF)
  message(STATUS "NUMA disabled on APPLE")
endif()
if (USE_NUMA)
  # Check the presence of hwloc library
  find_package(hwloc)
  if (HWLOC_FOUND)
    message(STATUS "Hardware locality (hwloc) libraries found. ${HWLOC_INCLUDE_DIRS}")
    add_definitions( "-DGEANT_USE_NUMA" )
    include_directories( AFTER SYSTEM ${HWLOC_INCLUDE_DIRS})
    set(NUMA_LIBRARIES -L${HWLOC_LIBDIR} -l${HWLOC_LIBRARIES})
    set(NUMA_ALL_LIBRARIES GeantNuma ${NUMA_LIBRARIES})
  else()
    message(STATUS "Hardware locality (hwloc) libraries not found. Disabling NUMA.")
    set(USE_NUMA OFF)
  endif()
endif()

#----------------------------------------------------------------------------
# Find GPERFTOOLS
#----------------------------------------------------------------------------
if (GPERFTOOLS)
  find_package(Gperftools)
  if (GPERFTOOLS_FOUND)
    add_definitions("-DGEANT_PERFTOOLS")
    message(STATUS "Found Gperftools in " ${GPERFTOOLS_LIBRARIES})
    set(PROFILE_LIBRARIES ${GPERFTOOLS_LIBRARIES})
    include_directories( AFTER SYSTEM ${GPERFTOOLS_INCLUDE_DIR})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -gdwarf")
  else()
    message(STATUS "Gperftools libraries not found. Install libgoogle-perftools-dev. Disabling Gperftools.")
    set(GPERFTOOLS OFF)
  endif()
endif()

# Nudy
if (WITH_NUDY AND NOT ROOT_mathmore_FOUND)
  message(STATUS "Nudy disabled since ROOT_mathmore was not found")
  set(WITH_NUDY OFF)
endif()

# KNL
if (KNL)
 set(USE_UMESIMD ON)
 if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -xMIC-AVX512")
 else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=knl")
 endif()
endif()

#----------------------------------------------------------------------------
# Load macros
#
include(cmake/modules/GeantMacros.cmake)

#----------------------------------------------------------------------------------------------
# see if we have CCACHE ( to avoid recompilation on branch switches )
#
# TBD: Temporary disabling ccache for GeantV (no sudo access to CI node)
#find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  message(STATUS "found ccache")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)


#----------------------------------------------------------------------------------------------
# Include creation and submission to dashboard cdash.cern.ch
#
include(CTest)

if (CTEST OR USE_TBB)
  set(DATA_DOWNLOAD ON)
  message(STATUS "Testing with CTest enabled.")
  enable_testing()
endif()

#----------------------------------------------------------------------------------------------
# Set data to be dowloaded
#
if(DATA_DOWNLOAD)
  file(MAKE_DIRECTORY data)
  message(STATUS "Downloading data files")

  DOWNLOAD_IF_NOT_INSTALLED("http://geant-server.web.cern.ch/geant-server/cms2018.root"
    "cms2018.root" "${CMAKE_SOURCE_DIR}/data/" "caf4c95d19317d04d42d56c8c45db911")
  DOWNLOAD_IF_NOT_INSTALLED("http://geant-server.web.cern.ch/geant-server/cms2018.gdml"
    "cms2018.gdml" "${CMAKE_SOURCE_DIR}/data/" "e7abc674d0e28b7a713a97c35b7c4bb9")
  DOWNLOAD_IF_NOT_INSTALLED("http://geant-server.web.cern.ch/geant-server/pp14TeVminbias.root"
    "pp14TeVminbias.root" "${CMAKE_SOURCE_DIR}/data/" "109b827c595ae4a62b74f08f860abbb7")
endif()

#----------------------------------------------------------------------------------------------
# Handle compiler flags
#
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
  # using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5")
    set(DISABLE_WEFFC ON)
    if ( Vc_FOUND )  # Actually this should really be if VecGeomConfigureToUseVC
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fabi-version=6")
    endif()
  endif()
  if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -faligned-new -Wno-format-truncation")
  endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
  if("${CMAKE_VERSION}" VERSION_LESS "3.6")
    # add flag by hand if CMake does not support ICC
    if("${CMAKE_CXX_STANDARD}" MATCHES "11|14")
      add_compile_options(-std=c++${CMAKE_CXX_STANDARD})
    else()
      message(FATAL_ERROR "Unsupported C++ standard requested")
    endif()
  endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

if (NOT DISABLE_WEFFC)
  set(WEFFC_FLAG -Weffc++)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DGEANT_DEBUG)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${WEFFC_FLAG} -ggdb")
  if(APPLE)
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunknown-pragmas")
   else()
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
   endif()
 else()
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
 endif()

 # set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

 if (KNC)
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmic")
   else()
     message(FATAL_ERROR "KNC native or offload compilation requires Intel Compiler.")
   endif()
 endif()


 get_property(GEANTV_COMPILE_DEFINITIONS GLOBAL PROPERTY COMPILE_DEFINITIONS)
 get_property(GEANTV_COMPILE_FLAGS_NOW GLOBAL PROPERTY COMPILE_FLAGS)
 set(GEANTV_CXX_NOW_FLAGS                "${CMAKE_CXX_FLAGS}")
 set(CMAKE_CXX_FLAGS_DEBUG               "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2")
 set(GEANTV_CXX_NOW_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ")
 set(GEANTV_CXX_NOW_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} ")
 set(GEANTV_CXX_NOW_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ")
 set(GEANTV_CXX_NOW_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ")
 set(GEANTV_EXE_NOW_LINKER_FLAGS         "${CMAKE_EXE_LINKER_FLAGS} ")
 if (CUDA)
   # Currently we are stuck with gcc 4.8 and a broken Weffc++ so let's not us it there
   set(GEANTV_WARNING_FLAGS "-W -Wall -Wno-narrowing -Wwrite-strings -Wcast-qual -Wshadow -Wno-long-long -pedantic")
 else()
   set(GEANTV_WARNING_FLAGS "-W -Wall ${WEFFC_FLAG} -Wno-narrowing -Wwrite-strings -Wcast-qual -Wshadow -Wno-long-long -pedantic")
 endif()

 set(GEANTV_CXX_FLAGS                "${CMAKE_CXX_FLAGS} ${GEANTV_WARNING_FLAGS}")
 set(GEANTV_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ${GEANTV_WARNING_FLAGS}")
 set(GEANTV_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} ${GEANTV_WARNING_FLAGS}")
 set(GEANTV_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ${GEANTV_WARNING_FLAGS}")
 set(GEANTV_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${GEANTV_WARNING_FLAGS}")
 set(GEANTV_EXE_LINKER_FLAGS         "${CMAKE_EXE_LINKER_FLAGS} ${GEANTV_WARNING_FLAGS}")
 set(GEANTV_COMPILE_FLAGS            "${GEANTV_COMPILE_FLAGS_NOW} ${GEANTV_WARNING_FLAGS}")

#----------------------------------------------------------------------------------------------
# Handle custom alloactor
#
if(CUSTOM_ALLOCATOR)
  find_package(CAllocator)
endif()

#----------------------------------------------------------------------------------------------
# CUDA section
#
if (CUDA)
  option(GEANT_CUDA_SINGLE_SOURCE "Compile all the GeantV CUDA code in a single compilation unit." ON)

  # We must have those over-ride placed here as they need to be done before loading FindCUDA.cmake
  set(CUDA_USE_STATIC_CUDA_RUNTIME OFF CACHE BOOL "Use the static version of the CUDA runtime library if available")
  set(CUDA_SEPARABLE_COMPILATION ON CACHE BOOL "Compile CUDA objects with separable compilation enabled.")

  find_package(CUDA REQUIRED)

  if (NOT CUDA_ARCH)
    if (CUDA_SEPARABLE_COMPILATION)
      set(CUDA_ARCH 30)
    else()
      message(FATAL_ERROR "VecGeom requires using separable compilation")
    endif()
    if (NOT VECGEOM_CUDA_ARCH)
      message(FATAL_ERROR "VecGeom not configured with CUDA/NVCC")
    endif()
    string(REGEX REPLACE "-arch=sm_([0-9]+)" "\\1" VECGEOM_CUDA_ARCH_NUM "${VECGEOM_CUDA_ARCH}")
    if (${VECGEOM_CUDA_ARCH_NUM} LESS ${CUDA_ARCH})
      message(FATAL_ERROR "GeantV must be configured with a CUDA SM (${CUDA_ARCH}) less than VecGeom's (${VECGEOM_CUDA_ARCH_NUM})" )
    elseif(${CUDA_ARCH} LESS ${VECGEOM_CUDA_ARCH_NUM})
      # all good
    elseif(${CUDA_ARCH} EQUAL ${VECGEOM_CUDA_ARCH_NUM})
      # all good
    else()
      message(WARNING "GeantV must be configured with a CUDA SM (${CUDA_ARCH}) less than VecGeom's (${VECGEOM_CUDA_ARCH_NUM}) but one of the two is invalid." )
    endif()
  endif()
  set(CUDA_ARCH "-arch=sm_${CUDA_ARCH}")

  if (NOT CPP11)
    message(FATAL_ERROR "NVCC (at least as of Cuda 8.0) does not support standard newer than C++11")
  endif()

  set(
    CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}
    -std c++11
    -Xcompiler ,\"-Wno-unused-function\"
    -Xcompiler ,\"-Wno-unused-parameter\"
    -O2 --use_fast_math -Xcudafe "--diag_suppress=code_is_unreachable"
    -Xcudafe "--diag_suppress=initialization_not_reachable"
    -D_MWAITXINTRIN_H_INCLUDED
    -D_FORCE_INLINES
  )

  if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -g -G)
  endif()
  # set(CUDA_SEPARABLE_COMPILATION ON)
  # set(CUDA_VERBOSE_BUILD ON)

  if (VECGEOM_CUDA_LIBRARY)
    SET(VECGEOM_LIBRARIES ${VECGEOM_LIBRARIES} ${CUDA_LIBRARIES} )
    message(STATUS "Updated VecGeom libraries to ${VECGEOM_LIBRARIES}")
  endif()

  include_directories(AFTER SYSTEM ${CUDA_INCLUDE_DIRS})
  message(STATUS "Added to include directories:  ${CUDA_INCLUDE_DIRS}")

  # FindCUDA.mk generate cmake file for each .o file using
  # the cmake command:
  #   file(GENERATE
  #      OUTPUT "${custom_target_script}"
  #      INPUT "${custom_target_script_pregen}"
  #      )
  # However this fails to register the output as a 'BYPRODUCTS'
  # and thus, since the .o file depends on it, trigger the CMP0058
  # warning (when using Ninja).
  # Using a add_custom_target/command does not work there as
  # neither the OUTPUT not the BYPRODUCTS parameter supports
  # generator expression (and ${custom_target_script} is a
  # generator expression depending on the configuration type).
  # Consequently there is no (known to the author) way of
  # correctly adding the correct information at the moment.
  # So let's just use the old policy (even-though it is
  # deprecated ... )
  # Note: this is a feature introduced in CMake 3.6 to
  # support  generator expressions in CUDA_NVCC_FLAGS
  # and since we imported that version's FindCUDA.cmake
  # it applies to all cmake that support CMP0058.
  if (NOT (CMAKE_VERSION LESS 3.3))
     cmake_policy(SET CMP0058 OLD)
  endif()

endif()

#----------------------------------------------------------------------------------------------
# Include directories
#
include_directories("${PROJECT_SOURCE_DIR}/run/scheduler/inc")
include_directories("${PROJECT_SOURCE_DIR}/core/base/inc")
include_directories("${PROJECT_SOURCE_DIR}/magneticfield/inc")
#include_directories("${PROJECT_SOURCE_DIR}/physics/kernel/material/inc")
include_directories("${PROJECT_SOURCE_DIR}/examples/physics/inc")
include_directories("${PROJECT_SOURCE_DIR}/examples/magneticfield/simplifiedCMS/include")

string(REPLACE " -DVECGEOM_ROOT" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#----------------------------------------------------------------------------------------------
# Add subdirectories
#
add_custom_target(_headers ALL)

message(STATUS "COMPILATION FLAGS ARE -${GEANTV_CXX_FLAGS} - ${GEANTV_COMPILATION_FLAGS}")

add_subdirectory (core)

add_subdirectory (run)
add_subdirectory (magneticfield)

add_subdirectory (examples)

if (WITH_NUDY)
  add_subdirectory (physics/neutron/nudy)
endif()

add_subdirectory(physics)
if (BUILD_REAL_PHYSICS_TESTS)
  add_subdirectory(physics/tests)
endif()

add_subdirectory(physics/kernel/material)

add_subdirectory(physics/generators)



#----------------------------------------------------------------------------------------------
# Process Doxygen file
#
if(DOXYGEN_FOUND)
  set(DOXYFILE_SOURCE_DIRS "${DOXYFILE_SOURCE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/doc")
  configure_file("${DOXYFILE_IN}" "${DOXYFILE}" @ONLY)
endif()
