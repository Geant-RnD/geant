#include "TARCGeometryConstruction.h"

using namespace vecgeom;
using namespace geantphysics;
using namespace geant;
using namespace tarcapp;


TARCGeometryConstruction::TARCGeometryConstruction(Geant::GeantRunManager *runmgr)
 : Geant::GeantVDetectorConstruction(runmgr), fGDMLFileName(fGeomFileName) {
  // fTargetLogicalVolumeID = -1;
}


TARCGeometryConstruction::~TARCGeometryConstruction() {

}

void TARCGeometryConstruction::CreateGeometry() {
  LoadGeometry(fGDMLFileName.c_str());
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
