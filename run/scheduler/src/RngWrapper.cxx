#include "Geant/RngWrapper.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

RngState_s* RngWrapper::GenerateState(size_t streamId)
{
  scalarRNG->Initialize(streamId);
  return scalarRNG->GetState();
}

void RngWrapper::Join(RngState_s** trackRngState,  int start)
{
  for( int i= 0; i < kVecLenD ; ++i ) {
    assert( trackRngState[start+i] != nullptr );
    vectorRNG->SetStateAt(i, trackRngState[start+i]);
  }
}

void RngWrapper::Split(RngState_s** trackRngState,  int start)
{
  for( int i= 0; i < kVecLenD ; ++i ) {
    assert( trackRngState[start+i] != nullptr );
    *trackRngState[start+i] = vectorRNG->GetStateAt(i);
  }
}

} // namespace GEANT_IMPL_NAMESPACE
} // namespace geant
