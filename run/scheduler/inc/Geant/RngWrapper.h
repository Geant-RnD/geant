#ifndef GEANTV_RNGWRAPPER_H
#define GEANTV_RNGWRAPPER_H

#include <VecCoreLib/Rng.h>
#include <VecCore/VecCore>
#include "Geant/VectorTypes.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

// Global using declaration defining geant::RngState_t and geant::RngSize_t
#define GEANT_RNG_MRG32K3A

#if defined(GEANT_RNG_MRG32K3A)

using RngState_s = typename vecRng::MRG32k3a<vecCore::backend::Scalar>::State_t;
using RngState_v = typename vecRng::MRG32k3a<VectorBackend>::State_t;
using RngSize_t  = size_t;

namespace RngProxy {
/** @brief Gather kVecLenD scalar states into a vector state*/
void Gather(RngState_s **/*states*/, RngState_v & /*vstate*/);

/** @brief Gather kVecLenD scalar states into a vector state*/
void Scatter(RngState_v const &/*vstate*/, RngState_s **/*states*/);

/** @brief Generate a state by skipping forward from a previous state, given the index of the daughter */
GEANT_FORCE_INLINE
void GenerateState(RngState_s const & /*mother*/, RngState_s & /*state*/);

/** @brief Fill a vector state from an array of scalar states */
GEANT_FORCE_INLINE
void VectorState(RngState_s ** /*states*/, RngState_v & /*vstate*/);

/** @brief Generate a uniformly distributed random number between (0,1) from a given state **/
GEANT_FORCE_INLINE
double Uniform(RngState_s &state);

/** @brief Generate a uniformly distributed random number between (a,b) from a given state **/
GEANT_FORCE_INLINE
double Uniform(RngState_s &state, double a, double b);

/** @brief Generate an array of  uniformly distributed random numbers between (min,max) from a given state */
GEANT_FORCE_INLINE
void UniformArray(RngState_s &state, size_t n, double *array, const double min = 0., const double max = 1.);

/** @brief Generate Gaussian-distributed random number from a given state */
GEANT_FORCE_INLINE
double Gauss(RngState_s &state, double mean, double sigma);

// Vector case
void Uniform(RngState_v &state, Double_v &rng, Double_v min = Double_v(0.), Double_v max = Double_v(1.));

} // namespace RngProxy

#elif defined(GEANT_OTHER_RNG)

#endif // Rng engine types

class RngWrapper {
public:
  RngWrapper()
  {
    void *buff     = vecCore::AlignedAlloc(32, sizeof(vecRng::MRG32k3a<vecCore::backend::Scalar>));
    mrg32k3aScalar = new (buff) vecRng::MRG32k3a<vecCore::backend::Scalar>;

    buff        = vecCore::AlignedAlloc(32, sizeof(vecRng::MRG32k3a<VectorBackend>));
    mrg32k3aVec = new (buff) vecRng::MRG32k3a<VectorBackend>;

    mrg32k3aScalar->Initialize();
    mrg32k3aVec->Initialize();
  }

  ~RngWrapper()
  {
    mrg32k3aScalar->~MRG32k3a();
    vecCore::AlignedFree(mrg32k3aScalar);
    mrg32k3aVec->~MRG32k3a();
    vecCore::AlignedFree(mrg32k3aVec);
  }

  GEANT_FORCE_INLINE
  double uniform() { return mrg32k3aScalar->Uniform<vecCore::backend::Scalar>(); }

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
  Double_v uniformV() { return mrg32k3aVec->Uniform<VectorBackend>(); }

  double Gauss(double mean, double sigma) { return mrg32k3aScalar->Gauss<vecCore::backend::Scalar>(mean, sigma); }

private:
  vecRng::MRG32k3a<vecCore::backend::Scalar> *mrg32k3aScalar;
  vecRng::MRG32k3a<VectorBackend> *mrg32k3aVec;
};
} // namespace GEANT_IMPL_NAMESPACE
} // namespace geant

#endif // GEANTV_VECRNGWRAPPER_H
