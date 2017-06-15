#include "GeantTrack.h"

#include "globals.h"
#include "Geant/Error.h"
#include <execinfo.h>
#include "GeantPropagator.h"

namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
GeantTrack::GeantTrack()
    : fEvent(-1), fEvslot(-1), fParticle(-1), fMother(0), fPDG(0), fGVcode(0), fEindex(0), fCharge(0), fProcess(-1),
      fNsteps(0), fMaxDepth(0), fStage(0), fGeneration(0), fSpecies(kHadron), fStatus(kAlive), fMass(0), fXpos(0), fYpos(0), fZpos(0), fXdir(0), fYdir(0),
      fZdir(0), fP(0), fE(0), fTime(0), fEdep(0), fPstep(1.E20), fStep(0), fSnext(0), fSafety(0), fNintLen(0), fIntLen(0),
      fBoundary(false), fPending(false), fOwnPath(true), fPath(0), fNextpath(0) {
  // Dummy constructor
  //
  // msc ------
  fLambda0       = 0.;  // elastic mean free path
  fLambda1       = 0.;  // first transport mean free path
  fScrA          = 0.;  // screening parameter if any
  fG1            = 0.;  // first transport coef.
  fRange         = 1.e+20; // range of the particle
  //******************
  fTheInitialRange         = 1.e+21;   // the initial (first step or first step in volume) range value of the particle
  fTheTrueStepLenght       = 0.;   // the true step length
  fTheTransportDistance    = 0.;   // the straight line distance between the pre- and true post-step points
  fTheZPathLenght          = 0.;   // projection of transport distance along the original direction
  fTheTrueGeomLimit        = 1.e+20; // geometrical limit converted to true step length
  fTheDisplacementVectorX  = 0.;   // displacement vector components X,Y,Z
  fTheDisplacementVectorY  = 0.;
  fTheDisplacementVectorZ  = 0.;
  fTheNewDirectionX        = 0.;   // new direction components X,Y,Z (at the post-step point due to msc)
  fTheNewDirectionY        = 0.;
  fTheNewDirectionZ        = 1.;
  fPar1                    =-1.;
  fPar2                    = 0.;
  fPar3                    = 0.;

  fIsEverythingWasDone       = false; // to indicate if everything could be done in the step limit phase
  fIsMultipleSacettring      = false; // to indicate that msc needs to be perform (i.e. compute angular deflection)
  fIsSingleScattering        = false; // to indicate that single scattering needs to be done
  fIsEndedUpOnBoundary       = false; // ?? flag to indicate that geometry was the winer
  fIsNoScatteringInMSC       = false; // to indicate that no scattering happend (i.e. forward) in msc
  fIsNoDisplace              = false; // to indicate that displacement is not computed
  fIsInsideSkin              = false; // to indicate that the particle is within skin from/to boundary
  fIsWasOnBoundary           = false; // to indicate that boundary crossing happend recently
  fIsFirstStep               = true ; // to indicate that the first step is made with the particle
  fIsFirstRealStep           = false; // to indicate that the particle is making the first real step in the volume i.e.
                                      // just left the skin
}

/* Obtain a backtrace and print it to stdout. */
void printrace(void) {
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace(array, 10);
  strings = backtrace_symbols(array, size);

  printf("Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++)
    printf("%s\n", strings[i]);

  free(strings);
}


//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
GeantTrack::GeantTrack(int ipdg, int maxdepth)
    : fEvent(-1), fEvslot(-1), fParticle(-1), fMother(0), fPDG(ipdg), fGVcode(0), fEindex(0), fCharge(0), fProcess(-1),
      fNsteps(0), fMaxDepth(maxdepth), fStage(0), fGeneration(0), fSpecies(kHadron), fStatus(kAlive), fMass(0), fXpos(0), fYpos(0), fZpos(0), fXdir(0), fYdir(0),
      fZdir(0), fP(0), fE(0), fTime(0), fEdep(0), fPstep(1.E20), fStep(0), fSnext(0), fSafety(0), fNintLen(0), fIntLen(0),
      fBoundary(false), fPending(false), fOwnPath(true), fPath(0), fNextpath(0) {
  // Constructor
  fPath = VolumePath_t::MakeInstance(maxdepth);
  fNextpath = VolumePath_t::MakeInstance(maxdepth);
  //
  // msc ------
  fLambda0       = 0.;  // elastic mean free path
  fLambda1       = 0.;  // first transport mean free path
  fScrA          = 0.;  // screening parameter if any
  fG1            = 0.;  // first transport coef.
  fRange         = 1.e+20;
  //******************
  fTheInitialRange         = 1.e+21;   // the initial (first step or first step in volume) range value of the particle
  fTheTrueStepLenght       = 0.;   // the true step length
  fTheTransportDistance    = 0.;   // the straight line distance between the pre- and true post-step points
  fTheZPathLenght          = 0.;   // projection of transport distance along the original direction
  fTheTrueGeomLimit        = 1.e+20; // geometrical limit converted to true step length
  fTheDisplacementVectorX  = 0.;   // displacement vector components X,Y,Z
  fTheDisplacementVectorY  = 0.;
  fTheDisplacementVectorZ  = 0.;
  fTheNewDirectionX        = 0.;   // new direction components X,Y,Z (at the post-step point due to msc)
  fTheNewDirectionY        = 0.;
  fTheNewDirectionZ        = 1.;
  fPar1                    =-1.;
  fPar2                    = 0.;
  fPar3                    = 0.;

  fIsEverythingWasDone       = false; // to indicate if everything could be done in the step limit phase
  fIsMultipleSacettring      = false; // to indicate that msc needs to be perform (i.e. compute angular deflection)
  fIsSingleScattering        = false; // to indicate that single scattering needs to be done
  fIsEndedUpOnBoundary       = false; // ?? flag to indicate that geometry was the winer
  fIsNoScatteringInMSC       = false; // to indicate that no scattering happend (i.e. forward) in msc
  fIsNoDisplace              = false; // to indicate that displacement is not computed
  fIsInsideSkin              = false; // to indicate that the particle is within skin from/to boundary
  fIsWasOnBoundary           = false; // to indicate that boundary crossing happend recently
  fIsFirstStep               = true ; // to indicate that the first step is made with the particle
  fIsFirstRealStep           = false; // to indicate that the particle is making the first real step in the volume i.e.
                                      // just left the skin
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
GeantTrack::GeantTrack(void *addr, int maxdepth)
    : fEvent(-1), fEvslot(-1), fParticle(-1), fMother(0), fPDG(0), fGVcode(0), fEindex(0), fCharge(0), fProcess(-1),
      fNsteps(0), fMaxDepth(maxdepth), fStage(0), fGeneration(0), fSpecies(kHadron), fStatus(kAlive), fMass(0), fXpos(0), fYpos(0), fZpos(0), fXdir(0), fYdir(0),
      fZdir(0), fP(0), fE(0), fTime(0), fEdep(0), fPstep(1.E20), fStep(0), fSnext(0), fSafety(0), fNintLen(0), fIntLen(0),
      fBoundary(false), fPending(false), fOwnPath(true), fPath(nullptr), fNextpath(nullptr) {
  // In place private constructor
  char *path_addr = round_up_align((char*)addr + sizeof(GeantTrack));
  fPath = VolumePath_t::MakeInstanceAt(maxdepth, path_addr);
  path_addr += round_up_align(VolumePath_t::SizeOfInstance(maxdepth));
  fNextpath = VolumePath_t::MakeInstanceAt(maxdepth, path_addr);
  //
  // msc ------
  fLambda0       = 0.;  // elastic mean free path
  fLambda1       = 0.;  // first transport mean free path
  fScrA          = 0.;  // screening parameter if any
  fG1            = 0.;  // first transport coef.
  fRange         = 1.e+20;
  //******************
  fTheInitialRange         = 1.e+21;   // the initial (first step or first step in volume) range value of the particle
  fTheTrueStepLenght       = 0.;   // the true step length
  fTheTransportDistance    = 0.;   // the straight line distance between the pre- and true post-step points
  fTheZPathLenght          = 0.;   // projection of transport distance along the original direction
  fTheTrueGeomLimit        = 1.e+20; // geometrical limit converted to true step length
  fTheDisplacementVectorX  = 0.;   // displacement vector components X,Y,Z
  fTheDisplacementVectorY  = 0.;
  fTheDisplacementVectorZ  = 0.;
  fTheNewDirectionX        = 0.;   // new direction components X,Y,Z (at the post-step point due to msc)
  fTheNewDirectionY        = 0.;
  fTheNewDirectionZ        = 1.;
  fPar1                    =-1.;
  fPar2                    = 0.;
  fPar3                    = 0.;

  fIsEverythingWasDone       = false; // to indicate if everything could be done in the step limit phase
  fIsMultipleSacettring      = false; // to indicate that msc needs to be perform (i.e. compute angular deflection)
  fIsSingleScattering        = false; // to indicate that single scattering needs to be done
  fIsEndedUpOnBoundary       = false; // ?? flag to indicate that geometry was the winer
  fIsNoScatteringInMSC       = false; // to indicate that no scattering happend (i.e. forward) in msc
  fIsNoDisplace              = false; // to indicate that displacement is not computed
  fIsInsideSkin              = false; // to indicate that the particle is within skin from/to boundary
  fIsWasOnBoundary           = false; // to indicate that boundary crossing happend recently
  fIsFirstStep               = true ; // to indicate that the first step is made with the particle
  fIsFirstRealStep           = false; // to indicate that the particle is making the first real step in the volume i.e.
                                      // just left the skin
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
GeantTrack::GeantTrack(const GeantTrack &other)
    : fEvent(other.fEvent), fEvslot(other.fEvslot), fParticle(other.fParticle), fMother(other.fMother), fPDG(other.fPDG),
      fGVcode(other.fGVcode), fEindex(other.fEindex), fCharge(other.fCharge), fProcess(other.fProcess),
      fNsteps(other.fNsteps), fMaxDepth(other.fMaxDepth), fStage(other.fStage), fGeneration(other.fGeneration), fSpecies(other.fSpecies),
      fStatus(other.fStatus), fMass(other.fMass), fXpos(other.fXpos), fYpos(other.fYpos), fZpos(other.fZpos), fXdir(other.fXdir),
      fYdir(other.fYdir), fZdir(other.fZdir), fP(other.fP), fE(other.fE), fTime(other.fTime), fEdep(other.fEdep),
      fPstep(other.fPstep), fStep(other.fStep), fSnext(other.fSnext), fSafety(other.fSafety), fNintLen(other.fNintLen), fIntLen(other.fIntLen),
      fBoundary(other.fBoundary), fPending(other.fPending), fOwnPath(true), fPath(0), fNextpath(0) {
  // Copy constructor
  fPath = VolumePath_t::MakeInstance(fMaxDepth);
  fNextpath = VolumePath_t::MakeInstance(fMaxDepth);
  *fPath = *other.fPath;
  *fNextpath = *other.fNextpath;
  //
  // msc ----
  // copy msc members
  fLambda0       = other.fLambda0;  // elastic mean free path
  fLambda1       = other.fLambda1;  // first transport mean free path
  fScrA          = other.fScrA;  // screening parameter if any
  fG1            = other.fG1;  // first transport coef.
  fRange         = other.fRange;
  //******************
  fTheInitialRange         = other.fTheInitialRange;   // the initial (first step or first step in volume) range value of the particle
  fTheTrueStepLenght       = other.fTheTrueStepLenght;   // the true step length
  fTheTransportDistance    = other.fTheTransportDistance;   // the straight line distance between the pre- and true post-step points
  fTheZPathLenght          = other.fTheZPathLenght;   // projection of transport distance along the original direction
  fTheTrueGeomLimit        = other.fTheTrueGeomLimit;    // geometrical limit converted to true step length
  fTheDisplacementVectorX  = other.fTheDisplacementVectorX;   // displacement vector components X,Y,Z
  fTheDisplacementVectorY  = other.fTheDisplacementVectorY;
  fTheDisplacementVectorZ  = other.fTheDisplacementVectorZ;
  fTheNewDirectionX        = other.fTheNewDirectionX;   // new direction components X,Y,Z (at the post-step point due to msc)
  fTheNewDirectionY        = other.fTheNewDirectionY;
  fTheNewDirectionZ        = other.fTheNewDirectionZ;
  fPar1                    = other.fPar1;
  fPar2                    = other.fPar2;
  fPar3                    = other.fPar2;

  fIsEverythingWasDone     = other.fIsEverythingWasDone; // to indicate if everything could be done in the step limit phase
  fIsMultipleSacettring    = other.fIsMultipleSacettring; // to indicate that msc needs to be perform (i.e. compute angular deflection)
  fIsSingleScattering      = other.fIsSingleScattering; // to indicate that single scattering needs to be done
  fIsEndedUpOnBoundary     = other.fIsEndedUpOnBoundary; // ?? flag to indicate that geometry was the winer
  fIsNoScatteringInMSC     = other.fIsNoScatteringInMSC; // to indicate that no scattering happend (i.e. forward) in msc
  fIsNoDisplace            = other.fIsNoDisplace; // to indicate that displacement is not computed
  fIsInsideSkin            = other.fIsInsideSkin; // to indicate that the particle is within skin from/to boundary
  fIsWasOnBoundary         = other.fIsWasOnBoundary; // to indicate that boundary crossing happend recently
  fIsFirstStep             = other.fIsFirstStep; // to indicate that the first step is made with the particle
  fIsFirstRealStep         = other.fIsFirstRealStep; // to indicate that the particle is making the first real step in the volume i.e.
                                      // just left the skin
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
GeantTrack &GeantTrack::operator=(const GeantTrack &other) {
  // Assignment
  if (&other != this) {
    fEvent = other.fEvent;
    fEvslot = other.fEvslot;
    fParticle = other.fParticle;
    fMother = other.fMother;
    fPDG = other.fPDG;
    fGVcode = other.fGVcode;
    fEindex = other.fEindex;
    fCharge = other.fCharge;
    fProcess = other.fProcess;
    fNsteps = other.fNsteps;
    fSpecies = other.fSpecies;
    fStatus = other.fStatus;
    fMass = other.fMass;
    fXpos = other.fXpos;
    fYpos = other.fYpos;
    fZpos = other.fZpos;
    fXdir = other.fXdir;
    fYdir = other.fYdir;
    fZdir = other.fZdir;
    fP = other.fP;
    fE = other.fE;
    fTime = other.fTime;
    fEdep = other.fEdep;
    fPstep = other.fPstep;
    fStep = other.fStep;
    fSnext = other.fSnext;
    fSafety = other.fSafety;
    fNintLen = other.fNintLen;
    fIntLen = other.fIntLen;
    fBoundary = other.fBoundary;
    fPending = other.fPending;
    fMaxDepth = other.fMaxDepth;
    fStage = other.fStage;
    fGeneration = other.fGeneration;
    *fPath = *other.fPath;
    *fNextpath = *other.fNextpath;
    //
    // msc ----
    // copy msc members
    fLambda0       = other.fLambda0;  // elastic mean free path
    fLambda1       = other.fLambda1;  // first transport mean free path
    fScrA          = other.fScrA;  // screening parameter if any
    fG1            = other.fG1;  // first transport coef.
    fRange         = other.fRange;
    //******************
    fTheInitialRange         = other.fTheInitialRange;   // the initial (first step or first step in volume) range value of the particle
    fTheTrueStepLenght       = other.fTheTrueStepLenght;   // the true step length
    fTheTransportDistance    = other.fTheTransportDistance;   // the straight line distance between the pre- and true post-step points
    fTheZPathLenght          = other.fTheZPathLenght;   // projection of transport distance along the original direction
    fTheTrueGeomLimit        = other.fTheTrueGeomLimit; // geometrical limit converted to true step length
    fTheDisplacementVectorX  = other.fTheDisplacementVectorX;   // displacement vector components X,Y,Z
    fTheDisplacementVectorY  = other.fTheDisplacementVectorY;
    fTheDisplacementVectorZ  = other.fTheDisplacementVectorZ;
    fTheNewDirectionX        = other.fTheNewDirectionX;   // new direction components X,Y,Z (at the post-step point due to msc)
    fTheNewDirectionY        = other.fTheNewDirectionY;
    fTheNewDirectionZ        = other.fTheNewDirectionZ;
    fPar1                    = other.fPar1;
    fPar2                    = other.fPar2;
    fPar3                    = other.fPar2;

    fIsEverythingWasDone     = other.fIsEverythingWasDone; // to indicate if everything could be done in the step limit phase
    fIsMultipleSacettring    = other.fIsMultipleSacettring; // to indicate that msc needs to be perform (i.e. compute angular deflection)
    fIsSingleScattering      = other.fIsSingleScattering; // to indicate that single scattering needs to be done
    fIsEndedUpOnBoundary     = other.fIsEndedUpOnBoundary; // ?? flag to indicate that geometry was the winer
    fIsNoScatteringInMSC     = other.fIsNoScatteringInMSC; // to indicate that no scattering happend (i.e. forward) in msc
    fIsNoDisplace            = other.fIsNoDisplace; // to indicate that displacement is not computed
    fIsInsideSkin            = other.fIsInsideSkin; // to indicate that the particle is within skin from/to boundary
    fIsWasOnBoundary         = other.fIsWasOnBoundary; // to indicate that boundary crossing happend recently
    fIsFirstStep             = other.fIsFirstStep; // to indicate that the first step is made with the particle
    fIsFirstRealStep         = other.fIsFirstRealStep; // to indicate that the particle is making the first real step in the volume i.e.
                                        // just left the skin
  }
  return *this;
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
GeantTrack::~GeantTrack() {
  // Destructor.
  if (fOwnPath) {
    VolumePath_t::ReleaseInstance(fPath);
    VolumePath_t::ReleaseInstance(fNextpath);
  }
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void GeantTrack::Clear(const char *) {
  // Resets track content.
  fEvent = -1;
  fEvslot = -1;
  fParticle = -1;
  fMother = 0;
  fPDG = 0;
  fGVcode = 0;
  fEindex = 0;
  fCharge = 0;
  fProcess = -1;
  fNsteps = 0;
  fSpecies = kHadron;
  fStatus = kAlive;
  fMass = 0.;
  fXpos = 0.;
  fYpos = 0.;
  fZpos = 0.;
  fXdir = 0.;
  fYdir = 0.;
  fZdir = 0.;
  fP = 0.;
  fE = 0.;
  fTime = 0.;
  fEdep = 0;
  fPstep = 1.E20;
  fStep = 0.;
  fSnext = 0.;
  fSafety = 0.;
  fNintLen = 0.;
  fIntLen = 0.;
  fBoundary = false;
  fPending = false;
  fMaxDepth = 0;
  fStage = 0;
  fGeneration = 0;
  //
  // msc ------
  fLambda0       = 0.;  // elastic mean free path
  fLambda1       = 0.;  // first transport mean free path
  fScrA          = 0.;  // screening parameter if any
  fG1            = 0.;  // first transport coef.
  fRange         = 1.e+20;
  //******************
  fTheInitialRange         = 1.e+21;   // the initial (first step or first step in volume) range value of the particle
  fTheTrueStepLenght       = 0.;   // the true step length
  fTheTransportDistance    = 0.;   // the straight line distance between the pre- and true post-step points
  fTheZPathLenght          = 0.;   // projection of transport distance along the original direction
  fTheTrueGeomLimit        = 1.e+20; // geometrical limit converted to true step length
  fTheDisplacementVectorX  = 0.;   // displacement vector components X,Y,Z
  fTheDisplacementVectorY  = 0.;
  fTheDisplacementVectorZ  = 0.;
  fTheNewDirectionX        = 0.;   // new direction components X,Y,Z (at the post-step point due to msc)
  fTheNewDirectionY        = 0.;
  fTheNewDirectionZ        = 1.;
  fPar1                    =-1.;
  fPar2                    = 0.;
  fPar3                    = 0.;

  fIsEverythingWasDone       = false; // to indicate if everything could be done in the step limit phase
  fIsMultipleSacettring      = false; // to indicate that msc needs to be perform (i.e. compute angular deflection)
  fIsSingleScattering        = false; // to indicate that single scattering needs to be done
  fIsEndedUpOnBoundary       = false; // ?? flag to indicate that geometry was the winer
  fIsNoScatteringInMSC       = false; // to indicate that no scattering happend (i.e. forward) in msc
  fIsNoDisplace              = false; // to indicate that displacement is not computed
  fIsInsideSkin              = false; // to indicate that the particle is within skin from/to boundary
  fIsWasOnBoundary           = false; // to indicate that boundary crossing happend recently
  fIsFirstStep               = true ; // to indicate that the first step is made with the particle
  fIsFirstRealStep           = false; // to indicate that the particle is making the first real step in the volume i.e.
                                      // just left the skin
#ifdef USE_VECGEOM_NAVIGATOR
  fPath->Clear();
  fNextpath->Clear();
#endif
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
Volume_t const*GeantTrack::GetVolume() const {
#ifdef USE_VECGEOM_NAVIGATOR
  return fPath->Top()->GetLogicalVolume();
#else
  return (fPath->GetCurrentNode()->GetVolume());
#endif
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
Volume_t const*GeantTrack::GetNextVolume() const {
#ifdef USE_VECGEOM_NAVIGATOR
  // Next volume the track is entering
  return fNextpath->Top()->GetLogicalVolume();
#else
  // Next volume the track is entering
  return fNextpath->GetCurrentNode()->GetVolume();
#endif
}

//______________________________________________________________________________
Material_t *GeantTrack::GetMaterial() const {
   // Current material the track is into
#ifdef USE_VECGEOM_NAVIGATOR
   auto med = (Medium_t *) GetVolume()->GetTrackingMediumPtr();
#else
   auto med = GetVolume()->GetMedium();
#endif
   if (!med)
      return 0;
   return med->GetMaterial();
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void GeantTrack::SetPath(VolumePath_t const *const path) {
  // Set path.
  *fPath = *path;
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void GeantTrack::SetNextPath(VolumePath_t const *const path) {
  // Set next path.
  *fNextpath = *path;
}

//______________________________________________________________________________
void GeantTrack::Print(const char *msg) const {
  const char *status[8] = {"alive", "killed", "inflight", "boundary", "exitSetup", "physics", "postponed", "new"};
#ifdef USE_VECGEOM_NAVIGATOR
  Geant::Print(msg,
      "evt=%d slt=%d part=%d mth=%d pdg=%d gvc=%d eind=%d bind=%d chg=%d proc=%d nstp=%d spc=%d status=%s mass=%g "
      "xpos=%g ypos=%g zpos=%g xdir=%g ydir=%g zdir=%g mom=%g ene=%g time=%g pstp=%g stp=%g snxt=%g saf=%g nil=%g ile=%g bdr=%d\n",
      fEvent, fEvslot, fParticle, fMother, fPDG, fGVcode, fEindex, fBindex,
      fCharge, fProcess, fNsteps, (int)fSpecies, status[int(fStatus)],
      fMass, fXpos, fYpos, fZpos, fXdir, fYdir, fZdir, fP, fE,
      fTime, fPstep, fStep, fSnext, fSafety, fNintLen, fIntLen, fBoundary);

#ifndef VECCORE_CUDA
  fPath->Print();
  fNextpath->Print();
#endif
#else
  TString path;
  fPath->GetPath(path);
  TString nextpath;
  fNextpath->GetPath(nextpath);

  Geant::Print(msg, "evt=%d slt=%d part=%d mth=%d pdg=%d gvc=%d eind=%d bind=%d chg=%d proc=%d nstp=%d "
         "spc=%d status=%s mass=%g xpos=%g ypos=%g zpos=%g xdir=%g ydir=%g zdir=%g mom=%g ene=%g "
         "time=%g edep=%g pstp=%g stp=%g snxt=%g saf=%g nil=%g ile=%g bdr=%d\n pth=%s npth=%s\n",
         fEvent, fEvslot, fParticle, fMother, fPDG, fGVcode, fEindex, fBindex,
         fCharge, fProcess, fNsteps, (int)fSpecies, status[int(fStatus)],
         fMass, fXpos, fYpos, fZpos, fXdir, fYdir, fZdir, fP, fE,
         fTime, fEdep, fPstep, fStep, fSnext, fSafety, fNintLen, fIntLen, fBoundary, path.Data(),
         nextpath.Data());
#endif
}

//______________________________________________________________________________
void GeantTrack::PrintTracks(TrackVec_t &tracks)
{
  for (auto track : tracks) track->Print("xxx");
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
size_t GeantTrack::SizeOfInstance(size_t maxdepth) {
  // return the contiguous memory size needed to hold a GeantTrack
  // The VecGeom navigation state requires alignment, so we need to account for the
  // worst possible scenario:
  //
  // |--padding--|--padding--|--padding--|--padding--|--padding--| (adresses)
  // |--*start----*empty_spc.*fPath-------*empty_spc.*fNextpath--  (obj. layout)
  return ( sizeof(GeantTrack) +
           2 * VolumePath_t::SizeOfInstance(maxdepth) +
           2 * GEANT_ALIGN_PADDING );
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
GeantTrack *GeantTrack::MakeInstanceAt(void *addr, int maxdepth) {
  return new (addr) GeantTrack(addr, maxdepth);
}

} // GEANT_IMPL_NAMESPACE
} // Geant
