#include "TARCPrimaryGenerator.h"

using namespace vecgeom;
using namespace geantphysics;
using namespace geant;
using namespace tarcapp;

const int TARCPrimaryGenerator::gInfNumPrimaryPerEvt = 1;
const int TARCPrimaryGenerator::gSupNumPrimaryPerEvt = 10000000;
const double TARCPrimaryGenerator::gInfBeamEnergy    = 0.0001 * geant::eV;
const double TARCPrimaryGenerator::gSupBeamEnergy    = 100.0  * geant::GeV;

int TARCPrimaryGenerator::gNumPrimaryPerEvt(-1);
double TARCPrimaryGenerator::gPrimaryEnergy(-1.0);
std::string TARCPrimaryGenerator::gPrimaryType("");
double TARCPrimaryGenerator::gPrimaryDir[3] = {0.0, 0.0, 0.0};

const int TARCPrimaryGenerator::gNumberCandidateParticles = 5;

const std::string TARCPrimaryGenerator::gNameParticlesVector[TARCPrimaryGenerator::gNumberCandidateParticles] = {
  "e-",
  "e+",
  "gamma",
  "p+",
  "n0"
};

const std::map<std::string, int> TARCPrimaryGenerator::gPrimaNameToIndexMap = {
  {"e-",      0},
  {"e+",      1},
  {"gamma",   2},
  {"p+",      3},
  {"n0",      4}
};


//TARCPrimaryGenerator::TARCPrimaryGenerator( const TARCGeometryConstruction *geomTARC) : fGeomSetup(geomTARC) {
TARCPrimaryGenerator::TARCPrimaryGenerator() {
  fIsUserNumPrimaryPerEvt       = false;
  fIsUserPrimaryType            = false;
  fIsUserPrimaryDir             = false;
  fIsUserPrimaryEnergy          = false;
  fNumPrimaryPerEvt             = 1;
  //fPrimaryParticleName          = "e-"; //  ----------> make protons
  fPrimaryParticleEnergy        = 1.0 * geant::GeV;


  fPrimaryParticleName          = "p+";
  fPDG                          = 0;
  fGVPartIndex                  = 0;
  fPrimaryKE                    = 1.0*geant::GeV;
  fXPos                         = 0.0;
  fYPos                         = 0.0;
  fZPos                         = 0.0;
  fXDir                         = 0.0;
  fYDir                         = 0.0;
  fZDir                         = 1.0;
}

TARCPrimaryGenerator::~TARCPrimaryGenerator() {}

/*
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
  */

// Set number of primary vertex and fixed vertex position. Provide these info to event server.
Geant::GeantEventInfo TARCPrimaryGenerator::NextEvent(Geant::GeantTaskData* td) {
  Geant::GeantEventInfo current;
  int numPrimaryPerEvt = fNumPrimaryPerEvt;
  if (!fIsUserNumPrimaryPerEvt){
    int idum         = static_cast<int>(td->fRndm->uniform()*(gSupNumPrimaryPerEvt - gInfNumPrimaryPerEvt + 1));
    numPrimaryPerEvt = std::max(gInfNumPrimaryPerEvt + idum, 1);
  }
  current.ntracks      = fNumPrimaryPerEvt;  // primary vertex is always fixed at (0, 0, 0)
  current.xvert        = fXPos;
  current.yvert        = fYPos;
  current.zvert        = fZPos;
  return current;
}

void TARCPrimaryGenerator::GetTrack(int n, Geant::GeantTrack &gtrack, Geant::GeantTaskData* td ) {
  // Select primary particle randomly if not set by the user
  std::string pParticleName = fPrimaryParticleName;
  if (!fIsUserPrimaryType){
    int caseBeamParticle = static_cast<int>(td->fRndm->uniform()*TARCPrimaryGenerator::gNumberCandidateParticles);
    pParticleName        = gNameParticlesVector[caseBeamParticle];
  }

    // Select beam energy randomly if not set by the user.
  double pParticleEkin = fPrimaryParticleEnergy;
  if (!fIsUserPrimaryEnergy){
    pParticleEkin = gInfBeamEnergy + td->fRndm->uniform() * (gSupBeamEnergy - gInfBeamEnergy);
  }

  // Select direction randomly if not set by the user.
  double pParticleDir[3] = {fXDir, fYDir, fZDir};
  if (!fIsUserPrimaryDir){
    double *rndArray = td->fDblArray;
    td->fRndm->uniform_array(2, rndArray);
    const double cost = 1.0 - 2.0 * rndArray[0];
    double sint2 = 1.0 - cost * cost;
    sint2 = (sint2 < 0.0) ? 0.0 : sint2;
    const double sint = std::sqrt(sint2);
    const double phi  = rndArray[1] * geant::kTwoPi;
    pParticleDir[0] = sint * std::cos(phi);
    pParticleDir[1] = sint * std::sin(phi);
    pParticleDir[2] = cost;
  }

  // Get the particle
  const geantphysics::Particle* pParticle = geantphysics::Particle::GetParticleByName(pParticleName);
  if (!pParticle) {
    std::cerr << " Error:: TARCPrimaryGenerator::getTrack()  \n"
              << " unknown particle name = " << pParticle <<"\n"
              << std::endl;
    exit(-1);
  }

  const int pPartPDG        = pParticle->GetPDGCode();
  const int pPartGVc        = pParticle->GetInternalCode();
  const double pPartMass    = pParticle->GetPDGMass();
  const double pPartCharge  = pParticle->GetPDGCharge();
  const double pPartEt      = pParticleEkin + pPartMass;
  const double pPartPt      = std::sqrt((pPartEt - pPartMass) * ( pPartEt + pPartMass));

  // Set the primary track
  gtrack.SetPDG(pPartPDG);
  gtrack.SetGVcode( pPartGVc);
  gtrack.SetPosition(fXPos, fYPos, fZPos);
  gtrack.SetDirection(pParticleDir[0],pParticleDir[1],pParticleDir[2]);
  gtrack.SetCharge(pPartCharge);
  gtrack.SetMass(pPartMass);
  gtrack.SetE(pPartEt);
  gtrack.SetP(pPartPt);
}

void TARCPrimaryGenerator::SetNumberOfPrimaryPerEvent(const int primPerEvt){
  fNumPrimaryPerEvt       = primPerEvt;
  gNumPrimaryPerEvt       = fNumPrimaryPerEvt;
  fIsUserNumPrimaryPerEvt = true;
}

void TARCPrimaryGenerator::SetPrimaryEnergy(const double ekin) {
  fPrimaryParticleEnergy = ekin;
  gPrimaryEnergy         = fPrimaryParticleEnergy;
  fIsUserPrimaryEnergy   = true;
}

void TARCPrimaryGenerator::SetPrimaryName(const std::string& pname) {
  fPrimaryParticleName  = pname;
  gPrimaryType          = fPrimaryParticleName;
  fIsUserPrimaryType    = true;
}


void TARCPrimaryGenerator::SetGunPosition(const double rPos[3]) {
  fXPos = rPos[0];
  fYPos = rPos[1];
  fZPos = rPos[2];
}

void TARCPrimaryGenerator::SetPrimaryDirection(const double pdir[3]) {
  double norm = 1.0 /std::sqrt(pdir[0] * pdir[0] + pdir[1] * pdir[1] + pdir[2] * pdir[2]);
  fXDir              = pdir[0] * norm;
  fYDir              = pdir[1] * norm;
  fZDir              = pdir[2] * norm;
  gPrimaryDir[0]     = fXDir;
  gPrimaryDir[1]     = fYDir;
  gPrimaryDir[2]     = fZDir;
  fIsUserPrimaryDir  = true;
}

int TARCPrimaryGenerator::GetPrimaryTypeIndex(const std::string& pname){
  int index = gPrimaNameToIndexMap.find(pname)->second;
  return index;
}


// print results at the end of run
void TARCPrimaryGenerator::Print(){
  using geant::MeV;
  std::string myStr = "primaries were generated: \n";
  if (gNumPrimaryPerEvt < 0.0){
    myStr += " Primaries per event : random for each event on [" +
             std::to_string(gInfNumPrimaryPerEvt)                +
             ", "                                                +
             std::to_string(gSupNumPrimaryPerEvt)                +
             "]\n";
  } else {
    myStr += " Primaries per event : "                           +
             std::to_string(gNumPrimaryPerEvt)                   +
             "\n";
  }
  if (gPrimaryEnergy < 0.0) {
    myStr += "Primary energy:  random for each primary on ["     +
             std::to_string(gInfBeamEnergy/MeV)                  +
             "MeV"                                               +
             std::to_string(gSupBeamEnergy/MeV)                  +
             "MeV] \n";
  } else {
    myStr += " Primary ebergy :"                                 +
             std::to_string(gPrimaryEnergy/MeV)                  +
             " [MeV] \n";
  }
  if (!(gPrimaryDir[0] && gPrimaryDir[1] && gPrimaryDir[2])) {
    myStr += " Primary direction : isotropic for each primary \n";
  } else {
    std::string sdir = "[";
    sdir += std::to_string(gPrimaryDir[0])              + ", "  +
            std::to_string(gPrimaryDir[1])              + ", "  +
            std::to_string(gPrimaryDir[2])              + "]\n";
    myStr += " Primary direction : " + sdir;
  }
  if (gPrimaryType == "") {
    myStr += " Primary type : randomly selected for each primary from \n";
    for (int i = 0; i < gNumberCandidateParticles; i++)
      myStr += " type index: " + std::to_string(i) + ", name: " + gNameParticlesVector[i] + "\n";
  } else {
    myStr += " Primary type :  " + gPrimaryType + "\n";
  }
  std::cout << "\n ======== INFO on Run Conditions ==================================== \n"
            << myStr
            << "------------------------------------------------------------------------\n";
}
