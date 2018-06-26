
#include "Geant/FissionProcess.h"

namespace geantphysics {

FissionProcess::FissionProcess(const std::string &name) : HadronicProcess(name)
{
  // set process type to be an Fission process
  SetType(HadronicProcessType::kFission);
  // set to be a continuous-discrete process
  SetIsContinuous(false);
  SetIsDiscrete(true);
  // fill the list of particles that this process can be used to
  AddToListParticlesAlloedToAssigned(Proton::Definition());
  AddToListParticlesAlloedToAssigned(Neutron::Definition());
  AddToListParticlesAlloedToAssigned(PionPlus::Definition());
  AddToListParticlesAlloedToAssigned(PionMinus::Definition());
  AddToListParticlesAlloedToAssigned(PionZero::Definition());
  AddToListParticlesAlloedToAssigned(KaonPlus::Definition());
  AddToListParticlesAlloedToAssigned(KaonMinus::Definition());
  AddToListParticlesAlloedToAssigned(KaonZero::Definition());
  AddToListParticlesAlloedToAssigned(KaonShort::Definition());
  AddToListParticlesAlloedToAssigned(KaonLong::Definition());
}

} // namespace geantphysics
