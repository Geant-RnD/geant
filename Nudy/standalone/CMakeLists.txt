file (GLOB sources 
 ${CMAKE_SOURCE_DIR}/Nudy/src/*.cc
 ${CMAKE_SOURCE_DIR}/Nudy/standalone/NudyMain.cc
)

file (GLOB headers 
 ${CMAKE_SOURCE_DIR}/Nudy/inc/*.h
)

include_directories(
 ${CMAKE_SOURCE_DIR}/Nudy/inc
)

#---------------------------------
#  Executable
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/NUDY/bin)


add_executable(NudyMain ${CMAKE_SOURCE_DIR}/Nudy/standalone/NudyMain.cc ${sources})
target_link_libraries(NudyMain -L${CMAKE_LIBRARY_OUTPUT_DIRECTORY} Nudy )

install(TARGETS NudyMain DESTINATION bin)
