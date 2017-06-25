#ifndef GEANT_RUN_MANAGER_H
#define GEANT_RUN_MANAGER_H

#include <iostream>
#include <thread>
#include <atomic>
#include "base/Vector.h"
#include "base/BitSet.h"
#include "Geant/Config.h"
#include "Geant/Typedefs.h"
#include "GeantTaskData.h"
#include "GeantEventServer.h"
#include "GeantEventDispatcher.h"
#include "GeantDistributeManager.h"
#include "GeantConfig.h"
#ifdef USE_ROOT
#include "TGeoExtension.h"
#endif

using namespace veccore;
class PhysicsInterface;

GEANT_DEVICE_DECLARE_CONV(class,GeantPropagator);

namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {

class GeantPropagator;
class TaskBroker;
class PhysicsProcessOld;
class GeantVApplication;
class GeantVTaskMgr;
class GeantEventServer;
class GeantEvent;
class PrimaryGenerator;
class MCTruthMgr;

#ifdef USE_HPC
class GeantEventReceiver;
#endif

// Volume-basket manager connector structure attached to volumes as extension
#if defined(USE_ROOT) && !defined(VECCORE_CUDA)
class VBconnector : public TGeoExtension {
#else
class VBconnector {
#endif
public:
  int index;                      /** Index of basket manager */
  VECCORE_ATT_HOST_DEVICE
  VBconnector(int i) : index(i) {}
#if defined(USE_ROOT) && !defined(VECCORE_CUDA)
  virtual TGeoExtension *Grab() { return this; }
  virtual void Release() const {}
#endif
};

class GeantRunManager
{
private:
  bool fInitialized = false;
  int fNpropagators = 0;          /** Number of propagators */
  int fNthreads     = 0;          /** Number of threads per propagator */
  int fNvolumes     = 0;          /** Number of active volumes in the geometry */
  int fNprimaries   = 0;          /** Total number of primaries in the run */
  int fNbuff        = 0;          /** Number of event slots per propagator */
  int fNfeedProp    = 0;          /** Number of propagators with initial feed */
  int fInitialShare = 0;          /** Initial basket share for each propagator */
  GeantConfig *fConfig = nullptr; /** Run configuration */
  TaskBroker *fBroker = nullptr;  /** Task broker */

  GeantVApplication *fApplication = nullptr;    /** User application */
  GeantVApplication *fStdApplication = nullptr; /** Standard application */
  GeantVTaskMgr     *fTaskMgr = nullptr;        /** GeantV task manager */
  PhysicsProcessOld *fProcess = nullptr;           /** For now the only generic process pointing to the tabulated physics */
  PhysicsProcessOld *fVectorPhysicsProcess = nullptr; /** Interface to vector physics final state sampling */
  PhysicsInterface *fPhysicsInterface; /** The new, real physics interface */
  PrimaryGenerator *fPrimaryGenerator = nullptr;   /** Primary generator */
  MCTruthMgr *fTruthMgr = nullptr;              /** MCTruth manager */
  GeantEventServer *fEventServer = nullptr;     /** The event server */

#ifdef USE_HPC
  GeantEventReceiver *fEventReceiver = nullptr;
#endif
   
  vector_t<GeantPropagator *> fPropagators;
  vector_t<Volume_t const *> fVolumes;

  // State data
  std::atomic_int fPriorityEvents; /** Number of prioritized events */
  std::atomic_int fTaskId; /** Counter providing unique task id's */
  BitSet *fDoneEvents = nullptr;   /** Array of bits marking done events */
//  int *fNtracks = nullptr;         /** ![fNbuff] Number of tracks per slot */
//  GeantEvent **fEvents = nullptr;  /** ![fNbuff]    Array of events */
  GeantTaskData **fTaskData = nullptr; /** ![fNthreads] Data private to threads */
  GeantPropagator *fFedPropagator = nullptr; /** Propagator currently being fed */
  std::vector<std::thread> fListThreads; /** Vector of threads */

private:
  bool LoadVecGeomGeometry();
  void InitNavigators();
// It will go to the DetectorConstruction base class
#ifdef USE_VECGEOM_NAVIGATOR
  std::function<void*(TGeoMaterial const *)> CreateMaterialConversion();
#endif

public:
  GeantRunManager() {}
  GeantRunManager(unsigned int npropagators, unsigned int nthreads, GeantConfig *config);
  ~GeantRunManager();

// Accessors
  GEANT_FORCE_INLINE
  BitSet* GetDoneEvents() { return fDoneEvents; }

  GEANT_FORCE_INLINE
  int  GetNthreads() { return fNthreads; }

  GEANT_FORCE_INLINE
  int  GetNthreadsTotal() { return (fNthreads*fNpropagators); }

  GEANT_FORCE_INLINE
  int  GetNpropagators() { return fNpropagators; }

  GEANT_FORCE_INLINE
  GeantConfig *GetConfig() { return fConfig; }

  VECCORE_ATT_HOST_DEVICE
  GEANT_FORCE_INLINE
  int  GetNvolumes() { return fNvolumes; }

  VECCORE_ATT_HOST_DEVICE
  GEANT_FORCE_INLINE
  vector_t<Volume_t const *> &GetVolumes() { return fVolumes; }

  GEANT_FORCE_INLINE
  int  GetNprimaries() const { return fNprimaries; }

  GEANT_FORCE_INLINE
  void  SetNprimaries(int nprim) { fNprimaries = nprim; }

  GEANT_FORCE_INLINE
  int  GetInitialShare() const { return fInitialShare; }

  GEANT_FORCE_INLINE
  void  SetInitialShare(int nbaskets) { fInitialShare = nbaskets; }

  GEANT_FORCE_INLINE
  std::atomic_int &GetPriorityEvents() { return fPriorityEvents; }

  GEANT_FORCE_INLINE
  int DecreasePriorityEvents() { return fPriorityEvents.fetch_sub(1); }

  GEANT_FORCE_INLINE
  int GetNpriority() const { return fPriorityEvents.load(); }

  GEANT_FORCE_INLINE
  Volume_t const *GetVolume(int ivol) { return fVolumes[ivol]; }

  GEANT_FORCE_INLINE
  GeantEvent *GetEvent(int i) { return fEventServer->GetEvent(i); }

  GEANT_FORCE_INLINE
  GeantEventServer *GetEventServer() const { return fEventServer; }
#ifdef USE_HPC
  GEANT_FORCE_INLINE
  GeantEventReceiver *GetEventReceiver() const { return fEventReceiver; }
#endif

  GEANT_FORCE_INLINE
  GeantTaskData *GetTaskData(int tid) { return fTaskData[tid]; }

  GEANT_FORCE_INLINE
  int  GetTaskId() { return (fTaskId.fetch_add(1)); }

//  GEANT_FORCE_INLINE
//  int GetNtracks(int islot) const { return fNtracks[islot]; }

  GeantPropagator *GetIdlePropagator() const;

  GEANT_FORCE_INLINE
  GeantPropagator *GetFedPropagator() const { return fFedPropagator; }

  GEANT_FORCE_INLINE
  void SetCoprocessorBroker(TaskBroker *broker) { fBroker = broker; }

  GEANT_FORCE_INLINE
  void SetUserApplication(GeantVApplication *app) { fApplication = app; }

  GEANT_FORCE_INLINE
  void SetTaskMgr(GeantVTaskMgr *taskmgr) { fTaskMgr = taskmgr; }

  GEANT_FORCE_INLINE
  void SetPhysicsInterface(PhysicsInterface *interface) { fPhysicsInterface = interface; }

  GEANT_FORCE_INLINE
  PhysicsInterface *GetPhysicsInterface() const { return fPhysicsInterface; }

  GEANT_FORCE_INLINE
  void SetPhysicsProcess(PhysicsProcessOld *proc) { fProcess = proc; }

  GEANT_FORCE_INLINE
  void SetVectorPhysicsProcess(PhysicsProcessOld *proc) { fVectorPhysicsProcess = proc; }

  GEANT_FORCE_INLINE
  PrimaryGenerator *GetPrimaryGenerator() const { return fPrimaryGenerator; }

  GEANT_FORCE_INLINE
  void SetPrimaryGenerator(PrimaryGenerator *gen) { fPrimaryGenerator = gen; }

  GEANT_FORCE_INLINE
  void SetMCTruthMgr(MCTruthMgr *mcmgr) { fTruthMgr = mcmgr; }

#ifdef USE_HPC
  GEANT_FORCE_INLINE
  void SetEventReceiver(GeantEventReceiver *receiver) { fEventReceiver = receiver; }
#endif

  GEANT_FORCE_INLINE
  MCTruthMgr *GetMCTruthMgr() const { return fTruthMgr; }

  /** @brief Function checking if transport is completed */
  bool TransportCompleted() const;

  /** @brief Check if event is finished */
  bool IsDoneEvent(int ievt) { return fDoneEvents->TestBitNumber(ievt); }

  /** @brief Mark an event as finished */
  void SetDoneEvent(int ievt) { fDoneEvents->SetBitNumber(ievt); }

  /** @brief Initialize classes for RK Integration */
  void PrepareRkIntegration();

  /** @brief Implementation of work stealing */
  int ProvideWorkTo(GeantPropagator *prop);

  void EventTransported(int evt);
  bool Initialize();
  bool FinishRun();
  bool LoadGeometry(const char *filename);
  void RunSimulation();
  void StopTransport();

};

} // GEANT_IMPL_NAMESPACE
} // Geant

#endif // GEANT_RUN_MANAGER_H
