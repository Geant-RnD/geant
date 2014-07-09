# Finds VecGeom installation ( the vectorized geometry library )
# This module sets up VecGeom information 
# It defines:
# VECGEOM_FOUND          If the library is found
# VECGEOM_INCLUDE_DIR    PATH to the include directory
# VECGEOM_LIBRARIES      Most common libraries
# VECGEOM_LIBRARY_DIR    PATH to the library directory 

# look if an environment variable VCROOT exists

set(VECGEOMROOT $ENV{VECGEOMROOT})

find_library(VECGEOM_LIBRARIES libvecgeom_cpp.a PATHS ${VECGEOMROOT}/lib)
# find_library(USOLIDS_LIBRARIES libusolids.a PATHS ${VECGEOMROOT}/lib)
# if (VECGEOM_LIBRARIES AND USOLIDS_LIBRARIES) 
if (VECGEOM_LIBRARIES) 
   set(VECGEOM_FOUND TRUE)	
   string(REPLACE "/lib/libvecgeom_cpp.a" "" VECGEOMROOT  ${VECGEOM_LIBRARIES})
#   set(VECGEOM_LIBRARIES ${VECGEOM_LIBRARIES} ${USOLIDS_LIBRARIES})
   set(VECGEOM_INCLUDE_DIR ${VECGEOMROOT}/include)
   set(VECGEOM_LIBRARY_DIR ${VECGEOMROOT}/lib)
   message(STATUS "Found VecGeom in ${VECGEOM_LIBRARIES}")		
   message(STATUS "VECGEOM_INCLUDE_DIR = ${VECGEOM_INCLUDE_DIR}")
   message(STATUS "VECGEOM_LIBRARY_DIR = ${VECGEOM_LIBRARY_DIR}")
# As vecgeom can be compiled with the coverage option, to be on the safe side we add -lgcov to the vecgeom libraries
   set(VECGEOM_LIBRARIES ${VECGEOM_LIBRARIES} -lgcov)
else()
   message(STATUS "VecGeom library not found; try to set a VECGEOMROOT environment variable to the base   installation path or add -DVECGEOMROOT = to the cmake command")	
endif()



