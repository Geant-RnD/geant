//===--- EventSet.h - Geant-V ---------------------------------*- C++ -*-===//
//
//                     Geant-V Prototype
//
//===----------------------------------------------------------------------===//
/**
 * @file EventSet.h
 * @brief An event set workload that holds the processing status
 * @author Andrei Gheata
 */
//===----------------------------------------------------------------------===//

#ifndef GEANT_EVENT_SET
#define GEANT_EVENT_SET

#include <atomic>
#include <vector>

#include "Geant/Config.h"
#include "Geant/condition_locker.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

class Event;
class EventServer;

/** @brief Class describing a set of events as GeantV workload */
class EventSet {

  struct EventMarker {
    Event *fEvent = nullptr;        /** Event */
    int              fEventNumber;       /** Event number */
    std::atomic_flag fDone;              /** Done flag */
  };

private:
  bool               fDone = false;      /** Event set completed (transported) */
  size_t             fNevents = 0;       /** Number of events */
  EventMarker      **fMarkers = nullptr; /** Array of event markers */
  std::atomic_int    fNdone;             /** Number of completed events */
  size_t             fNadded = 0;        /** Number of events already added to the set */
  condition_locker   fLocker;            /** Locker used to put to sleep and wake-up the thread delivering this */

private:
  // forbidden copying
  EventSet(const EventSet &other);
  const EventSet &operator=(const EventSet &other);

  /** @brief Check if an event is contained in the event set. */
  GEANT_FORCE_INLINE
  bool Contains(int event_number, size_t &slot) const {
    for (slot = 0; slot < fNevents; ++slot) 
      if (fMarkers[slot]->fEventNumber == event_number) return true;
    return false;
  }

public:  
  /** @brief Constructor allowing to define the event set content later-on. */
  EventSet(size_t nevents);

  /** @brief Constructor providing the full set of events. */
  EventSet(std::vector<Event*> const &events);

  /** @brief Destructor */
  ~EventSet() { delete [] fMarkers; }

  /** @brief Complete the event set adding events one-by-one. */
  bool AddEvent(Event *event);
  
  void AddSetToServer(EventServer *evserv) const;

  /** @brief Mark one event in the set as done. Return true if the operation completes the set. */
  bool MarkDone(int event_number);
  
  /** @brief Print the data set */
  void Print();
  
  /** @brief Check if the all events in the set are completed. */
  GEANT_FORCE_INLINE
  bool IsDone() const { return fDone; }
  
  /** @brief Sleep until awaken by the completion of the event set */
  void SleepUntilDone() { fLocker.Wait(); }
  
};

} // GEANT_IMPL_NAMESPACE
} // Geant

#endif
