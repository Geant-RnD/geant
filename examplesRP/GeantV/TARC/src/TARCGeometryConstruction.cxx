#include "TARCGeometryConstruction.h"
#include "SystemOfUnits.h"
#include "Material.h"
#include "Region.h"


namespace userapplication {
TARCGeometryConstruction::TARCGeometryConstruction(Geant::GeantRunManager *runmgr) : Geant::GeantVDetectorConstruction(runmgr) {
  fTargetLogicalVolumeID = -1;
}


TARCGeometryConstruction::~TARCGeometryConstruction() {/* nothing to do */}

void TARCGeometryConstruction::CreateMaterials() {

}


void TARCGeometryConstruction::CreateGeometry() {
  if (!GeantVDetectorConstruction::LoadGeometry(GeomFileName)){
    std::cout << "Error in loading the GDML file." << std::endl;
  }
}

}  // end of namespace
