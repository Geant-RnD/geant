#ifndef GEANTV_RNGWRAPPER_H
#define GEANTV_RNGWRAPPER_H

#include <VecCore/VecCore>
#include "Geant/VectorTypes.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

// Global using declaration defining geant::RngObject_s and geant::RngObject_v

#define GEANT_RNG_MRG32K3A

#if defined(GEANT_RNG_MRG32K3A)

#include "VecMath/Rng/MRG32k3a.h"
using RngObject_s  = typename vecRng::MRG32k3a<vecRng::ScalarBackend>;
using RngObject_v  = typename vecRng::MRG32k3a<vecRng::VectorBackend>; 

#elif defined(GEANT_RNG_THREEFRY)

#include "VecMath/Rng/Threefry.h"
using RngObject_s  = typename vecRng::Threefry<vecRng::ScalarBackend>;
using RngObject_v  = typename vecRng::Threefry<vecRng::VectorBackend>; 

#elif defined(GEANT_RNG_PHILOX)

#include "VecMath/Rng/Philox.h"
using RngObject_s  = typename vecRng::Philox<vecRng::ScalarBackend>;
using RngObject_v  = typename vecRng::Philox<vecRng::VectorBackend>; 

#endif // Rng engine types

// Global using declaration defining geant::RngState_t and geant::RngStream_t

using RngState_s   = typename RngObject_s::State_t;
using RngState_v   = typename RngObject_v::State_t;
using RngStream_t  = size_t;

class RngWrapper {
public:
  RngWrapper()
  {
    void *buff     = vecCore::AlignedAlloc(32, sizeof(RngObject_s));
    scalarRNG = new (buff) RngObject_s;

    buff        = vecCore::AlignedAlloc(32, sizeof(RngObject_v));
    vectorRNG = new (buff) RngObject_v;

    scalarRNG->Initialize();
    vectorRNG->Initialize();
  }

  ~RngWrapper()
  {
    vecCore::AlignedFree(scalarRNG);
    vecCore::AlignedFree(vectorRNG);
  }

  GEANT_FORCE_INLINE
  RngStream_t GetStreamIndex(RngState_s *state) 
  { 
    return scalarRNG->template UniformIndex<vecRng::ScalarBackend>(state); 
  } 

  RngState_s const &GenerateState(size_t streamId);

  /** @brief Generate stream id from run/event/primary */
  RngStream_t GenerateStreamIndex(size_t /*run*/, size_t /*event*/, size_t primary)
  {
    return (RngStream_t)primary;
  }

  void PrintState(RngState_s const &state) { return scalarRNG->PrintState(state); }
  void PrintState() { return scalarRNG->PrintState(); }

  void PrintState(RngState_v const &state) { return vectorRNG->PrintState(state); }

  void Join(RngState_s** trackRngState,  int start); 

  void Split(RngState_s** trackRngState,  int start); 

  GEANT_FORCE_INLINE
  double uniform() { return scalarRNG->Uniform<vecCore::backend::Scalar>(); }

  GEANT_FORCE_INLINE
  double uniform(double a, double b) { return a + (b - a) * uniform(); }

  GEANT_FORCE_INLINE
  void uniform_array(size_t n, double *array, const double min = 0., const double max = 1.)
  {
    for (size_t i = 0; i < n; ++i) {
      array[i] = uniform(min, max);
    }
  }

  GEANT_FORCE_INLINE
  Double_v uniformV() { return vectorRNG->Uniform<VectorBackend>(); }

  double Gauss(double mean, double sigma) { return scalarRNG->Gauss<vecCore::backend::Scalar>(mean, sigma); }

private:
  RngObject_s *scalarRNG;
  RngObject_v *vectorRNG;
};

} // namespace GEANT_IMPL_NAMESPACE
} // namespace geant

#endif // GEANTV_VECRNGWRAPPER_H
