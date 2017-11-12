/***********************************************************
 * @file TARC.cxx
 * @author Abhijit Bhattacharyya
 * @brief tarc main code.
 * @date Oct 2017
 ***********************************************************/

#include "TARC.h"

namespace userapplication {

TARC::TARC(Geant::GeantRunManager *runmgr, TARCGeometryConstruction *geom, TARCPrimaryGenerator *gun)
: Geant::GeantVApplication(runmgr), fGeomSetup(geom), fPrimaryGun(gun){
  fInitialized           = false;
  fTargetLogicalVolumeID = -1;
  fNumPrimaryPerEvent    = -1;
  fNumBufferedEvents     = -1;
}

TARC::~TARC(){}

bool TARC::Initialize() {
  if (fInitialized) return true;
  std::cout << " Initializing......" << std::endl;
  if (!fGeomSetup) {
    Geant::Error("TARC::Initialize", "Geometry is not available!");
    return false;
  }
  //    fTargetLogicalVolumeID = fGeomSetup->GetTargetLogicalVolumeID();
  fInitialized = true;
  return true;
}


} // namespace ends Here
