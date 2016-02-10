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

#ifndef GEANTV_MIC
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#endif

class GeantHitBlock;
#include "GeantFwd.h"

/** @brief GeantVApplication class */
#ifndef GEANTV_MIC
class GeantVApplication : public TObject {
#else
class GeantVApplication {
#endif

public:
  using GeantTrack = Geant::GeantTrack;
  using GeantTrack_v = Geant::GeantTrack_v;
  using GeantTaskData = Geant::GeantTaskData;
  
  /** @brief GeantVApplication constructor */	
  GeantVApplication();

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

  /**
   * @brief Function of digitization
   * 
   * @param event Event for digitization
   */
  virtual void Digitize(int event) = 0;

  /** @brief User FinishRun function */
  virtual void FinishRun() = 0;

#ifndef GEANTV_MIC
  ClassDef(GeantVApplication, 1) // User application
#endif
};
#endif
