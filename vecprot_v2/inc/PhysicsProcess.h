//===--- PhysicsProcess.h - Geant-V -----------------------------*- C++ -*-===//
//
//                     Geant-V Prototype
//
//===----------------------------------------------------------------------===//
/**
 * @file PhysicsProcess.h
 * @brief Definition of physical processes used in Geant-V prototype
 * @details Toy physics processes for our propagator prototype.
 * Currently including:
 * 1. Single scattering as a discrete process;
 * 2. Energy loss as continuous process;
 * 3. Generic interaction as discrete process, producing secondaries.
 * @author Andrei Gheata
 */
//===----------------------------------------------------------------------===//

#ifndef GEANT_PHYSICSPROCESS
#define GEANT_PHYSICSPROCESS
#include "Geant/Config.h"
#include "Geant/Typedefs.h"

#ifndef GEANTV_MIC
#ifndef ROOT_TNamed
#include "TNamed.h"
#endif
#include "TMutex.h"
#else
#define MIC_BIT(n) (1ULL<<(n))
#endif
#include "base/Global.h"
#include "GeantFwd.h"


/**
 * @brief Class describing physics processes
 */
#ifndef GEANTV_MIC
class PhysicsProcess : public TNamed {
public:
  using GeantTrack_v = Geant::GeantTrack_v;
  using GeantTaskData = Geant::GeantTaskData;

  /**
   * @enum EProcessType
   * @brief Enum ProcessType definition
   */
  enum EProcessType { kDiscrete = BIT(14), kContinuous = BIT(15) };

public:
  /**
   * @brief PhysicsProcess constructor
   */
  PhysicsProcess() : TNamed() {}
  /**
   * @brief PhysicsProcess parametrized constructor
   *
   * @param name Name of physics process
   */
  PhysicsProcess(const char *name) : TNamed(name, "") {}

  /** @brief PhysicsProcess destructor */
  virtual ~PhysicsProcess() {}

  /**
   * @brief Function that check type
   *
   * @param type EProcessType type
   * @return Boolean value -> (bool) ((fBits & type) != 0);
   */
  bool IsType(EProcessType type) { return TObject::TestBit(type); }

  /** @brief Function of initialization */
  virtual void Initialize() {}

  /**
   * @brief Function that compute length of interaction ?????
   *
   * @param mat Material_t material
   * @param ntracks Number of tracks
   * @param tracks Vector of tracks_v
   * @param lengths Partial process lengths
   * @param td Thread data
   */
  virtual void ComputeIntLen(Material_t *mat, int ntracks, GeantTrack_v &tracks, double *lengths,
                             GeantTaskData *td) = 0;

  /**
   * @brief Function that provides posterior steps
   *
   * @param mat Material_t material
   * @param ntracks Number of tracks
   * @param tracks Vector of tracks_v
   * @param nout Number of surviving tracks
   * @param td Thread data
   */
  virtual void PostStep(Material_t *mat, int ntracks, GeantTrack_v &tracks, int &nout, GeantTaskData *td) = 0;

  /**
   * @brief Post step type of intraction sampling function
   * @details Sampling:
   * 1. Target atom and type of the interaction for each primary tracks
   * 2. All inf. regarding sampling output is stored in the tracks
   *
   * @param mat Material_t material
   * @param ntracks Number of tracks
   * @param tracks Vector of tracks_v
   * @param td  Thread data
   */
  virtual void PostStepTypeOfIntrActSampling(Material_t *mat, int ntracks, GeantTrack_v &tracks,
                                             GeantTaskData *td) = 0;

  /**
   * @brief Post step final state sampling function
   * @details Sampling final states for each primary tracks based on target atom and
   * interaction type sampled by PostStepTypeOfIntrActSampling;
   * updating primary track properties and inserting secondary tracks;
   * number of inserted secondary tracks will be stored in nout at termination;
   *
   * @param mat Material_t material
   * @param ntracks Number of tracks
   * @param tracks Vector of tracks_v
   * @param nout Number of tracks in the output
   * @param td Thread data
   */
  virtual void PostStepFinalStateSampling(Material_t *mat, int ntracks, GeantTrack_v &tracks, int &nout,
                                          GeantTaskData *td) = 0;

  /**
   * @todo  Need to be implemented
   */
  virtual void AtRest(int /*ntracks*/, GeantTrack_v & /*tracks*/, int & /*nout*/, GeantTaskData * /*td*/) {}
  GEANT_CUDA_DEVICE_CODE

  /**
   * @todo Need to be implemented
   */
  virtual void Eloss(Material_t * /*mat*/, int /*ntracks*/, GeantTrack_v & /*tracks*/, int & /*nout*/,
                     GeantTaskData * /*td*/) {}
  GEANT_CUDA_DEVICE_CODE

  /**
   * @todo Need to be implemented
   */
  virtual void ApplyMsc(Material_t * /*mat*/, int /*ntracks*/, GeantTrack_v & /*tracks*/, GeantTaskData * /*td*/) {}

  ClassDef(PhysicsProcess, 1) // Physics process base class
};

#else  // check GEANTV_MIC

class PhysicsProcess{
protected:
string fName;
private:
long int fBits;

public:
  using GeantTrack_v = Geant::GeantTrack_v;
  using GeantTaskData = Geant::GeantTaskData;

  /**
   * @enum EProcessType
   * @brief Enum ProcessType definition
   */
  enum EProcessType { kDiscrete = MIC_BIT(14), kContinuous = MIC_BIT(15) };

public:
  /**
   * @brief PhysicsProcess constructor
   */
  PhysicsProcess(){}
  /**
   * @brief PhysicsProcess parametrized constructor
   *
   * @param name Name of physics process
   */
  PhysicsProcess(const char *name):fName(name){}

  /** @brief PhysicsProcess destructor */
  virtual ~PhysicsProcess() {}


   void SetBit(int n){ std::cerr<<"SetBit not yet imlpemented\n";};
   

  /**
   * @brief Function that check type
   *
   * @param type EProcessType type
   * @return Boolean value -> (bool) ((fBits & type) != 0);
   */
  bool IsType(EProcessType type) { (bool) ((fBits & type) != 0); }

  /** @brief Function of initialization */
  virtual void Initialize() {}

  /**
   * @brief Function that compute length of interaction ?????
   *
   * @param mat Material_t material
   * @param ntracks Number of tracks
   * @param tracks Vector of tracks_v
   * @param lengths Partial process lengths
   * @param td Thread data
   */
  virtual void ComputeIntLen(Material_t *mat, int ntracks, GeantTrack_v &tracks, double *lengths,
                             GeantTaskData *td) = 0;
 /**
   * @brief Function that provides posterior steps
   *
   * @param mat Material_t material
   * @param ntracks Number of tracks
   * @param tracks Vector of tracks_v
   * @param nout Number of surviving tracks
   * @param td Thread data
   */
  virtual void PostStep(Material_t *mat, int ntracks, GeantTrack_v &tracks, int &nout, GeantTaskData *td) = 0;

  /**
   * @brief Post step type of intraction sampling function
   * @details Sampling:
   * 1. Target atom and type of the interaction for each primary tracks
   * 2. All inf. regarding sampling output is stored in the tracks
   *
   * @param mat Material_t material
   * @param ntracks Number of tracks
   * @param tracks Vector of tracks_v
   * @param td  Thread data
   */
  virtual void PostStepTypeOfIntrActSampling(Material_t *mat, int ntracks, GeantTrack_v &tracks,
                                             GeantTaskData *td) = 0;

  /**
   * @brief Post step final state sampling function
   * @details Sampling final states for each primary tracks based on target atom and
   * interaction type sampled by PostStepTypeOfIntrActSampling;
   * updating primary track properties and inserting secondary tracks;
   * number of inserted secondary tracks will be stored in nout at termination;
   *
   * @param mat Material_t material
   * @param ntracks Number of tracks
   * @param tracks Vector of tracks_v
   * @param nout Number of tracks in the output
   * @param td Thread data
   */
  virtual void PostStepFinalStateSampling(Material_t *mat, int ntracks, GeantTrack_v &tracks, int &nout,
                                          GeantTaskData *td) = 0;
 /**
   * @todo  Need to be implemented
   */
  virtual void AtRest(int /*ntracks*/, GeantTrack_v & /*tracks*/, int & /*nout*/, GeantTaskData * /*td*/) {}

  /**
   * @todo Need to be implemented
   */
  virtual void Eloss(Material_t * /*mat*/, int /*ntracks*/, GeantTrack_v & /*tracks*/, int & /*nout*/,
                     GeantTaskData * /*td*/) {}

  /**
   * @todo Need to be implemented
   */
  virtual void ApplyMsc(Material_t * /*mat*/, int /*ntracks*/, GeantTrack_v & /*tracks*/, GeantTaskData * /*td*/) {}

};

#endif
#endif
