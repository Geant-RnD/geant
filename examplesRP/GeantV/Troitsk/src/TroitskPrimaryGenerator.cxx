#include "TroitskPrimaryGenerator.h"

namespace userapplication {
  TroitskPrimaryGenerator::TroitskPrimaryGenerator(
    const TroitskGeometryConstruction *geomTroitsk) : fGeometry(geomTroitsk) {
    fPrimaryParticleName          = "proton";
    fPrimaryPerEvent              = 1;
    fParticle                     = nullptr;
    fPDG                          = 0;
    fGVPartIndex                  = 0;
    fPrimaryKE                    = 0.3*geant::GeV;
    fXPos                         = 0.0;
    fYPos                         = 0.0;
    fZPos                         = 0.0;
    fXDir                         = 1.0;
    fYDir                         = 0.0;
    fZDir                         = 0.0;
    fMass                         = 0.0;
    fCharge                       = 0.0;
    fEnergyTotal                  = 0.0;
    fMomentumTotal                = 0.0;
  }

  TroitskPrimaryGenerator::~TroitskPrimaryGenerator() {}

  void TroitskPrimaryGenerator::InitPrimaryGenerator() {
    fParticle            = geantphysics::Particle::GetParticleByName( fPrimaryParticleName );
    if ( !fParticle ) {
      std::cerr << " \n *** ERROR::TroitskPrimaryGenerator::InitPrimaryGenerator()     \n "
                << "        unknown particle name = " << fPrimaryParticleName <<   "\n "
                << std::endl;
      exit(-1);
    }
    fPDG                 = fParticle->GetPDGCode();
    fGVPartIndex         = fParticle->GetInternalCode();
    fMass                = fParticle->GetPDGMass();
    fCharge              = fParticle->GetPDGCharge();
    fEnergyTotal         = fPrimaryKE + fMass;
    fMomentumTotal       = std::sqrt((fEnergyTotal - fMass) * ( fEnergyTotal + fMass));
    //fXPos
  }

  Geant::GeantEventInfo TroitskPrimaryGenerator::NextEvent() {
    Geant::GeantEventInfo current;
    current.ntracks      = fPrimaryPerEvent;
    current.xvert        = fXPos;
    current.yvert        = fYPos;
    current.zvert        = fZPos;
    return current;
  }

  void TroitskPrimaryGenerator::GetTrack(int, Geant::GeantTrack &gtrack ) {
    gtrack.SetPDG(fPDG);
    gtrack.SetGVcode( fGVPartIndex );
    gtrack.fXpos  = fXPos;
    gtrack.fYpos  = fYPos;
    gtrack.fZpos  = fZPos;
    gtrack.fXdir  = fXDir;
    gtrack.fYdir  = fYDir;
    gtrack.fZdir  = fZDir;
    gtrack.SetCharge( fCharge );
    gtrack.SetMass( fMass );
    gtrack.fE = fEnergyTotal;
    gtrack.SetP(fMomentumTotal);

  }

} // namespace ends here
