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

#include "management/GeoManager.h"  // vecgeom::GeoManager
#include "Geant/Error.h"

#include "GeantEvent.h"
#include "GeantTrackVec.h"
#include "GeantRunManager.h"
#include "GeantTaskData.h"
#include "globals.h"

#include "SystemOfUnits.h"
#include "PhysicsData.h"
#include "MaterialCuts.h"
#include "Material.h"
#include "MaterialCuts.h"
#include "LightTrack.h"
#include "PhysicsProcess.h"
#include "EMPhysicsProcess.h"
#include "PhysicsManagerPerParticle.h"

#include "TARCGeometryConstruction.h"
#include "TARCPrimaryGenerator.h"
#include "TARCData.h"

#include "Geant/Typedefs.h"
#include "GeantFwd.h"
#include "GeantTaskData.h"

#define SQR(x) x * x

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

namespace tarcapp {
  class TARCGeometryConstruction;
  class TARCPrimaryGenerator;

  class TARCapp : public Geant::GeantVApplication {
  public:
    //TARC(Geant::GeantRunManager *runmgr, TARCGeometryConstruction *geom, TARCPrimaryGenerator *gun);
    TARCapp(Geant::GeantRunManager *, TARCGeometryConstruction *, TARCPrimaryGenerator *);
    virtual ~TARCapp();

    void RetrieveLogicalVolumesFromGDML(){
      vecgeom::GeoManager::Instance().GetAllLogicalVolumes(fLVolumeList);
    }

    void RetrievePlacedVolumesFromGDML() {
      vecgeom::GeoManager::Instance().getAllPlacedVolumes(fPVolumeList);
    }

    virtual bool Initialize(); // @brief Interface to Initialize application

    virtual void AttachUserData(Geant::GeantTaskData *td);

    virtual void SteppingActions(Geant::GeantTrack &, Geant::GeantTaskData *);

    virtual void FinishEvent(Geant::GeantEvent * );

    void SetPerformanceMode(bool val) { fIsPerformance = val;}

    virtual void FinishRun();
    virtual vecgeom::Vector3D<double> GetGun() { return fGunPos; }
    virtual void SetGun(double x, double y, double z) { fGunPos[0] = x; fGunPos[1] = y; fGunPos[2] = z; }
    virtual void fixGun();private:

  private:
    TARCapp(const TARCapp &) = delete;
    TARCapp &operator=(const TARCapp &) = delete;

  private:
    bool                                               fIsPerformance;
    bool                                               fInitialized;

    std::vector<vecgeom::LogicalVolume*>               fLVolumeList;
    std::vector<vecgeom::VPlacedVolume*>               fPVolumeList;

    int                                                fNumPrimaryPerEvent;
    int                                                fNumBufferedEvents;
    TARCGeometryConstruction                          *fGeomSetup;
    TARCPrimaryGenerator                              *fPrimaryGun;
    std::mutex                                         fMutex;
    vecgeom::Vector3D<double>                          fGunPos;
    double                                             fPrimaryParticleCharge;
    Geant::TaskDataHandle<TARCThreadDataEvents>       *fDataHandlerEvents;
    // Geant::TaskDataHandle<TARCThreadDataRun>       *fDataHandlerRun;
    TARCData                                          *fData;

  };  // class ends

} // namespace ends

#endif
