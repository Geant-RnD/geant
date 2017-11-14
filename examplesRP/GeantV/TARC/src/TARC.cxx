/***********************************************************
 * @file TARC.cxx
 * @author Abhijit Bhattacharyya
 * @brief tarc main code.
 * @date Oct 2017
 ***********************************************************/

#include "TARC.h"

using namespace vecgeom;
using namespace geantphysics;
using namespace Geant;

namespace userapplication {

TARC::TARC(Geant::GeantRunManager *runmgr, TARCGeometryConstruction *geom, TARCPrimaryGenerator *gun)
: Geant::GeantVApplication(runmgr), fGeomSetup(geom), fPrimaryGun(gun){
  fInitialized           = false;
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

 RetrieveLogicalVolumesFromGDML(); // Retrieve asscoiated Logical volumes and place in array

std::cout << " There are in total " << fLogiVolumeList.size() << " logical volumes \n";

 for (size_t i=0; i<fLogiVolumeList.size(); ++i) {
    Material *mat = (Material*)fLogiVolumeList[i]->GetMaterialPtr();
    vecgeom::Region *reg = fLogiVolumeList[i]->GetRegion();

    std::cout << " LogicalVolume with name = " << fLogiVolumeList[i]->GetLabel()
              << " id-> " << fLogiVolumeList[i]->id()
              << " has material -> " << mat->GetName()
              << " in Region = " << reg->GetName() << " index " << reg->GetIndex()
              << std::endl;
  }

  fInitialized = true;
  return true;
}


} // namespace ends Here
