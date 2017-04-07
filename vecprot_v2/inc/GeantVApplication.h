//===--- GeantVApplication.h - Geant-V --------------------------*- C++ -*-===//
//
//                     Geant-V Prototype               
//
//===----------------------------------------------------------------------===//
/**
 * @file GeantVApplication.h
 * @brief Implementation of user application in Geant-V prototype 
 * @author Andrei Gheata 
 */
//===----------------------------------------------------------------------===//

#ifndef GEANT_VAPPLICATION
#define GEANT_VAPPLICATION

#include "GeantFwd.h"
#include "GeantTrack.h"

namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {

class GeantRunManager;
class GeantEvent;

/** @brief GeantVApplication class */
class GeantVApplication {
public:
  GeantRunManager *fRunMgr; /*taskData*/
  
  /** @brief GeantVApplication constructor */	
  GeantVApplication(GeantRunManager *runmgr);

  void SetRunManager(GeantRunManager *runmgr);

  /** @brief GeantVApplication destructor */
  virtual ~GeantVApplication() {}

  /** @brief Function of initialization */
  virtual bool Initialize() = 0;

  /**
   * @brief User callback function for scoring
   * 
   * @param tid  Thread ID
   * @param npart Number of tracks
   * @param tracks Set of tracks
   */
  virtual void StepManager(int npart, const GeantTrack_v &tracks, GeantTaskData *td) = 0;

  //=== N E W   I N T E R F A C E S ===//
  virtual void SteppingActions(GeantTrack &/*track*/, GeantTaskData */*td*/) {} // = 0;
  virtual void SteppingActions(TrackVec_t &/*tracks*/, GeantTaskData */*td*/) {} // = 0;

  /**
   * @brief Function of digitization
   * 
   * @param event Event for digitization
   */
  virtual void Digitize(GeantEvent *event) = 0;

  /** @brief User FinishRun function */
  virtual void FinishRun() = 0;

};

} // GEANT_IMPL_NAMESPACE
} // Geant

#endif
