#include "GeantTrack.h"

#include "globals.h"
#include "Geant/Error.h"
#include <execinfo.h>

#ifdef USE_VECGEOM_NAVIGATOR
#pragma message("Compiling against VecGeom")
#include "backend/Backend.h"
#include "navigation/SimpleNavigator.h"
#include "navigation/ABBoxNavigator.h"
#include "volumes/PlacedVolume.h" // equivalent of TGeoNode
#include "base/Vector3D.h"
#include "base/Transformation3D.h"
#include "base/Global.h"
#include "management/GeoManager.h"
#include "base/SOA3D.h"
#ifdef CROSSCHECK
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#endif
#else
#pragma message("Compiling against TGeo")
#include <iostream>
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#endif

#include "WorkloadManager.h"

#include "GeantTaskData.h"
#include "ConstFieldHelixStepper.h"
#include "GeantScheduler.h"

// #ifdef  RUNGE_KUTTA
#pragma message("Compiling using Runge-Kutta for integration")
#include "GUFieldPropagatorPool.h"
#include "GUFieldPropagator.h"
// #endif

#ifdef __INTEL_COMPILER
#include <immintrin.h>
#else
#include "mm_malloc.h"
#endif
#include <cassert>

#ifdef GEANTV_MIC
 typedef std::string TString;
#endif
namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {

namespace host_constant {
#ifdef USE_VECGEOM_NAVIGATOR
const double gTolerance = vecgeom::kTolerance;
#else
const double gTolerance = TGeoShape::Tolerance();
#endif
}

//______________________________________________________________________________
GeantTrack::GeantTrack()
    : fEvent(-1), fEvslot(-1), fParticle(-1), fPDG(0), fGVcode(0), fEindex(0), fCharge(0), fProcess(-1), fVindex(0),
      fNsteps(0), fSpecies(kHadron), fStatus(kAlive), fMass(0), fXpos(0), fYpos(0), fZpos(0), fXdir(0), fYdir(0),
      fZdir(0), fP(0), fE(0), fTime(0), fEdep(0), fPstep(1.E20), fStep(0), fSnext(0), fSafety(0), fBoundary(false),
      fPending(false), fPath(0), fNextpath(0) {
  // Dummy constructor
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
GeantTrack::GeantTrack(int ipdg)
    : fEvent(-1), fEvslot(-1), fParticle(-1), fPDG(ipdg), fGVcode(0), fEindex(0), fCharge(0), fProcess(-1), fVindex(0),
      fNsteps(0), fSpecies(kHadron), fStatus(kAlive), fMass(0), fXpos(0), fYpos(0), fZpos(0), fXdir(0), fYdir(0),
      fZdir(0), fP(0), fE(0), fTime(0), fEdep(0), fPstep(1.E20), fStep(0), fSnext(0), fSafety(0), fBoundary(false),
      fPending(false), fPath(0), fNextpath(0) {
  // Constructor
  int maxdepth = GeantPropagator::Instance()->fMaxDepth;
  fPath = VolumePath_t::MakeInstance(maxdepth);
  fNextpath = VolumePath_t::MakeInstance(maxdepth);
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
GeantTrack::GeantTrack(int ipdg, int maxdepth)
    : fEvent(-1), fEvslot(-1), fParticle(-1), fPDG(ipdg), fGVcode(0), fEindex(0), fCharge(0), fProcess(-1), fVindex(0),
      fNsteps(0), fSpecies(kHadron), fStatus(kAlive), fMass(0), fXpos(0), fYpos(0), fZpos(0), fXdir(0), fYdir(0),
      fZdir(0), fP(0), fE(0), fTime(0), fEdep(0), fPstep(1.E20), fStep(0), fSnext(0), fSafety(0), fBoundary(false),
      fPending(false), fPath(0), fNextpath(0) {
  // Constructor
  fPath = VolumePath_t::MakeInstance(maxdepth);
  fNextpath = VolumePath_t::MakeInstance(maxdepth);
}

//______________________________________________________________________________
GeantTrack::GeantTrack(const GeantTrack &other)
    : fEvent(other.fEvent), fEvslot(other.fEvslot), fParticle(other.fParticle), fPDG(other.fPDG),
      fGVcode(other.fGVcode), fEindex(other.fEindex), fCharge(other.fCharge), fProcess(other.fProcess),
      fVindex(other.fVindex), fNsteps(other.fNsteps), fSpecies(other.fSpecies), fStatus(other.fStatus),
      fMass(other.fMass), fXpos(other.fXpos), fYpos(other.fYpos), fZpos(other.fZpos), fXdir(other.fXdir),
      fYdir(other.fYdir), fZdir(other.fZdir), fP(other.fP), fE(other.fE), fTime(other.fTime), fEdep(other.fEdep),
      fPstep(other.fPstep), fStep(other.fStep), fSnext(other.fSnext), fSafety(other.fSafety), fBoundary(other.fBoundary),
      fPending(other.fPending), fPath(0), fNextpath(0) {
  // Copy constructor
  int maxdepth = GeantPropagator::Instance()->fMaxDepth;
  fPath = VolumePath_t::MakeInstance(maxdepth);
  fNextpath = VolumePath_t::MakeInstance(maxdepth);
  *fPath = *other.fPath;
  *fNextpath = *other.fNextpath;
}

//______________________________________________________________________________
GeantTrack &GeantTrack::operator=(const GeantTrack &other) {
  // Assignment
  if (&other != this) {
    fEvent = other.fEvent;
    fEvslot = other.fEvslot;
    fParticle = other.fParticle;
    fPDG = other.fPDG;
    fGVcode = other.fGVcode;
    fEindex = other.fEindex;
    fCharge = other.fCharge;
    fProcess = other.fProcess;
    fVindex = other.fVindex;
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
    fBoundary = other.fBoundary;
    fPending = other.fPending;
    int maxdepth = GeantPropagator::Instance()->fMaxDepth;
    fPath = VolumePath_t::MakeInstance(maxdepth);
    fNextpath = VolumePath_t::MakeInstance(maxdepth);
    *fPath = *other.fPath;
    *fNextpath = *other.fNextpath;
  }
  return *this;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
GeantTrack::~GeantTrack() {
  // Destructor.
  VolumePath_t::ReleaseInstance(fPath);
  VolumePath_t::ReleaseInstance(fNextpath);
}

//______________________________________________________________________________
void GeantTrack::ReadFromVector(const GeantTrack_v &arr, int i) {
  // Fill track from array
  fEvent = arr.fEventV[i];
  fEvslot = arr.fEvslotV[i];
  fParticle = arr.fParticleV[i];
  fPDG = arr.fPDGV[i];
  fGVcode = arr.fGVcodeV[i];
  fEindex = arr.fEindexV[i];
  fCharge = arr.fChargeV[i];
  fProcess = arr.fProcessV[i];
  fVindex = arr.fVindexV[i];
  fNsteps = arr.fNstepsV[i];
  fSpecies = arr.fSpeciesV[i];
  fStatus = arr.fStatusV[i];
  fMass = arr.fMassV[i];
  fXpos = arr.fXposV[i];
  fYpos = arr.fYposV[i];
  fZpos = arr.fZposV[i];
  fXdir = arr.fXdirV[i];
  fYdir = arr.fYdirV[i];
  fZdir = arr.fZdirV[i];
  fP = arr.fPV[i];
  fE = arr.fEV[i];
  fTime = arr.fTimeV[i];
  fEdep = arr.fEdepV[i];
  fPstep = arr.fPstepV[i];
  fStep = arr.fStepV[i];
  fSnext = arr.fSnextV[i];
  fSafety = arr.fSafetyV[i];
  fBoundary = arr.fBoundaryV[i];
  fPending = arr.fPendingV[i];
  //   if (!fPath) fPath = wm->NavStates()->borrow();
  *fPath = *arr.fPathV[i];
  //   if (!fNextpath) fNextpath = wm->NavStates()->borrow();
  *fNextpath = *arr.fNextpathV[i];
}

//______________________________________________________________________________
void GeantTrack::Clear(const char *) {
  // Resets track content.
  fEvent = -1;
  fEvslot = -1;
  fParticle = -1;
  fPDG = 0;
  fGVcode = 0;
  fEindex = 0;
  fCharge = 0;
  fProcess = -1;
  fVindex = 0;
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
  fBoundary = false;
  fPending = false;
}

//______________________________________________________________________________
double GeantTrack::Curvature() const {
  // Curvature
  if (fCharge == 0)
    return 0.;
  const double tiny = 1.E-30;
  return fabs(kB2C * fCharge * gPropagator->fBmag / (Pt() + tiny));
}

//______________________________________________________________________________
Volume_t const*GeantTrack::GetVolume() const {
#ifdef USE_VECGEOM_NAVIGATOR
// The check below not needed anymore - everything got from path
//  assert(fVindexV[i] == fPathV[i]->Top()->GetLogicalVolume()->id());
  return fPath->Top()->GetLogicalVolume();
#else
  // TODO: get rid of fVindexV
  return (fPath->GetCurrentNode()->GetVolume());
#endif
}

//______________________________________________________________________________
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
bool GeantTrack::IsNormalized(double tolerance) const {
  // Check if track direction is normalized within tolerance
  double norm = fXdir * fXdir + fYdir * fYdir + fZdir * fZdir;
  if (fabs(1. - norm) > tolerance)
    return false;
  return true;
}

//______________________________________________________________________________
void GeantTrack::SetPath(VolumePath_t const *const path) {
  // Set path.
  *fPath = *path;
}

//______________________________________________________________________________
void GeantTrack::SetNextPath(VolumePath_t const *const path) {
  // Set next path.
  *fNextpath = *path;
}

//______________________________________________________________________________
void GeantTrack::Print(const char *location) const {
//  TString spath;
  //   if (path) path->GetPath(spath);
  Geant::Print(location, "=== Track %d (ev=%d): Process=%d, pstep=%g Charge=%d  Position:(%f,%f,%f) Dir:(%f,%f,%f) P:%g E:%g snext=%g safety=%g nsteps=%d",
         fParticle, fEvent, fProcess, fPstep, fCharge, fXpos, fYpos, fZpos, fXdir, fYdir, fZdir, P(), fE, fSnext,
         fSafety, fNsteps);
}

//______________________________________________________________________________
GeantTrack_v::GeantTrack_v()
    : fNtracks(0), fNselected(0), fCompact(true), fMixed(false), fMaxtracks(0), fHoles(0), fSelected(0), fMaxDepth(0),
      fBufSize(0), fVPstart(0), fBuf(0), fEventV(0), fEvslotV(0), fParticleV(0), fPDGV(0), fGVcodeV(0), fEindexV(0),
      fChargeV(0), fProcessV(0), fVindexV(0), fNstepsV(0), fSpeciesV(0), fStatusV(0), fMassV(0), fXposV(0), fYposV(0),
      fZposV(0), fXdirV(0), fYdirV(0), fZdirV(0), fPV(0), fEV(0), fTimeV(0), fEdepV(0), fPstepV(0), fStepV(0),
      fSnextV(0), fSafetyV(0), fBoundaryV(0), fPendingV(0), fPathV(0), fNextpathV(0) {
  // Dummy ctor.
}

//______________________________________________________________________________
GeantTrack_v::GeantTrack_v(int size, int maxdepth)
    : fNtracks(0), fNselected(0), fCompact(true), fMixed(false), fMaxtracks(0), fHoles(0), fSelected(0),
      fMaxDepth(maxdepth), fBufSize(0), fVPstart(0), fBuf(0), fEventV(0), fEvslotV(0), fParticleV(0), fPDGV(0),
      fGVcodeV(0), fEindexV(0), fChargeV(0), fProcessV(0), fVindexV(0), fNstepsV(0), fSpeciesV(0), fStatusV(0),
      fMassV(0), fXposV(0), fYposV(0), fZposV(0), fXdirV(0), fYdirV(0), fZdirV(0), fPV(0), fEV(0), fTimeV(0), fEdepV(0),
      fPstepV(0), fStepV(0), fSnextV(0), fSafetyV(0), fBoundaryV(0), fPendingV(0), fPathV(0), fNextpathV(0) {
  // Constructor with maximum capacity.
  Resize(size);
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
GeantTrack_v *GeantTrack_v::MakeInstanceAt(void *addr, unsigned int nTracks, int maxdepth) {
  return new (addr) GeantTrack_v(addr, nTracks, maxdepth);
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
GeantTrack_v::GeantTrack_v(void *addr, unsigned int nTracks, int maxdepth)
    : fNtracks(0), fNselected(0), fCompact(true), fMixed(false), fMaxtracks(round_up_align(nTracks)), fHoles(0),
      fSelected(0), fMaxDepth(maxdepth), fBufSize(0), fVPstart(0), fBuf(0), fEventV(0), fEvslotV(0), fParticleV(0),
      fPDGV(0), fGVcodeV(0), fEindexV(0), fChargeV(0), fProcessV(0), fVindexV(0), fNstepsV(0), fSpeciesV(0),
      fStatusV(0), fMassV(0), fXposV(0), fYposV(0), fZposV(0), fXdirV(0), fYdirV(0), fZdirV(0), fPV(0), fEV(0),
      fTimeV(0), fEdepV(0), fPstepV(0), fStepV(0), fSnextV(0), fSafetyV(0), fBoundaryV(0), fPendingV(0), fPathV(0),
      fNextpathV(0) {

  // Constructor with maximum capacity.
  fBuf = ((char *)addr) + round_up_align(sizeof(GeantTrack_v));
  fBufSize = BufferSize(nTracks, maxdepth);
  memset(fBuf, 0, fBufSize);
  AssignInBuffer(fBuf, nTracks);
  memset(fPathV, 0, nTracks * sizeof(VolumePath_t *));
  memset(fNextpathV, 0, nTracks * sizeof(VolumePath_t *));
}

//______________________________________________________________________________
GeantTrack_v::GeantTrack_v(const GeantTrack_v &track_v)
    : fNtracks(0), fNselected(track_v.fNselected), fCompact(track_v.fCompact), fMixed(track_v.fMixed),
      fMaxtracks(track_v.fMaxtracks), fHoles(0), fSelected(0), fMaxDepth(track_v.fMaxDepth), fBufSize(track_v.fBufSize),
      fVPstart(0), fBuf(0), fEventV(0), fEvslotV(0), fParticleV(0), fPDGV(0), fGVcodeV(0), fEindexV(0), fChargeV(0),
      fProcessV(0), fVindexV(0), fNstepsV(0), fSpeciesV(0), fStatusV(0), fMassV(0), fXposV(0), fYposV(0), fZposV(0),
      fXdirV(0), fYdirV(0), fZdirV(0), fPV(0), fEV(0), fTimeV(0), fEdepV(0), fPstepV(0), fStepV(0), fSnextV(0),
      fSafetyV(0), fBoundaryV(0), fPendingV(0), fPathV(0), fNextpathV(0) {
// Copy constructor
#ifndef GEANT_CUDA_DEVICE_BUILD
  fNtracks.store(track_v.fNtracks);
#else
  fNtracks = track_v.fNtracks;
#endif
  fBuf = (char *)_mm_malloc(fBufSize, GEANT_ALIGN_PADDING);
  memcpy(fBuf, track_v.fBuf, fBufSize);
  AssignInBuffer(&fBuf[0], fMaxtracks);
}

//______________________________________________________________________________
GeantTrack_v &GeantTrack_v::operator=(const GeantTrack_v &track_v) {
  // Assignment operator
  if (&track_v != this) {
#ifndef GEANT_CUDA_DEVICE_BUILD
    fNtracks.store(track_v.fNtracks);
#else
    fNtracks = track_v.fNtracks;
#endif
    int size = track_v.fMaxtracks;
    fMaxDepth = track_v.fMaxDepth;
    fBufSize = track_v.fBufSize;
    if (fMaxtracks < size) {
      _mm_free(fBuf);
      fBuf = (char *)_mm_malloc(fBufSize, GEANT_ALIGN_PADDING);
    }
    fMaxtracks = size;
    fNselected = track_v.fNselected;
    fHoles = 0;
    fSelected = 0;
    fCompact = track_v.fCompact;
    fMixed = track_v.fMixed;
    memcpy(fBuf, track_v.fBuf, size * sizeof(GeantTrack));
    AssignInBuffer(&fBuf[0], size);
  }
  return *this;
}

//______________________________________________________________________________
GeantTrack_v::~GeantTrack_v() {
  // Destructor.
  int ntracks = GetNtracks();
  for (auto i = 0; i < ntracks; ++i) {
    VolumePath_t::ReleaseInstance(fPathV[i]);
    VolumePath_t::ReleaseInstance(fNextpathV[i]);
  }
  _mm_free(fBuf);
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
void GeantTrack_v::AssignInBuffer(char *buff, int size) {
  // Assign all internal class arrays in the supplied buffer, padded by supplied
  // size.

  const int size_intn = size * sizeof(int);
  const int size_doublen = size * sizeof(double);
  const int size_booln = size * sizeof(bool);
  char *buf = buff;
  fEventV = (int *)buf;
  buf += size_intn;
  fEvslotV = (int *)buf;
  buf += size_intn;
  fParticleV = (int *)buf;
  buf += size_intn;
  fPDGV = (int *)buf;
  buf += size_intn;
  fGVcodeV = (int *)buf;
  buf += size_intn;
  fEindexV = (int *)buf;
  buf += size_intn;
  fChargeV = (int *)buf;
  buf += size_intn;
  fProcessV = (int *)buf;
  buf += size_intn;
  fVindexV = (int *)buf;
  buf += size_intn;
  fNstepsV = (int *)buf;
  buf += size_intn;
  fSpeciesV = (Species_t *)buf;
  buf += size * sizeof(Species_t);
  fStatusV = (TrackStatus_t *)buf;
  buf += size * sizeof(TrackStatus_t);
  fMassV = (double *)buf;
  buf += size_doublen;
  fXposV = (double *)buf;
  buf += size_doublen;
  fYposV = (double *)buf;
  buf += size_doublen;
  fZposV = (double *)buf;
  buf += size_doublen;
  fXdirV = (double *)buf;
  buf += size_doublen;
  fYdirV = (double *)buf;
  buf += size_doublen;
  fZdirV = (double *)buf;
  buf += size_doublen;
  fPV = (double *)buf;
  buf += size_doublen;
  fEV = (double *)buf;
  buf += size_doublen;
  fTimeV = (double *)buf;
  buf += size_doublen;
  fEdepV = (double *)buf;
  buf += size_doublen;
  fPstepV = (double *)buf;
  buf += size_doublen;
  fStepV = (double *)buf;
  buf += size_doublen;
  fSnextV = (double *)buf;
  buf += size_doublen;
  fSafetyV = (double *)buf;
  buf += size_doublen;
  fBoundaryV = (bool *)buf;
  buf += size_booln;
  fPendingV = (bool *)buf;
  buf += size_booln;
  fPathV = (VolumePath_t **)buf;
  buf += size * sizeof(VolumePath_t *);
  fNextpathV = (VolumePath_t **)buf;
  buf += size * sizeof(VolumePath_t *);

  // Now the start of objects, we need to align the memory.
  buf = round_up_align(buf);
  fVPstart = buf;
  size_t size_vpath = VolumePath_t::SizeOfInstance(fMaxDepth);
  // Allocate VolumePath_t objects in the reserved buffer space
  for (auto i = 0; i < 2 * size; ++i)
    VolumePath_t::MakeInstanceAt(fMaxDepth, buf + i * size_vpath);
  buf += 2 * size * size_vpath;

  // Now the start of objects, we need to align the memory.
  buf = round_up_align(buf);
  size_t size_bits = BitSet::SizeOfInstance(size);
  fHoles = BitSet::MakeInstanceAt(size, buf);
  buf += size_bits;

  // Now the start of objects, we need to align the memory.
  buf = round_up_align(buf);
  fSelected = BitSet::MakeInstanceAt(size, buf);
}

//______________________________________________________________________________
void GeantTrack_v::CopyToBuffer(char *buff, int size) {
  // Copy existing track arrays into new buffer, padded by supplied size
  int ntracks = GetNtracks();
  const int size_int = ntracks * sizeof(int);
  const int size_double = ntracks * sizeof(double);
  const int size_intn = size * sizeof(int);
  const int size_doublen = size * sizeof(double);
  char *buf = buff;
  memcpy(buf, fEventV, size_int);
  fEventV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fEvslotV, size_int);
  fEvslotV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fParticleV, size_int);
  fParticleV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fPDGV, size_int);
  fPDGV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fGVcodeV, size_int);
  fGVcodeV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fEindexV, size_int);
  fEindexV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fChargeV, size_int);
  fChargeV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fProcessV, size_int);
  fProcessV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fVindexV, size_int);
  fVindexV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fNstepsV, size_int);
  fNstepsV = (int *)buf;
  buf += size_intn;
  memcpy(buf, fSpeciesV, ntracks * sizeof(Species_t));
  fSpeciesV = (Species_t *)buf;
  buf += size * sizeof(Species_t);
  memcpy(buf, fStatusV, ntracks * sizeof(TrackStatus_t));
  fStatusV = (TrackStatus_t *)buf;
  buf += size * sizeof(TrackStatus_t);
  memcpy(buf, fMassV, size_double);
  fMassV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fXposV, size_double);
  fXposV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fYposV, size_double);
  fYposV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fZposV, size_double);
  fZposV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fXdirV, size_double);
  fXdirV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fYdirV, size_double);
  fYdirV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fZdirV, size_double);
  fZdirV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fPV, size_double);
  fPV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fEV, size_double);
  fEV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fTimeV, size_double);
  fTimeV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fEdepV, size_double);
  fEdepV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fPstepV, size_double);
  fPstepV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fStepV, size_double);
  fStepV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fSnextV, size_double);
  fSnextV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fSafetyV, size_double);
  fSafetyV = (double *)buf;
  buf += size_doublen;
  memcpy(buf, fBoundaryV, ntracks * sizeof(bool));
  fBoundaryV = (bool *)buf;
  buf += size * sizeof(bool);
  memcpy(buf, fPendingV, ntracks * sizeof(bool));
  fPendingV = (bool *)buf;
  buf += size * sizeof(bool);
  //   memcpy(buf, fPathV, ntracks*sizeof(VolumePath_t*));
  VolumePath_t **pathV = (VolumePath_t **)buf;
  buf += size * sizeof(VolumePath_t *);
  //   memcpy(buf, fNextpathV, ntracks*sizeof(VolumePath_t*));
  VolumePath_t **nextpathV = (VolumePath_t **)buf;
  buf += size * sizeof(VolumePath_t *);

  // Now the start of objects, we need to align the memory.
  buf = round_up_align(buf);
  fVPstart = buf;
  size_t size_vpath = VolumePath_t::SizeOfInstance(fMaxDepth);
  // Allocate VolumePath_t objects in the reserved buffer space
  for (auto i = 0; i < 2 * size; ++i)
    VolumePath_t::MakeInstanceAt(fMaxDepth, fVPstart + i * size_vpath);
  // Copy existing path and nextpath into new buffer
  for (auto i = 0; i < ntracks; ++i) {
    pathV[i] = reinterpret_cast<VolumePath_t *>(fVPstart + i * size_vpath);
    nextpathV[i] = reinterpret_cast<VolumePath_t *>(fVPstart + (size + i) * size_vpath);
    fPathV[i]->CopyTo(pathV[i]);
    fNextpathV[i]->CopyTo(nextpathV[i]);
    VolumePath_t::ReleaseInstance(fPathV[i]);
    VolumePath_t::ReleaseInstance(fNextpathV[i]);
  }
  // Set the new pointers to arrays
  fPathV = pathV;
  fNextpathV = nextpathV;
  buf += 2 * size * size_vpath;

  // Now the start of objects, we need to align the memory.
  buf = round_up_align(buf);
  size_t size_bits = BitSet::SizeOfInstance(size);
  BitSet *holes = BitSet::MakeCopyAt(*fHoles, buf);
  BitSet::ReleaseInstance(fHoles);
  fHoles = holes;
  buf += size_bits;

  // Now the start of objects, we need to align the memory.
  buf = round_up_align(buf);
  BitSet *selected = BitSet::MakeInstanceAt(size, buf);
  BitSet::ReleaseInstance(fSelected);
  fSelected = selected;
}

//______________________________________________________________________________
bool GeantTrack_v::IsSame(const GeantTrack_v &tr1, int i1, const GeantTrack_v &tr2, int i2) {
  // Compare two tracks.
  Long64_t chk1, chk2;
  chk1 = tr1.fEventV[i1] + tr1.fEvslotV[i1] + tr1.fParticleV[i1] + tr1.fPDGV[i1] + tr1.fGVcodeV[i1] + tr1.fEindexV[i1] +
         tr1.fChargeV[i1] + tr1.fProcessV[i1] + tr1.fVindexV[i1] + tr1.fNstepsV[i1] + (Long64_t)tr1.fSpeciesV[i1] +
         (Long64_t)tr1.fStatusV[i1];
  chk2 = tr2.fEventV[i2] + tr2.fEvslotV[i2] + tr2.fParticleV[i2] + tr2.fPDGV[i2] + tr2.fGVcodeV[i2] + tr2.fEindexV[i2] +
         tr2.fChargeV[i2] + tr2.fProcessV[i2] + tr2.fVindexV[i2] + tr2.fNstepsV[i2] + (Long64_t)tr2.fSpeciesV[i2] +
         (Long64_t)tr2.fStatusV[i2];
  if (chk1 != chk2)
    return false;
  double dchk1, dchk2;
  dchk1 = (Long64_t)tr1.fMassV[i1] + tr1.fXposV[i1] + tr1.fYposV[i1] + tr1.fZposV[i1] + tr1.fXdirV[i1] +
          tr1.fYdirV[i1] + tr1.fZdirV[i1] + tr1.fPV[i1] + tr1.fEdepV[i1] + tr1.fEV[i1] + tr1.fPstepV[i1] +
          tr1.fStepV[i1] + tr1.fSnextV[i1] + tr1.fSafetyV[i1];
  dchk2 = (Long64_t)tr2.fMassV[i2] + tr2.fXposV[i2] + tr2.fYposV[i2] + tr2.fZposV[i2] + tr2.fXdirV[i2] +
          tr2.fYdirV[i2] + tr2.fZdirV[i2] + tr2.fPV[i2] + tr2.fEdepV[i2] + tr2.fEV[i2] + tr2.fPstepV[i2] +
          tr2.fStepV[i2] + tr2.fSnextV[i2] + tr2.fSafetyV[i2];
  if (!Math::AreEqualAbs(dchk1, dchk2, 1.E-10))
    return false;
  if (tr1.fPendingV[i1] != tr2.fPendingV[i2])
    return false;
  return true;
}

//______________________________________________________________________________
bool GeantTrack_v::IsNormalized(int itr, double tolerance) const {
  // Check if track direction is normalized within tolerance
  double norm = fXdirV[itr] * fXdirV[itr] + fYdirV[itr] * fYdirV[itr] + fZdirV[itr] * fZdirV[itr];
  if (fabs(1. - norm) > tolerance)
    return false;
  return true;
}

//______________________________________________________________________________
void GeantTrack_v::CheckTracks() {
  //  for (int i=0; i<fMaxtracks; ++i)
  //     if (fNextpathV[i]) fNextpathV[i]->SetClient(gPropagator);
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
size_t GeantTrack_v::BufferSize(size_t nTracks, size_t maxdepth) {
  // return the contiguous memory size needed to hold a GeantTrack_v's data

  size_t size = round_up_align(nTracks); // When called internally this ought to be a nop
  size_t size_nav = 2 * size * VolumePath_t::SizeOfInstance(maxdepth);
  // NOTE: Most likely the 'real' problem here is that BitSet::SizeOfInstance return
  // a number that is as small as possible rather than a number that is usuable to
  // be able to make array of BitSet.
  size_t size_bits = 2 * round_up_align(BitSet::SizeOfInstance(size));

  // Since we already round nTracks, we only need to round the last two
  // to have the proper space for object alignment
  return size * sizeof(GeantTrack) + round_up_align(size_nav) + size_bits;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
size_t GeantTrack_v::SizeOfInstance(size_t nTracks, size_t maxdepth) {
  // return the contiguous memory size needed to hold a GeantTrack_v

  return round_up_align(sizeof(GeantTrack_v))+BufferSize(nTracks,maxdepth);
}

//______________________________________________________________________________
void GeantTrack_v::Resize(int newsize) {
  // Resize the container.
  int size = round_up_align(newsize);
  if (size < GetNtracks()) {
    Geant::Error("Resize","Cannot resize to less than current track content");
    return;
  }
  fBufSize = BufferSize(size, fMaxDepth);
  if (!fCompact)
    Compact();

  char *buf = (char *)_mm_malloc(fBufSize, GEANT_ALIGN_PADDING);
  memset(buf, 0, fBufSize);
  fMaxtracks = size;
  if (!fBuf) {
    // All arrays are contiguous in a single buffer and aligned with the
    // same padding GEANT_ALIGN_PADDING
    fBuf = buf;
    AssignInBuffer(buf, size);
    memset(fPathV, 0, size * sizeof(VolumePath_t *));
    memset(fNextpathV, 0, size * sizeof(VolumePath_t *));
  } else {
    // Resize container
    CopyToBuffer(buf, size);
    _mm_free(fBuf);
    fBuf = buf;
  }
  fHoles->ResetAllBits();
  fSelected->ResetAllBits();
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
int GeantTrack_v::AddTrack(GeantTrack &track, bool /*import*/) {
  // Add new track to the array. If addition is done on top of non-compact array,
  // the track will be inserted without updating the number of tracks. If track is
  // imported just copy the pointers to the navigation states and reset the sources.
  // Returns the location where the track was added.
  int itrack = GetNtracks();
  if (!fCompact)
    itrack = fHoles->FirstSetBit();
  if (itrack == fMaxtracks) {
#ifndef GEANT_CUDA_DEVICE_BUILD
    Resize(2 * fMaxtracks);
#else
    printf("Error in GeantTrack_v::AddTrack, resizing is not supported in device code\n");
#endif
  }
  fHoles->ResetBitNumber(itrack);
  fSelected->ResetBitNumber(itrack);
  fEventV[itrack] = track.fEvent;
  fEvslotV[itrack] = track.fEvslot;
  fParticleV[itrack] = track.fParticle;
  fPDGV[itrack] = track.fPDG;
  fGVcodeV[itrack] = track.fGVcode;
  fEindexV[itrack] = track.fEindex;
  fChargeV[itrack] = track.fCharge;
  fProcessV[itrack] = track.fProcess;
  fVindexV[itrack] = track.fVindex;
  fNstepsV[itrack] = track.fNsteps;
  fSpeciesV[itrack] = track.fSpecies;
  fStatusV[itrack] = track.fStatus;
  fMassV[itrack] = track.fMass;
  fXposV[itrack] = track.fXpos;
  fYposV[itrack] = track.fYpos;
  fZposV[itrack] = track.fZpos;
  fXdirV[itrack] = track.fXdir;
  fYdirV[itrack] = track.fYdir;
  fZdirV[itrack] = track.fZdir;
  fPV[itrack] = track.fP;
  fEV[itrack] = track.fE;
  fTimeV[itrack] = track.fTime;
  fEdepV[itrack] = track.fEdep;
  fPstepV[itrack] = track.fPstep;
  fStepV[itrack] = track.fStep;
  fSnextV[itrack] = track.fSnext;
  fSafetyV[itrack] = track.fSafety;
  fBoundaryV[itrack] = track.fBoundary;
  fPendingV[itrack] = track.fPending;
  // Copy the volume paths
  size_t size_vpath = VolumePath_t::SizeOfInstance(fMaxDepth);
  fPathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + itrack * size_vpath);
  track.fPath->CopyTo(fPathV[itrack]);
  fNextpathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + (fMaxtracks + itrack) * size_vpath);
  track.fNextpath->CopyTo(fNextpathV[itrack]);
  fNtracks++;
  return itrack;
}

//______________________________________________________________________________
int GeantTrack_v::AddTrackSync(GeantTrack &track) {
  // Add track in a concurrent way. Assumes that this array
  // Is currently being filled while held by the basket manager and NOT being
  // transported.
  // The array has to be compact and should have enough alocated space.
  // Returns the location where the track was added.
  assert(fCompact);
  assert(GetNtracks() < fMaxtracks);
  int itrack = fNtracks++;
  fEventV[itrack] = track.fEvent;
  fEvslotV[itrack] = track.fEvslot;
  fParticleV[itrack] = track.fParticle;
  fPDGV[itrack] = track.fPDG;
  fGVcodeV[itrack] = track.fGVcode;
  fEindexV[itrack] = track.fEindex;
  fChargeV[itrack] = track.fCharge;
  fProcessV[itrack] = track.fProcess;
  fVindexV[itrack] = track.fVindex;
  fNstepsV[itrack] = track.fNsteps;
  fSpeciesV[itrack] = track.fSpecies;
  fStatusV[itrack] = track.fStatus;
  fMassV[itrack] = track.fMass;
  fXposV[itrack] = track.fXpos;
  fYposV[itrack] = track.fYpos;
  fZposV[itrack] = track.fZpos;
  fXdirV[itrack] = track.fXdir;
  fYdirV[itrack] = track.fYdir;
  fZdirV[itrack] = track.fZdir;
  fPV[itrack] = track.fP;
  fEV[itrack] = track.fE;
  fTimeV[itrack] = track.fTime;
  fEdepV[itrack] = track.fEdep;
  fPstepV[itrack] = track.fPstep;
  fStepV[itrack] = track.fStep;
  fSnextV[itrack] = track.fSnext;
  fSafetyV[itrack] = track.fSafety;
  fBoundaryV[itrack] = track.fBoundary;
  fPendingV[itrack] = track.fPending;
  // Copy the volume paths
  size_t size_vpath = VolumePath_t::SizeOfInstance(fMaxDepth);
  fPathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + itrack * size_vpath);
  track.fPath->CopyTo(fPathV[itrack]);
  fNextpathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + (fMaxtracks + itrack) * size_vpath);
  track.fNextpath->CopyTo(fNextpathV[itrack]);
  return itrack;
}

//______________________________________________________________________________
void GeantTrack_v::GetTrack(int i, GeantTrack &track) const {
  // Extract a single track from array.
  track.ReadFromVector(*this, i);
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
int GeantTrack_v::AddTrack(GeantTrack_v &arr, int i, bool /*import*/) {
// Add track from different array
// If addition is done on top of non-compact array,
// the track will be inserted without updating the number of tracks.
// Returns the location where the track was added.
#ifdef VERBOSE
  arr.PrintTrack(i);
#endif
  int itrack = GetNtracks();
  if (!fCompact)
    itrack = fHoles->FirstSetBit();
  if (itrack == fMaxtracks) {
#ifndef GEANT_CUDA_DEVICE_BUILD
    Resize(2 * fMaxtracks);
#else
    printf("Error in GeantTrack_v::AddTrack, resizing is not supported in device code\n");
#endif
  }
  fHoles->ResetBitNumber(itrack);
  fSelected->ResetBitNumber(itrack);

  fEventV[itrack] = arr.fEventV[i];
  fEvslotV[itrack] = arr.fEvslotV[i];
  fParticleV[itrack] = arr.fParticleV[i];
  fPDGV[itrack] = arr.fPDGV[i];
  fGVcodeV[itrack] = arr.fGVcodeV[i];
  fEindexV[itrack] = arr.fEindexV[i];
  fChargeV[itrack] = arr.fChargeV[i];
  fProcessV[itrack] = arr.fProcessV[i];
  fVindexV[itrack] = arr.fVindexV[i];
  fNstepsV[itrack] = arr.fNstepsV[i];
  fSpeciesV[itrack] = arr.fSpeciesV[i];
  fStatusV[itrack] = arr.fStatusV[i];
  fMassV[itrack] = arr.fMassV[i];
  fXposV[itrack] = arr.fXposV[i];
  fYposV[itrack] = arr.fYposV[i];
  fZposV[itrack] = arr.fZposV[i];
  fXdirV[itrack] = arr.fXdirV[i];
  fYdirV[itrack] = arr.fYdirV[i];
  fZdirV[itrack] = arr.fZdirV[i];
  fPV[itrack] = arr.fPV[i];
  fEV[itrack] = arr.fEV[i];
  fTimeV[itrack] = arr.fTimeV[i];
  fEdepV[itrack] = arr.fEdepV[i];
  fPstepV[itrack] = arr.fPstepV[i];
  fStepV[itrack] = arr.fStepV[i];
  fSnextV[itrack] = arr.fSnextV[i];
  fSafetyV[itrack] = arr.fSafetyV[i];
  fBoundaryV[itrack] = arr.fBoundaryV[i];
  fPendingV[itrack] = arr.fPendingV[i];
  // Copy the volume paths
  size_t size_vpath = VolumePath_t::SizeOfInstance(fMaxDepth);
  fPathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + itrack * size_vpath);
  arr.fPathV[i]->CopyTo(fPathV[itrack]);
  fNextpathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + (fMaxtracks + itrack) * size_vpath);
  arr.fNextpathV[i]->CopyTo(fNextpathV[itrack]);
  fNtracks++;
  return itrack;
}

//______________________________________________________________________________
int GeantTrack_v::AddTrackSync(GeantTrack_v &arr, int i) {
  // Add track from different array in a concurrent way. Assumes that this array
  // Is currently being filled while held by the basket manager and NOT being
  // transported.
  // The array has to be compact and should have enough alocated space.
  // Returns the location where the track was added.
  assert(fCompact);
  assert(GetNtracks() < fMaxtracks);
#ifdef VERBOSE
  arr.PrintTrack(i);
#endif
  // WorkloadManager *wm = WorkloadManager::Instance();
  int itrack = fNtracks++;

  fEventV[itrack] = arr.fEventV[i];
  fEvslotV[itrack] = arr.fEvslotV[i];
  fParticleV[itrack] = arr.fParticleV[i];
  fPDGV[itrack] = arr.fPDGV[i];
  fGVcodeV[itrack] = arr.fGVcodeV[i];
  fEindexV[itrack] = arr.fEindexV[i];
  fChargeV[itrack] = arr.fChargeV[i];
  fProcessV[itrack] = arr.fProcessV[i];
  fVindexV[itrack] = arr.fVindexV[i];
  fNstepsV[itrack] = arr.fNstepsV[i];
  fSpeciesV[itrack] = arr.fSpeciesV[i];
  fStatusV[itrack] = arr.fStatusV[i];
  fMassV[itrack] = arr.fMassV[i];
  fXposV[itrack] = arr.fXposV[i];
  fYposV[itrack] = arr.fYposV[i];
  fZposV[itrack] = arr.fZposV[i];
  fXdirV[itrack] = arr.fXdirV[i];
  fYdirV[itrack] = arr.fYdirV[i];
  fZdirV[itrack] = arr.fZdirV[i];
  fPV[itrack] = arr.fPV[i];
  fEV[itrack] = arr.fEV[i];
  fTimeV[itrack] = arr.fTimeV[i];
  fEdepV[itrack] = arr.fEdepV[i];
  fPstepV[itrack] = arr.fPstepV[i];
  fStepV[itrack] = arr.fStepV[i];
  fSnextV[itrack] = arr.fSnextV[i];
  fSafetyV[itrack] = arr.fSafetyV[i];
  fBoundaryV[itrack] = arr.fBoundaryV[i];
  fPendingV[itrack] = arr.fPendingV[i];
  // Copy the volume paths
  size_t size_vpath = VolumePath_t::SizeOfInstance(fMaxDepth);
  fPathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + itrack * size_vpath);
  arr.fPathV[i]->CopyTo(fPathV[itrack]);
  fNextpathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + (fMaxtracks + itrack) * size_vpath);
  arr.fNextpathV[i]->CopyTo(fNextpathV[itrack]);
  return itrack;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
int GeantTrack_v::AddTrackSyncAt(int itrack, GeantTrack_v &arr, int i) {
  // Add track from different array in a concurrent way. Assumes that this array
  // Is currently being filled while held by the basket manager and NOT being
  // transported.
  // The array has to be compact and should have enough alocated space.
  // Returns the location where the track was added.
  // This *assumes* that no track has been yet added to this array slot.
  // During the use of this routines (likely on a coprocessor where multiple
  // threads are accessing the same GeantTrack_v, the GeantTrack_v is in
  // a somewhat unsual state, where the array is not compact but fHole
  // is not maintain properly (See non concurrent save code comment out
  // below.

  assert(itrack < fMaxtracks);
#ifdef VERBOSE
  arr.PrintTrack(i);
#endif

  // Technically, after setting fHoles to all on,
  // we really should be doing:
  //   fHoles->ResetBitNumber(itrack);
  //   fSelected->ResetBitNumber(itrack);
  // which use bit operation which are not (yet?)
  // done atomically,
  // and we should do:
  //   atomically: fNtracks = max(fNtracks,itrack)

#ifdef GEANT_CUDA_DEVICE_BUILD
  atomicAdd(&fNtracks, 1);
#else
  ++fNtracks;
#endif

  fEventV[itrack] = arr.fEventV[i];
  fEvslotV[itrack] = arr.fEvslotV[i];
  fParticleV[itrack] = arr.fParticleV[i];
  fPDGV[itrack] = arr.fPDGV[i];
  fGVcodeV[itrack] = arr.fGVcodeV[i];
  fEindexV[itrack] = arr.fEindexV[i];
  fChargeV[itrack] = arr.fChargeV[i];
  fProcessV[itrack] = arr.fProcessV[i];
  fVindexV[itrack] = arr.fVindexV[i];
  fNstepsV[itrack] = arr.fNstepsV[i];
  fSpeciesV[itrack] = arr.fSpeciesV[i];
  fStatusV[itrack] = arr.fStatusV[i];
  fMassV[itrack] = arr.fMassV[i];
  fXposV[itrack] = arr.fXposV[i];
  fYposV[itrack] = arr.fYposV[i];
  fZposV[itrack] = arr.fZposV[i];
  fXdirV[itrack] = arr.fXdirV[i];
  fYdirV[itrack] = arr.fYdirV[i];
  fZdirV[itrack] = arr.fZdirV[i];
  fPV[itrack] = arr.fPV[i];
  fEV[itrack] = arr.fEV[i];
  fTimeV[itrack] = arr.fTimeV[i];
  fEdepV[itrack] = arr.fEdepV[i];
  fPstepV[itrack] = arr.fPstepV[i];
  fStepV[itrack] = arr.fStepV[i];
  fSnextV[itrack] = arr.fSnextV[i];
  fSafetyV[itrack] = arr.fSafetyV[i];
  fBoundaryV[itrack] = arr.fBoundaryV[i];
  fPendingV[itrack] = arr.fPendingV[i];
  // Copy the volume paths
  size_t size_vpath = VolumePath_t::SizeOfInstance(fMaxDepth);
  fPathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + itrack * size_vpath);
  arr.fPathV[i]->CopyTo(fPathV[itrack]);
  fNextpathV[itrack] = reinterpret_cast<VolumePath_t *>(fVPstart + (fMaxtracks + itrack) * size_vpath);
  arr.fNextpathV[i]->CopyTo(fNextpathV[itrack]);
  return itrack;
}

//______________________________________________________________________________
void GeantTrack_v::AddTracks(GeantTrack_v &arr, int istart, int iend, bool /*import*/) {
  // Add tracks from different array. Single thread at a time.
  int ncpy = iend - istart + 1;
  int ntracks = GetNtracks();
  if (ntracks + ncpy >= fMaxtracks) {
    Resize(Math::Max<double>(2 * fMaxtracks, ntracks + ncpy));
  }
  memcpy(&fEventV[ntracks], &arr.fEventV[istart], ncpy * sizeof(int));
  memcpy(&fEvslotV[ntracks], &arr.fEvslotV[istart], ncpy * sizeof(int));
  memcpy(&fParticleV[ntracks], &arr.fParticleV[istart], ncpy * sizeof(int));
  memcpy(&fPDGV[ntracks], &arr.fPDGV[istart], ncpy * sizeof(int));
  memcpy(&fGVcodeV[ntracks], &arr.fGVcodeV[istart], ncpy * sizeof(int));
  memcpy(&fEindexV[ntracks], &arr.fEindexV[istart], ncpy * sizeof(int));
  memcpy(&fChargeV[ntracks], &arr.fChargeV[istart], ncpy * sizeof(int));
  memcpy(&fProcessV[ntracks], &arr.fProcessV[istart], ncpy * sizeof(int));
  memcpy(&fVindexV[ntracks], &arr.fVindexV[istart], ncpy * sizeof(int));
  memcpy(&fNstepsV[ntracks], &arr.fNstepsV[istart], ncpy * sizeof(int));
  memcpy(&fSpeciesV[ntracks], &arr.fSpeciesV[istart], ncpy * sizeof(Species_t));
  memcpy(&fStatusV[ntracks], &arr.fStatusV[istart], ncpy * sizeof(TrackStatus_t));
  memcpy(&fMassV[ntracks], &arr.fMassV[istart], ncpy * sizeof(double));
  memcpy(&fXposV[ntracks], &arr.fXposV[istart], ncpy * sizeof(double));
  memcpy(&fYposV[ntracks], &arr.fYposV[istart], ncpy * sizeof(double));
  memcpy(&fZposV[ntracks], &arr.fZposV[istart], ncpy * sizeof(double));
  memcpy(&fXdirV[ntracks], &arr.fXdirV[istart], ncpy * sizeof(double));
  memcpy(&fYdirV[ntracks], &arr.fYdirV[istart], ncpy * sizeof(double));
  memcpy(&fZdirV[ntracks], &arr.fZdirV[istart], ncpy * sizeof(double));
  memcpy(&fPV[ntracks], &arr.fPV[istart], ncpy * sizeof(double));
  memcpy(&fEV[ntracks], &arr.fEV[istart], ncpy * sizeof(double));
  memcpy(&fTimeV[ntracks], &arr.fTimeV[istart], ncpy * sizeof(double));
  memcpy(&fEdepV[ntracks], &arr.fEdepV[istart], ncpy * sizeof(double));
  memcpy(&fPstepV[ntracks], &arr.fPstepV[istart], ncpy * sizeof(double));
  memcpy(&fStepV[ntracks], &arr.fStepV[istart], ncpy * sizeof(double));
  memcpy(&fSnextV[ntracks], &arr.fSnextV[istart], ncpy * sizeof(double));
  memcpy(&fSafetyV[ntracks], &arr.fSafetyV[istart], ncpy * sizeof(double));
  memcpy(&fBoundaryV[ntracks], &arr.fBoundaryV[istart], ncpy * sizeof(bool));
  memcpy(&fPendingV[ntracks], &arr.fPendingV[istart], ncpy * sizeof(bool));

  size_t size_vpath = VolumePath_t::SizeOfInstance(fMaxDepth);
  for (auto i = ntracks, j = istart; i < (ntracks + ncpy); ++i, ++j) {
    fPathV[i] = reinterpret_cast<VolumePath_t *>(fVPstart + i * size_vpath);
    fNextpathV[i] = reinterpret_cast<VolumePath_t *>(fVPstart + (fMaxtracks + i) * size_vpath);
    arr.fPathV[j]->CopyTo(fPathV[i]);
    arr.fNextpathV[j]->CopyTo(fNextpathV[i]);
  }
  fSelected->ResetBitNumber(ntracks + ncpy - 1);
  fHoles->ResetBitNumber(ntracks + ncpy - 1);
  fNtracks += ncpy;
}

//______________________________________________________________________________
void GeantTrack_v::SwapTracks(int i, int j) {
  // Swap two tracks in the container
  double tdbl;
  int tint;
  bool tbool;
  VolumePath_t *tptr;
  tint = fEventV[i];
  fEventV[i] = fEventV[j];
  fEventV[j] = tint;
  tint = fEvslotV[i];
  fEvslotV[i] = fEvslotV[j];
  fEvslotV[j] = tint;
  tint = fParticleV[i];
  fParticleV[i] = fParticleV[j];
  fParticleV[j] = tint;
  tint = fPDGV[i];
  fPDGV[i] = fPDGV[j];
  fPDGV[j] = tint;
  tint = fGVcodeV[i];
  fGVcodeV[i] = fGVcodeV[j];
  fGVcodeV[j] = tint;
  tint = fEindexV[i];
  fEindexV[i] = fEindexV[j];
  fEindexV[j] = tint;
  tint = fChargeV[i];
  fChargeV[i] = fChargeV[j];
  fChargeV[j] = tint;
  tint = fProcessV[i];
  fProcessV[i] = fProcessV[j];
  fProcessV[j] = tint;
  tint = fVindexV[i];
  fVindexV[i] = fVindexV[j];
  fVindexV[j] = tint;
  tint = fNstepsV[i];
  fNstepsV[i] = fNstepsV[j];
  fNstepsV[j] = tint;
  Species_t tspec = fSpeciesV[i];
  fSpeciesV[i] = fSpeciesV[j];
  fSpeciesV[j] = tspec;
  TrackStatus_t tstat = fStatusV[i];
  fStatusV[i] = fStatusV[j];
  fStatusV[j] = tstat;
  tdbl = fMassV[i];
  fMassV[i] = fMassV[j];
  fMassV[j] = tdbl;
  tdbl = fXposV[i];
  fXposV[i] = fXposV[j];
  fXposV[j] = tdbl;
  tdbl = fYposV[i];
  fYposV[i] = fYposV[j];
  fYposV[j] = tdbl;
  tdbl = fZposV[i];
  fZposV[i] = fZposV[j];
  fZposV[j] = tdbl;
  tdbl = fXdirV[i];
  fXdirV[i] = fXdirV[j];
  fXdirV[j] = tdbl;
  tdbl = fYdirV[i];
  fYdirV[i] = fYdirV[j];
  fYdirV[j] = tdbl;
  tdbl = fZdirV[i];
  fZdirV[i] = fZdirV[j];
  fZdirV[j] = tdbl;
  tdbl = fPV[i];
  fPV[i] = fPV[j];
  fPV[j] = tdbl;
  tdbl = fEV[i];
  fEV[i] = fEV[j];
  fEV[j] = tdbl;
  tdbl = fTimeV[i];
  fTimeV[i] = fTimeV[j];
  fTimeV[j] = tdbl;
  tdbl = fEdepV[i];
  fEdepV[i] = fEdepV[j];
  fEdepV[j] = tdbl;
  tdbl = fPstepV[i];
  fPstepV[i] = fPstepV[j];
  fPstepV[j] = tdbl;
  tdbl = fStepV[i];
  fStepV[i] = fStepV[j];
  fStepV[j] = tdbl;
  tdbl = fSnextV[i];
  fSnextV[i] = fSnextV[j];
  fSnextV[j] = tdbl;
  tdbl = fSafetyV[i];
  fSafetyV[i] = fSafetyV[j];
  fSafetyV[j] = tdbl;
  tbool = fBoundaryV[i];
  fBoundaryV[i] = fBoundaryV[j];
  fBoundaryV[j] = tbool;
  tbool = fPendingV[i];
  fPendingV[i] = fPendingV[j];
  fPendingV[j] = tbool;
  tptr = fPathV[i];
  fPathV[i] = fPathV[j];
  fPathV[j] = tptr;
  tptr = fNextpathV[i];
  fNextpathV[i] = fNextpathV[j];
  fNextpathV[j] = tptr;
  bool sel = fSelected->TestBitNumber(j);
  fSelected->SetBitNumber(j, fSelected->TestBitNumber(i));
  fSelected->SetBitNumber(i, sel);
}

//______________________________________________________________________________
void GeantTrack_v::ReplaceTrack(int i, int j) {
  // Replace content of track i with the one of track j
  // WorkloadManager *wm = WorkloadManager::Instance();
  fEventV[i] = fEventV[j];
  fEvslotV[i] = fEvslotV[j];
  fParticleV[i] = fParticleV[j];
  fPDGV[i] = fPDGV[j];
  fGVcodeV[i] = fGVcodeV[j];
  fEindexV[i] = fEindexV[j];
  fChargeV[i] = fChargeV[j];
  fProcessV[i] = fProcessV[j];
  fVindexV[i] = fVindexV[j];
  fNstepsV[i] = fNstepsV[j];
  fSpeciesV[i] = fSpeciesV[j];
  fStatusV[i] = fStatusV[j];
  fMassV[i] = fMassV[j];
  fXposV[i] = fXposV[j];
  fYposV[i] = fYposV[j];
  fZposV[i] = fZposV[j];
  fXdirV[i] = fXdirV[j];
  fYdirV[i] = fYdirV[j];
  fZdirV[i] = fZdirV[j];
  fPV[i] = fPV[j];
  fEV[i] = fEV[j];
  fTimeV[i] = fTimeV[j];
  fEdepV[i] = fEdepV[j];
  fPstepV[i] = fPstepV[j];
  fStepV[i] = fStepV[j];
  fSnextV[i] = fSnextV[j];
  fSafetyV[i] = fSafetyV[j];
  fBoundaryV[i] = fBoundaryV[j];
  fPendingV[i] = fPendingV[j];
  //   if (!fPathV[i]) fPathV[i] = wm->NavStates()->Borrow();
  //   if (!fNextpathV[i]) fNextpathV[i] = wm->NavStates()->Borrow();
  fPathV[i] = fPathV[j];         // fPathV[j] = 0;
  fNextpathV[i] = fNextpathV[j]; // fNextpathV[j] = 0;
  fSelected->SetBitNumber(i, fSelected->TestBitNumber(j));
}

//______________________________________________________________________________
void GeantTrack_v::DeleteTrack(int /*itr*/) {
  // Delete branch arrays for this track. The track should not have a copy, this has
  // to be called after a killed track is removed by the scheduler.
  //   WorkloadManager *wm = WorkloadManager::Instance();
  //   wm->NavStates()->release(fPathV[itr]);
  //   fPathV[itr] = 0;
  //   wm->NavStates()->release(fNextpathV[itr]);
  //   fNextpathV[itr] = 0;
}

//______________________________________________________________________________
void GeantTrack_v::RemoveTracks(int from, int to) {
// Remove tracks from the container. The method assumes that the tracks were
// copied to another container beforehand.
#ifndef GEANT_CUDA_DEVICE_BUILD
  if (!fCompact)
    Geant::Error("RemoveTracks","Not compact");
#endif
  int ntracks = GetNtracks();
  if (to >= ntracks - 1) {
    int nzero = ntracks - from;
    memset(&fPathV[from], 0, nzero * sizeof(VolumePath_t *));
    memset(&fNextpathV[from], 0, nzero * sizeof(VolumePath_t *));
  }
  int ncpy = fNtracks - to - 1;
  memmove(&fEventV[from], &fEventV[to + 1], ncpy * sizeof(int));
  memmove(&fEvslotV[from], &fEvslotV[to + 1], ncpy * sizeof(int));
  memmove(&fParticleV[from], &fParticleV[to + 1], ncpy * sizeof(int));
  memmove(&fPDGV[from], &fPDGV[to + 1], ncpy * sizeof(int));
  memmove(&fGVcodeV[from], &fGVcodeV[to + 1], ncpy * sizeof(int));
  memmove(&fEindexV[from], &fEindexV[to + 1], ncpy * sizeof(int));
  memmove(&fChargeV[from], &fChargeV[to + 1], ncpy * sizeof(int));
  memmove(&fProcessV[from], &fProcessV[to + 1], ncpy * sizeof(int));
  memmove(&fVindexV[from], &fVindexV[to + 1], ncpy * sizeof(int));
  memmove(&fNstepsV[from], &fNstepsV[to + 1], ncpy * sizeof(int));
  memmove(&fSpeciesV[from], &fSpeciesV[to + 1], ncpy * sizeof(Species_t));
  memmove(&fStatusV[from], &fStatusV[to + 1], ncpy * sizeof(TrackStatus_t));
  memmove(&fMassV[from], &fMassV[to + 1], ncpy * sizeof(double));
  memmove(&fXposV[from], &fXposV[to + 1], ncpy * sizeof(double));
  memmove(&fYposV[from], &fYposV[to + 1], ncpy * sizeof(double));
  memmove(&fZposV[from], &fZposV[to + 1], ncpy * sizeof(double));
  memmove(&fXdirV[from], &fXdirV[to + 1], ncpy * sizeof(double));
  memmove(&fYdirV[from], &fYdirV[to + 1], ncpy * sizeof(double));
  memmove(&fZdirV[from], &fZdirV[to + 1], ncpy * sizeof(double));
  memmove(&fPV[from], &fPV[to + 1], ncpy * sizeof(double));
  memmove(&fEV[from], &fEV[to + 1], ncpy * sizeof(double));
  memmove(&fTimeV[from], &fTimeV[to + 1], ncpy * sizeof(double));
  memmove(&fEdepV[from], &fEdepV[to + 1], ncpy * sizeof(double));
  memmove(&fPstepV[from], &fPstepV[to + 1], ncpy * sizeof(double));
  memmove(&fStepV[from], &fStepV[to + 1], ncpy * sizeof(double));
  memmove(&fSnextV[from], &fSnextV[to + 1], ncpy * sizeof(double));
  memmove(&fSafetyV[from], &fSafetyV[to + 1], ncpy * sizeof(double));
  memmove(&fBoundaryV[from], &fBoundaryV[to + 1], ncpy * sizeof(bool));
  memmove(&fPendingV[from], &fPendingV[to + 1], ncpy * sizeof(bool));
  memmove(&fPathV[from], &fPathV[to + 1], ncpy * sizeof(VolumePath_t *));
  memmove(&fNextpathV[from], &fNextpathV[to + 1], ncpy * sizeof(VolumePath_t *));
  fNtracks -= to - from + 1;
  fSelected->ResetAllBits();
  fNselected = 0;
}

//______________________________________________________________________________
int GeantTrack_v::Compact(GeantTrack_v *moveto) {
  // Compact the holes in the array. Return number of active elements. This will
  // lose the track fields in the holes, so information from the holes has to be
  // copied beforehand
  int ntracks = GetNtracks();
  if (ntracks == 0 || fCompact)
    return 0;
  fCompact = true;
  int firsthole = fHoles->FirstSetBit();
  while (firsthole < ntracks) {
    int lastactive = fHoles->LastNullBit(ntracks - 1);
    if (lastactive < ntracks) {
      // move last holes (if any)
      if (moveto && (ntracks - lastactive - 1 > 0))
        moveto->AddTracks(*this, lastactive + 1, ntracks - 1, true);
      ntracks = lastactive + 1;
      if (firsthole == ntracks) {
        SetNtracks(ntracks);
        return ntracks;
      }
    } else {
      // No active tracks left. First copy the hole track to the output
      if (moveto)
        moveto->AddTracks(*this, firsthole, firsthole + ntracks - 1, true);
      SetNtracks(0);
      return 0;
    }
    // replace content of first hole with the last active track
    if (moveto)
      moveto->AddTrack(*this, firsthole, true);
    ReplaceTrack(firsthole, lastactive);
    fHoles->SetBitNumber(firsthole, false);
    fHoles->SetBitNumber(lastactive, true);
    firsthole = fHoles->FirstSetBit(firsthole + 1);
    ntracks--;
  }
  fSelected->ResetAllBits();
  fNselected = 0;
  SetNtracks(ntracks);
  return ntracks;
}

//______________________________________________________________________________
int GeantTrack_v::Reshuffle() {
  // Reshuffle tracks according the selection mask. The selected tracks will be
  // moved at the beginning of the array. Tracks should be compacted before.
  if (GetNtracks() == 0)
    return 0;
  fNselected = GetNtracks();
  int firsthole = fSelected->FirstNullBit();
  while (firsthole < fNselected) {
    int lastsel = fSelected->LastSetBit(fNselected - 1);
    if (lastsel >= fNselected)
      return 0;
    fNselected = lastsel + 1;
    if (firsthole == fNselected)
      return fNselected;
    // exchange tracks pointed by firsthole and lastactive
    SwapTracks(firsthole, lastsel);
    fSelected->SetBitNumber(firsthole, true);
    fSelected->SetBitNumber(lastsel, false);
    firsthole = fSelected->FirstNullBit(firsthole + 1);
    fNselected--;
  }
  return fNselected;
}

//______________________________________________________________________________
bool GeantTrack_v::Contains(int evstart, int nevents) const {
  // Check if the array contains tracks from a given event range
  int evend = evstart + nevents;
  int ntracks = GetNtracks();
  for (int itr = 0; itr < ntracks; itr++) {
    if (fEventV[itr] >= evstart && fEventV[itr] < evend)
      return true;
  }
  return false;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
void GeantTrack_v::Clear(const char *) {
  // Clear track content and selections
  fNselected = 0;
  int ntracks = GetNtracks();
  if (ntracks) {
    memset(fPathV, 0, ntracks * sizeof(VolumePath_t *));
    memset(fNextpathV, 0, ntracks * sizeof(VolumePath_t *));
  }
  fHoles->ResetAllBits();
  fSelected->ResetAllBits();
  fCompact = true;
  SetNtracks(0);
}

//______________________________________________________________________________
int GeantTrack_v::PropagateStraight(int ntracks, double *crtstep) {
  // Propagate first ntracks along a straight line (neutral particles, no mag.
  // field or for last tiny step). The array has to be reshuffled after selecting
  // the interesting particles using Select method.
  // The crossing tracks get masked as holes in the array.

  // Find next volume
  int icrossed = 0;
  for (int i = 0; i < ntracks; i++) {
    if (fBoundaryV[i]) {
      //*fPathV[i] = *fNextpathV[i];
      fStatusV[i] = kBoundary;
      icrossed++;
    }
  }
  for (int i = 0; i < ntracks; i++) {
    fPstepV[i] -= crtstep[i];
    fSafetyV[i] = 0;
    // Change path to reflect the physical volume for the current track; The
    // relevant path is fPath[i] if the frombdr flag is not set or fNextpath[i]
    // otherwise
    fXposV[i] += crtstep[i] * fXdirV[i];
    fYposV[i] += crtstep[i] * fYdirV[i];
    fZposV[i] += crtstep[i] * fZdirV[i];
#ifdef USE_VECGEOM_NAVIGATOR
//      CheckLocationPathConsistency(i);
#endif
  }
  return icrossed;
}

//______________________________________________________________________________
void GeantTrack_v::PropagateInVolume(int ntracks, const double *crtstep, GeantTaskData *td) {
  // Propagate the selected tracks with crtstep values. The method is to be called
  // only with  charged tracks in magnetic field. The method decreases the fPstepV
  // fSafetyV and fSnextV with the propagated values while increasing the fStepV.
  // The status and boundary flags are set according to which gets hit first:
  // - physics step (bdr=0)
  // - safety step (bdr=0)
  // - snext step (bdr=1)
  for (int i = 0; i < ntracks; i++) {
    PropagateInVolumeSingle(i, crtstep[i], td);
  }
}


//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
void GeantTrack_v::PropagateInVolumeSingle(int i, double crtstep, GeantTaskData * td) {
  // Propagate the selected track with crtstep value. The method is to be called
  // only with  charged tracks in magnetic field.The method decreases the fPstepV
  // fSafetyV and fSnextV with the propagated values while increasing the fStepV.
  // The status and boundary flags are set according to which gets hit first:
  // - physics step (bdr=0)
  // - safety step (bdr=0)
  // - snext step (bdr=1)

   // Double_t c = 0.;
   // const Double_t *point = 0;
   // const Double_t *newdir = 0;

   bool useRungeKutta;
#ifdef GEANT_CUDA_DEVICE_BUILD
   const double bmag = gPropagator_fBmag;
   constexpr auto gPropagator_fUseRK = false; // Temporary work-around until actual implementation ..
   useRungeKutta= gPropagator_fUseRK;   //  Something like this is needed - TBD
#else
   const double bmag = gPropagator->fBmag;
   useRungeKutta= gPropagator->fUseRungeKutta;
#endif

   // static unsigned long icount= 0;
   // if( icount++ < 2 )  std::cout << " PropagateInVolumeSingle: useRungeKutta= " << useRungeKutta << std::endl;

// #ifdef RUNGE_KUTTA
   GUFieldPropagator *fieldPropagator = nullptr;
#ifndef GEANT_CUDA_DEVICE_BUILD
   if( useRungeKutta ){
      // Initialize for the current thread -- move to GeantPropagator::Initialize()
      static GUFieldPropagatorPool* fieldPropPool= GUFieldPropagatorPool::Instance();
      assert( fieldPropPool );

      fieldPropagator = fieldPropPool->GetPropagator(td->fTid);
      assert( fieldPropagator );
   }
#endif

  // Reset relevant variables
  fStatusV[i] = kInFlight;
  fPstepV[i] -= crtstep;
  if (fPstepV[i] < 1.E-10) {
    fPstepV[i] = 0;
    fStatusV[i] = kPhysics;
  }
  fSafetyV[i] -= crtstep;
  if (fSafetyV[i] < 1.E-10)
    fSafetyV[i] = 0;
  fSnextV[i] -= crtstep;
  if (fSnextV[i] < 1.E-10) {
    fSnextV[i] = 0;
    if (fBoundaryV[i]) {
      fStatusV[i] = kBoundary;
    }
  }
  fStepV[i] += crtstep;
#ifdef USE_VECGEOM_NAVIGATOR
//  CheckLocationPathConsistency(i);
#endif
// alternative code with lean stepper would be:
// ( stepper header has to be included )

  using ThreeVector = vecgeom::Vector3D<double>;
  // typedef vecgeom::Vector3D<double>  ThreeVector;   
  ThreeVector Position(fXposV[i], fYposV[i], fZposV[i]);
  ThreeVector Direction(fXdirV[i], fYdirV[i], fZdirV[i]);
  ThreeVector PositionNew(0.,0.,0.);
  ThreeVector DirectionNew(0.,0.,0.);

  if( useRungeKutta ) {
#ifndef GEANT_NVCC
     fieldPropagator->DoStep(Position,    Direction,    fChargeV[i], fPV[i], crtstep,
                             PositionNew, DirectionNew);
#endif
  } else {
     // Old - constant field
     Geant::ConstBzFieldHelixStepper stepper(bmag);
     stepper.DoStep<ThreeVector,double,int>(Position,    Direction,    fChargeV[i], fPV[i], crtstep,
                                         PositionNew, DirectionNew);
  }

  fXposV[i] = PositionNew.x();
  fYposV[i] = PositionNew.y();
  fZposV[i] = PositionNew.z();

  //  maybe normalize direction here  // Math::Normalize(dirnew);
  DirectionNew = DirectionNew.Unit();   
  fXdirV[i] = DirectionNew.x();
  fYdirV[i] = DirectionNew.y();
  fZdirV[i] = DirectionNew.z();

#if 0
  ThreeVector SimplePosition = Position + crtstep * Direction;
  // double diffpos2 = (PositionNew - Position).Mag2();
  double diffpos2 = (PositionNew - SimplePosition).Mag2();
  //   -- if (Math::Sqrt(diffpos)>0.01*crtstep) {     
  const double drift= 0.01*crtstep;
  if ( diffpos2>drift*drift ){
      double diffpos= Math::Sqrt(diffpos2);
      // Geant::Print("PropagateInVolumeSingle","relative difference in pos = %g", diffpos/crtstep);
      Geant::Print("PropagateInVolumeSingle","difference in pos = %g (abs) %g (relative) , step= %g",
                   diffpos, diffpos/crtstep, crtstep);
  }
#endif
}

#ifdef USE_VECGEOM_NAVIGATOR
void GeantTrack_v::CheckLocationPathConsistency(int itr) const {
  VECGEOM_NAMESPACE::NavigationState *a =
      VECGEOM_NAMESPACE::NavigationState::MakeInstance(VECGEOM_NAMESPACE::GeoManager::Instance().getMaxDepth());
  a->Clear();
  VECGEOM_NAMESPACE::SimpleNavigator nav;
  nav.LocatePoint(VECGEOM_NAMESPACE::GeoManager::Instance().GetWorld(),
                  VECGEOM_NAMESPACE::Vector3D<VECGEOM_NAMESPACE::Precision>(fXposV[itr], fYposV[itr], fZposV[itr]), *a,
                  true);
  if (a->Top() != NULL && a->Top() != fPathV[itr]->Top()) {
    Geant::Print("","INCONSISTENT LOCATION PATH PAIR PRODUCED FOR TRACK %d", itr);
#ifdef VECGEOM_ROOT
    Geant::Print("","REAL");
    a->GetCurrentNode()->Print();
    Geant::Print("","REPORTED");
    fPathV[itr]->GetCurrentNode()->Print();
//  printrace();
#endif
  }

  // release object
  VECGEOM_NAMESPACE::NavigationState::ReleaseInstance(a);
}
#endif

#ifdef USE_VECGEOM_NAVIGATOR
GEANT_CUDA_BOTH_CODE
void GeantTrack_v::NavFindNextBoundaryAndStep(int ntracks, const double *pstep, const double *x, const double *y,
                                              const double *z, const double *dirx, const double *diry,
                                              const double *dirz, VolumePath_t **pathin, VolumePath_t **pathout,
                                              double *step, double *safe, bool *isonbdr, const GeantTrack_v * /*trk*/) {
  // Printf("In vec find next boundary and step\n");
  using VECGEOM_NAMESPACE::SimpleNavigator;
  using VECGEOM_NAMESPACE::ABBoxNavigator;
  using VECGEOM_NAMESPACE::Precision;
  using VECGEOM_NAMESPACE::Vector3D;
  using VECGEOM_NAMESPACE::GeoManager;
  typedef Vector3D<Precision> Vector3D_t;

  //     VolumePath_t * a = new VolumePath_t( GeoManager::Instance().getMaxDepth() );

#ifdef GEANT_NVCC
  SimpleNavigator nav;
#else
  ABBoxNavigator nav;
#endif
  for (int i = 0; i < ntracks; ++i) {
    // Check if current safety allows for the proposed step
    if (safe[i] > pstep[i]) {
      step[i] = pstep[i];
      isonbdr[i] = false;
      continue;
    }
#ifdef VERBOSE
    if (pstep[i] < 0.) {
      std::cerr << " NEGATIVE PSTEP " << pstep[i] << "\n";
    }
#endif

    //    	a->Clear();
    //    	nav.LocatePoint( GeoManager::Instance().GetWorld(),
    //    			Vector3D_t( x[i], y[i], z[i] ), *a, true );
    //        if( a->Top() != NULL && a->Top() != pathin[i]->Top() )
    //         {
    //             Printf("INCONSISTENT PATH TRACK %d, boundary state %d", i, isonbdr[i] );
    //             a->GetCurrentNode()->Print();
    //             pathin[i]->GetCurrentNode()->Print();
    //             Printf("environment supposed path" );
    //             nav.InspectEnvironmentForPointAndDirection(
    //                             Vector3D_t( x[i], y[i], z[i] )  /*global pos */,
    //                             Vector3D_t( dirx[i], diry[i], dirz[i] )  /*global dir*/ ,
    //                             *pathin[i]);
    //             Printf( "environment reported path" );
    //             nav.InspectEnvironmentForPointAndDirection(
    //                                         Vector3D_t( x[i], y[i], z[i] )  /*global pos*/ ,
    //                                         Vector3D_t( dirx[i], diry[i], dirz[i] )  /*global
    //                                         dir*/ ,
    //                                         *a);
    //         }

    //      assert( a->Top() == pathin[i]->Top() );
    nav.FindNextBoundaryAndStep(Vector3D_t(x[i], y[i], z[i]) /* global pos */,
                                Vector3D_t(dirx[i], diry[i], dirz[i]) /* global dir */, *pathin[i],
                                *pathout[i] /* the paths */, Math::Min<double>(1.E20, pstep[i]), step[i]);
    step[i] = Math::Max<double>(2. * gTolerance, step[i] + 2. * gTolerance);
    safe[i] = (isonbdr[i]) ? 0 : nav.GetSafety(Vector3D_t(x[i], y[i], z[i]), *pathin[i]);
    safe[i] = Math::Max<double>(safe[i], 0);

#ifdef CROSSCHECK
    //************
    // CROSS CHECK USING TGEO
    //************
    TGeoNavigator *rootnav = gGeoManager->GetCurrentNavigator();
    rootnav->ResetState();
    rootnav->SetCurrentPoint(x[i], y[i], z[i]);
    rootnav->SetCurrentDirection(dirx[i], diry[i], dirz[i]);
    TGeoBranchArray *tmp = pathin[i]->ToTGeoBranchArray();
    tmp->UpdateNavigator(rootnav);
    delete tmp;
    rootnav->FindNextBoundaryAndStep(Math::Min<double>(1.E20, pstep[i]), !isonbdr[i]);
    double stepcmp = Math::Max<double>(2 * gTolerance, rootnav->GetStep());
    double safecmp = rootnav->GetSafeDistance();
    // pathin[i]->GetCurrentNode()->Print();
    // Printf("## PSTEP %lf VECGEOMSTEP %lf ROOTSTEP %lf", pstep[i], step[i], stepcmp);
    // Printf("## PSTEP %lf ONBOUND %d VECGEOMSAFETY %lf ROOTSAFETY %lf BRUTEFORCEROOT %lf",
    // pstep[i],
    //       isonbdr[i], safe[i], rootnav->Safety());

    // check nextpath
    tmp = pathout[i]->ToTGeoBranchArray();
    tmp->InitFromNavigator(rootnav);
    // Printf("## VECGEOMNEXTNODE %p ROOTNEXTNODE %p", pathout[i]->GetCurrentNode(),
    // tmp->GetCurrentNode());
    // Printf("## VECGEOMBOUNDARY %d ROOTBOUNDARY %d", pathout[i]->IsOnBoundary(),
    // rootnav->IsOnBoundary());

    // if( safe[i] != safecmp )
    // {
    //    nav.InspectSafetyForPoint(
    //                               Vector3D_t( x[i], y[i], z[i] )  /*global pos*/,
    //                               *pathin[i] );
    //}
    if (Math::Abs(step[i] - stepcmp) > 1E-6) {
      Geant::Print("","## PSTEP %lf VECGEOMSTEP %lf ROOTSTEP %lf", pstep[i], step[i], stepcmp);
      Geant::Print("","## PSTEP %lf ONBOUND %d VECGEOMSAFETY %lf ROOTSAFETY %lf BRUTEFORCEROOT %lf", pstep[i], isonbdr[i],
             safe[i], rootnav->Safety());

      // check nextpath
      tmp = pathout[i]->ToTGeoBranchArray();
      tmp->InitFromNavigator(rootnav);
      Geant::Print("","## VECGEOMNEXTNODE %p ROOTNEXTNODE %p", pathout[i]->GetCurrentNode(), tmp->GetCurrentNode());
      Geant::Print("","## VECGEOMBOUNDARY %d ROOTBOUNDARY %d", pathout[i]->IsOnBoundary(), rootnav->IsOnBoundary());

      Geant::Print("","INCONSISTENT STEP");
      nav.InspectEnvironmentForPointAndDirection(Vector3D_t(x[i], y[i], z[i]) /*global pos*/,
                                                 Vector3D_t(dirx[i], diry[i], dirz[i]), *pathin[i]);
    }
//    if( pathout[i]->IsOnBoundary() != rootnav->IsOnBoundary() )
//          {
//           Printf("INCONSISTENT BOUNDARY");
//           Printf("## PSTEP %lf VECGEOMSTEP %lf ROOTSTEP %lf", pstep[i], step[i], stepcmp);
//                 Printf("## PSTEP %lf ONBOUND %d VECGEOMSAFETY %lf ROOTSAFETY %lf BRUTEFORCEROOT
//                 %lf", pstep[i],
//                            isonbdr[i], safe[i], rootnav->Safety());
//
//                     // check nextpath
//                     tmp = pathout[i]->ToTGeoBranchArray();
//                     tmp->InitFromNavigator( rootnav );
//                     Printf("## VECGEOMNEXTNODE %p ROOTNEXTNODE %p", pathout[i]->GetCurrentNode(),
//                     tmp->GetCurrentNode());
//                    Printf("## VECGEOMBOUNDARY %d ROOTBOUNDARY %d", pathout[i]->IsOnBoundary(),
//                    rootnav->IsOnBoundary());
//
//           // nav.InspectEnvironmentForPointAndDirection(
//           //           Vector3D_t( x[i], y[i], z[i] )  /*global pos*/ ,
//            //          Vector3D_t( dirx[i], diry[i], dirz[i] ),
//             //                             *pathin[i] );
//          }
#endif
    // onboundary with respect to new point
    isonbdr[i] = pathout[i]->IsOnBoundary();

#ifdef VERBOSE
    Geant::Print("","navfindbound on %p track %d with pstep %lf yields step %lf and safety %lf\n", this, i, pstep[i], step[i],
           safe[i]);
#endif
  }
  //   delete a;
}
//GEANT_CUDA_BOTH_CODE
//void GeantTrack_v::NavFindNextBoundaryAndStep_v(Int_t ntracks, const Double_t *pstep, const Double_t *x,
//                                              const Double_t *y, const Double_t *z, const Double_t *dirx,
//                                              const Double_t *diry, const Double_t *dirz, VolumePath_t **pathin,
//                                              VolumePath_t **pathout, Double_t *step, Double_t *safe, Bool_t *isonbdr,
//                                              const GeantTrack_v * /*trk*/,
//                                              GeantTaskData *td) {
//  // Printf("In vec find next boundary and step\n");
//  using VECGEOM_NAMESPACE::SimpleNavigator;
//  using VECGEOM_NAMESPACE::Precision;
//  using VECGEOM_NAMESPACE::Vector3D;
//  using VECGEOM_NAMESPACE::GeoManager;
//  using VECGEOM_NAMESPACE::SOA3D;
//  typedef Vector3D<Precision> Vector3D_t;
//  typedef SOA3D<double> SOA3D_t;
//
//  SimpleNavigator nav;
//  // TODO: vectorize loop
//  for (Int_t i = 0; i < ntracks; ++i) {
//    // Check if current safety allows for the proposed step
//    if (safe[i] > pstep[i]) {
//      step[i] = pstep[i];
//      isonbdr[i] = false;
//      continue;
//    }
//  }
//
//  // call the vector interface
//  // we need to transform our arrays to SOA
//   nav.FindNextBoundaryAndStep(SOA3D_t(const_cast<double*>(x),const_cast<double*>(y),const_cast<double*>(z),ntracks),
//                               SOA3D_t(const_cast<double*>(dirx),const_cast<double*>(diry),const_cast<double*>(dirz),ntracks),
//                               td->GetSOA3Dworkspace1(ntracks),
//                               td->GetSOA3Dworkspace2(ntracks),
//                               pathin,
//                               pathout,
//                               pstep,
//                               safe,
//                               step,
//                               td->GetIntArray(ntracks));
//
//  // rectify some parts ( as in the scalar case )
//  // TODO: vectorize loop
//  for (unsigned int i = 0; i < ntracks; ++i) {
//    step[i] = Math::Max<double>(2. * gTolerance, step[i] + 2. * gTolerance);
//    isonbdr[i] = pathout[i]->IsOnBoundary();
//  }
//}

#else
//______________________________________________________________________________
void GeantTrack_v::NavFindNextBoundaryAndStep(int ntracks, const double *pstep, const double *x, const double *y,
                                              const double *z, const double *dirx, const double *diry,
                                              const double *dirz, VolumePath_t **pathin, VolumePath_t **pathout,
                                              double *step, double *safe, bool *isonbdr, const GeantTrack_v * /*trk*/) {
  // Vector version of TGeo FNB (To be implemented the vectorized navigator)
  // Apply to all particles (charged or not).
  //    pstep = proposed steps by physics
  //    x,y,z, dirx,diry, dirz = initial positions and directions
  //    safety = safety values for the initial points
  //    step = distances to next boundary (or to physics step if closer)
  //    isonbdr = starting points on boundary flags (used to decide on safety computation)
  //              use also as output to notify if the step is boundary or physics
  //    pathin = starting paths
  //    pathout = final path after propagation to next boundary
  const double epserr = 1.E-3; // push value in case of repeated geom error
  TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
  int ismall;
  double snext;
  TGeoNode *nextnode, *lastnode;
  double pt[3];
#ifdef BUG_HUNT
  int index = (int)(x - trk->fXposV);
#endif // BUG_HUNT
  for (int itr = 0; itr < ntracks; itr++) {
#ifdef BUG_HUNT
    index += itr;
#endif // BUG_HUNT
    ismall = 0;
    step[itr] = 0;
    // Check if current safety allows for the proposed step
    if (safe[itr] > pstep[itr]) {
      step[itr] = pstep[itr];
      isonbdr[itr] = false;
      continue;
    }
    // Reset navigation state flags and safety to start fresh
    nav->ResetState();
    // Setup start state
    nav->SetCurrentPoint(x[itr], y[itr], z[itr]);
    nav->SetCurrentDirection(dirx[itr], diry[itr], dirz[itr]);
    pathin[itr]->UpdateNavigator(nav);
    nextnode = nav->GetCurrentNode();
    while (nextnode) {
      lastnode = nextnode;
      // Compute distance to next boundary and propagate internally
      nextnode = nav->FindNextBoundaryAndStep(Math::Min<double>(1.E20, pstep[itr]), !isonbdr[itr]);
      snext = nav->GetStep();
      // Adjust step to be non-negative and cross the boundary
      step[itr] = Math::Max<double>(2 * gTolerance, snext + 2 * gTolerance);
      // Check for repeated small steps starting from boundary
      if (isonbdr[itr] && (snext < 1.E-8) && (pstep[itr] > 1.E-8)) {
        ismall++;
        if ((ismall < 3) && (nextnode != lastnode)) {
          // Make sure we don't have a thin layer artefact so repeat search
          nextnode = nav->FindNextBoundaryAndStep(Math::Min<double>(1.E20, pstep[itr] - snext), !isonbdr[itr]);
          snext = nav->GetStep();
          step[itr] += snext;
          // If step still small, repeat
          if (snext < 1.E-8)
            continue;
          // We managed to cross with macroscopic step: reset error counter and exit loop
          ismall = 0;
          break;
        } else {
          if (ismall > 3) {
            // Mark track to be killed
            step[itr] = -1;
            break;
          }
          // The block below can only happen if crossing into the same node on different geometry
          // branch with small step. Try to relocate the next point by making an epserr push
          memcpy(pt, nav->GetCurrentPoint(), 3 * sizeof(double));
          const double *dir = gGeoManager->GetCurrentDirection();
          for (int j = 0; j < 3; j++)
            pt[j] += epserr * dir[j];
          step[itr] += epserr;
          nav->CdTop();
          nextnode = nav->FindNode(pt[0], pt[1], pt[2]);
          if (nav->IsOutside())
            break;
          continue;
        }
      }
      // All OK here, reset error counter and exit loop
      ismall = 0;
      break;
    }
    // Update safety, boundary flag and next path
    safe[itr] = isonbdr[itr] ? 0. : nav->GetSafeDistance();
    isonbdr[itr] = nav->IsOnBoundary();
    pathout[itr]->InitFromNavigator(nav);
#ifdef VERBOSE
    double bruteforces = nav->Safety();
    Geant::Print("","##TGEOM  ## TRACK %d BOUND %d PSTEP %lg STEP %lg SAFETY %lg BRUTEFORCES %lg TOBOUND %d", itr, isonbdr[itr],
           pstep[itr], step[itr], safe[itr], bruteforces, nav->IsOnBoundary());
// assert( safe[itr]<=bruteforces );
#endif // VERBOSE

#ifdef CROSSCHECK
    // crosscheck with what VECGEOM WOULD GIVE IN THIS SITUATION
    // ---------------------------------------------------------
    VECGEOM_NAMESPACE::NavigationState vecgeom_in_state(VECGEOM_NAMESPACE::GeoManager::Instance().getMaxDepth());
    VECGEOM_NAMESPACE::NavigationState vecgeom_out_state(VECGEOM_NAMESPACE::GeoManager::Instance().getMaxDepth());
    vecgeom_in_state = *pathin[itr];
    VECGEOM_NAMESPACE::SimpleNavigator vecnav;
    double vecgeom_step;
    typedef VECGEOM_NAMESPACE::Vector3D<VECGEOM_NAMESPACE::Precision> Vector3D_t;
    vecnav.FindNextBoundaryAndStep(Vector3D_t(x[itr], y[itr], z[itr]) /* global pos */,
                                   Vector3D_t(dirx[itr], diry[itr], dirz[itr]) /* global dir */, vecgeom_in_state,
                                   vecgeom_out_state /* the paths */, Math::Min<double>(1.E20, pstep[itr]), vecgeom_step);
    vecgeom_step = Math::Max<double>(2 * gTolerance, vecgeom_step);
    double vecgeom_safety;
    vecgeom_safety = vecnav.GetSafety(Vector3D_t(x[itr], y[itr], z[itr]), vecgeom_in_state);
    vecgeom_safety = (vecgeom_safety < 0) ? 0. : vecgeom_safety;
    Geant::Print("","--VECGEOM-- TRACK %d BOUND %d PSTEP %lg STEP %lg SAFETY %lg TOBOUND %d", itr, isonbdr[itr], pstep[itr],
           vecgeom_step, vecgeom_safety, vecgeom_out_state.IsOnBoundary());
// end crosscheck with what VECGEOM WOULD GIVE IN THIS SITUATION
// ---------------------------------------------------------
#endif // CROSSCHECK
  }
}
#endif // USE_VECGEOM_NAVIGATOR

//______________________________________________________________________________
void GeantTrack_v::NavIsSameLocation(int ntracks, VolumePath_t **start, VolumePath_t **end, bool *same,
                                     GeantTaskData *td) {
  // Implementation of TGeoNavigator::IsSameLocation with vector input
  for (int i = 0; i < ntracks; i++) {
    same[i] = NavIsSameLocationSingle(i, start, end, td);
  }
}

#ifdef USE_VECGEOM_NAVIGATOR
//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
bool GeantTrack_v::NavIsSameLocationSingle(int itr, VolumePath_t **start, VolumePath_t **end, GeantTaskData *td) {
#ifdef VERBOSE
  Geant::Print("","In NavIsSameLocation single %p for track %d", this, itr);
#endif
  // TODO: We should provide this function as a static function
  VECGEOM_NAMESPACE::SimpleNavigator simplenav;

  // this creates a tmpstate and copies in the state from end[itr]
  // we should avoid the creation of a state object here and rather use
  // some thread data?
  // was: VECGEOM_NAMESPACE::NavigationState tmpstate( *end[itr] );
  // new:
  VECGEOM_NAMESPACE::NavigationState *tmpstate = td->GetPath();
// VECGEOM_NAMESPACE::NavigationState::MakeInstance(end[itr]->GetMaxLevel());

// cross check with answer from ROOT
#ifdef CROSSCHECK
  TGeoBranchArray *sb = start[itr]->ToTGeoBranchArray();
  TGeoBranchArray *eb = end[itr]->ToTGeoBranchArray();
#endif

  // TODO: not using the direction yet here !!
  bool samepath = simplenav.HasSamePath(
      VECGEOM_NAMESPACE::Vector3D<VECGEOM_NAMESPACE::Precision>(fXposV[itr], fYposV[itr], fZposV[itr]), *start[itr],
      *tmpstate);
  if (!samepath) {
    // Printf("CORRECTING STATE FOR TRACK %d", itr);
    // start[itr]->GetCurrentNode()->Print();
    tmpstate->CopyTo(end[itr]);
    // end[itr]->GetCurrentNode()->Print();
    // assert(end[itr]->Top() != start[itr]->Top());
  }

#ifdef CROSSCHECK
  TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
  nav->ResetState();
  nav->SetLastSafetyForPoint(0, 0, 0, 0);
  nav->SetCurrentPoint(fXposV[itr], fYposV[itr], fZposV[itr]);
  nav->SetCurrentDirection(fXdirV[itr], fYdirV[itr], fZdirV[itr]);
  sb->UpdateNavigator(nav);
  bool rootsame = nav->IsSameLocation(fXposV[itr], fYposV[itr], fZposV[itr], true);
  if (rootsame != samepath) {
    Geant::Print("","INCONSISTENT ANSWER ROOT(%d) VECGEOM(%d)", rootsame, samepath);
    std::cout << VECGEOM_NAMESPACE::Vector3D<VECGEOM_NAMESPACE::Precision>(fXposV[itr], fYposV[itr], fZposV[itr])
              << "\n";
    Geant::Print("","old state");
    sb->Print();
    nav->ResetState();
    nav->SetLastSafetyForPoint(0, 0, 0, 0);
    nav->SetCurrentPoint(fXposV[itr], fYposV[itr], fZposV[itr]);
    nav->SetCurrentDirection(fXdirV[itr], fYdirV[itr], fZdirV[itr]);
    sb->UpdateNavigator(nav);
    nav->InspectState();
    bool rootsame = nav->IsSameLocation(fXposV[itr], fYposV[itr], fZposV[itr], true);
    nav->InspectState();
    eb->InitFromNavigator(nav);
    Geant::Print("","new state");
    eb->Print();
    Geant::Print("","VERSUS VECGEOM OLD AND NEW");
    start[itr]->printVolumePath();
    end[itr]->printVolumePath();
  } else {
    //  Printf("CONSISTENT SAME LOCATION");
  }

  delete sb;
  delete eb;
#endif // CROSSCHECK
  // VECGEOM_NAMESPACE::NavigationState::ReleaseInstance(tmpstate);

  return samepath;
}
#else
//______________________________________________________________________________
bool GeantTrack_v::NavIsSameLocationSingle(int itr, VolumePath_t **start, VolumePath_t **end, GeantTaskData */*td*/) {
  // Implementation of TGeoNavigator::IsSameLocation for single particle
  TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
  nav->ResetState();
  nav->SetLastSafetyForPoint(0, 0, 0, 0);
  nav->SetCurrentPoint(fXposV[itr], fYposV[itr], fZposV[itr]);
  nav->SetCurrentDirection(fXdirV[itr], fYdirV[itr], fZdirV[itr]);
  start[itr]->UpdateNavigator(nav);
  if (!nav->IsSameLocation(fXposV[itr], fYposV[itr], fZposV[itr], true)) {
    end[itr]->InitFromNavigator(nav);
#ifdef BUG_HUNT
    GeantPropagator *prop = GeantPropagator::Instance();
    BreakOnStep(prop->fDebugEvt, prop->fDebugTrk, prop->fDebugStp, prop->fDebugRep, "NavIsSameLoc:CROSSED", itr);
#endif
    return false;
  }
// Track not crossing -> remove boundary flag
#ifdef BUG_HUNT
  BreakOnStep(prop->fDebugEvt, prop->fDebugTrk, prop->fDebugStp, prop->fDebugRep, "NavIsSameLoc:SAME", itr);
#endif
  return true;
}
#endif

//______________________________________________________________________________
int GeantTrack_v::SortByStatus(TrackStatus_t status) {
  // Sort tracks by a given status.
  int nsel = 0;
  int ntracks = GetNtracks();
  for (int itr = 0; itr < ntracks; itr++) {
    if (fStatusV[itr] == status) {
      Select(itr);
      nsel++;
    }
  }
  if (nsel) {
    if (nsel < ntracks)
      Reshuffle();
    else
      DeselectAll();
  }
  return nsel;
}

//______________________________________________________________________________
int GeantTrack_v::SortByLimitingDiscreteProcess() {
  // Sort tracks for which the step was limited by discrete processes.
  int nsel = 0;
  int ntracks = GetNtracks();
  for (int itr = 0; itr < ntracks; itr++) {
    if (fStatusV[itr] == kPhysics && fEindexV[itr] == 1000) {
      Select(itr);
      nsel++;
    }
  }
  if (nsel) {
    if (nsel < ntracks)
      Reshuffle();
    else
      DeselectAll();
  }
  return nsel;
}

//______________________________________________________________________________
int GeantTrack_v::RemoveByStatus(TrackStatus_t status, GeantTrack_v &output) {
  // Remove tracks with given status from the container to the output vector,
  // then compact.
  int nremoved = 0;
  int ntracks = GetNtracks();
  for (int itr = 0; itr < ntracks; itr++) {
    if (fStatusV[itr] == status) {
      MarkRemoved(itr);
      nremoved++;
    }
  }
  if (!fCompact)
    Compact(&output);
  return nremoved;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
void GeantTrack_v::PrintTrack(int itr, const char *msg) const {
  // Print info for a given track
  const char *status[8] = {"alive", "killed", "inflight", "boundary", "exitSetup", "physics", "postponed", "new"};
#ifdef USE_VECGEOM_NAVIGATOR
  Geant::Print(msg,
      "== Track %d: evt=%d slt=%d part=%d pdg=%d gVc=%d chg=%d proc=%d vid=%d nstp=%d spc=%d status=%s mass=%g\
              xpos=%g ypos=%g zpos=%g xdir=%g ydir=%g zdir=%g mom=%g ene=%g time=%g pstp=%g stp=%g snxt=%g saf=%g bdr=%d\n\n",
      itr, fEventV[itr], fEvslotV[itr], fParticleV[itr], fPDGV[itr], fGVcodeV[itr],
      fChargeV[itr], fProcessV[itr], fVindexV[itr], fNstepsV[itr], (int)fSpeciesV[itr], status[int(fStatusV[itr])],
      fMassV[itr], fXposV[itr], fYposV[itr], fZposV[itr], fXdirV[itr], fYdirV[itr], fZdirV[itr], fPV[itr], fEV[itr],
      fTimeV[itr], fPstepV[itr], fStepV[itr], fSnextV[itr], fSafetyV[itr], fBoundaryV[itr]);

#else
  TString path;
  fPathV[itr]->GetPath(path);
  TString nextpath;
  fNextpathV[itr]->GetPath(nextpath);

  Geant::Print(msg, "== Track %d: evt=%d slt=%d part=%d pdg=%d gVc=%d eind=%d chg=%d proc=%d vid=%d nstp=%d "
         "spc=%d status=%s mass=%g xpos=%g ypos=%g zpos=%g xdir=%g ydir=%g zdir=%g mom=%g ene=%g "
         "time=%g edep=%g pstp=%g stp=%g snxt=%g saf=%g bdr=%d\n pth=%s npth=%s\n",
         itr, fEventV[itr], fEvslotV[itr], fParticleV[itr], fPDGV[itr], fEindexV[itr], fGVcodeV[itr],
         fChargeV[itr], fProcessV[itr], fVindexV[itr], fNstepsV[itr], (int)fSpeciesV[itr], status[int(fStatusV[itr])],
         fMassV[itr], fXposV[itr], fYposV[itr], fZposV[itr], fXdirV[itr], fYdirV[itr], fZdirV[itr], fPV[itr], fEV[itr],
         fTimeV[itr], fEdepV[itr], fPstepV[itr], fStepV[itr], fSnextV[itr], fSafetyV[itr], fBoundaryV[itr], path.Data(),
         nextpath.Data());
#endif
}

//______________________________________________________________________________
void GeantTrack_v::PrintTracks(const char *msg) const {
  // Print all tracks
  int ntracks = GetNtracks();
  Geant::Print(msg,"");
  for (int i = 0; i < ntracks; i++)
    PrintTrack(i);
}

#ifdef USE_VECGEOM_NAVIGATOR

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
void GeantTrack_v::ComputeTransportLength(int ntracks, GeantTaskData *td) {
#ifndef GEANT_CUDA_DEVICE_BUILD
  static std::atomic<int> icalls(0);
  ++icalls;
#endif

#if 0
  // Printf("In vec find next boundary and step\n");
   using VECGEOM_NAMESPACE::SimpleNavigator;
   using VECGEOM_NAMESPACE::Precision;
   using VECGEOM_NAMESPACE::Vector3D;
   using VECGEOM_NAMESPACE::GeoManager;
   using VECGEOM_NAMESPACE::SOA3D;
   typedef Vector3D<Precision> Vector3D_t;
   typedef SOA3D<double> SOA3D_t;

   // TODO: vectorize loop
   for (int i = 0; i < ntracks; ++i) {
     // Check if current safety allows for the proposed step
     if (fSafetyV[i] > fPstepV[i]) {
       fSnextV[i] = fPstepV[i];
       fBoundaryV[i] = false;
       continue;
     }
   }

   // call the vector interface
   // we need to transform our arrays to SOA
   SimpleNavigator nav;
   nav.FindNextBoundaryAndStep(SOA3D_t(const_cast<double*>(fXposV),const_cast<double*>(fYposV),const_cast<double*>(fZposV),ntracks),
                               SOA3D_t(const_cast<double*>(fXdirV),const_cast<double*>(fYdirV),const_cast<double*>(fZdirV),ntracks),
                               *td->GetSOA3DWorkspace1(ntracks),
                               *td->GetSOA3DWorkspace2(ntracks),
                               fPathV,
                               fNextpathV,
                               fPstepV,
                               fSafetyV,
                               fSnextV,
                               td->GetIntArray(ntracks));

   // Update number of calls to geometry (consider a vector call as N scalar calls)
   td->fNsnext += ntracks;
   // rectify some parts ( as in the scalar case )
   // TODO: vectorize loop
   for (unsigned int itr = 0; itr < ntracks; ++itr) {
     fSnextV[itr] = Math::Max<double>(2. * gTolerance, fSnextV[itr] + 2. * gTolerance);
     fBoundaryV[itr] = fNextpathV[itr]->IsOnBoundary();
     if ((fNextpathV[itr]->IsOutside() && fSnextV[itr] < 1.E-6) || fSnextV[itr] > 1.E19)
           fStatusV[itr] = kExitingSetup;
   }
#else
  // OLD version calling the looped implementation
  // call the vector interface of GeantTrack_v
  NavFindNextBoundaryAndStep(ntracks, fPstepV, fXposV, fYposV, fZposV, fXdirV, fYdirV, fZdirV, fPathV, fNextpathV,
                             fSnextV, fSafetyV, fBoundaryV, this);
  // Update number of calls to geometry (consider a vector call as N scalar calls)
  td->fNsnext += ntracks;

  // perform a couple of additional checks/ set status flags and so on
  for (int itr = 0; itr < ntracks; ++itr) {
    if ((fNextpathV[itr]->IsOutside() && fSnextV[itr] < 1.E-6) || fSnextV[itr] > 1.E19)
      fStatusV[itr] = kExitingSetup;
  }
#endif
}
#else
//______________________________________________________________________________
void GeantTrack_v::ComputeTransportLength(int ntracks, GeantTaskData *td) {
  // Computes snext and safety for an array of tracks. For charged tracks these are the only
  // computed values, while for neutral ones the next node is checked and the boundary flag is set
  // if
  // closer than the proposed physics step.

  for (int i = 0; i < ntracks; ++i) {
    ComputeTransportLengthSingle(i, td);
  }
}
#endif

GEANT_CUDA_BOTH_CODE
//#if 0
#ifdef USE_VECGEOM_NAVIGATOR
void GeantTrack_v::ComputeTransportLengthSingle(int itr, GeantTaskData *td) {
// Computes snext and safety for a single track. For charged tracks these are the only
// computed values, while for neutral ones the next node is checked and the boundary flag is set if
// closer than the proposed physics step.
#ifndef GEANT_CUDA_DEVICE_BUILD
  static std::atomic<int> icalls(0);
  ++icalls;
#endif
  // inits navigator with current state
  // TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
  // nav->ResetState();
  // nav->SetCurrentPoint(fXposV[itr], fYposV[itr], fZposV[itr]);
  // nav->SetCurrentDirection(fXdirV[itr], fYdirV[itr], fZdirV[itr]);
  // fPathV[itr]->UpdateNavigator(nav);
  // nav->SetLastSafetyForPoint(fSafetyV[itr], fXposV[itr], fYposV[itr], fZposV[itr]);
  // nav->FindNextBoundaryAndStep( Math::Min<double>(1.E20, fPstepV[itr]), !fBoundaryV[itr] );

  //
  using VECGEOM_NAMESPACE::SimpleNavigator;
  using VECGEOM_NAMESPACE::ABBoxNavigator;
  using VECGEOM_NAMESPACE::Precision;
  using VECGEOM_NAMESPACE::Vector3D;
  typedef Vector3D<Precision> Vector3D_t;

  // In case the proposed step is within safety, no need to compute distance to next boundary
  if (fPstepV[itr] < fSafetyV[itr]) {
    fSnextV[itr] = fPstepV[itr];
    *fNextpathV[itr] = *fPathV[itr];
    fBoundaryV[itr] = false;
    return;
  }
  SimpleNavigator nav;
//  ABBoxNavigator nav;
  double step = 0.0;
  nav.FindNextBoundaryAndStep(Vector3D_t(fXposV[itr], fYposV[itr], fZposV[itr]),
                              Vector3D_t(fXdirV[itr], fYdirV[itr], fZdirV[itr]), *fPathV[itr], *fNextpathV[itr],
                              Math::Min<double>(1.E20, fPstepV[itr]), step);

  // Update number of calls to geometry
  td->fNsnext++;
  // get back step, safety, new geometry path, and other navigation information
  fSnextV[itr] = Math::Max<double>(2 * gTolerance, step);
  fSafetyV[itr] = (fBoundaryV[itr]) ? 0 : nav.GetSafety(Vector3D_t(fXposV[itr], fYposV[itr], fZposV[itr]), *fPathV[itr]);
  fSafetyV[itr] = (fSafetyV[itr] < 0) ? 0. : fSafetyV[itr];
  fBoundaryV[itr] = fNextpathV[itr]->IsOnBoundary();

  // if outside detector or enormous step mark particle as exiting the detector
  if (fNextpathV[itr]->IsOutside() || fSnextV[itr] > 1.E19)
    fStatusV[itr] = kExitingSetup;

  // force track to cross under certain conditions
}
#else
void GeantTrack_v::ComputeTransportLengthSingle(int itr, GeantTaskData *td) {
  // Computes snext and safety for a single track. For charged tracks these are the only
  // computed values, while for neutral ones the next node is checked and the boundary flag is set
  // if closer than the proposed physics step.

  // In case the proposed step is within safety, no need to compute distance to next boundary
  if (fPstepV[itr] < fSafetyV[itr]) {
    fSnextV[itr] = fPstepV[itr];
    *fNextpathV[itr] = *fPathV[itr];
    fBoundaryV[itr] = false;
    return;
  }
  NavFindNextBoundaryAndStep(1, &fPstepV[itr], &fXposV[itr], &fYposV[itr], &fZposV[itr], &fXdirV[itr], &fYdirV[itr],
                             &fZdirV[itr], &fPathV[itr], &fNextpathV[itr], &fSnextV[itr], &fSafetyV[itr],
                             &fBoundaryV[itr], this);
  // Update number of calls to geometry
  td->fNsnext++;
  // if outside detector or enormous step mark particle as exiting the detector
  if (fNextpathV[itr]->IsOutside() || fSnextV[itr] > 1.E19)
    fStatusV[itr] = kExitingSetup;
#ifdef CROSSCHECK
  VECGEOM_NAMESPACE::NavigationState vecgeom_in_state(VECGEOM_NAMESPACE::GeoManager::Instance().getMaxDepth());
  VECGEOM_NAMESPACE::NavigationState vecgeom_out_state(VECGEOM_NAMESPACE::GeoManager::Instance().getMaxDepth());
  vecgeom_in_state = *fPathV[itr];
  VECGEOM_NAMESPACE::SimpleNavigator vecnav;
  double vecgeom_step;
  typedef VECGEOM_NAMESPACE::Vector3D<VECGEOM_NAMESPACE::Precision> Vector3D_t;
  vecnav.FindNextBoundaryAndStep(Vector3D_t(fXposV[itr], fYposV[itr], fZposV[itr]) /* global pos */,
                                 Vector3D_t(fXdirV[itr], fYdirV[itr], fZdirV[itr]) /* global dir */, vecgeom_in_state,
                                 vecgeom_out_state /* the paths */, Math::Min<double>(1.E20, fPstepV[itr]), vecgeom_step);
  vecgeom_step = Math::Max<double>(2 * gTolerance, vecgeom_step);
  double vecgeom_safety;
  vecgeom_safety = vecnav.GetSafety(Vector3D_t(fXposV[itr], fYposV[itr], fZposV[itr]), vecgeom_in_state);
  vecgeom_safety = (vecgeom_safety < 0) ? 0. : vecgeom_safety;
  //   Printf("--VECGEOM-- TRACK %d BOUND %d PSTEP %lg STEP %lg SAFETY %lg TOBOUND %d",
  //                  i, isonbdr[i], pstep[i], vecgeom_step, vecgeom_safety,
  //                  vecgeom_out_state.IsOnBoundary());
  if (!(vecgeom_out_state.IsOutside() || fNextpathV[itr]->IsOutside()) &&
      (vecgeom_out_state.GetCurrentNode() != fNextpathV[itr]->GetCurrentNode())) {
    warnings++;
    InspectGeometryState(itr);
    Warning("", "NavigationWarning");
    if (warnings > 100)
      assert(!"NOT SAME RESULT FOR NEXTNODE");
  }
#endif
}
#endif
//______________________________________________________________________________
TransportAction_t GeantTrack_v::PostponedAction(int ntracks) const {
  // Check the action to be taken according the current policy
  if (!ntracks)
    return kDone;
  // Temporary hook
  if (ntracks == 1) {
    //      if (gPropagator->GetPolicy()<GeantPropagator::kPrioritize)
    //           return kPostpone;
    //      else
    return kSingle;
  }
  return kVector;
}

//______________________________________________________________________________
int GeantTrack_v::PropagateTracks(GeantTaskData *td) {
  // Propagate the ntracks in the current volume with their physics steps (already
  // computed)
  // Vectors are pushed downstream when efficient.
  GeantTrack_v &output = *td->fTransported;
  int ntracks = GetNtracks();
  // Check if tracking the remaining tracks can be postponed
  TransportAction_t action = PostponedAction(ntracks);
  if (action == kPostpone) {
    PostponeTracks(output);
    return 0;
  }
  if (action != kVector)
    return PropagateTracksScalar(td, 0);
// Compute transport length in geometry, limited by the physics step
#ifdef BUG_HUNT
  GeantPropagator *prop = GeantPropagator::Instance();
  BreakOnStep(prop->fDebugEvt, prop->fDebugTrk, prop->fDebugStp, prop->fDebugRep, "PropagateTracks");
#endif
  ComputeTransportLength(ntracks, td);
//         Printf("====== After ComputeTransportLength:");
//         PrintTracks();
#ifdef BUG_HUNT
  BreakOnStep(prop->fDebugEvt, prop->fDebugTrk, prop->fDebugStp, prop->fDebugRep, "AfterCompTransLen");
#endif

  int itr = 0;
  int icrossed = 0;
  int nsel = 0;
  double lmax;
  const double eps = 1.E-2; // 100 micron
  const double bmag = gPropagator->fBmag;

  // Remove dead tracks, propagate neutrals
  for (itr = 0; itr < ntracks; itr++) {
    // Mark dead tracks for copy/removal
    if (fSnextV[itr] < 0) {
      Error("ComputeTransportLength", "Track %d cannot cross boundary and has to be killed", fParticleV[itr]);
      PrintTrack(itr);
      fStatusV[itr] = kKilled;
    }
    if (fStatusV[itr] == kKilled) {
      MarkRemoved(itr);
      continue;
    }
    // Propagate straight tracks to the precomputed location and update state,
    // then mark them for copy/removal
    // (Inlined from PropagateStraight)
    if (fChargeV[itr] == 0 || bmag < 1.E-10) {
      // Do straight propagation to physics process or boundary
      if (fBoundaryV[itr]) {
        if (fNextpathV[itr]->IsOutside())
          fStatusV[itr] = kExitingSetup;
        else
          fStatusV[itr] = kBoundary;
        icrossed++;
      } else {
        fStatusV[itr] = kPhysics;
        // Update number of steps to physics
        td->fNphys++;
      }
      fPstepV[itr] -= fSnextV[itr];
      fStepV[itr] += fSnextV[itr];
      fSafetyV[itr] -= fSnextV[itr];
      if (fSafetyV[itr] < 0.)
        fSafetyV[itr] = 0;
      fXposV[itr] += fSnextV[itr] * fXdirV[itr];
      fYposV[itr] += fSnextV[itr] * fYdirV[itr];
      fZposV[itr] += fSnextV[itr] * fZdirV[itr];
      // Update total number of steps
      td->fNsteps++;
      if (fSnextV[itr] < 1.E-8) td->fNsmall++;
      MarkRemoved(itr);
      fSnextV[itr] = 0;
#ifdef USE_VECGEOM_NAVIGATOR
//            CheckLocationPathConsistency(itr);
#endif
    }
  }
  // Compact remaining tracks and move the removed oned to the output container
  if (!fCompact)
    Compact(&output);
  // Check if tracking the remaining tracks can be postponed
  action = PostponedAction(fNtracks);
  switch (action) {
  case kDone:
    return icrossed;
  case kSingle:
    icrossed += PropagateTracksScalar(td, 1);
    return icrossed;
  case kPostpone:
    PostponeTracks(output);
    return icrossed;
  case kVector:
    break;
  }
  // REMAINING ONLY CHARGED TRACKS IN MAG. FIELD
  // Continue with vectorized mode ...

  // New algorithm: we use the track sagitta to estimate the "bending" error,
  // i.e. what is the propagated length for which the track deviation in magnetic
  // field with respect to straight propagation is less than epsilon.
  // Take the maximum between the safety and the "bending" safety
  nsel = 0;
  ntracks = GetNtracks();
  double *steps = td->GetDblArray(ntracks);
  for (itr = 0; itr < fNtracks; itr++) {
    lmax = SafeLength(itr, eps);
    lmax = Math::Max<double>(lmax, fSafetyV[itr]);
    // Select step to propagate as the minimum among the "safe" step and:
    // the straight distance to boundary (if fboundary=1) or the proposed  physics
    // step (fboundary=0)
    steps[itr] =
        (fBoundaryV[itr]) ? Math::Min<double>(lmax, Math::Max<double>(fSnextV[itr], 1.E-4)) : Math::Min<double>(lmax, fPstepV[itr]);
    //    if (fBoundaryV[itr] && steps[itr]<1.E-8) steps[itr] = 1.E-3;
    // Printf("snext=%g lmax=%g", fSnextV[itr], lmax);
    //      Printf("track %d: step=%g (safelen=%g)", itr, steps[itr], lmax);
  }
  // Propagate the vector of tracks
  PropagateInVolume(ntracks, steps, td);
  //Update number of partial steps propagated in field
  td->fNmag += ntracks;
  //         Printf("====== After PropagateInVolume:");
  //         PrintTracks();
  // Some tracks made it to physics steps (kPhysics)
  //         -> remove and copy to output
  // Some tracks were propagated with steps greater than safety
  //         -> check possible crossing via NavIsSameLocation
  // Some tracks were propagated with steps less than safety
  //         -> keep in the container

  // Select tracks that made it to physics and copy to output
  ntracks = GetNtracks();
  for (itr = 0; itr < ntracks; itr++) {
    if (fStatusV[itr] == kPhysics) {
      MarkRemoved(itr);
      // Update number of steps to physics and total number of steps
      td->fNphys++;
      td->fNsteps++;
    }
  }
  if (!fCompact)
    Compact(&output);
  // Select tracks that are in flight or were propagated to boundary with
  // steps bigger than safety
  nsel = 0;
  ntracks = GetNtracks();
  for (itr = 0; itr < ntracks; itr++) {
    if (fSafetyV[itr] < 1.E-10 || fSnextV[itr] < 1.E-10) {
      Select(itr);
      nsel++;
    }
  }
  // The selected tracks may have crossed the boundaries - check that
  if (nsel) {
    if (nsel < GetNtracks())
      Reshuffle();
    else
      DeselectAll();
    bool *same = td->GetBoolArray(nsel);
    NavIsSameLocation(nsel, fPathV, fNextpathV, same, td);
    for (itr = 0; itr < nsel; itr++) {
      if (same[itr]) {
        fBoundaryV[itr] = false;
        continue;
      }
      // Boundary crossed
      fStatusV[itr] = kBoundary;
      if (fNextpathV[itr]->IsOutside())
        fStatusV[itr] = kExitingSetup;
      fBoundaryV[itr] = true;
      icrossed++;
      // Update number of steps to physics and total number of steps
      td->fNsteps++;  
      if (fStepV[itr] < 1.E-8) td->fNsmall++;
      MarkRemoved(itr);
    }
    //         Printf("====== After finding crossing tracks (ncross=%d):", icrossed);
    //         PrintTracks();
    if (!fCompact)
      Compact(&output);
  }
#ifdef BUG_HUNT
  BreakOnStep(prop->fDebugEvt, prop->fDebugTrk, prop->fDebugStp, prop->fDebugRep, "AfterPropagateTracks");
#endif
  return icrossed;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
int GeantTrack_v::PropagateSingleTrack(int itr, GeantTaskData *td, int stage) {
  // Propagate the tracks with their selected steps in a single loop,
  // starting from a given stage.

  int icrossed = 0;
  double step, lmax;
  const double eps = 1.E-2; // 1 micron
#ifdef GEANT_CUDA_DEVICE_BUILD
  const double bmag = gPropagator_fBmag;
#else
  const double bmag = gPropagator->fBmag;
#endif
// Compute transport length in geometry, limited by the physics step
#ifdef BUG_HUNT
  GeantPropagator *prop = GeantPropagator::Instance();
  BreakOnStep(prop->fDebugEvt, prop->fDebugTrk, prop->fDebugStp, prop->fDebugRep, "PropagateSingle", itr);
#endif
  ComputeTransportLengthSingle(itr, td);
#ifdef BUG_HUNT
  BreakOnStep(0, 15352, 0, 10, "AfterCompTranspLenSingle");
#endif
  // Mark dead tracks for copy/removal
  if (fSnextV[itr] < 0) {
    Error("ComputeTransportLength", "Track %d cannot cross boundary and has to be killed", fParticleV[itr]);
    PrintTrack(itr);
    fStatusV[itr] = kKilled;
  }
  if (fStatusV[itr] == kKilled) {
    MarkRemoved(itr);
    return icrossed;
  }
  // Stage 0: straight propagation
  if (stage == 0) {
    if (fChargeV[itr] == 0 || bmag < 1.E-10) {
      // Do straight propagation to physics process or boundary
      if (fBoundaryV[itr]) {
        //*fPathV[itr] = *fNextpathV[itr];
        if (fNextpathV[itr]->IsOutside())
          fStatusV[itr] = kExitingSetup;
        else
          fStatusV[itr] = kBoundary;
        icrossed++;
      } else {
        fStatusV[itr] = kPhysics;
        // Update number of steps to physics
        td->fNphys++;
      }
      fPstepV[itr] -= fSnextV[itr];
      fStepV[itr] += fSnextV[itr];
      fSafetyV[itr] -= fSnextV[itr];
      if (fSafetyV[itr] < 0.)
        fSafetyV[itr] = 0;
      fXposV[itr] += fSnextV[itr] * fXdirV[itr];
      fYposV[itr] += fSnextV[itr] * fYdirV[itr];
      fZposV[itr] += fSnextV[itr] * fZdirV[itr];
      // Update total number of steps
      td->fNsteps++;
      if (fSnextV[itr] < 1.E-8) td->fNsmall++;
      fSnextV[itr] = 0;
      MarkRemoved(itr);
#ifdef USE_VECGEOM_NAVIGATOR
//            CheckLocationPathConsistency(itr);
#endif
#ifdef BUG_HUNT
      BreakOnStep(prop->fDebugEvt, prop->fDebugTrk, prop->fDebugStp, prop->fDebugRep, "AfterPropagateSingleNeutral",
                  itr);
#endif
      return icrossed;
    }
  }
  // Stage 1: mag field propagation for tracks with pstep<safety
  if (stage <= 1) {
    // REMAINING ONLY CHARGED TRACKS IN MAG. FIELD
    // New algorithm: we use the track sagitta to estimate the "bending" error,
    // i.e. what is the propagated length for which the track deviation in magnetic
    // field with respect to straight propagation is less than epsilon.
    // Take the maximum between the safety and the "bending" safety
    lmax = SafeLength(itr, eps);
    lmax = Math::Max<double>(lmax, fSafetyV[itr]);
    // Select step to propagate as the minimum among the "safe" step and:
    // the straight distance to boundary (if frombdr=1) or the proposed  physics
    // step (frombdr=0)
    step = (fBoundaryV[itr]) ? Math::Min<double>(lmax, Math::Max<double>(fSnextV[itr], 1.E-4)) : Math::Min<double>(lmax, fPstepV[itr]);
    //      Printf("track %d: step=%g (safelen=%g)", itr, step, lmax);
    // int stepNum= fNstepsV[itr];
    // Printf("track %d: Step #=%3d len=%g proposed=%g (safelen=%9.3g) bndrFlg= %d distLin=%g  ",
    //    itr, stepNum, step, fPstepV[itr], lmax, fFrombdrV[itr], fSnextV[itr] );
    //  Printf("track %d: step=%g (safelen=%g)", itr, step, lmax);          
    PropagateInVolumeSingle(itr, step, td);
    //Update number of partial steps propagated in field
    td->fNmag++;
    //      Printf("====== After PropagateInVolumeSingle:");
    //      PrintTrack(itr);
    // The track may have made it to physics steps (kPhysics)
    //         -> remove and copy to output
    // The track may have been propagated with step greater than safety
    //         -> check possible crossing via NavIsSameLocation
    // The track may have been propagated with step less than safety
    //         -> keep in the container

    // Select tracks that made it to physics and copy to output
    if (fStatusV[itr] == kPhysics) {
      MarkRemoved(itr);
      // Update number of steps to physics and total number of steps
      td->fNphys++;
      td->fNsteps++;
      return icrossed;
    }
    // Select tracks that are in flight or were propagated to boundary with
    // steps bigger than safety
    if (fSafetyV[itr] < 1.E-10 || fSnextV[itr] < 1.E-10) {
      bool same = NavIsSameLocationSingle(itr, fPathV, fNextpathV, td);
      if (same) {
        fBoundaryV[itr] = false;
        return icrossed;
      }
      // Boundary crossed
      fStatusV[itr] = kBoundary;
      if (fNextpathV[itr]->IsOutside())
        fStatusV[itr] = kExitingSetup;
      fBoundaryV[itr] = true;
      icrossed++;
      // Update number of steps to physics and total number of steps
      td->fNsteps++;     
      if (fStepV[itr] < 1.E-8) td->fNsmall++;
      MarkRemoved(itr);
    }
#ifdef USE_VECGEOM_NAVIGATOR
//         CheckLocationPathConsistency(itr);
#endif
  }
#ifdef BUG_HUNT
  BreakOnStep(prop->fDebugEvt, prop->fDebugTrk, prop->fDebugStp, prop->fDebugRep, "AfterPropagateSingle", itr);
#endif
  return icrossed;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
int GeantTrack_v::PropagateTracksScalar(GeantTaskData *td, int stage) {
  // Propagate the tracks with their selected steps in a single loop,
  // starting from a given stage.

#ifndef GEANT_CUDA_DEVICE_BUILD
  GeantTrack_v &output = *td->fTransported;
#endif
  int icrossed = 0;
  int ntracks = GetNtracks();
  for (int itr = 0; itr < ntracks; itr++) {
    icrossed += PropagateSingleTrack(itr, td, stage);
  }
//   Printf("====== After finding crossing tracks (ncross=%d):", icrossed);
//   PrintTracks();
// Compact remaining tracks and move the removed oned to the output container
#ifndef GEANT_CUDA_DEVICE_BUILD
  if (!fCompact)
    Compact(&output);
#endif
  return icrossed;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
double GeantTrack_v::Curvature(int i) const {
  // Curvature assuming constant field is along Z
  const double tiny = 1.E-30;
#ifdef GEANT_CUDA_DEVICE_BUILD
  const double bmag = gPropagator_fBmag;
#else
  const double bmag = gPropagator->fBmag;
#endif
  return fabs(kB2C * fChargeV[i] * bmag / (Pt(i) + tiny));
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
double GeantTrack_v::SafeLength(int i, double eps) {
  // Returns the propagation length in field such that the propagated point is
  // shifted less than eps with respect to the linear propagation.
  double c = Curvature(i);
  if (c < 1.E-10)
    return 1.E20;
  return 2. * sqrt(eps / c);
}

//______________________________________________________________________________
int GeantTrack_v::PostponeTracks(GeantTrack_v &output) {
  // Postpone transport of remaining tracks and copy them to the output.
  int npostponed = GetNtracks();
  for (int itr = 0; itr < npostponed; itr++)
    fStatusV[itr] = kPostponed;
  // Move these tracks to the output container
  output.AddTracks(*this, 0, npostponed - 1, true);
  RemoveTracks(0, npostponed - 1);
  Clear();
  return npostponed;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
int GeantTrack_v::PostponeTrack(int itr, GeantTrack_v &output) {
  // Postpone transport of a track and copy it to the output.
  // Returns where in the output the track was added.

  fStatusV[itr] = kPostponed;
  // Move these tracks to the output container
  int new_itr = output.AddTrack(*this, itr, true);
  MarkRemoved(itr);
  return new_itr;
}


//______________________________________________________________________________
Volume_t const*GeantTrack_v::GetNextVolume(int i) const {
  // Next volume the track is getting into
#ifdef USE_VECGEOM_NAVIGATOR
  return fNextpathV[i]->Top()->GetLogicalVolume();
#else
  return fNextpathV[i]->GetCurrentNode()->GetVolume();
#endif
}

//______________________________________________________________________________
Volume_t const*GeantTrack_v::GetVolume(int i) const {
  // Current volume the track is into
#ifdef USE_VECGEOM_NAVIGATOR
// The check below not needed anymore - everything got from path
//  assert(fVindexV[i] == fPathV[i]->Top()->GetLogicalVolume()->id());
  return fPathV[i]->Top()->GetLogicalVolume();
#else
  // TODO: get rid of fVindexV
  return (fPathV[i]->GetCurrentNode()->GetVolume());
#endif
}

//______________________________________________________________________________
Material_t *GeantTrack_v::GetMaterial(int i) const {
  // Current material the track is into
#ifdef USE_VECGEOM_NAVIGATOR
  Medium_t *med = (Medium_t *)GetVolume(i)->GetTrackingMediumPtr();
#else
  Medium_t *med = (Medium_t *)GetVolume(i)->GetMedium();
#endif
  // TODO: better to use assert
  if (!med)
    return nullptr;
  return med->GetMaterial();
}

//______________________________________________________________________________
#ifdef USE_VECGEOM_NAVIGATOR
bool GeantTrack_v::CheckNavConsistency(int /*itr*/) {
  // TO IMPLEMENT WIRH VECGEOM
#else
bool GeantTrack_v::CheckNavConsistency(int itr) {
// Check consistency of navigation state for a given track.
// Debugging purpose
  double point[3], local[3];
  point[0] = fXposV[itr];
  point[1] = fYposV[itr];
  point[2] = fZposV[itr];
  fPathV[itr]->GetMatrix()->MasterToLocal(point, local);
  TGeoShape *shape = fPathV[itr]->GetCurrentNode()->GetVolume()->GetShape();
  int evt = fEventV[itr];
  int trk = fParticleV[itr];
  int stp = fNstepsV[itr];
  bool onbound = fBoundaryV[itr];
  bool inside = shape->Contains(local);
  double safin = shape->Safety(local, true);
  double safout = shape->Safety(local, false);

  // 1. Check that the current state really contains the particle position if the position is not declared on boundary.
  if (!onbound && !inside && safout > 0.01) {
    Printf("ERRINSIDE: evt=%d trk=%d stp=%d (%16.14f,  %16.14f, %16.14f) not inside. safout=%g", evt, trk, stp,
           point[0], point[1], point[2], safout);
    //    PrintTrack(itr);
    return false;
  }
  // 2. Check that the safety state is consistent
  if (!onbound && inside) {
    if (safin < fSafetyV[itr] - 1.E-8) {
      Printf("ERRSAFIN: evt=%d trk=%d stp=%d (%16.14f,  %16.14f, %16.14f) safin=%g smaller than track safety=%g", evt,
             trk, stp, point[0], point[1], point[2], safin, fSafetyV[itr]);
      return false;
    }
  }
#endif
  return true;
}

//______________________________________________________________________________
GEANT_CUDA_BOTH_CODE
bool GeantTrack_v::BreakOnStep(int evt, int trk, int stp, int nsteps, const char *msg, int itr) {
  // Return true if container has a track with a given number doing a given step from a given event
  // Debugging purpose
  int ntracks = GetNtracks();
  int start = 0;
  int end = ntracks;
  bool has_it = false;
  if (itr >= 0) {
    start = itr;
    end = itr + 1;
  }
  for (itr = start; itr < end; ++itr) {
    if ((fParticleV[itr] == trk) && (fEventV[itr] == evt) &&
        ((fNstepsV[itr] >= stp) && (fNstepsV[itr] < stp + nsteps))) {
      has_it = true;
#ifndef GEANT_NVCC
      PrintTrack(itr, msg);
#else
      (void)msg;
#endif
      break;
    }
  }
  if (!has_it)
    return false;
  // Put breakpoint at line below
  return true;
}

} // GEANT_IMPL_NAMESPACE

#ifdef GEANT_CUDA
#ifndef GEANT_NVCC

bool ToDevice(vecgeom::cxx::DevicePtr<cuda::GeantTrack_v> dest, cxx::GeantTrack_v *source, cudaStream_t stream) {
  // Since fPathV and fNextpathV are internal pointer, we need to fix them up.
  // assert(vecgeom::cuda::NavigationState::SizeOfInstance(fMaxDepth)
  //       == vecgeom::cxx::NavigationState::SizeOfInstance(fMaxDepth) );

  size_t bufferOffset = GeantTrack_v::round_up_align(vecgeom::cxx::DevicePtr<Geant::cuda::GeantTrack_v>::SizeOf());
  long offset = ((const char *)dest.GetPtr() + bufferOffset) - (const char *)source->Buffer();
  for (int hostIdx = 0; hostIdx < source->GetNtracks(); ++hostIdx) {
    // Technically this offset is a 'guess' and depends on the
    // host (cxx) and device (cuda) GeantTrack_v to be strictly aligned.
    if (source->fPathV[hostIdx])
      source->fPathV[hostIdx] = (VolumePath_t *)(((char *)source->fPathV[hostIdx]) + offset);
    if (source->fNextpathV[hostIdx])
      source->fNextpathV[hostIdx] = (VolumePath_t *)(((char *)source->fNextpathV[hostIdx]) + offset);
  }
  // const char* destBuf =  ((const char*)dest.GetPtr() + bufferOffset;
  // const char* sourBuf =  (const char*)source->Buffer();
  // for(int hostIdx = 0; hostIdx < source->GetNtracks(); ++hostIdx ) {
  //    fprintf(stderr,"Track[%d] : val=%p diff=%p off=%p\n", hostIdx, source->fPathV[hostIdx],
  //            ((const char*)source->fPathV[hostIdx]) - destBuf,  ((const char*)source->fPathV[hostIdx]) - offset);
  // }

  assert(((void *)source) == ((void *)(&(source->fNtracks))));

  fprintf(stderr,"Posting the copy from host=%p to device=%p and size=%ld\n",
          source->Buffer(),
          ((char*)dest.GetPtr()) + bufferOffset,
          source->BufferSize());
  // fMaxtracks, fMaxDepth and fBufSize ought to be invariant.
  GEANT_CUDA_ERROR(cudaMemcpyAsync(((char*)dest.GetPtr()) + bufferOffset,
                                   source->Buffer(),
                                   source->BufferSize(),
                                   cudaMemcpyHostToDevice, stream));
  // Copy stream->fInputBasket->fNtracks, stream->fInputBasket->fNselected, stream->fInputBasket->fCompact, stream->fInputBasket->fMixed
  GEANT_CUDA_ERROR(cudaMemcpyAsync(dest,
                                   source,
                                   sizeof(int)*2+sizeof(Bool_t)*2,
                                   cudaMemcpyHostToDevice, stream));

  return true;
}

void FromDeviceConversion(cxx::GeantTrack_v *dest, vecgeom::cxx::DevicePtr<cuda::GeantTrack_v> source) {
  size_t bufferOffset = GeantTrack_v::round_up_align(vecgeom::cxx::DevicePtr<Geant::cuda::GeantTrack_v>::SizeOf());
  // Since fPathV and fNextpathV are internal pointer, we need to fix them up.
  // assert(vecgeom::cuda::NavigationState::SizeOfInstance(fMaxDepth)
  //        == vecgeom::cxx::NavigationState::SizeOfInstance(fMaxDepth) );

  long offset = ((const char *)dest->Buffer()) - (((const char *)source.GetPtr()) + bufferOffset);
  for (int hostIdx = 0; hostIdx < dest->GetNtracks(); ++hostIdx) {
    // Technically this offset is a 'guess' and depends on the
    // host (cxx) and device (cuda) GeantTrack_v to be strictly aligned.
    if (dest->fPathV[hostIdx])
      dest->fPathV[hostIdx] = (VolumePath_t *)(((char *)dest->fPathV[hostIdx]) + offset);
    if (dest->fNextpathV[hostIdx])
      dest->fNextpathV[hostIdx] = (VolumePath_t *)(((char *)dest->fNextpathV[hostIdx]) + offset);
  }
}

bool FromDevice(cxx::GeantTrack_v *dest, vecgeom::cxx::DevicePtr<cuda::GeantTrack_v> source, cudaStream_t stream) {
  size_t bufferOffset = GeantTrack_v::round_up_align(vecgeom::cxx::DevicePtr<Geant::cuda::GeantTrack_v>::SizeOf());
  // fMaxtracks, fMaxDepth and fBufSize ought to be invariant.
  GEANT_CUDA_ERROR(cudaMemcpyAsync(dest,
                                   source.GetPtr(),
                                   sizeof(int)*2+sizeof(Bool_t)*2,
                                   cudaMemcpyDeviceToHost, stream));
  fprintf(stderr,"Posting the copy from device=%p to host=%p and size=%lu\n",
          ((char*)source.GetPtr()) + bufferOffset,
          dest->Buffer(),
          dest->BufferSize());
  GEANT_CUDA_ERROR(cudaMemcpyAsync(dest->Buffer(),
                                   ((char*)source.GetPtr()) + bufferOffset,
                                   dest->BufferSize(),
                                   cudaMemcpyDeviceToHost, stream));
  return true;
}
#endif
#endif

} // Geant
