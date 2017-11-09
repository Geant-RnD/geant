/*********************************************************
 * @file TARCGeometryConstruction.h
 * @author Abhijit Bhattacharyya
 * @brief This file is to construct detector geometry loading from ROOT file
 *        supplied by Alexander Howard
 * @date Oct 2017
 *********************************************************/

#ifndef TARC_GEOMETRY_CONSTRUCTION_H
#define TARC_GEOMETRY_CONSTRUCTION_H

#include "GeantRunManager.h"
#include "GeantVDetectorConstruction.h"

#ifdef USE_ROOT
#include "Rtypes.h"
#include "TGeoManager.h"
#endif

#include "Isotope.h"
#include "Material.h"
#include "MaterialProperties.h"
#include "Region.h"
#include "SystemOfUnits.h"
#include "PhysicalConstants.h"

#include "Geant/Typedefs.h"
#include "Geant/Config.h"
#include "GeantFwd.h"

#include <iostream>
#include <vector>
#include <string>

#ifdef USE_ROOT
class TGeoManager;
class TGeoMaterial;
#endif

namespace Geant {
  inline namespace GEANT_IMPL_NAMESPACE {
    class GeantVDetectorConstruction;
    class GeantRunManager;
  }
}

namespace geantphysics {
  inline namespace GEANT_IMPL_NAMESPACE {
    class Material;
  }
}

namespace userapplication {
class TARCGeometryConstruction : public Geant::GeantVDetectorConstruction {
public:
  // Constructor
  TARCGeometryConstruction(Geant::GeantRunManager *runmgr);
  // Destructor
  ~TARCGeometryConstruction();

  // interface method to define the geometry for the application
  virtual void CreateGeometry();

private:
  //const char *
  std::string GeomFileName = "tarc_geom.root";
  std::string             fTargetMatName;
  int                     fTargetLogicalVolumeID;
  int                     fTargetRegionIndx;
  double                  fTargetYZ;
  double                  fTargetX;
  double                  fWorldYZ;
  double                  fWorldX;

  geantphysics::Material *fTargetMaterial;
  geantphysics::Material *fWorldMaterial;

  geantphysics::Material *fPb;
  geantphysics::Material *fSteel;
  geantphysics::Material *fAir;
  geantphysics::Material *fGapMaterial;
  geantphysics::Material *fAbsorberMaterial;
};  // CLASS closes here

} // namepsac eends here

#endif   // DETECTORCONSTRUCTION_
