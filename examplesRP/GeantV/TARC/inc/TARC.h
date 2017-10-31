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
#include <cassert>

#ifndef GEANT_VAPPLICATION
#include "GeantVApplication.h"
#endif

#include "management/GeoManager.h"  // vecgeom::GeoManager
#include "GeantEvent.h"
#include "GeantTrackVec.h"
#include "GeantRunManager.h"
#include "GeantTaskData.h"
#include "Globals.h"

#include "Geant/Error.h"

#include "SystemOfUnits.h"
#include "PhysicsData.h"
#include "MaterialCuts.h"
#include "Material.h"
#include "LighTrack.h"
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

namespace ApplicationTARC {
  class TARCGeometryConstruction;

  class TARC : public geant::GeantVApplication {
  public:
    TARC(Geant::GeantRunManager *runmgr, TARCGeometryConstruction *geom); // add primaryGenerator
    virtual ~TARC();

  public:
    virtual bool Initialize(); // @brief Interface to Initialize application

  private:
    bool fInitialized;
    int fTargetLogicalVolumeID;
    TARCGeometryConstruction *fGeomSetup;

  }

} // namespace ends

#endif
