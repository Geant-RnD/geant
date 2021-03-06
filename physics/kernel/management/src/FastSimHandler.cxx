
#include "Geant/FastSimHandler.h"

// from geantV
#include "Geant/Propagator.h"
#include "Geant/TaskData.h"
#include "Geant/Track.h"
#include "Geant/Basket.h"

// from realphysics
#include "Geant/Material.h"
#include "Geant/MaterialCuts.h"
#include "Geant/Region.h"
#include "Geant/Particle.h"

#include "Geant/PhysicsProcess.h"
#include "Geant/PhysicsManagerPerParticle.h"
#include "Geant/LightTrack.h"

namespace geantphysics {

FastSimHandler::FastSimHandler(int threshold, geant::Propagator *propagator) : geant::Handler(threshold, propagator)
{
  SetName("FastSim");
}

FastSimHandler::~FastSimHandler() {}

//
// The select method of the FastSimStage selected only those tracks that (1) that has any Fast Sim process assigned i.e.
// their PhysicsManagerPerParticle object is not null in the given region and (2) they have Fast Sim processes that can
// be used
//
void FastSimHandler::DoIt(geant::Track *track, geant::Basket &output, geant::TaskData *td)
{
  // ---
  // int numSecondaries = 0;
  // here we will get the MaterialCuts from the LogicalVolume
  const MaterialCuts *matCut = static_cast<const MaterialCuts *>(
      (const_cast<vecgeom::LogicalVolume *>(track->GetVolume())->GetMaterialCutsPtr()));
  // get the internal code of the particle
  int particleCode         = track->GVcode();
  const Particle *particle = Particle::GetParticleByInternalCode(particleCode);
  // get the PhysicsManagerPerParticle for this particle: will be nullptr if the particle has no any PhysicsProcess-es
  PhysicsManagerPerParticle *pManager = particle->GetPhysicsManagerPerParticlePerRegion(matCut->GetRegionIndex());
  // put some asserts here to make sure (1) that the partcile has any processes, (2) the particle has at least one
  // process with continuous parts.
  assert(pManager != nullptr);                             // (1)
  assert(pManager->GetListFastSimProcesses().size() != 0); // (2)
  //
  // invoke the along step action for this track
  LightTrack primaryLT;
  // we will use members:
  //  fMaterialCutCoupleIndex <==>  // current MaterialCuts index
  //  fKinE         <==>  fE-fMass  // kinetic energy; will be set to the new kinetic energy
  //  fGVcode       <==>  fGVcode   // internal particle code
  //  fStepLength   <==>  fStep     // current step length
  //  fEdep         <==>  fEdep     // init to 0.0; will be set to the current energy deposit
  primaryLT.SetMaterialCutCoupleIndex(matCut->GetIndex());
  primaryLT.SetKinE(track->E() - track->Mass());
  primaryLT.SetMass(track->Mass());
  primaryLT.SetGVcode(track->GVcode());
  primaryLT.SetStepLength(track->GetStep());
  primaryLT.SetEnergyDeposit(0.0);

  //
  // execute fast sim
  pManager->FastSimAction(primaryLT, track, td);

  /*
  // update Track
  double newEkin = primaryLT.GetKinE();
  track->SetMass(primaryLT.GetMass());
  track->SetE(newEkin + track->Mass());
  track->SetP(std::sqrt(newEkin * (newEkin + 2.0 * track->Mass())));
  track->SetEdep(track->Edep() + primaryLT.GetEnergyDeposit());
  */

  // I guess, I need to kill the track here?? Or should it be in the method above??

  track->SetE(track->Mass());
  track->SetP(0.);
  track->Kill();
  track->SetStage(geant::kSteppingActionsStage);
  output.AddTrack(track);
}

//______________________________________________________________________________
void FastSimHandler::DoIt(geant::Basket &input, geant::Basket &output, geant::TaskData *td)
{
  // For the moment just loop and call scalar DoIt
  geant::TrackVec_t &tracks = input.Tracks();
  for (auto track : tracks) {
    DoIt(track, output, td);
  }
}

} // namespace geantphysics
