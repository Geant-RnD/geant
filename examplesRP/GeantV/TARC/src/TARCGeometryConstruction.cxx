#include "TARCGeometryConstruction.h"

using namespace vecgeom;
using namespace geantphysics;
using namespace geant;


namespace userapplication{
TARCGeometryConstruction::TARCGeometryConstruction(Geant::GeantRunManager *runmgr)
 : Geant::GeantVDetectorConstruction(runmgr) {
  fTargetLogicalVolumeID = -1;
}


TARCGeometryConstruction::~TARCGeometryConstruction() {/* nothing to do */}

/*
void TARCGeometryConstruction::CreateMaterials() {
  int z, ncomponents, natoms;
  double a, density, massfraction;

  // AIR
  geantphysics::Material* matAir = new geantphysics::Material(
    "Air", density = 1.290*geant::mg/geant::cm3, ncomponents = 2
  );
  matAir->AddElement(geantphysics::Element::NISTElement(z = 7), massfraction = 0.7);
  matAir->AddElement(geantphysics::Element::NISTElement(z = 8), massfraction = 0.3);

  // Pure Lead
  geantphysics::Material* matLeadHP = new geantphysics::Material(
    "LeadHP", z = 82, a = 207.2, density = 11.3*geant::g/geant::cm3
  );
}
*/

void TARCGeometryConstruction::CreateGeometry() {
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

}

} //namespace ends Here
