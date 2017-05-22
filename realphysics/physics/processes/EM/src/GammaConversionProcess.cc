
#include "GammaConversionProcess.h"

#include "Gamma.h"

namespace geantphysics {

GammaConversionProcess::GammaConversionProcess(const std::string &name) : EMPhysicsProcess(name) {
  // process type is kElectromagnetic in the base EMPhysicsProcess calss
  // set to be a discrete process
  SetIsDiscrete(true);
  // fill the list of particles that this process can be used to i.e. gamma particle
  AddToListParticlesAlloedToAssigned(Gamma::Definition());
}

}  // namespace geantphysics
