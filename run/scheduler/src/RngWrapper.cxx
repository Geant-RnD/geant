#include "Geant/RngWrapper.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

namespace RngProxy {

#if defined(GEANT_RNG_MRG32K3A)

// Recipes for gather/scatter of states
void Gather(RngState_s const **/*states*/, RngState_v & /*vstate*/) {}

void Scatter(RngState_v const & /*vstate*/, RngState_s **/*states*/) {}

// Recipes for generating new Rng states
void GenerateState(const RngSize_t /*pedegree_mother*/, const RngSize_t /*idaughter*/, RngState_s & /*state*/) {}

void GenerateState(RngState_s const & /*mother*/, RngState_s & /*state*/) {}

double Uniform(RngState_s &/*state*/)
{
  return 0.;
}

double Gauss(RngState_s &/*state*/, double /*mean*/, double /*sigma*/)
{
  return 0.;
}

void Uniform(RngState_v &/*state*/, Double_v &/*rng*/, Double_v /*min*/, Double_v /*max*/) {}

#elif defined(GEANT_OTHER_RNG)

// Recipes for generating new Rng states

#endif // Rng engine types

double Uniform(RngState_s &state, double a, double b)
{
  return a + (b - a) * RngProxy::Uniform(state);
}

void UniformArray(RngState_s &state, size_t n, double *array, const double min, const double max)
{
  for (size_t i = 0; i < n; ++i) {
    array[i] = RngProxy::Uniform(state, min, max);
  }
}

} // namespace RngProxy
} // namespace GEANT_IMPL_NAMESPACE
} // namespace geant
