#include "TroitskGeometryConstruction.h"


namespace userapplication {
TroitskGeometryConstruction::TroitskGeometryConstruction(Geant::GeantRunManager *runmgr) :
Geant::GeantVDetectorConstruction(runmgr) {
  fTargetLogicalVolumeID = -1;
}


TroitskGeometryConstruction::~TroitskGeometryConstruction() {/* nothing to do */}


void TroitskGeometryConstruction::CreateGeometry() {
  std::string rootGeomFilePath = std::getenv("TROITSKGDMLPATH");
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
}

}  // end of namespace
