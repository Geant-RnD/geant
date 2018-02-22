// See also GeantCudaUtils.cxx

#include "GeantCudaUtils.h"
#include "Propagator.h"
#include "Track.h"
#include "GeantConfig.h"
#include "globals.h"

namespace geant {
inline namespace cuda {

void CoprocessorBrokerInitConstant()
{
  double tolerance = 1e-7;
  GEANT_CUDA_ERROR(
      cudaMemcpyToSymbol(device_constant::gTolerance, &(tolerance), sizeof(double), size_t(0), cudaMemcpyHostToDevice));
}

} // cuda
} // Geant
