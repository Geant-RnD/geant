/***********************************************************
 * @file Troitsk.cxx
 * @author Abhijit Bhattacharyya
 * @brief troitsk main code.
 * @date Nov 2017
 ***********************************************************/

#include "Troitsk.h"

namespace userapplication{
  Troitsk::Troitsk(
    Geant::GeantRunManager *runmgr, TroitskGeometryConstruction *geom, TroitskPrimaryGenerator *gun
  ) : Geant::GeantVApplication(runmgr), fGeomSetup(geom), fPrimaryGun(gun){
    fInitialized = false;
    fTargetLogicalVolumeID = -1;
    fNumPrimaryPerEvent    = -1;
    fNumBufferedEvents     = -1;
  }

  Troitsk::~Troitsk(){}

  bool Troitsk::Initialize() {
    if (fInitialized) return true;
    std::cout << " Initializing........" << std::endl;
    if (!fGeomSetup) {
      Geant::Error("Troitsk::Initialize", "Geometry is not available!");
      return false;
    }
    //fTargetLogicalVolumeID = geom->GetTargetLogicalVolumeID();
    fInitialized = true;
    return true;
  }
} // end of namespace
