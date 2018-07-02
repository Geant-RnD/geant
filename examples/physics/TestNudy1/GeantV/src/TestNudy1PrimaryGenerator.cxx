
#include "TestNudy1PrimaryGenerator.h"

#include "TestNudy1DetectorConstruction.h"
#include "Geant/Particle.h"

// from geantV
#include "Geant/Track.h"

#include <cmath>

namespace userapplication {

TestNudy1PrimaryGenerator::TestNudy1PrimaryGenerator(const TestNudy1DetectorConstruction *det) : fDetector(det)
{
  fPrimaryParticleName = "neutron";
  fPrimaryPerEvent     = 1;
  fParticle            = nullptr;
  //
  fPDG         = 0;
  fGVPartIndex = 0;
  //
  fPrimaryEkin = 15 * geant::units::MeV;
  //
  fXPos = 0.;
  fYPos = 0.;
  fZPos = 0.;
  //
  fXDir = 1.;
  fYDir = 0.;
  fZDir = 0.;
  //
  fMass   = 0.;
  fCharge = 0.;
  fETotal = 0.;
  fPTotal = 0.;
}

TestNudy1PrimaryGenerator::~TestNudy1PrimaryGenerator()
{
}

void TestNudy1PrimaryGenerator::InitPrimaryGenerator()
{
  fParticle = geantphysics::Particle::GetParticleByName(fPrimaryParticleName);
  if (!fParticle) {
    std::cerr << "   \n *** ERROR::TestNudy1PrimaryGenerator::InitPrimaryGenerator()    \n"
              << "          unknown particle name = " << fPrimaryParticleName << " \n"
              << std::endl;
    exit(-1);
  }
  fPDG         = fParticle->GetPDGCode();
  fGVPartIndex = fParticle->GetInternalCode();
  fMass        = fParticle->GetPDGMass();
  fCharge      = fParticle->GetPDGCharge();
  fETotal      = fPrimaryEkin + fMass;
  fPTotal      = std::sqrt((fETotal - fMass) * (fETotal + fMass));
  //
  fXPos = 0.5 * (fDetector->GetWorldXStart() + fDetector->GetTargetXStart());
  fYPos = 0.;
  fZPos = 0.;
  //
  fXDir = 1.;
  fYDir = 0.;
  fZDir = 0.;
}

geant::EventInfo TestNudy1PrimaryGenerator::NextEvent(geant::TaskData * /*td*/)
{
  geant::EventInfo current;
  current.ntracks = fPrimaryPerEvent;
  current.xvert   = fXPos;
  current.yvert   = fYPos;
  current.zvert   = fZPos;
  return current;
}

void TestNudy1PrimaryGenerator::GetTrack(int /*n*/, geant::Track &gtrack, geant::TaskData * /*td*/)
{
  gtrack.SetPDG(fPDG);
  gtrack.SetGVcode(fGVPartIndex);
  gtrack.SetPosition(fXPos, fYPos, fZPos);
  gtrack.SetDirection(fXDir, fYDir, fZDir);
  //
  gtrack.SetCharge(fCharge);
  gtrack.SetMass(fMass);
  gtrack.SetE(fETotal);
  gtrack.SetP(fPTotal);
}

} // namespace userapplication
