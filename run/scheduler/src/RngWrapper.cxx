#include "Geant/RngWrapper.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

namespace RngProxy {

#if defined(GEANT_RNG_MRG32K3A)

// Recipes for generating new Rng states
void GenerateState(const RngSize_t /*pedegree_mother*/, const RngSize_t /*idaughter*/, RngState_s & /*state*/) {}

void GenerateState(RngState_s const & /*mother*/, RngState_s & /*state*/) {}

#elif defined(GEANT_OTHER_RNG)

// Recipes for generating new Rng states
void GenerateState(const RngSize_t /*pedegree_mother*/, const RngSize_t /*idaughter*/, RngState_s & /*state*/) {}

void GenerateState(RngState_s const & /*mother*/, RngState_s & /*state*/) {}

#endif // Rng engine types

} // namespace RngProxy
} // namespace GEANT_IMPL_NAMESPACE
} // namespace geant
