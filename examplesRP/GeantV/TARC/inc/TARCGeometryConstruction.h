/*********************************************************
 * @file TARCGeometryConstruction.h
 * @author Abhijit Bhattacharyya
 * @brief This file is to construct detector geometry loading from ROOT file
 *        supplied by Alexander Howard
 * This trial version is written following CMSFull simulation example.
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

namespace tarcapp {
  class TARCGeometryConstruction : public Geant::GeantVDetectorConstruction {
  public:
    // Constructor
    TARCGeometryConstruction(Geant::GeantRunManager *runmgr);
    // Destructor
    virtual ~TARCGeometryConstruction();

    // create materials for the setup
    //virtual void CreateMaterials();   // Material is already defined in the GDML file

    // interface method to define the geometry for the application
    virtual void CreateGeometry();
    void SetGDMLFile(const std::string& gdmlFile) { fGDMLFileName = gdmlFile;}

    void SetLVList(vecgeom::LogicalVolume *inpLV){ fLVolumeList.push_back(inpLV); }
    void SetPVList(vecgeom::VPlacedVolume *inpPV){ fPVolumeList.push_back(inpPV); }

  private:
    //const char *
    std::string fGeomFileName = "/home/vega/decrypt/GV_APPLI_TARC/examplesRP/GeantV/TARC/data/tarc_geometry.root";
    std::string fGDMLFileName;
    std::string             fTargetMatName;
    //int                     fTargetLogicalVolumeID;
    //int                     fTargetRegionIndx;
    double                  fTargetXY;
    double                  fTargetZ;
    double                  fWorldXY;
    double                  fWorldZ;

    double                  fGammaCut;
    double                  fElectronCut;
    double                  fPositronCut;

    std::vector<vecgeom::LogicalVolume*> fLVolumeList;
    std::vector<vecgeom::VPlacedVolume*> fPVolumeList;

    geantphysics::Material *fTargetMaterial;
    geantphysics::Material *fWorldMaterial;
  };  // CLASS closes here

} // namespace eends here

#endif   // DETECTORCONSTRUCTION_H
