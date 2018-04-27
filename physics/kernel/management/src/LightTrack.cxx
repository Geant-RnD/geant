
#include <assert.h>
#include <TString.h>
#include "Geant/LightTrack.h"

namespace geantphysics {

LightTrack::LightTrack()
    : fTrackStatus(LTrackStatus::kNew), fGVcode(-1), fGTrackIndex(-1), fMaterialCutCoupleIndex(-1), fProcessIndex(0),
      fTargetZ(0), fTargetN(0), fXdir(0.0), fYdir(0.0), fZdir(0.0), fKinE(-1.0), fMass(-1.0), fTime(-1.0), fWeight(1.0),
      fStepLength(0.0), fEdep(0.0), fNintLen(-1.0), fIntLen(0.0), fExtraInfo(nullptr)
{
}

LightTrack::LightTrack(const LTrackStatus aTrackStatus, const int aGVcode, const int aGTrackIndex,
                       const int aMaterialCutCoupleIndex, const int aProcessIndex, const int aTargetZ,
                       const int aTargetN, const double aXdir, const double aYdir, const double aZdir,
                       const double aKinE, const double aMass, const double aTime, const double aWeight,
                       const double aStepLength, const double aEdep, const double aNintLen, const double aIntLen,
                       ExtraInfo *aExtraInfo)
    : fTrackStatus(aTrackStatus), fGVcode(aGVcode), fGTrackIndex(aGTrackIndex),
      fMaterialCutCoupleIndex(aMaterialCutCoupleIndex), fProcessIndex(aProcessIndex), fTargetZ(aTargetZ),
      fTargetN(aTargetN), fXdir(aXdir), fYdir(aYdir), fZdir(aZdir), fKinE(aKinE), fMass(aMass), fTime(aTime),
      fWeight(aWeight), fStepLength(aStepLength), fEdep(aEdep), fNintLen(aNintLen), fIntLen(aIntLen),
      fExtraInfo(aExtraInfo)
{
}

LightTrack::LightTrack(const LightTrack &other)
    : fTrackStatus(other.fTrackStatus), fGVcode(other.fGVcode), fGTrackIndex(other.fGTrackIndex),
      fMaterialCutCoupleIndex(other.fMaterialCutCoupleIndex), fProcessIndex(other.fProcessIndex),
      fTargetZ(other.fTargetZ), fTargetN(other.fTargetN), fXdir(other.fXdir), fYdir(other.fYdir), fZdir(other.fZdir),
      fKinE(other.fKinE), fMass(other.fMass), fTime(other.fTime), fWeight(other.fWeight),
      fStepLength(other.fStepLength), fEdep(other.fEdep), fExtraInfo(other.fExtraInfo)
{
}

LightTrack &LightTrack::operator=(const LightTrack &other)
{
  if (this != &other) {
    fTrackStatus            = other.fTrackStatus;
    fGVcode                 = other.fGVcode;
    fGTrackIndex            = other.fGTrackIndex;
    fMaterialCutCoupleIndex = other.fMaterialCutCoupleIndex;
    fProcessIndex           = other.fProcessIndex;
    fTargetZ                = other.fTargetZ;
    fTargetN                = other.fTargetN;
    fXdir                   = other.fXdir;
    fYdir                   = other.fYdir;
    fZdir                   = other.fZdir;
    fKinE                   = other.fKinE;
    fMass                   = other.fMass;
    fTime                   = other.fTime;
    fWeight                 = other.fWeight;
    fStepLength             = other.fStepLength;
    fEdep                   = other.fEdep;
    SetExtraInfo(other.fExtraInfo);
  }
  return *this;
}

LightTrack::~LightTrack()
{
  // Note: we assume that the light track has the ownership of the extra information.
  if (fExtraInfo != nullptr) delete fExtraInfo;
}

//---------------------------------------
//--- LightTrack_v non-inline methods ---
//---------------------------------------

LightTrack_v::LightTrack_v() : fNtracks(0)
{
  for (int i = 0; i < kSOAMaxSize; ++i) {
    fExtraInfoV[i] = nullptr;
  }
  fXdirV = (double *)vecCore::AlignedAlloc(64, kSOAMaxSize * sizeof(double));
  fYdirV = (double *)vecCore::AlignedAlloc(64, kSOAMaxSize * sizeof(double));
  fZdirV = (double *)vecCore::AlignedAlloc(64, kSOAMaxSize * sizeof(double));
  fKinEV = (double *)vecCore::AlignedAlloc(64, kSOAMaxSize * sizeof(double));
  fEdepV = (double *)vecCore::AlignedAlloc(64, kSOAMaxSize * sizeof(double));
}

void LightTrack_v::GetTrack(const int i, LightTrack &aLightTrack) const
{
  assert(i < fNtracks);
  aLightTrack.SetTrackStatus(fTrackStatusV[i]);
  aLightTrack.SetGVcode(fGVcodeV[i]);
  aLightTrack.SetTrackIndex(fGTrackIndexV[i]);
  aLightTrack.SetMaterialCutCoupleIndex(fMaterialCutCoupleIndexV[i]);
  aLightTrack.SetProcessIndex(fProcessIndexV[i]);
  aLightTrack.SetTargetZ(fTargetZV[i]);
  aLightTrack.SetTargetN(fTargetNV[i]);
  aLightTrack.SetDirX(fXdirV[i]);
  aLightTrack.SetDirY(fYdirV[i]);
  aLightTrack.SetDirZ(fZdirV[i]);
  aLightTrack.SetKinE(fKinEV[i]);
  aLightTrack.SetMass(fMassV[i]);
  aLightTrack.SetTime(fTimeV[i]);
  aLightTrack.SetWeight(fWeightV[i]);
  aLightTrack.SetStepLength(fStepLengthV[i]);
  aLightTrack.SetEnergyDeposit(fEdepV[i]);
  aLightTrack.SetExtraInfo(fExtraInfoV[i]);
}
void LightTrack_v::SetTrack(const int i, const LightTrack &aLightTrack)
{
  assert(i < fNtracks);
  fTrackStatusV[i]            = aLightTrack.GetTrackStatus();
  fGVcodeV[i]                 = aLightTrack.GetGVcode();
  fGTrackIndexV[i]            = aLightTrack.GetTrackIndex();
  fMaterialCutCoupleIndexV[i] = aLightTrack.GetMaterialCutCoupleIndex();
  fProcessIndexV[i]           = aLightTrack.GetProcessIndex();
  fTargetZV[i]                = aLightTrack.GetTargetZ();
  fTargetNV[i]                = aLightTrack.GetTargetN();
  fXdirV[i]                   = aLightTrack.GetDirX();
  fYdirV[i]                   = aLightTrack.GetDirY();
  fZdirV[i]                   = aLightTrack.GetDirZ();
  fKinEV[i]                   = aLightTrack.GetKinE();
  fMassV[i]                   = aLightTrack.GetMass();
  fTimeV[i]                   = aLightTrack.GetTime();
  fWeightV[i]                 = aLightTrack.GetWeight();
  fStepLengthV[i]             = aLightTrack.GetStepLength();
  fEdepV[i]                   = aLightTrack.GetEnergyDeposit();
  fExtraInfoV[i]              = aLightTrack.GetExtraInfo();
}

void LightTrack_v::AddTrack(LightTrack &aLightTrack)
{
  int itrack                       = fNtracks;
  fTrackStatusV[itrack]            = aLightTrack.GetTrackStatus();
  fGVcodeV[itrack]                 = aLightTrack.GetGVcode();
  fGTrackIndexV[itrack]            = aLightTrack.GetTrackIndex();
  fMaterialCutCoupleIndexV[itrack] = aLightTrack.GetMaterialCutCoupleIndex();
  fProcessIndexV[itrack]           = aLightTrack.GetProcessIndex();
  fTargetZV[itrack]                = aLightTrack.GetTargetZ();
  fTargetNV[itrack]                = aLightTrack.GetTargetN();
  fXdirV[itrack]                   = aLightTrack.GetDirX();
  fYdirV[itrack]                   = aLightTrack.GetDirY();
  fZdirV[itrack]                   = aLightTrack.GetDirZ();
  fKinEV[itrack]                   = aLightTrack.GetKinE();
  fMassV[itrack]                   = aLightTrack.GetMass();
  fTimeV[itrack]                   = aLightTrack.GetTime();
  fWeightV[itrack]                 = aLightTrack.GetWeight();
  fStepLengthV[itrack]             = aLightTrack.GetStepLength();
  fEdepV[itrack]                   = aLightTrack.GetEnergyDeposit();
  fExtraInfoV[itrack]              = aLightTrack.GetExtraInfo();
  fNtracks++;
}

LightTrack_v::~LightTrack_v()
{
  for (int i = 0; i < kSOAMaxSize; ++i) {
    if (fExtraInfoV[i]) delete fExtraInfoV[i];
  }
  vecCore::AlignedFree(fXdirV);
  vecCore::AlignedFree(fYdirV);
  vecCore::AlignedFree(fZdirV);
  vecCore::AlignedFree(fKinEV);
  vecCore::AlignedFree(fEdepV);
}

} // namespace geantphysics
