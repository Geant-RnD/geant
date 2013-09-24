#include "GeantBasket.h"
#include "globals.h"
#include "GeantTrack.h"
#include "GeantPropagator.h"
#include "PhysicsProcess.h"
#include "WorkloadManager.h"

#include "TThread.h"
#include "TArrayI.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoBranchArray.h"

ClassImp(GeantBasket)

//______________________________________________________________________________
GeantBasket::GeantBasket()
            :TObject(),
             fVolId(0),
             fMatId(0),
             fTracksIn(),
             fTracksOut()
{
// dummy ctor.
}

//______________________________________________________________________________
GeantBasket::GeantBasket(Int_t size, Int_t volId, Int_t matId)
            :TObject(),
             fVolId(volId),
             fMatId(matId),
             fTracksIn(size),
             fTracksOut(size)
{
// ctor.
}

//______________________________________________________________________________
GeantBasket::~GeantBasket()
{
// dtor.
}
   
//______________________________________________________________________________
void GeantBasket::AddTrack(const GeantTrack *track)
{
// Add a new track to this basket;
   fTracksIn.AddTrack(*track);
}

//______________________________________________________________________________
void GeantBasket::AddTrack(const GeantTrack_v &tracks, Int_t itr)
{
// Add track from a track_v array
   fTracksIn.AddTrack(tracks, itr);
}

//______________________________________________________________________________
void GeantBasket::AddTracks(const GeantTrack_v &tracks, Int_t istart, Int_t iend)
{
// Add multiple tracks from a track_v array
   fTracksIn.AddTracks(tracks, istart, iend);
}
   
//______________________________________________________________________________
void GeantBasket::Clear(Option_t *option)
{
// Clear basket;
   fTracksIn.Clear(option);
   fTracksOut.Clear(option);
}   

//______________________________________________________________________________
Bool_t GeantBasket::Contains(Int_t evstart, Int_t nevents) const
{
// Checks if any of the array of tracks belongs to the given event.
   return fTracksIn.Contains(evstart, nevents);
}      

//______________________________________________________________________________
void GeantBasket::Print(Option_t *) const
{
// Print basket content.
}

//______________________________________________________________________________
void GeantBasket::PrintTrack(Int_t itr, Bool_t input) const
{
// Print a given track.
}
