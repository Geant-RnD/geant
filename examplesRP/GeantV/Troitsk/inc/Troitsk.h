/**********************************************************
 * @file Troitsk.h
 * @author Abhijit Bhattacharyya
 * @brief this is header for the main Troitsk
 * @date Nov 2017
 *********************************************************/

#ifndef TROITSK_H
#define TROITSK_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include <mutex>

#ifndef GEANT_VAPPLICATION
#include "GeantVApplication.h"
#endif

#include "management/GeoManager.h"  // vecgeom::GeoManager
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
#include "LightTrack.h"
#include "PhysicsProcess.h"
#include "EMPhysicsProcess.h"
#include "PhysicsManagerPerParticle.h"

#include "Geant/Typedefs.h"
#include "GeantFwd.h"
#include "GeantTaskData.h"

namespace GEANT_IMPL_NAMESPACE {
  namespace Geant {
    class GeantRunManager;
    class GeantTaskDataHandle;
    class GeantEvent;
    class GeantTrack;
  }
}

namespace userapplication {
  class TroitskGeometryConstruction;
  class TroitskPrimaryGenerator;

  class Troitsk : public Geant::GeantVApplication {
  public:
    Troitsk(
      Geant::GeantRunManager *runmgr, TroitskGeometryConstruction *geom, TroitskPrimaryGenerator *gun
    );
    virtual ~Troitsk();

  public:
    virtual bool Initialize(); // @brief Interface to Initialize application

  private:
    Troitsk(const Troitsk &) = delete;
    Troitsk &operator=(const Troitsk &) = delete;

  private:
    bool fInitialized;
    int fTargetLogicalVolumeID;
    int fNumPrimaryPerEvent;
    int fNumBufferedEvents;
    TroitskGeometryConstruction *fGeomSetup;
    TroitskPrimaryGenerator *fPrimaryGun;
    std::mutex fMutex;

  }; // class ends
} // namespace ends

#endif
