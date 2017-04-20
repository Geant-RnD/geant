//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//
// -------------------------------------------------------------------
//      GEANT 4 class file --- Copyright CERN 1998
//      CERN Geneva Switzerland
//
//
//      File name:     compt.cc
//
//      Author:        V.Ivanchenko
//
//      Creation date: 12 September 2011 from
//
//      Modifications:
// -------------------------------------------------------------------

#include "globals.hh"
#include "G4ios.hh"
#include <fstream>
#include <iomanip>


#include <getopt.h>
#include <err.h>

#include "G4Material.hh"
#include "G4ElementVector.hh"
#include "G4ProcessManager.hh"
#include "G4VParticleChange.hh"
#include "G4ParticleChange.hh"

#include "G4ParticleTable.hh"
#include "G4DynamicParticle.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

#include "G4ForceCondition.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4Step.hh"
#include "G4GRSVolume.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4StateManager.hh"
#include "G4NistManager.hh"

#include "Histo.hh"
#include "G4Timer.hh"

#include "G4TouchableHistory.hh"

//#include "G4KleinNishinaModel.hh"
#include "G4KleinNishinaCompton.hh"

#include "G4ParticleChangeForGamma.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4LossTableManager.hh"


//
// default values of the input parameters
static std::string   materialName("G4_Pb");               // material is lead
static int           numHistBins       = 100;             // number of histogram bins between min/max values
static double        numSamples        = 1.e+7;           // number of required final state samples
static double        primaryEnergy     = 100.0;           // primary particle energy in [MeV]

static struct option options[] = {
  {"material-name     (with a G4_ prefix i.e. NIST material)      - default: G4_Pb"  , required_argument, 0, 'm'},
  {"primary-energy    (in internal energy units i.e. [MeV])       - default: 100"    , required_argument, 0, 'E'},
  {"number-of-samples (number of required final state samples)    - default: 1.e+7"  , required_argument, 0, 'f'},
  {"number-of-bins    (number of bins in the histogram)           - default: 100"    , required_argument, 0, 'n'},
  {"help"                                                                            , no_argument      , 0, 'h'},
  {0, 0, 0, 0}
};
void help();



int main(int argc, char** argv) {
  //
  // Get input parameters
  while (true) {
    int c, optidx = 0;
    c = getopt_long(argc, argv, "hm:E:f:n:", options, &optidx);
    if (c == -1)
      break;
    switch (c) {
    case 0:
      c = options[optidx].val;
    /* fall through */
    case 'm':
       materialName = optarg;
       break;
    case 'E':
      primaryEnergy = (double)strtof(optarg, NULL);
      if (primaryEnergy<=0)
        errx(1, "primary particle energy must be positive");
      break;
    case 'f':
      numSamples = (double)strtof(optarg, NULL);
      if (numSamples<=0)
        errx(1, "number of final state samples must be positive");
      break;
    case 'n':
      numHistBins = (int)strtof(optarg, NULL);
      if (numHistBins<=0)
        errx(1, "number of histogram bins must be positive");
      break;
    case 'h':
       help();
       return 0;
       break;
    default:
      help();
      errx(1, "unknown option %c", c);
    }
  }


  G4cout << "========================================================" << G4endl;
  G4cout << "======        Compton Scattering Test Starts    ========" << G4endl;
  G4cout << "========================================================" << G4endl;

  G4String mname(materialName);         // material
  G4double   energy  = primaryEnergy;   // primary energy of the e-/e+
  G4double   stat    = numSamples;      // number of samples
  //G4int      verbose = 1;
  G4Material *mat    = G4NistManager::Instance()->FindOrBuildMaterial(mname);

  if(!mat) { exit(1); }

  // Set random engine to MTwist: the same that we use in GeantV (we use the std c++11 inmp.)
  //CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  CLHEP::HepRandom::setTheEngine(new CLHEP::MTwistEngine);
  //CLHEP::HepRandom::setTheEngine(new CLHEP::Ranlux64Engine);

  //CLHEP::HepRandomEngine* rndmEngine = G4Random::getTheEngine();
  //G4cout<< "  rndmEngine = " <<rndmEngine->name() <<G4endl;


  // Track
  G4ThreeVector aPosition = G4ThreeVector(0.,0.,0.);
  G4ThreeVector aDirection = G4ThreeVector(0.0,0.0,1.0);
  G4Gamma::Gamma();
  G4Electron::Electron();
  const G4ParticleDefinition* part = G4Gamma::Gamma();

  G4DynamicParticle dParticle(part,aDirection,energy);

  G4cout.setf( std::ios::scientific, std::ios::floatfield );

  G4ParticleTable* partTable = G4ParticleTable::GetParticleTable();
  partTable->SetReadiness();


  //--------- Geometry definition
  G4double dimX = 100.0*cm;
  G4double dimY = 100.0*cm;
  G4double dimZ = 100.0*cm;

  G4Box* sFrame = new G4Box ("Box",dimX, dimY, dimZ);
  G4LogicalVolume* lFrame = new G4LogicalVolume(sFrame,mat,"Box",0,0,0);
  G4PVPlacement* pFrame = new G4PVPlacement(0,G4ThreeVector(),"Box",
                                            lFrame,0,false,0);

  G4DataVector cuts;
  cuts.push_back(1.*mm);
  G4ProductionCuts* pcut = new G4ProductionCuts();
  pcut->SetProductionCut(cuts[0], 0); // set cut for gamma
  pcut->SetProductionCut(cuts[0], 1); // set cut for e-
  pcut->SetProductionCut(cuts[0], 2); // set cut for e+
  pcut->SetProductionCut(cuts[0], 3); // set cut for alpha

  G4MaterialCutsCouple* couple = new G4MaterialCutsCouple(mat, pcut);
  couple->SetIndex(0);

  G4Region* reg = new G4Region("DefaultRegionForTheWorld");
  reg->AddRootLogicalVolume(lFrame);
  reg->UsedInMassGeometry(true);
  reg->SetProductionCuts(pcut);
  reg->RegisterMaterialCouplePair(mat, couple);
//  G4cout << reg->FindCouple(mat) << G4endl;

  G4ProductionCutsTable* theCoupleTable = G4ProductionCutsTable::GetProductionCutsTable();
  theCoupleTable->UpdateCoupleTable(pFrame);


  // -------------------------------------------------------------------
  // -------- Start run processing
  G4StateManager* g4State=G4StateManager::GetStateManager();
  if (! g4State->SetNewState(G4State_Init)) {
    G4cout << "error changing G4state"<< G4endl;;
  }

//
//  G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
//  G4LossTableManager::Instance()->SetAtomDeexcitation(de);
//  de->SetFluo(false);
//  de->InitialiseAtomicDeexcitation();

  // Instantiate models
  G4KleinNishinaCompton knc;
//  G4KleinNishinaModel knm;

  // Set particle change object
  G4ParticleChangeForGamma* fParticleChange = new G4ParticleChangeForGamma();
  knc.SetParticleChange(fParticleChange, 0);  // G4KleinNishinaCompton
//  knm.SetParticleChange(fParticleChange, 0);  // G4KleinNishinaModel

  // Initilise models
  knc.Initialise(part, cuts);
//  knm.Initialise(part, cuts);

  // ------- Histograms name
  Histo    histo;
  G4String nname = mname; // material name
  G4String ss    = "G4_"; // strip the G4_ prefix
  G4String::size_type ipos = nname.find(ss);
  if (ipos!=std::string::npos) {
     nname.erase(ipos, ss.length());
  }
  //
  // create histogram for gamma energy
  G4String hname    = "compton_G4_gamma_energy_" + nname;
  G4int    nbins    = numHistBins;
  G4double xmin     = 0.0;
  G4double xmax     = 1.0;
  histo.Add1D("1",hname,nbins,xmin,xmax);
  //
  // same for gamma angular distr.
  hname    = "compton_G4_gamma_angular_" + nname;
  //nbins    = numHistBins;
  xmin     = -1.0;
  xmax     =  1.0;
  histo.Add1D("2",hname,nbins,xmin,xmax);
  //
  // Create histogram for electron energy
  hname    = "compton_G4_electron_energy_" + nname;
  //nbins    = numHistBins;
  xmin     =  0.0;
  xmax     =  1.0;
  histo.Add1D("3",hname,nbins,xmin,xmax);
  //
  // same for electron angular
  hname    = "compton_G4_electron_angular_" + nname;
  //nbins    = numHistBins;
  xmin     = -1.0;
  xmax     =  1.0;
  histo.Add1D("4",hname,nbins,xmin,xmax);

  //
  //
  hname = "compton_G4_" + nname;
  histo.SetFileName(hname);
  histo.Book();
//  G4cout << "Histograms are booked output file <" << hname << "> "
//	 << G4endl;

  // -------- Track
  G4Track* track = new G4Track(&dParticle,0.0,aPosition);
  G4TouchableHandle fpTouchable(new G4TouchableHistory());
  track->SetTouchableHandle(fpTouchable);

  // -------- Step
  if(!G4StateManager::GetStateManager()->SetNewState(G4State_Idle)) {
    G4cout << "G4StateManager PROBLEM! " << G4endl;
  }


  // print outs
  G4cout<< mat;
  G4ProductionCutsTable::GetProductionCutsTable()->DumpCouples();
  G4cout<< "   -------------------------------------------------------------------------------- "<<G4endl;
  G4cout<< "   Particle       =  " << part->GetParticleName()                                    << G4endl;
  G4cout<< "   -------------------------------------------------------------------------------- "<<G4endl;
  G4cout<< "   Kinetic energy =  " << energy/MeV << "  [MeV] "                                   << G4endl;
  G4cout<< "   -------------------------------------------------------------------------------- "<<G4endl;
  G4cout<< "   Model name     =  " << knc.GetName()                                              << G4endl;
  G4cout<< "   -------------------------------------------------------------------------------- "<<G4endl;
  G4cout<< "   Sampling is running : .........................................................  " << G4endl;

  //
  // sampling
  std::vector<G4DynamicParticle*> vdp;
  dParticle.SetKineticEnergy(energy);
  dParticle.SetMomentumDirection(0.,0.,1.);
  G4double cost, e1, costg, e2;
  G4Timer *timer;

  // G4KleinNishinaCompton begin
  timer = new G4Timer();
  timer->Start();
  for (long int iter=0; iter<stat; ++iter) {
    fParticleChange->InitializeForPostStep(*track);
    knc.SampleSecondaries(&vdp,couple,&dParticle,0.0,energy);
    if (vdp.size()>0) {
      e1 = vdp[0]->GetKineticEnergy()/energy;
      histo.Fill(2,e1,1.0);
      cost = vdp[0]->GetMomentumDirection().z();
      histo.Fill(3,cost,1.0);
      e2 = fParticleChange->GetProposedKineticEnergy()/energy;
      if (e2>0.0) {
        histo.Fill(0,e2,1.0);
        costg = fParticleChange->GetProposedMomentumDirection().z();
        histo.Fill(1,costg,1.0);
      }
      delete vdp[0];
      vdp.clear();
    }
  }
  timer->Stop();
  double timeInSec = timer->GetRealElapsed();
  delete timer;

  G4cout<< "   -------------------------------------------------------------------------------- "<<G4endl;
  G4cout<< "   Time of sampling =  " << timeInSec << " [s]" << G4endl;
  G4cout<< "   -------------------------------------------------------------------------------- "<<G4endl;

  // -------- Committing the transaction with the tree
  histo.ScaleH1(0, 1./stat);
  histo.ScaleH1(1, 1./stat);
  histo.ScaleH1(2, 1/stat);
  histo.ScaleH1(3, 1./stat);
  //
  histo.Save();
  G4cout<< "   Histogram is written  into file =  " << hname << G4endl;
  G4cout<< "   -------------------------------------------------------------------------------- "<<G4endl;
  G4cout<< "   ================================================================================ "<< G4endl << G4endl;

  delete pFrame;
  delete lFrame;
  delete sFrame;
  partTable->DeleteAllParticles();

  G4cout << "###### End of test #####" << G4endl;
}


void help() {
  G4cout<<"\n "<<std::setw(120)<<std::setfill('=')<<""<<std::setfill(' ')<<G4endl;
  G4cout<<"  Geant4 comptonTest application for testing Geant4 compton(G4KleinNishinaCompton) model."
           << G4endl;
  G4cout<<"\n  Usage: comptonTest_G4 [OPTIONS] \n"<<G4endl;
  for (int i = 0; options[i].name != NULL; i++) {
    printf("\t-%c  --%s\n", options[i].val, options[i].name);
  }
  G4cout<<"\n "<<std::setw(120)<<std::setfill('=')<<""<<std::setfill(' ')<<G4endl;
}
