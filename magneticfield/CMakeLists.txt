#----------------------------------------------------------------------------------------------
# Add source files & include directories
#
file(GLOB sources src/*.cxx) # (T*.cxx src/GU*.cxx)
file(GLOB headers inc/Geant/*.h)   # (T*.h inc/GU*.h)

include_directories(
  inc
  ${CMAKE_SOURCE_DIR}/exmaples/magneticfield/simplifiedCMS/inc
  ${CMAKE_SOURCE_DIR}/base/inc
)  

# - print include directories
# get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
# message(STATUS "Include directories: ${inc_dirs}")

#----------------------------------------------------------------------------------------------
# Add the library
#
# ROOT_GENERATE_DICTIONARY(vMagFieldDict ${headers} MODULE Vphysproc LINKDEF inc/Geant/LinkDef.h)
add_library(Vmagfield SHARED ${sources} )
target_link_libraries(Vmagfield  -L${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${VECGEOM_LIBRARIES} ${VecCoreLib_LIBRARIES})

#----------------------------------------------------------------------------------------------
# Install the files
#
install(TARGETS Vmagfield LIBRARY DESTINATION lib)
install(FILES ${headers} DESTINATION inc/Geant)
add_subdirectory(tests)

#----------------------------------------------------------------------------------------------
# Add this directory to Doxygen
#
if(DOXYGEN_FOUND)
  set(DOXYFILE_SOURCE_DIRS "${DOXYFILE_SOURCE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
endif()
