#include "LinearPropagationHandler.h"

#include "GeantTaskData.h"

namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
LinearPropagationHandler::LinearPropagationHandler(int threshold, GeantPropagator *propagator)
               : Handler(threshold, propagator)
{
// Default constructor
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
LinearPropagationHandler::~LinearPropagationHandler()
{
// Destructor
}  


//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void LinearPropagationHandler::DoIt(GeantTrack *track, Basket& output, GeantTaskData *td)
{
// Scalar geometry length computation. The track is moved into the output basket.

  // Do straight propagation to physics process or boundary
  track->fPstep -= track->fSnext;
  track->fStep += track->fSnext;
  track->fSafety -= track->fSnext;
  if (track->fSafety < 0.)
    track->fSafety = 0;
  track->fXpos += track->fSnext * track->fXdir;
  track->fYpos += track->fSnext * track->fYdir;
  track->fZpos += track->fSnext * track->fZdir;
  // Update total number of steps
  td->fNsteps++;

  if (track->fBoundary) {
    if (track->fNextpath->IsOutside())
      track->fStatus = kExitingSetup;
    else
      track->fStatus = kBoundary;
  } else {
    track->fStatus = kPhysics;
    // Update number of steps to physics
    td->fNphys++;
  }

  if (track->fSnext < 1.E-8) td->fNsmall++;
  track->fSnext = 0;
  
  // Update time of flight and number of interaction lengths
  track->fTime += track->TimeStep(track->fStep);
  track->fNintLen -= track->fStep/track->fIntLen;

  // Set follow-up stage to be ContinuousProcStage
  track->SetStage(kContinuousProcStage);

  // Copy to output
  output.AddTrack(track);
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void LinearPropagationHandler::DoIt(Basket &input, Basket& output, GeantTaskData *td)
{
// Vector geometry length computation. The tracks are moved into the output basket.
  TrackVec_t &tracks = input.Tracks();  
  // This loop should autovectorize
  for (auto track : tracks) {
    track->fPstep -= track->fSnext;
    track->fStep += track->fSnext;
    track->fSafety -= track->fSnext;
    if (track->fSafety < 0.)
      track->fSafety = 0;
    track->fXpos += track->fSnext * track->fXdir;
    track->fYpos += track->fSnext * track->fYdir;
    track->fZpos += track->fSnext * track->fZdir;
  }

  // Update total number of steps
  td->fNsteps += tracks.size();

  for (auto track : tracks) {
    if (track->fBoundary) {
      if (track->fNextpath->IsOutside())
        track->fStatus = kExitingSetup;
      else
        track->fStatus = kBoundary;
    } else {
      track->fStatus = kPhysics;
      // Update number of steps to physics
      td->fNphys++;
    }
    if (track->fSnext < 1.E-8) td->fNsmall++;
    track->fSnext = 0;    
    // Update time of flight and number of interaction lengths
    track->fTime += track->TimeStep(track->fStep);
    track->fNintLen -= track->fStep/track->fIntLen;  
  }

  // Copy tracks to output
#ifndef VECCORE_CUDA
  std::move(tracks.begin(), tracks.end(), std::back_inserter(output.Tracks()));
#else
  for (auto track : tracks) output.AddTrack(track);
#endif
}

} // GEANT_IMPL_NAMESPACE
} // Geant
