
#ifndef LIGHT_TRACK
#define LIGHT_TRACK

#include <atomic>
#include <Geant/RngWrapper.h>
#include <Geant/VectorTypes.h>

namespace geantphysics {

/**
 * @brief  Light track for the physics of Geant-V prototype
 * @class  LightTrack
 * @author M Novak, A Ribon
 * @date   november 2015
 *
 * As the name suggests, this class is supposed to be a smaller, lighter version of Track, to be used by the
 * physics processes. It is foreseen that different processes, in particular electromagnetic and hadronic ones, need
 * different information: this should be placed in a new object, whose class should be inherited from the (now empty)
 * ExtraInfo class, and the pointer to this object is kept in the LightTrack.
 */

/**
 * @brief Class ExtraInfo : an empty class to inherit from in order
 *                          to put extra information.
 */
class ExtraInfo {
}; // Base class for any "ExtraInfo" class

enum class LTrackStatus {
  kAlive,        /** track is alive i.e. has enough kinetic energy to track further */
  kStopButAlive, /** stopped but need to check if it has any at-rest interaction */
  kKill,         /** track reached the end of its life */
  kNew           /** new track */
};

/**
 * @brief Class LightTrack
 */
class LightTrack {
private:
  LTrackStatus fTrackStatus = LTrackStatus::kNew;   /** Status of the track */
  int fGVcode = -1;                 /** GV particle code */
  int fGTrackIndex = -1;            /** Index of the track in the Track_v vector */
  int fMaterialCutCoupleIndex = -1; /** Index of the material-cut couple where the track is */
  int fProcessIndex = 0;           /** Index of the selected process (in the list of
                                   active discrete or at-rest processes kept by the
                                   PhysicsManagerPerParticle; negative if the winner
                                   is a continuous process) */
  int fTargetZ = 0;                /** Atomic number (Z) of the target atom where the
                                   particle has a physical interaction */
  int fTargetN = 0;                /** Number of nucleons of the target atom where the
                                   particle has a physical interaction */

  double fXdir = 0.;       /** X direction (normalized, adimensional) of the particle */
  double fYdir = 0.;       /** Y direction (normalized, adimensional) of the particle */
  double fZdir = 0.;       /** Z direction (normalized, adimensional) of the particle */
  double fKinE = -1.;       /** Kinetic energy (in GeV) of the particle */
  double fLogKinE = -1.;    /** Logarithm of kinetic energy (in GeV) of the particle */
  double fMass = -1.;       /** Dynamic mass (in GeV) of the particle */
  double fTime = -1.;       /** Time (global, in sec) of the particle */
  double fWeight = 1.;     /** Weight (adimensional) of the particle */
  double fStepLength = 0.; /** True, physical length (in cm) of the last step of the particle */
  double fEdep = 0.;       /** Energy deposit (in GeV) in the last step of the particle */
                      /** Note: in the future we could add the non-ionizing energy deposit
                                in the last step of the particle */
  double fNintLen = -1.;    /** Number of discrete interaction left */
  double fIntLen = 0.;     /** Total mean free path i.e. macroscopic cross section */
  geant::RngState_s *fRngState = nullptr; /** Rng state */

  ExtraInfo *fExtraInfo = nullptr; /** Pointer to an arbitrary struct to keep extra information */

public:
  /** @brief LightTrack default constructor */
  LightTrack();

  /** @brief LightTrack complete constructor */
  LightTrack(const LTrackStatus aTrackStatus, const int aGVcode, const int aGTrackIndex,
             const int aMaterialCutCoupleIndex, const int aProcessIndex, const int aTargetZ, const int aTargetN,
             const double aXdir, const double aYdir, const double aZdir, const double aKinE, const double aLogKinE,
             const double aMass, const double aTime, const double aWeight, const double aStepLength, const double aEdep,
             const double aNintLen, const double aIntLen, geant::RngState_s *state, ExtraInfo *aExtraInfo = 0);

  /** @brief LightTrack copy constructor */
  LightTrack(const LightTrack &other);

  /** @brief Operator = */
  LightTrack &operator=(const LightTrack &other);

  /** @brief LightTrack destructor */
  ~LightTrack();

  //--- Getters ---

  /** @brief Method that returns the track status */
  LTrackStatus GetTrackStatus() const { return fTrackStatus; }

  /** @brief Method that returns the GV particle code */
  int GetGVcode() const { return fGVcode; }

  /** @brief Method that returns the track index */
  int GetTrackIndex() const { return fGTrackIndex; }

  /** @brief Method that returns the material-cut couple index */
  int GetMaterialCutCoupleIndex() const { return fMaterialCutCoupleIndex; }

  /** @brief Method that returns the process index */
  int GetProcessIndex() const { return fProcessIndex; }

  /** @brief Method that returns the target atomic number, Z */
  int GetTargetZ() const { return fTargetZ; }

  /** @brief Method that returns the target number of nucleons */
  int GetTargetN() const { return fTargetN; }

  /** @brief Method that returns the X direction value (normalized, adimensional) */
  double GetDirX() const { return fXdir; }

  /** @brief Method that returns the Y direction value (normalized, adimensional) */
  double GetDirY() const { return fYdir; }

  /** @brief Method that returns the Z direction value (normalized, adimensional) */
  double GetDirZ() const { return fZdir; }

  /** @brief Method that returns the kinetic energy (unit: energy) */
  double GetKinE() const { return fKinE; }

  /** @brief Method that returns the logarithm of the kinetic energy */
  double GetLogKinE() const { return fLogKinE; }

  /** @brief Method that returns the dynamic mass (unit: energy) */
  double GetMass() const { return fMass; }

  /** Method that returns the global time (unit: time) */
  double GetTime() const { return fTime; }

  /** Method that returns the weight (adimensional) */
  double GetWeight() const { return fWeight; }

  /** Method that returns the true, physical length of the last step (unit: length) */
  double GetStepLength() const { return fStepLength; }

  /** Method that returns the energy deposit in the last step (unit: energy) */
  double GetEnergyDeposit() const { return fEdep; }

  /** Method to get the number of interaction length left. */
  double GetNumOfInteractionLegthLeft() const { return fNintLen; }

  /** Method to get the total mean free path i.e. the macroscopic cross section . */
  double GetTotalMFP() const { return fIntLen; }

  /** Method to get the RngState of the track */
  geant::RngState_s *GetRngState() const { return fRngState; }

  /** Method that returns the pointer to extra information */
  ExtraInfo *GetExtraInfo() const { return fExtraInfo; }

  //--- Setters ---

  /**
   * @brief Method that sets the GV particle code
   * @param aGVcode GV particle code
   */
  void SetGVcode(const int aGVcode) { fGVcode = aGVcode; }

  /**
   * @brief Method that sets the track index
   * @param aTrackIndex track index
   */
  void SetTrackIndex(const int aTrackIndex) { fGTrackIndex = aTrackIndex; }

  /**
   * @brief Method that sets the material-cut couple index
   * @param aMaterialCutCoupleIndex material-cut couple index
   */
  void SetMaterialCutCoupleIndex(const int aMaterialCutCoupleIndex)
  {
    fMaterialCutCoupleIndex = aMaterialCutCoupleIndex;
  }

  /**
   * @brief Method that sets the track status
   * @param aTrackStatus track status
   */
  void SetTrackStatus(const LTrackStatus aTrackStatus) { fTrackStatus = aTrackStatus; }

  /**
   * @brief Method that sets the process index
   * @param aProcessIndex process index
   */
  void SetProcessIndex(const int aProcessIndex) { fProcessIndex = aProcessIndex; }

  /**
   * @brief Method that sets the target atomic number, Z
   * @param aTargetZ target Z
   */
  void SetTargetZ(const int aTargetZ) { fTargetZ = aTargetZ; }

  /**
   * @brief Method that sets the target number of nucleons
   * @param aTargetN target number of nucleons
   */
  void SetTargetN(const int aTargetN) { fTargetN = aTargetN; }

  /**
   * @brief Method that sets direction
   * @param aXdir, aYdir, aZdir direction components (normalized, adimensional)
   */
  void SetDirection(const double aXdir, const double aYdir, const double aZdir)
  {
    fXdir = aXdir;
    fYdir = aYdir;
    fZdir = aZdir;
  }

  /**
   * @brief Method that sets the X direction value
   * @param aXdir X direction (normalized, adimensional)
   */
  void SetDirX(const double aXdir) { fXdir = aXdir; }

  /** @brief Method that sets the Y direction value
   * @param aYdir Y direction (normalized, adimensional)
   */
  void SetDirY(const double aYdir) { fYdir = aYdir; }

  /** @brief Method that sets the Z direction value
   * @param aZdir Z direction (normalized, adimensional)
   */
  void SetDirZ(const double aZdir) { fZdir = aZdir; }

  /**
   * @brief Method that sets the kinetic energy
   * @param aKinE kinetic energy (unit: energy)
   */
  void SetKinE(const double aKinE) { fKinE = aKinE; }

  /**
   * @brief Method that sets the logarithm of the kinetic energy
   * @param aLogKinE logarithm of kinetic energy
   */
  void SetLogKinE(const double aLogKinE) { fLogKinE = aLogKinE; }

  /**
   * @brief Method that sets the dynamic mass
   * @param aMass dynamic mass (unit: energy)
   */
  void SetMass(const double aMass) { fMass = aMass; }

  /**
   * @brief Method that sets the global time
   * @param aTime global time (unit: time)
   */
  void SetTime(const double aTime) { fTime = aTime; }

  /**
   * @brief Method that sets the weight
   * @param aWeight weight (adimensional)
   */
  void SetWeight(const double aWeight) { fWeight = aWeight; }

  /**
   * @brief Method that sets the true, physical length of the last step
   * @param aStepLength (unit: length)
   */
  void SetStepLength(const double aStepLength) { fStepLength = aStepLength; }

  /**
   * @brief Method that sets the energy deposit in the last step
   * @param aEdep (unit: energy)
   */
  void SetEnergyDeposit(const double aEdep) { fEdep = aEdep; }

  /**
   * @brief Method that sets the number of discrete interaction left.
   * @param[in] val Number of discrete interactions.
   */
  void SetN(const double val) { fIntLen = val; }

  /**
   * @brief Method that sets the number of interaction length left .
   * @param[in] val Number of interaction length left.
   */
  void SetNumOfInteractionLegthLeft(const double val) { fNintLen = val; }

  /**
   * @brief Method that sets the total mean free path i.e. the macroscopic cross section .
   * @param[in] val Value of the total mean free path in internal [1/length] unit.
   */
  void SetTotalMFP(const double val) { fIntLen = val; }

  /** @brief Method to set the RngState of the track */
  void SetRngState(geant::RngState_s *state) { fRngState = state; }
  
  /**
   * @brief Method that sets the pointer to extra information
   * @param aExtraInfo pointer to extra information
   */
  void SetExtraInfo(ExtraInfo *aExtraInfo)
  {
    // If the pointer is already set, and it is different from the new one,
    // then delete the old extra information before pointing to the new one.
    // Note: we assume that the light track has the ownership of the extra information.
    if (fExtraInfo != nullptr && fExtraInfo != aExtraInfo) delete fExtraInfo;
    fExtraInfo = aExtraInfo;
  }
};

/**
 * @brief SOA (Structure Of Arrays) for LightTrack
 *
 */
constexpr int kSOAMaxSize = 1024;

class LightTrack_v {
public:
  int fNtracks; /** number of tracks contained */

  LTrackStatus fTrackStatusV[kSOAMaxSize];   /** Status of the tracks */
  int fGVcodeV[kSOAMaxSize];                 /** GV particle codes */
  int fGTrackIndexV[kSOAMaxSize];            /** Indices of the tracks in the Track_v vector */
  int fMaterialCutCoupleIndexV[kSOAMaxSize]; /** Indices of the material-cut couples where the tracks are */
  int fProcessIndexV[kSOAMaxSize];           /** Indices of the selected processes (in the list of
                                     active discrete or at-rest processes kept by the
                                     PhysicsManagerPerParticle[kSOAMaxSize]; negative if the winner
                                     is a continuous process) */
  int fTargetZV[kSOAMaxSize];                /** Atomic numbers (Z) of the target atoms where the
                                     particles have a physical interaction */
  int fTargetNV[kSOAMaxSize];                /** Numbers of nucleons of the target atoms where the
                                     particles have a physical interaction */

  double *fXdirV;                   /** X directions (normalized, adimensional) of the particles */
  double *fYdirV;                   /** Y directions (normalized, adimensional) of the particles */
  double *fZdirV;                   /** Z directions (normalized, adimensional) of the particles */
  double *fKinEV;                   /** Kinetic energies (in GeV) of the particles */
  double *fLogKinEV;                /** Logarithms of kinetic energies of the particles */
  double *fEdepV;                   /** Energy deposits (in GeV) in the last step of the particles */
  double fMassV[kSOAMaxSize];       /** Dynamic masses (in GeV) of the particles */
  double fTimeV[kSOAMaxSize];       /** Times (global, in sec) of the particles */
  double fWeightV[kSOAMaxSize];     /** Weights (adimensional) of the particles */
  double fStepLengthV[kSOAMaxSize]; /** True, physical lengths (in cm) of the last step of the particles */
  double fNintLenV[kSOAMaxSize];    /** Number of discrete interaction left */
  double fIntLenV[kSOAMaxSize];     /** Total mean free path i.e. macroscopic scross section */
  short fSortKey[kSOAMaxSize];
  geant::RngState_s *fRngStates[kSOAMaxSize]; /** Array of scalar RNG states */
  geant::RngState_v fVectorRngState;       /** Vector Rng state */

  ExtraInfo *fExtraInfoV[kSOAMaxSize]; /** Pointers to arbitrary structs to keep extra information */

private:
  /** @brief Copy constructor (not allowed) */
  LightTrack_v(const LightTrack_v &track_v);

  /** @brief Operator= (not allowed) */
  LightTrack_v &operator=(const LightTrack_v &track_v);

public:
  /** @brief LightTrack_v constructor */
  LightTrack_v();

  /** @brief LightTrack_v destructor */
  ~LightTrack_v();

  void ClearTracks()
  {
    std::fill(fTrackStatusV, fTrackStatusV + fNtracks, LTrackStatus::kNew);
    fNtracks = 0;
  }
  int InsertTrack()
  {
    assert(fNtracks + 1 < kSOAMaxSize);
    return fNtracks++;
  }

  /** @brief Method that returns the number of tracks contained */
  int GetNtracks() const { return fNtracks; }

  /** @brief Method that set number of tracks contained */
  void SetNtracks(const int ntracks) { fNtracks = ntracks; }

  /**
   * @brief Method that returns a track
   * @param i Bit number 'i'
   * @param aLightTrack track to be returned
   */
  void GetTrack(const int i, LightTrack &aLightTrack) const;

  void SetTrack(const int i, const LightTrack &aLightTrack);

  /**
   * @brief Method that add a track
   * @param aLightTrack track that should be added
   */
  void AddTrack(LightTrack &aLightTrack);

  //--- Getters ---

  /** @brief Method that returns the track status */
  LTrackStatus GetTrackStatus(int i) const { return fTrackStatusV[i]; }

  /** @brief Method that returns the GV particle code */
  int GetGVcode(int i) const { return fGVcodeV[i]; }

  /** @brief Method that returns the track index */
  int GetTrackIndex(int i) const { return fGTrackIndexV[i]; }

  /** @brief Method that returns the material-cut couple index */
  int GetMaterialCutCoupleIndex(int i) const { return fMaterialCutCoupleIndexV[i]; }

  /** @brief Method that returns the process index */
  int GetProcessIndex(int i) const { return fProcessIndexV[i]; }

  /** @brief Method that returns the target atomic number, Z */
  int GetTargetZ(int i) const { return fTargetZV[i]; }

  /** @brief Method that returns the target number of nucleons */
  int GetTargetN(int i) const { return fTargetNV[i]; }

  /** @brief Method that returns the X direction value (normalized, adimensional) */
  double GetDirX(int i) const { return fXdirV[i]; }

  geant::Double_v GetDirXVec(int i) const
  {
    geant::Double_v tmp;
    vecCore::Load(tmp, &fXdirV[i]);
    return tmp;
  }

  /** @brief Method that returns the Y direction value (normalized, adimensional) */
  double GetDirY(int i) const { return fYdirV[i]; }

  geant::Double_v GetDirYVec(int i) const
  {
    geant::Double_v tmp;
    vecCore::Load(tmp, &fYdirV[i]);
    return tmp;
  }

  /** @brief Method that returns the Z direction value (normalized, adimensional) */
  double GetDirZ(int i) const { return fZdirV[i]; }

  geant::Double_v GetDirZVec(int i) const
  {
    geant::Double_v tmp;
    vecCore::Load(tmp, &fZdirV[i]);
    return tmp;
  }

  /** @brief Method that returns the kinetic energy (unit: energy) */
  double GetKinE(int i) const { return fKinEV[i]; }

  geant::Double_v GetKinEVec(int i) const
  {
    geant::Double_v tmp;
    vecCore::Load(tmp, &fKinEV[i]);
    return tmp;
  }

  /** @brief Method that returns the logarithm of the kinetic energy */
  double GetLogKinE(int i) const { return fLogKinEV[i]; }

  geant::Double_v GetLogKinEVec(int i) const
  {
    geant::Double_v tmp;
    vecCore::Load(tmp, &fLogKinEV[i]);
    return tmp;
  }

  double *GetKinEArr(int i = 0) { return &fKinEV[i]; }

  /** @brief Method that returns the dynamic mass (unit: energy) */
  double GetMass(int i) const { return fMassV[i]; }

  /** Method that returns the global time (unit: time) */
  double GetTime(int i) const { return fTimeV[i]; }

  /** Method that returns the weight (adimensional) */
  double GetWeight(int i) const { return fWeightV[i]; }

  /** Method that returns the true, physical length of the last step (unit: length) */
  double GetStepLength(int i) const { return fStepLengthV[i]; }

  /** Method that returns the energy deposit in the last step (unit: energy) */
  double GetEnergyDeposit(int i) const { return fEdepV[i]; }

  /** Method to get the number of interaction length left. */
  double GetNumOfInteractionLegthLeft(int i) const { return fNintLenV[i]; }

  /** Method to get the total mean free path i.e. the macroscopic cross section . */
  double GetTotalMFP(int i) const { return fIntLenV[i]; }

  /** Method that returns the pointer to extra information */
  ExtraInfo *GetExtraInfo(int i) const { return fExtraInfoV[i]; }

  //--- Setters ---

  /**
   * @brief Method that sets the GV particle code
   * @param aGVcode GV particle code
   */
  void SetGVcode(const int aGVcode, int i) { fGVcodeV[i] = aGVcode; }

  /**
   * @brief Method that sets the track index
   * @param aTrackIndex track index
   */
  void SetTrackIndex(const int aTrackIndex, int i) { fGTrackIndexV[i] = aTrackIndex; }

  /**
   * @brief Method that sets the material-cut couple index
   * @param aMaterialCutCoupleIndex material-cut couple index
   */
  void SetMaterialCutCoupleIndex(const int aMaterialCutCoupleIndex, int i)
  {
    fMaterialCutCoupleIndexV[i] = aMaterialCutCoupleIndex;
  }

  /**
   * @brief Method that sets the track status
   * @param aTrackStatus track status
   */
  void SetTrackStatus(const LTrackStatus aTrackStatus, int i) { fTrackStatusV[i] = aTrackStatus; }

  /**
   * @brief Method that sets the process index
   * @param aProcessIndex process index
   */
  void SetProcessIndex(const int aProcessIndex, int i) { fProcessIndexV[i] = aProcessIndex; }

  /**
   * @brief Method that sets the target atomic number, Z
   * @param aTargetZ target Z
   */
  void SetTargetZ(const int aTargetZ, int i) { fTargetZV[i] = aTargetZ; }

  /**
   * @brief Method that sets the target number of nucleons
   * @param aTargetN target number of nucleons
   */
  void SetTargetN(const int aTargetN, int i) { fTargetNV[i] = aTargetN; }

  /**
   * @brief Method that sets direction
   * @param aXdir, aYdir, aZdir direction components (normalized, adimensional)
   */
  void SetDirection(const double aXdir, const double aYdir, const double aZdir, int i)
  {
    fXdirV[i] = aXdir;
    fYdirV[i] = aYdir;
    fZdirV[i] = aZdir;
  }

  /**
   * @brief Method that sets the X direction value
   * @param aXdir X direction (normalized, adimensional)
   */
  void SetDirX(const double aXdir, int i) { fXdirV[i] = aXdir; }

  /** @brief Method that sets the Y direction value
   * @param aYdir Y direction (normalized, adimensional)
   */
  void SetDirY(const double aYdir, int i) { fYdirV[i] = aYdir; }

  /** @brief Method that sets the Z direction value
   * @param aZdir Z direction (normalized, adimensional)
   */
  void SetDirZ(const double aZdir, int i) { fZdirV[i] = aZdir; }

  /**
   * @brief Method that sets the kinetic energy
   * @param aKinE kinetic energy (unit: energy)
   */
  void SetKinE(const double aKinE, int i) { fKinEV[i] = aKinE; }

  /**
   * @brief Method that sets the logarithm of the kinetic energy
   * @param aKinE kinetic energy (unit: energy)
   */
  void SetLogKinE(const double aLogKinE, int i) { fLogKinEV[i] = aLogKinE; }

  /**
   * @brief Method that sets the dynamic mass
   * @param aMass dynamic mass (unit: energy)
   */
  void SetMass(const double aMass, int i) { fMassV[i] = aMass; }

  /**
   * @brief Method that sets the global time
   * @param aTime global time (unit: time)
   */
  void SetTime(const double aTime, int i) { fTimeV[i] = aTime; }

  /**
   * @brief Method that sets the weight
   * @param aWeight weight (adimensional)
   */
  void SetWeight(const double aWeight, int i) { fWeightV[i] = aWeight; }

  /**
   * @brief Method that sets the true, physical length of the last step
   * @param aStepLength (unit: length)
   */
  void SetStepLength(const double aStepLength, int i) { fStepLengthV[i] = aStepLength; }

  /**
   * @brief Method that sets the energy deposit in the last step
   * @param aEdep (unit: energy)
   */
  void SetEnergyDeposit(const double aEdep, int i) { fEdepV[i] = aEdep; }

  void SetEnergyDepositVec(geant::Double_v aEdep, int i) { vecCore::Store(aEdep, &fEdepV[i]); }

  /**
   * @brief Method that sets the number of discrete interaction left.
   * @param[in] val Number of discrete interactions.
   */
  void SetN(const double val, int i) { fIntLenV[i] = val; }

  /**
   * @brief Method that sets the number of interaction length left .
   * @param[in] val Number of interaction length left.
   */
  void SetNumOfInteractionLegthLeft(const double val, int i) { fNintLenV[i] = val; }

  /**
   * @brief Method that sets the total mean free path i.e. the macroscopic cross section .
   * @param[in] val Value of the total mean free path in internal [1/length] unit.
   */
  void SetTotalMFP(const double val, int i) { fIntLenV[i] = val; }

  /**
   * @brief Method that sets the pointer to extra information
   * @param aExtraInfo pointer to extra information
   */
  void SetExtraInfo(ExtraInfo *aExtraInfo, int i)
  {
    // If the pointer is already set, and it is different from the new one,
    // then delete the old extra information before pointing to the new one.
    // Note: we assume that the light track has the ownership of the extra information.
    if (fExtraInfoV[i] != nullptr && fExtraInfoV[i] != aExtraInfo) delete fExtraInfoV[i];
    fExtraInfoV[i] = aExtraInfo;
  }

  void Uniform(geant::Double_v const &min, geant::Double_v const &max, geant::Double_v &rng, size_t stride = 0)
  {
    geant::RngProxy::Gather(fRngStates + stride, fVectorRngState);
    geant::RngProxy::Uniform(fVectorRngState, rng, min, max);
    geant::RngProxy::Scatter(fVectorRngState, fRngStates + stride);
  }

  short *GetSortKeyV() { return &fSortKey[0]; }

  int SortTracks()
  {
    int ones  = fNtracks - 1;
    int zeros = 0;
    while (zeros <= ones) {
      if (fSortKey[zeros] == 0) {
        ++zeros;
        continue;
      }
      if (fSortKey[ones] == 1) {
        --ones;
        continue;
      }
      if (fSortKey[zeros] > fSortKey[ones]) {
        SwapTracks(zeros, ones);
        ++zeros;
        --ones;
      }
    }

    return zeros;
  }

private:
  void SwapTracks(int i, int j)
  {
    std::swap(fTrackStatusV[i], fTrackStatusV[j]);
    std::swap(fGVcodeV[i], fGVcodeV[j]);
    std::swap(fGTrackIndexV[i], fGTrackIndexV[j]);
    std::swap(fMaterialCutCoupleIndexV[i], fMaterialCutCoupleIndexV[j]);
    std::swap(fProcessIndexV[i], fProcessIndexV[j]);
    std::swap(fTargetZV[i], fTargetZV[j]);
    std::swap(fTargetNV[i], fTargetNV[j]);
    std::swap(fXdirV[i], fXdirV[j]);
    std::swap(fYdirV[i], fYdirV[j]);
    std::swap(fZdirV[i], fZdirV[j]);
    std::swap(fKinEV[i], fKinEV[j]);
    std::swap(fLogKinEV[i], fLogKinEV[j]);
    std::swap(fMassV[i], fMassV[j]);
    std::swap(fTimeV[i], fTimeV[j]);
    std::swap(fWeightV[i], fWeightV[j]);
    std::swap(fStepLengthV[i], fStepLengthV[j]);
    std::swap(fEdepV[i], fEdepV[j]);
    std::swap(fNintLenV[i], fNintLenV[j]);
    std::swap(fIntLenV[i], fIntLenV[j]);
    std::swap(fSortKey[i], fSortKey[j]);
    std::swap(fExtraInfoV[i], fExtraInfoV[j]);
  }
};

} // end of namespace geantphysics

#endif
