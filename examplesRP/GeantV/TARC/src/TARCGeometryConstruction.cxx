#include "TARCGeometryConstruction.h"

using namespace vecgeom;
using namespace geantphysics;
using namespace geant;


namespace tarc{
TARCGeometryConstruction::TARCGeometryConstruction(Geant::GeantRunManager *runmgr)
 : Geant::GeantVDetectorConstruction(runmgr) {
  // fTargetLogicalVolumeID = -1;
}


TARCGeometryConstruction::~TARCGeometryConstruction() {/* nothing to do */}

void TARCGeometryConstruction::CreateGeometry() {
  /*
  std::string rootGeomFilePath = std::getenv("TARCGDMLPATH");
  rootGeomFilePath += "/" + GeomFileName;
  const char* rootGeomFile = rootGeomFilePath.c_str();
  // bool wasLoaded = GeantVDetectorConstruction::LoadGeometry(GeomFileName);
  bool wasLoaded = GeantVDetectorConstruction::LoadGeometry(rootGeomFile);
  if (!wasLoaded){
    std::cout << "Error in loading the GDML file." << std::endl;
    return;
  } else {
    std::cout << GeomFileName << " loaded." <<std::endl;
  }
  */
}

} //namespace ends Here
