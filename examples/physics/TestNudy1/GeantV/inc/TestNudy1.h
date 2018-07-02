
#ifndef TESTEM5_H
#define TESTEM5_H

#ifndef GEANT_VAPPLICATION
#include "Geant/UserApplication.h"
#endif

#include "Geant/Typedefs.h"
#include "Geant/Fwd.h"
#include "Geant/TaskData.h"

namespace GEANT_IMPL_NAMESPACE {
namespace geant {
class RunManager;
class TaskDataHandle;
class Event;
class Track;
} // namespace geant
} // namespace GEANT_IMPL_NAMESPACE

#include "Hist1.h"
#include "TestNudy1Data.h"

#include <mutex>
#include <vector>

namespace userapplication {

/**
 * @brief GeantV implementation of the Geant4 TestNudy1 application.
 *
 * The application simulates passage of particles (type and energy configurable) through a simple thin target (material,
 * thickness, secondary production cuts are configurable). The main purpose of the simulation is to generate angular
 * distribution of charged particles transmitted through the target. However, several other quantities (like mean energy
 * deposit in target, energy leakage[both primary and secondary] and energy balance, mean number of charged and neutral
 * steps in the target, mean number of secondary partciles per particle type, transmitting/backscattering coefficients,
 * etc.) will also be collected and reported at the simulation.
 *
 * @class   TestNudy1
 * @author  M Novak
 * @date    July 2017
 */

class TestNudy1DetectorConstruction;
class TestNudy1PrimaryGenerator;

/** @brief TestNudy1 user application */
class TestNudy1 : public geant::UserApplication {
public:
  /** @brief Constructor TestNudy1 */
  TestNudy1(geant::RunManager *runmgr, TestNudy1DetectorConstruction *det, TestNudy1PrimaryGenerator *gun);

  /** @brief Destructor TestNudy1 */
  virtual ~TestNudy1();

  /** @brief Interface method to allow registration of user defined thread local data. */
  virtual void AttachUserData(geant::TaskData *td);

  /** @brief Interface method to initialize the application. */
  virtual bool Initialize();

  /** @brief Interace method that is called at the end of each simulation step. */
  virtual void SteppingActions(geant::Track &track, geant::TaskData *td);

  /** @brief Interace method that is called when the transportation of an event (including all primary and their
   *        secondary particles) is completed .*/
  virtual void FinishEvent(geant::Event *event);

  /** @brief Interface method that is called at the end of the simulation (when the transportation of all events are
   *        are completed). */
  virtual void FinishRun();

  // Some application specific methods to set the angular distribution histogram parameters.
  void SetHist1FileName(const std::string &name) { fHist1FileName = name; }
  void SetHist1NumBins(int val) { fHist1NumBins = val; }
  void SetHist1Min(double val) { fHist1Min = val; }
  void SetHist1Max(double val) { fHist1Max = val; }

private:
  /** @brief Copy constructor TestNudy1 (deleted) */
  TestNudy1(const TestNudy1 &) = delete;
  /** @brief Operator= for TestNudy1 (deleted) */
  TestNudy1 &operator=(const TestNudy1 &) = delete;

private:
  std::string fHist1FileName;
  bool fInitialized;
  // ID of the target logical volume (used to check if the current step was done in the target)
  // this data will be obtained from the TestNudy1DetectorConstruction at initialization
  int fTargetLogicalVolumeID;
  // some data regarding the number of primaries per event and number of buffered events (i.e. number of event-slots)
  // these data will be obtained from the RunManager::GeantConfig object at initialization
  int fNumPrimaryPerEvent;
  int fNumBufferedEvents;
  // histogram configuration data (can be changed from input arguments)
  int fHist1NumBins;
  double fHist1Min;
  double fHist1Max;
  //
  double fPrimaryParticleCharge;
  // user defined thread local data structure handlers to obtain the thread local data structures (defined and
  // registered by the user) during the simulation (in the SteppingActions(i.e. at the end of each simulation step),
  // Digitization(i.e. at the end of an event) and FinishRun (i.e. at the end of the simulation):
  //
  // 1. merged from all working threads when transportation of an event (with all corresponding primary and secondary
  //    particles) are completed
  geant::TaskDataHandle<TestNudy1ThreadDataEvents> *fDataHandlerEvents;
  // 2. merged from all working threads when transportation of all events (i.e. end of the simulation) are completed
  geant::TaskDataHandle<TestNudy1ThreadDataRun> *fDataHandlerRun;
  // a unique, run-global user defined data structure to store cumulated quantities per primary particle during the
  // simulation
  TestNudy1Data *fData;
  //
  TestNudy1DetectorConstruction *fDetector;
  TestNudy1PrimaryGenerator *fPrimaryGun;
  // mutex to prevent multiple threads writing into the unique, run-global TestNudy1Data object (in the Digitization
  // after the merge of the user defined per-event data distributed among the threads)
  std::mutex fMutex;
};

} // namespace userapplication

#endif // TESTEM5_H
