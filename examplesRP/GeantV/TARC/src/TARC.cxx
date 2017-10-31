/***********************************************************
 * @file TARC.cxx
 * @author Abhijit Bhattacharyya
 * @brief tarc main code.
 * @date Oct 2017
 ***********************************************************/

#include "TARC.h"

namespace ApplicationTARC{
  TARC::TARC(Geant::GeantRunManager *runmgr, TARCGeometryConstruction *geom){ // add primaryGenerator
    fInitialized = false;
    fTargetLogicalVolumeID = -1;
  }

  TARC::~TARC(){}

  bool TARC::Initialize() {
    if (fInitialized) return true;
    if (!fGeomSetup) {
      Geant::Error("TARC::Initialize", "Geometry is not available!");
      return false;
    }
    fTargetLogicalVolumeID = geom->GetTargetLogicalVolumeID();
    fInitialized = true;
    return true;
  }
} // end of namespace
