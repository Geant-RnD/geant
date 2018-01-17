/**********************************************************
 * @file TARC.h
 * @author Abhijit Bhattacharyya
 * @brief this is header for the main TARC
 * @date Oct 2017
 *********************************************************/

#ifndef TARC_H
#define TARC_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <mutex>
#include <cassert>

#ifndef GEANT_VAPPLICATION
#include "GeantVApplication.h"
#endif

#include "base/TypeMap.h" // VECGEOM_IMPL_NAMESPACE
#include "Geant/Config.h" // GEANT_IMPL_NAMESPACE

#include "management/GeoManager.h"  // vecgeom::GeoManager
#include "volumes/LogicalVolume.h"
#include "volumes/PlacedVolume.h"

#include "Region.h"

#include "GeantEvent.h"
#include "GeantTrackVec.h"
#include "GeantRunManager.h"
#include "GeantTaskData.h"
#include "globals.h"

#include "Geant/Error.h"

#include "SystemOfUnits.h"
#include "PhysicsData.h"
#include "MaterialCuts.h"
#include "Material.h"
#include "MaterialCuts.h"
#include "LightTrack.h"
#include "PhysicsProcess.h"
#include "EMPhysicsProcess.h"
#include "PhysicsManagerPerParticle.h"

#include "Geant/Typedefs.h"
#include "GeantFwd.h"
#include "GeantTaskData.h"

namespace vecgeom {
  inline namespace VECGEOM_IMPL_NAMESPACE {
    class Region;
  }
}

namespace geantphysics {
  inline namespace GEANT_IMPL_NAMESPACE {
    class Material;
  }
}

namespace GEANT_IMPL_NAMESPACE {
  namespace Geant {
    class GeantRunManager;
    class GeantTaskDataHandle;
    class GeantEvent;
    class GeantTrack;
  }
}

namespace userapplication {
  class TARCGeometryConstruction;
  class TARCPrimaryGenerator;

  class TARC : public Geant::GeantVApplication {
  public:
    TARC(Geant::GeantRunManager *runmgr, TARCGeometryConstruction *geom, TARCPrimaryGenerator *gun);
    virtual ~TARC();

  public:
    void RetrieveLogicalVolumesFromGDML(){
      vecgeom::GeoManager::Instance().GetAllLogicalVolumes(fLVolumeList);
    }

    void RetrievePlacedVolumesFromGDML() {
      vecgeom::GeoManager::Instance().getAllPlacedVolumes(fPVolumeList);
    }

    virtual bool Initialize(); // @brief Interface to Initialize application

    virtual vecgeom::Vector3D<double> GetGun() { return fGunPos; }
    virtual void SetGun(double x, double y, double z) { fGunPos[0] = x; fGunPos[1] = y; fGunPos[2] = z; }
    virtual void fixGun();

  private:
    TARC(const TARC &) = delete;
    TARC &operator=(const TARC &) = delete;

  private:
    bool fInitialized;
    std::vector<vecgeom::LogicalVolume*> fLVolumeList;
    std::vector<vecgeom::VPlacedVolume*> fPVolumeList;
    int fNumPrimaryPerEvent;
    int fNumBufferedEvents;
    TARCGeometryConstruction *fGeomSetup;
    TARCPrimaryGenerator *fPrimaryGun;
    std::mutex fMutex;
    vecgeom::Vector3D<double> fGunPos;

  };  // class ends

} // namespace ends

#endif
