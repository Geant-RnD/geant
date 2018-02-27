#include "EventServer.h"

#include "Geant/Error.h"

#include "Geant/VBconnector.h"
#include "Geant/Track.h"
#include "Geant/Event.h"
#include "RunManager.h"
#include "LocalityManager.h"
#include "Geant/PrimaryGenerator.h"
#include "TaskData.h"
#include "Basket.h"
#include "StackLikeBuffer.h"
#include "Geant/MCTruthMgr.h"

#include "navigation/SimpleNavigator.h"
#include "volumes/PlacedVolume.h"
#include "management/GeoManager.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

using namespace vecgeom;

//______________________________________________________________________________
EventServer::EventServer(int nactive_max, RunManager *runmgr)
  :fNevents(0), fNactiveMax(nactive_max), fNactive(0), fNserved(0), fLastActive(-1), fCurrentEvent(0),
   fNload(0), fNstored(0), fNcompleted(0), fEvent(nullptr), fRunMgr(runmgr),
   fFreeSlots(AdjustSize(runmgr->GetConfig()->fNbuff)), fPendingEvents(4096), fDoneEvents(4096)
{
// Constructor
  assert(nactive_max > 0 && nactive_max < 4096);
  fLastActive.store(-1);

  // Create event slots
  fEvents = new Event*[fNactiveMax];
  for (size_t slot = 0; slot < size_t(fNactiveMax); ++slot) {
    fEvents[slot] = nullptr;
    fFreeSlots.enqueue(slot);
  }

  // Create empty events
  bool generate = fRunMgr->GetConfig()->fRunMode == GeantConfig::kGenerator;
  int nthreads = runmgr->GetNthreadsTotal();
  int ngen = vecCore::math::Max(fNactiveMax, nthreads);
  if (generate) {
    fNevents = fRunMgr->GetConfig()->fNtotal;
    ngen = vecCore::math::Min(ngen, fNevents);
  }

  unsigned int error = 0;
  if (generate) {
    // Pre-generate events at least as many as threads if possible
    fGenLock.clear(std::memory_order_release);
    TaskData *td = fRunMgr->GetTDManager()->GetTaskData(0);
    for (int i = 0; i < ngen; ++i) {
      GenerateNewEvent(td, error);
      assert(error == 0 && "EventServer::ctor ERROR in event generation");
    }
    Event *event = nullptr;
    // Activate the first event
    ActivateEvent(event, error);
    assert(error == 0 && "EventServer::ctor ERROR in event activation");
  }
}

//______________________________________________________________________________
EventServer::~EventServer()
{
// Destructor
  for (int i = 0; i < fNactiveMax; ++i) {
    delete fEvents[i];
  }
  Event *event;
  while (fDoneEvents.dequeue(event)) delete event;
}

//______________________________________________________________________________
Event *EventServer::GenerateNewEvent(TaskData *td, unsigned int &error)
{
// Generates a new event in standalone GeantV mode by filling an empty one and
// putting it in the pending events queue.
//
// The method may fail due to:
// - the method is in use by another thread (error = 1)
// - all events were generated already generated (error = 2)
// - queue of empty events is empty. Should not happen. (error = 3)
// - event could not be added to the pending queue. Should not happen (error = 4)

  // The method has to be locked since thread safety not yet required for the generator.
  // Policy: if someone else working here, just return
  if (fGenLock.test_and_set(std::memory_order_acquire)) {
    error = 1;
    return nullptr;
  }
  int nload = fNload.load();
  if (nload >= fNevents) {
    error = 2;
    fGenLock.clear(std::memory_order_release);
    return nullptr;
  }
  error = 0;
  // Now just get next event from the generator
  EventInfo eventinfo = fRunMgr->GetPrimaryGenerator()->NextEvent(td);
  int ntracks = eventinfo.ntracks;
  int nloadtmp = nload;
  while (!ntracks) {
    Error("GenerateNewEvent", "### Problem with generator: event %d has no tracks", nloadtmp++);
    eventinfo = fRunMgr->GetPrimaryGenerator()->NextEvent(td);
    ntracks = eventinfo.ntracks;
  }

  Event *event = new Event();
  event->SetNprimaries(ntracks);
  event->SetVertex(eventinfo.xvert, eventinfo.yvert, eventinfo.zvert);
  // Populate event with primary tracks from the generator
  for (int itr=0; itr<ntracks; ++itr) {
    Track &track = td->GetNewTrack();
    track.SetParticle(event->AddPrimary(&track));
    fRunMgr->GetPrimaryGenerator()->GetTrack(itr, track, td);
  }

  fGenLock.clear(std::memory_order_release);
  if (!AddEvent(event)) {
    error = 4;
    return nullptr;
  }
  return event;
}

//______________________________________________________________________________
bool EventServer::AddEvent(Event *event)
{
// Adds one event into the queue of pending events.
  bool external_loop = fRunMgr->GetConfig()->fRunMode == GeantConfig::kExternalLoop;
  int evt = fNload.fetch_add(1);
  if (external_loop) evt = event->GetEvent();
  event->SetEvent(evt);
  // The vertex must be defined
  vecgeom::Vector3D<double> vertex = event->GetVertex();
  int ntracks = event->GetNprimaries();

  // start new event in MCTruthMgr
  if(fRunMgr->GetMCTruthMgr()) fRunMgr->GetMCTruthMgr()->OpenEvent(evt);
  
  // Initialize navigation path for the vertex
  //Volume_t *vol = 0;
  // Initialize the start path
  VolumePath_t *startpath = VolumePath_t::MakeInstance(fRunMgr->GetConfig()->fMaxDepth);
  vecgeom::SimpleNavigator nav;
  startpath->Clear();
  nav.LocatePoint(GeoManager::Instance().GetWorld(), vertex, *startpath, true);

  // Check and fix tracks
  for (int itr=0; itr<ntracks; ++itr) {
    Track &track = *event->GetPrimary(itr);
    track.SetPrimaryParticleIndex(itr);
    track.SetPath(startpath);
    track.SetNextPath(startpath);
    track.SetEvent(evt);
    if (!track.IsNormalized()) {
      track.Print("Not normalized");
      track.Normalize();
    }
    if (track.GVcode() < 0) {
      Error("AddEvent", "GeantV particle codes not initialized. Looks like primary generator was not initialized !!!");
      return false;
    }
    track.SetBoundary(false);
    track.SetStatus(kNew);
    event->fNfilled++;
    if (fRunMgr->GetMCTruthMgr()) fRunMgr->GetMCTruthMgr()->AddTrack(track);
  }
  VolumePath_t::ReleaseInstance(startpath);

  if (!fPendingEvents.enqueue(event)) {
    // Should never happen
    Error("AddEvent", "Event pool is full");
    return false;
  }
  fNprimaries += event->GetNprimaries();
  event->SetPriorityThr(fRunMgr->GetConfig()->fPriorityThr);
  // Update number of stored events
  int nstored = fNstored.fetch_add(1) + 1;
  if (nstored <= fNactiveMax) fNtracksInit += event->GetNprimaries();
  if (nstored == fNactiveMax) {
    // Check consistency of parallelism settings
    int nthreads = fRunMgr->GetNthreadsTotal();
    int basket_size = fRunMgr->GetConfig()->fNperBasket;
    int ntracksperevent = fNtracksInit/fNactiveMax;
    fNbasketsInit = fNtracksInit/basket_size;
    if (fNtracksInit % basket_size > 0) fNbasketsInit++;
    printf("=== Imported %d primaries from %d buffered events\n", fNtracksInit, fNactiveMax);
    printf("=== Buffering %d baskets of size %d feeding %d threads\n", fNbasketsInit, basket_size, nthreads);
    if (fNbasketsInit < nthreads || fNactiveMax < nthreads) {
      if (fNbasketsInit < nthreads)
        printf("### \e[5mWARNING!    Concurrency settings are not optimal. Not enough baskets to feed all threads.\e[m\n###\n");
      if (fNactiveMax < nthreads)
        printf("### \e[5mWARNING!    Increase number of buffered events to minimum %d\e[m\n###\n", nthreads);
      printf("###                          Rule of thumb:\n");
      printf("###  ==================================================================\n");
      printf("### ||  Nbuff_events * Nprimaries_per_event > Nthreads * basket_size  ||\n");
      printf("###  ==================================================================\n###\n");
      printf("### Either of the following options can solve this problem:\n");
      printf("###    => increase number of buffered events slots to minimum %d\n", nthreads * basket_size / ntracksperevent);
      printf("###    => decrease number of threads to maximum %d\n", vecCore::math::Max(1, fNtracksInit/basket_size));
    }
  }
 
  if (external_loop && !fEvent.load()) {
    unsigned int error = 0;
    event = nullptr;
    // Activate the first event
    ActivateEvent(event, error);
    assert(error == 0 && "EventServer::AddEvent ERROR in event activation");
  }
  fEventsServed = false;
  fHasTracks = true;
  return true;
}

//______________________________________________________________________________
Event *EventServer::ActivateEvent(Event *event, unsigned int &error, TaskData *td)
{
// Activates one event replacing the current one (if matching the expected value).
// The method can fail due to one of the following reasons:
// - All events already served (nactive = ntotal) in generator mode
// - All events from buffer served (nactive = nstored) in external loop mode
// - No slots available (should never happen since activation is driven by slot release)
// - All buffered events were served (should not happen in generator mode)

  if (fEventsServed) {
    error = kDone;
    return nullptr;
  }

  // A slot should always be available when calling ActivateEvent
  size_t slot;
  if (!fFreeSlots.dequeue(slot)) {
    error = kCSlots;
    return nullptr;
  }

  // Move old event from slot to queue of empty events
  if (fEvents[slot]) {
    Event *done_event = fEvents[slot];
    fEvents[slot] = nullptr;
    done_event->Clear(td);
    fDoneEvents.enqueue(done_event);
  }

  // Get a new generated event from pending queue
  if (!fPendingEvents.dequeue(fEvents[slot])) {
    fFreeSlots.enqueue(slot);
    error = kCEvents;
    return nullptr;
  }

  // Pre-activate slot and event number
  fEvents[slot]->SetSlot(slot);
  int nactive = fNactive.fetch_add(1) + 1;
  //fEvents[slot]->SetEvent(nactive - 1);
  
  
  // Try to replace the active event with the new one
  if (!fEvent.compare_exchange_strong(event, fEvents[slot])) {
    // Bad luck, someone else was faster.
    // Now we have to release the event and then the slot
    fNactive--;
    fPendingEvents.enqueue(fEvents[slot]);
    fEvents[slot] = nullptr;
    fFreeSlots.enqueue(slot);
    // 'event' contains now the pointer to the event activated by someone else
    error = kNoerr;
    return event;
  }

  // Check if all events were served
  if (fRunMgr->GetConfig()->fRunMode == GeantConfig::kGenerator) {
    if (nactive == fNevents) fEventsServed = true;
  } else {
    if (nactive == fNstored.load()) fEventsServed = true;
  }
  fHasTracks = true;
  error = kNoerr;
  return event;
}

//______________________________________________________________________________
void EventServer::CompletedEvent(Event *event, TaskData *td)
{
// Signals that event 'evt' was fully transported.
  size_t slot = event->GetSlot();
  fNcompleted++;

  assert(event->Transported());
  assert(event == fEvents[slot]);

  // Release slot as fast as possible. Activation will take care of recycling
  // the old event and putting a new generated one in place.
  fFreeSlots.enqueue(slot);

  // Generate new event
  if (fRunMgr->GetConfig()->fRunMode == GeantConfig::kGenerator) {
    unsigned int error = 1; // Generator busy
    while (error == 1) GenerateNewEvent(td, error);
  }
}

//______________________________________________________________________________
Track *EventServer::GetNextTrack(TaskData *td, unsigned int &error)
{
// Fetch next track of the current event. Increments current event if no more
// tracks. If current event matches last activated one, resets fHasTracks flag.
// If max event fully dispatched, sets the fDone flag.

  Event *event = nullptr;
  int itr;
  while (1) {
    // Book next track index
    bool valid;
    event = fEvent.load();
    itr = event->DispatchTrack(valid);
    if (!valid) {
      assert(event->IsDispatched());
      // Current event dispatched, try to activate new event
      event = ActivateEvent(event, error, td);
      if (!event) {
        if (error == kDone) fHasTracks = false;
        if (error) return nullptr;
        continue;
      }
      itr = event->DispatchTrack(valid);
    }
    if (valid) break;
  }
  Track *track = event->GetPrimary(itr)->Clone(td);
  track->SetEvent(event->GetEvent());
  track->SetEvslot(event->GetSlot());
  return track;
}

//______________________________________________________________________________
int EventServer::FillBasket(Basket *basket, int ntracks, TaskData *td, unsigned int &error)
{
// Fill concurrently a basket of tracks, up to the requested number of tracks.
// The client should test first the track availability using HasTracks().
  if (!fHasTracks) return 0;
  int ndispatched = 0;
  for (int i=0; i<ntracks; ++i) {
    Track *track = GetNextTrack(td, error);
    if (!track) break;
    basket->AddTrack(track);
    ndispatched++;
  }
  if (fInitialPhase) {
    int nserved = fNserved.fetch_add(1) + 1;
    if (nserved >= fNbasketsInit) fInitialPhase = false;
  }
  return ndispatched;
}

//______________________________________________________________________________
int EventServer::FillStackBuffer(StackLikeBuffer *buffer, int ntracks, TaskData *td, unsigned int &error)
{
// Fill concurrently up to the requested number of tracks into a stack-like buffer.
// The client should test first the track availability using HasTracks().

// *** I should template on the container to be filled, making sure that all
//     containers provide AddTrack(Track *)
  if (!fHasTracks) {
    error = kDone;
    return 0;
  }
  int ndispatched = 0;
  for (int i=0; i<ntracks; ++i) {
    Track *track = GetNextTrack(td, error);
    if (!track) break;
    buffer->AddTrack(track);
    ndispatched++;
  }
  if (fInitialPhase) {
    int nserved = fNserved.fetch_add(1) + 1;
    if (nserved >= fNbasketsInit) fInitialPhase = false;
  }

  if (ndispatched > 0) error = kNoerr;
  return ndispatched;
}

} // GEANT_IMPL_NAMESPACE
} // Geant
