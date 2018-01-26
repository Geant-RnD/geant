//
//  GeantVProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 2016/04/25.
//

#include <vector>
#include <memory>
#include <atomic>
#include "ConfiguredProducer.h"
#include "Event.h"
#include "Waiter.h"

#include "tbb/task.h"

#include "Geant/Config.h"
#include "GeantRunManager.h"
#include "EventSet.h"

//#include "HepMCGenerator.h"
#include "PhysicsProcessHandler.h"
#include "PhysicsListManager.h"

#include "CMSApplicationTBB.h"
#include "CMSPhysicsList.h"
#include "CMSDetectorConstruction.h"
#include "CMSParticleGun.h"

using namespace Geant;

namespace {

  /* Stand in for work done by GeantV via TBB based tasks*/

  class SimulateTask : public tbb::task {
    tbb::task* m_waitingTask;

  public:
    SimulateTask(tbb::task* iWaiting) :
      m_waitingTask(iWaiting) {}

    tbb::task* execute() override {
      printf("SimTask: iWaiting=%p\n", m_waitingTask);

      //IMPORTANT: decrementing the reference count is what
      // tells the framework the simulation is done with
      // this event.
      m_waitingTask->decrement_ref_count();
      return nullptr;
    }

  };
}

namespace demo {

  class GeantVProducer : public ConfiguredProducer {
  public:
    GeantVProducer(const boost::property_tree::ptree& iConfig);
  private:
    virtual void produce(edm::Event&) override;
    //void runSimulation(tbb::task*);

    /** Functions using new GeantV interface */
    bool RunTransportTask(size_t nevents, unsigned int eventID, unsigned int transitionID);
    int BookEvents(int nrequested);

    /** @brief Generate an event set to be processed by a single task.
	Not required as application functionality, the event reading or generation
	can in the external event loop.
    */
    Geant::EventSet* GenerateEventSet(size_t nevents, GeantTaskData *td);

    inline void LockEventGenerator() { while (fEventGeneratorLock.test_and_set(std::memory_order_acquire)) {}; }
    inline void UnlockEventGenerator() { fEventGeneratorLock.clear(std::memory_order_release); }

    int fNevents;
    std::vector<const Getter*> m_getters;
    GeantConfig* fConfig;
    GeantRunManager* fRunMgr;
    PrimaryGenerator* fPrimaryGenerator;
    std::atomic_flag fEventGeneratorLock;              /** Spinlock for event set access locking */
  };

  GeantVProducer::GeantVProducer(const boost::property_tree::ptree& iConfig)
    : ConfiguredProducer(iConfig,kThreadSafeBetweenInstances)
    , fConfig(0)
    , fRunMgr(0)
    , fPrimaryGenerator(0)
    , fNevents(iConfig.get<int>("Nevents"))
  {
    fEventGeneratorLock.clear();
    registerProduct(demo::DataKey());
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }

    int n_threads = iConfig.get<int>("Nthreads");
    int n_propagators = 1;
    int n_track_max = 500;
    int n_learn_steps = 0;
    int n_reuse = 100000;
    bool monitor = false, score = false, debug = false, coprocessor = false, usev3 = true, usenuma = false;
    bool performance = true;

    //e.g. cms2015.root, cms2018.gdml, ExN03.root
    std::string cms_geometry_filename = iConfig.get<std::string>("geometry");

    std::string xsec_filename = iConfig.get<std::string>("xsec");
    std::string fstate_filename = iConfig.get<std::string>("fstate");

    std::string hepmc_event_filename = iConfig.get<std::string>("hepmc");
    // pp14TeVminbias.root sequence #stable: 608 962 569 499 476 497 429 486 465 619
    // minbias_14TeV.root sequence #stable: 81 84 93 97 87 60 106 91 92 60
    // if empty, use gun generator!

    // instantiate configuration helper
    fConfig = new GeantConfig();

    fConfig->fRunMode = GeantConfig::kExternalLoop;

    fConfig->fGeomFileName = cms_geometry_filename;
    fConfig->fNtotal = fNevents;
    fConfig->fNbuff = n_threads; // iConfig.get<int>("Nbuffered");
    // Default value is 1. (0.1 Tesla)
    fConfig->fBmag = 40.; // 4 Tesla

    // V3 options
    fConfig->fNstackLanes = 10;
    fConfig->fNmaxBuffSpill = 128;  // New configuration parameter!!!
    fConfig->fUseV3 = usev3;

    fConfig->fUseRungeKutta = false;  // Enable use of RK integration in field for charged particles
    // prop->fEpsilonRK = 0.001;      // Revised / reduced accuracy - vs. 0.0003 default

    fConfig->fUseNuma = usenuma;
    fConfig->fNminThreshold = 5 * n_threads;
    fConfig->fNaverage = 50;

    fConfig->fUseMonitoring = monitor;
    fConfig->SetMonitored(GeantConfig::kMonQueue, false);
    fConfig->SetMonitored(GeantConfig::kMonMemory, monitor);
    fConfig->SetMonitored(GeantConfig::kMonBasketsPerVol, false);
    fConfig->SetMonitored(GeantConfig::kMonVectors, false);
    fConfig->SetMonitored(GeantConfig::kMonConcurrency, false);
    fConfig->SetMonitored(GeantConfig::kMonTracksPerEvent, false);

    // Threshold for prioritizing events (tunable [0, 1], normally <0.1)
    // If set to 0 takes the default value of 0.01
    fConfig->fPriorityThr = 0.05;

    // Initial vector size, this is no longer an important model parameter,
    // because is gets dynamically modified to accomodate the track flow
    fConfig->fNperBasket = 16; // Initial vector size (tunable)

    // This is now the most important parameter for memory considerations
    fConfig->fMaxPerBasket = n_track_max;  // Maximum vector size (tunable)

    fConfig->fEmin = 3.e-6; //  [3 KeV] energy cut
    fConfig->fEmax = 0.3;   // [300MeV] used for now to select particle gun energy

    // Activate debugging using -DBUG_HUNT=ON in your cmake build
    if (debug) {
      fConfig->fUseDebug = true;
      fConfig->fDebugTrk = 1;
      //propagator->fDebugEvt = 0;
      //propagator->fDebugStp = 0;
      //propagator->fDebugRep = 10;
    }

    // Number of steps for learning phase (tunable [0, 1e6])
    // if set to 0 disable learning phase
    fConfig->fLearnSteps = n_learn_steps;
    if (performance) fConfig->fLearnSteps = 0;

    // Activate I/O
    fConfig->fFillTree = false;

    // Set threshold for tracks to be reused in the same volume
    fConfig->fNminReuse = n_reuse;

    // Activate standard scoring   
    fConfig->fUseStdScoring = true;
    if (performance) fConfig->fUseStdScoring = false;

    // Activate vectorized geometry
    fConfig->fUseVectorizedGeom = 10;

     // Create run manager
    std::cerr<<"*** GeantRunManager: instantiating with "<< n_propagators <<" propagators and "<< n_threads <<" threads.\n";
    fRunMgr = new GeantRunManager(n_propagators, n_threads, fConfig);

    // Detector construction
    fRunMgr->SetDetectorConstruction( new CMSDetectorConstruction(cms_geometry_filename.c_str(), fRunMgr) );

    // Create the tabulated physics process
    std::cerr<<"*** GeantRunManager: setting physics process...\n";
    //fRunMgr->SetPhysicsProcess( new TTabPhysProcess("tab_phys", xsec_filename.c_str(), fstate_filename.c_str()));

    // create the real physics main manager/interface object and set it in the GeantRunManager
    fRunMgr->SetPhysicsInterface(new geantphysics::PhysicsProcessHandler());

    // Create user defined physics list for the full CMS application
    geantphysics::PhysicsListManager::Instance().RegisterPhysicsList(new cmsapp::CMSPhysicsList());

#ifdef USE_VECGEOM_NAVIGATOR
#ifdef USE_ROOT
    fRunMgr->LoadVecGeomGeometry();
#else
    fRunMgr->LoadGeometry(cms_geometry_filename.c_str());
#endif
#endif

    // Setup a primary generator
    printf("hepmc_event_filename=%s\n", hepmc_event_filename.c_str());
    if (hepmc_event_filename.empty()) {
      std::cerr<<"*** GeantRunManager: setting up a GunGenerator...\n";
      double x = rand();
      double y = rand();
      double z = rand();
      double r = sqrt(x*x+y*y+z*z);
      //fPrimaryGenerator = new GunGenerator(fConfig->fNaverage, 11, fConfig->fEmax, 0, 0, 0, x/r, y/r, z/r);
      //fPrimaryGenerator = new GunGenerator(fConfig->fNaverage, 11, fConfig->fEmax, -8, 0, 0, 1, 0, 0);

      cmsapp::CMSParticleGun *cmsgun = new cmsapp::CMSParticleGun();
      cmsgun->SetNumPrimaryPerEvt(fConfig->fNaverage);
      cmsgun->SetPrimaryName("e-");
      cmsgun->SetPrimaryEnergy(fConfig->fEmax);
      //cmsgun->SetPrimaryDirection(parGunPrimaryDir);
      fRunMgr->SetPrimaryGenerator(fPrimaryGenerator);
      fPrimaryGenerator = cmsgun;
    }
    // else {
    //   //.. here for a HepMCGenerator
    //   std::cerr<<"*** GeantRunManager: setting up a HepMCGenerator...\n";
    //   fPrimaryGenerator = new HepMCGenerator(hepmc_event_filename);
    // }

    CMSApplicationTBB *cmsApp = new CMSApplicationTBB(fRunMgr);
    std::cerr<<"*** GeantRunManager: setting up CMSApplicationTBB...\n";
    fRunMgr->SetUserApplication( cmsApp );
    if (score) {
      cmsApp->SetScoreType(CMSApplicationTBB::kScore);
    } else {
      cmsApp->SetScoreType(CMSApplicationTBB::kNoScore);
    }

    // Start simulation for all propagators
    std::cerr<<"*** GeantRunManager: initializing...\n";
    fRunMgr->Initialize();
    fPrimaryGenerator->InitPrimaryGenerator();

    printf("==========================================================================\n");
    printf("= GeantV run started with %d propagator(s) using %d worker threads each ====\n",
	   fRunMgr->GetNpropagators(), fRunMgr->GetNthreads());

    if (!fConfig->fUsePhysics) printf("  Physics OFF\n");
    else                       printf("  Physics ON\n");

    if (!fConfig->fUseRungeKutta) printf("  Runge-Kutta integration OFF\n");
    else                          printf("  Runge-Kutta integration ON with epsilon= %g\n", fConfig->fEpsilonRK);
    printf("==========================================================================\n");
  }

  void 
  GeantVProducer::produce(edm::Event& iEvent) {

    // not actually needed because GeantVProducer does not depend on any other cms module
    int sum=0;
    for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
        it != itEnd;
        ++it) {
      sum += iEvent.get(*it);
    }
    //std::cerr<<"GeantVProducer::produce(): m_getters.size() = "<< m_getters.size() <<" and sum="<< sum <<"\n";

    std::cerr << "GeantVProducer::produce(): *** Run GeantV simulation task ***\n";
    // if first argument is set to >1, then that number of events will be given to GeantV on each EvenSet
    RunTransportTask(1, iEvent.index(), iEvent.transitionID());

    std::cerr<<"GeantVProducer <"<< label().c_str() <<"> at "<< this <<": adding to event...\n";
    iEvent.put(this,"",static_cast<int>(iEvent.index()));
    std::cerr<<"GeantVProducer <"<< label().c_str() <<"> at "<< this <<": done!\n";
  }

  /// This is the entry point for the user code to transport as a task a set of events
  bool GeantVProducer::RunTransportTask(size_t nevents, unsigned int iEventIndex, unsigned int iTransitionID)
  {
    // First book a transport task from GeantV run manager
    int ntotransport = BookEvents(nevents);

    GeantTaskData *td = fRunMgr->BookTransportTask();
    std::cerr<<" RunTransportTask: td= "<< td <<", nevts="<< nevents <<" and ntotransp="<< ntotransport <<" toy EventID="<<iEventIndex<<" transID="<< iTransitionID <<"\n";
    if (!td) return false;

    // ... then create the event set
    Geant::EventSet *evset = GenerateEventSet(ntotransport, td);

    // ... finally invoke the GeantV transport task
    bool transported = fRunMgr->RunSimulationTask(evset, td);

    // Now we could run some post-transport task
    std::cerr<<" RunTransportTask: task "<< td->fTid <<" nevts="<< nevents
	     <<" and ntotransp="<< ntotransport<<": transported="<< transported <<"\n";

    return transported;
  }

  int GeantVProducer::BookEvents(int nrequested) {
    static atomic_int ntotal(0);
    int nbooked = 0;
    for (int i = 0; i< nrequested; ++i) {
      int ibooked = ntotal.fetch_add(1);
      if (ibooked < fNevents) nbooked++;
    }
    return nbooked;
  }

  Geant::EventSet *GeantVProducer::GenerateEventSet(size_t nevents, Geant::GeantTaskData *td)
  {
    using EventSet = Geant::EventSet;
    using GeantEvent = Geant::GeantEvent;
    using GeantEventInfo = Geant::GeantEventInfo;
    using GeantTrack = Geant::GeantTrack;

    EventSet *evset = new EventSet(nevents);
    LockEventGenerator();
    for (size_t i=0 ; i< nevents; ++i) {
      GeantEvent *event = new GeantEvent();
      GeantEventInfo event_info = fPrimaryGenerator->NextEvent(td);
      while (event_info.ntracks == 0) {
	std::cerr<<"Discarding empty event\n";
	event_info = fPrimaryGenerator->NextEvent(td);
      }
      event->SetNprimaries(event_info.ntracks);
      event->SetVertex(event_info.xvert, event_info.yvert, event_info.zvert);
      for (int itr = 0; itr < event_info.ntracks; ++itr) {
	GeantTrack &track = td->GetNewTrack();
	int trackIndex = event->AddPrimary(&track);
	track.SetParticle(trackIndex);
	track.SetPrimaryParticleIndex(itr);
	fPrimaryGenerator->GetTrack(itr, track, td);
      }
      evset->AddEvent(event);
    }
    UnlockEventGenerator();
    return evset;
  }

}
REGISTER_PRODUCER(demo::GeantVProducer);
