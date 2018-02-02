#include "TARCPrimaryGenerator.h"

using namespace vecgeom;
using namespace geantphysics;
using namespace geant;

namespace tarc {
  TARCPrimaryGenerator::TARCPrimaryGenerator( const TARCGeometryConstruction *geomTARC) : fGeomSetup(geomTARC) {
    fPrimaryParticleName          = "proton";
    fPrimaryPerEvent              = 1;
    fParticle                     = nullptr;
    fPDG                          = 0;
    fGVPartIndex                  = 0;
    fPrimaryKE                    = 0.3*geant::GeV;
    fXPos                         = 0.0;
    fYPos                         = 0.0;
    fZPos                         = 0.0;
    fXDir                         = 0.0;
    fYDir                         = 0.0;
    fZDir                         = 1.0;
    fMass                         = 0.0;
    fCharge                       = 0.0;
    fEnergyTotal                  = 0.0;
    fMomentumTotal                = 0.0;
  }

  TARCPrimaryGenerator::~TARCPrimaryGenerator() {}

  void TARCPrimaryGenerator::InitPrimaryGenerator() {
    fParticle            = geantphysics::Particle::GetParticleByName( fPrimaryParticleName );
    if ( !fParticle ) {
      std::cerr << " \n *** ERROR::TARCPrimaryGenerator::InitPrimaryGenerator()     \n "
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


    fXPos                = 0;
    fYPos                = 0;
    fZPos                = 0;

    fXDir                = 0.0;
    fYDir                = 0.0;
    fZDir                = 1.0;
  }

  Geant::GeantEventInfo TARCPrimaryGenerator::NextEvent() {
    Geant::GeantEventInfo current;
    current.ntracks      = fPrimaryPerEvent;
    current.xvert        = fXPos;
    current.yvert        = fYPos;
    current.zvert        = fZPos;
    return current;
  }

  void TARCPrimaryGenerator::GetTrack(int n, Geant::GeantTrack &gtrack ) {
    gtrack.SetPDG(fPDG);
    gtrack.SetGVcode( fGVPartIndex );
    /*
    gtrack.fXpos  = fXPos;
    gtrack.fYpos  = fYPos;
    gtrack.fZpos  = fZPos;
    gtrack.fXdir  = fXDir;
    gtrack.fYdir  = fYDir;
    gtrack.fZdir  = fZDir;
    gtrack.SetCharge( fCharge );
    gtrack.SetMass( fMass );
    gtrack.fE = fEnergyTotal;
    */
    gtrack.SetP(fMomentumTotal);

  }

} // namespace ends here
