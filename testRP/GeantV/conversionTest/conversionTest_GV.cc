/**
 * @brief  conversionTest_GV: GeantV-Real-physics test for testing models for gamma photons conversion into e-/e+ pairs.
 *
 * @author F Hariri, M Novak
 * @date   May 2017
 *
 * Run ./conversionTest_GV --help for more details!
 * The corresponding quantities/distributions can be obtained by using the conversionTest_G4 Geant4 test.
 */


#include <iostream>
#include <vector>
#include <cstdio>
#include <ctime>

#include <getopt.h>
#include <err.h>

#include "Material.h"
#include "Element.h"
#include "MaterialCuts.h"

// vecgeom includes
#include "volumes/LogicalVolume.h"
#include "volumes/Box.h"

#include "Region.h"
#include "PhysicsParameters.h"

#include "ELossTableManager.h"
#include "ELossTableRegister.h"

#include "Particle.h"
#include "Electron.h"
#include "Positron.h"
#include "Gamma.h"

#include "EMModel.h"
#include "BetheHeitlerPairModel.h"

#include "LightTrack.h"
#include "PhysicsData.h"

// a simple histogram class: can be changed later
#include "Hist.h"

using geantphysics::Material;
using geantphysics::Element;
using geantphysics::MaterialCuts;

using geantphysics::PhysicsParameters;

using geantphysics::Particle;
using geantphysics::Electron;
using geantphysics::Positron;
using geantphysics::Gamma;

using geantphysics::ELossTableManager;
using geantphysics::ELossTableRegister;

// the two brem. model
using geantphysics::EMModel;
using geantphysics::BetheHeitlerPairModel;

using geantphysics::LightTrack;
using geantphysics::PhysicsData;

using userapplication::Hist;

//
// default values of the input parameters
static std::string   materialName("NIST_MAT_Pb");         // material is lead
static int           numHistBins       = 100;             // number of histogram bins between min/max values
static double        numSamples        = 1.e+7;           // number of required final state samples
static double        primaryEnergy     = 0.1;             // primary particle energy in [GeV]

static struct option options[] = {
  {"material-name     (with a NIST_MAT_ prefix; see more in material doc.)     - default: NIST_MAT_Pb"        , required_argument, 0, 'm'},
  {"primary-energy    (in internal energy units i.e. [GeV])                    - default: 0.1"                , required_argument, 0, 'E'},
  {"number-of-samples (number of required final state samples)                 - default: 1.e+7"              , required_argument, 0, 'f'},
  {"number-of-bins    (number of bins in the histogram)                        - default: 100"                , required_argument, 0, 'n'},
  {"help"                                                                                                     , no_argument      , 0, 'h'},
  {0, 0, 0, 0}
};
void help();

//***********************************************************************************************//
//***** THIS WILL BE MODEL SPECIFIC: contains the final state sampling and hist. building  ******//
// method to create photon energy distribution using a SeltzerBergerBremsModel as input argument
double sampleDistribution(double numSamples, double primaryEnergy, const MaterialCuts *matCut,
                          Particle *primParticle, EMModel *emModel, Hist *h1, Hist *h2, Hist *h3,
                          Hist *h4);
//***********************************************************************************************//


//===========================================================================================//
//
int main(int argc, char *argv[]) {
  //
  //============================== Get input parameters =====================================//
  while (true) {
    int c, optidx = 0;
    c = getopt_long(argc, argv, "h:m:E:f:n:", options, &optidx);
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
  //===========================================================================================//

  //============================= Set user defined input data =================================//
  // Create target material: which is supposed to be a NIST Material
  Material *matDetector = Material::NISTMaterial(materialName);
  //
  // Create primnary gamma particle
  Particle *particle = Gamma::Definition();
  //
  // Set particle kinetic energy
  double kineticEnergy    = primaryEnergy;
  //
  // Set production cuts if needed: not used in Compton
  bool   iscutinlength    = true;
  double prodCutValue     = 1.*geant::mm;
  double gcut             = prodCutValue;
  double emcut            = prodCutValue;
  double epcut            = prodCutValue;
  //===========================================================================================//

  //============= Initialization i.e. building up and init the physics ========================//
  // Create a dummy vecgeom::geometry:
  //  - with only one volume i.e. world
  //  - create a region and set production cuts
  //
  // create a vecgeom::LogicalVolume
  vecgeom::UnplacedBox worldParams = vecgeom::UnplacedBox(1.,1.,1.);
  vecgeom::LogicalVolume worldl(&worldParams);
  // create one region and assigne to the logical volume
  vecgeom::Region *aRegion = new vecgeom::Region("ARegion",iscutinlength, gcut, emcut, epcut);
  worldl.SetRegion(aRegion);
  // create a dummy vecgeom::media for this volume:
  // Only the vecgeom::material index (that will be 0) is used later to identify our material during the init.
  worldl.SetTrackingMediumPtr(new vecgeom::Medium((matDetector->GetName()).c_str(),new vecgeom::Material(),new double()));
  vecgeom::GeoManager::Instance().SetWorld(worldl.Place());
  vecgeom::GeoManager::Instance().CloseGeometry();
  // Create all(we have only one) MaterialCuts
  MaterialCuts::CreateAll();
  //===========================================================================================//


  //*******************************************************************************************//
  //************                 THIS CONTAINS MODEL SPECIFIC PARTS                 ***********//
  //
  // Create a BetheHeitlerPairModel model for e-/e+ pair production:
  EMModel *emModel = new BetheHeitlerPairModel();
  // - Set low/high energy usage limits
  emModel->SetLowEnergyUsageLimit (100.0*geant::eV);
  emModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
  // by default, sampling tables are used: can be set to false not to use tables (==>rejection)
  //emModel->SetUseSamplingTables(false);
  //
  //*******************************************************************************************//


  //=========== Set the active regions of the model and one physics-parameter object ==========//
  // - Set the model to be active in region index 0
  (emModel->GetListActiveRegions()).resize(1); // one region
  (emModel->GetListActiveRegions())[0] = true; // make it active there
  // - Create one PhysicsParameters object (with defult values)
  PhysicsParameters *physPars = new PhysicsParameters();
  // - Set it to be active in region index 0
  (physPars->GetListActiveRegions()).resize(1);
  (physPars->GetListActiveRegions())[0] = true;
  // - Initialisation of the model
  emModel->Initialize();
  //===========================================================================================//


  //===========================================================================================//
  //== Use the EMModel interface methods of the model to compute some integrated quantities  ==//
  //
  // Get the MaterialCuts of the target: we have only one
  const MaterialCuts *matCut = MaterialCuts::GetMaterialCut(aRegion->GetIndex(),matDetector->GetIndex());
  std::cout<< "  "<< matCut->GetMaterial() << std::endl;
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;
  std::cout<< "   MaterialCuts: \n"   << matCut;
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;
  std::cout<< "   Particle       =  " << particle->GetName() << std::endl;
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;
  std::cout<< "   Kinetic energy =  " << kineticEnergy/geant::MeV << "  [MeV] " << std::endl;
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;
  std::cout<< "   Model name     =  " << emModel->GetName() << std::endl;
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;
  // check if we compute atomic-cross section: only for single elemnt materials
  bool isSingleElementMaterial = false;
  if (matCut->GetMaterial()->GetNumberOfElements()==1) {
    isSingleElementMaterial = true;
  }
  //
  // Note: restrictedDEDX and unrestrictedDEDX will be non-zero value only in case of EMModels that has the ComputeDEDX
  //       method implemented i.e. in case of energy loss intercations (i.e. ioni. and brem.)
  double restrictedDEDX          = 0.0;
  double unRestrictedDEDX        = 0.0;
  // Note: atomicCrossSection is computed only in case of materials that has single element
  double atomicCrossSection      = 0.0;
  double macroscopicCrossSection = 0.0;
  //
  // use the model to compute restricted stopping power
  restrictedDEDX   = emModel->ComputeDEDX(matCut, kineticEnergy, particle);
  // use the model to compute un-restricted stopping power
  unRestrictedDEDX = emModel->ComputeDEDX(matCut, kineticEnergy, particle, true);
  // use the model to compute atomic cross section (only in case of single element materials)
  if (isSingleElementMaterial) {
    const Element *elem = (matCut->GetMaterial()->GetElementVector())[0];
    atomicCrossSection  = emModel->ComputeXSectionPerAtom(elem, matCut, kineticEnergy, particle);
  }
  // use the model to compute macroscopic cross section
  macroscopicCrossSection = emModel->ComputeMacroscopicXSection(matCut, kineticEnergy, particle);
  //
  // print out integrated quantities:
  // -atomic cross section
  if (isSingleElementMaterial) {
    std::cout<< "   cross section per atom      :";
    std::cout<< std::setw(14) << std::scientific << std::right << atomicCrossSection/(geant::barn)
             << std::setw(14) << std::left << "     [barn]";
    std::cout<<std::endl;
  }
  //
  // -macroscopic cross section
  std::cout<< "   cross section per volume    :";
  std::cout<< std::setw(14) << std::scientific << std::right << macroscopicCrossSection/(1./geant::cm)
           << std::setw(14) << std::left << "     [1/cm]";
  std::cout<<std::endl;
  //
  // -restricted stopping power
  std::cout<< "   resricted dE/dx  (MeV/cm)   :";
  std::cout<< std::setw(14) << std::scientific << std::right << restrictedDEDX/(geant::MeV/geant::cm)
           << std::setw(14) << std::left << "   [MeV/cm]";
  std::cout<<std::endl;
  //
  // -unrestricted stopping power
  std::cout<< "   unresricted dE/dx (MeV/cm)  :";
  std::cout<< std::setw(14) << std::scientific << std::right << unRestrictedDEDX/(geant::MeV/geant::cm)
           << std::setw(14) << std::left << "   [MeV/cm]";
  std::cout<<std::endl;
  //===========================================================================================//



  //*******************************************************************************************//
  //************                 THIS CONTAINS MODEL SPECIFIC PARTS                 ***********//
  //
  // Create a histogram to store the secondary e- energy distribution:
  // - the distribution of reduced total energy i.e. epsElectron=E_tot_electron/E_gamma
  double xMin =  0.0;
  double xMax =  1.0;
  Hist *histo_electron_energy    = new Hist(xMin, xMax, numHistBins);
  // Create a histogram to store the secondary e-  direction:
  // - the distribution of log10(0.5*(1.-cos(\theta_electron)))
  xMin = -12.;
  xMax =   0.;
  Hist *histo_electron_angle     = new Hist(xMin, xMax, numHistBins);
  // Create a histogram to store the secondary e+ energy distribution:
  // - the distribution of reduced total energy i.e. epsPositron=E_tot_positron/E_gamma
  xMin = 0.0;
  xMax = 1.0;
  Hist *histo_positron_energy = new Hist(xMin, xMax, numHistBins);
  // Create histogram to store the secondary e+ direction: cos(\theta_positron)
  // - the distribution of log10(0.5*(1.-cos(\theta_positron)))
  xMin = -12.;
  xMax =   0.;
  Hist *histo_positron_angle  = new Hist(xMin, xMax, numHistBins);
  //
  //
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;
  std::cout<< "   Sampling is running : .....................................................      " << std::endl;
  // call sampling method
  double timeInSec = sampleDistribution(numSamples, kineticEnergy, matCut, particle, emModel, histo_electron_energy,
                                        histo_electron_angle, histo_positron_energy, histo_positron_angle);
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;
  std::cout<< "   Time of sampling =  " << timeInSec << " [s]" << std::endl;
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;

  // print out histogram to file: fileName
  char fileName[512];
  sprintf(fileName,"conv_GV_electron_energy_%s",(matCut->GetMaterial()->GetName()).c_str());
  FILE *f     = fopen(fileName,"w");
  double norm = 1./numSamples;
  Hist *histo = histo_electron_energy;
  for (int i=0; i<histo->GetNumBins(); ++i) {
   fprintf(f,"%d\t%.8g\t%.8g\n",i,histo->GetX()[i]+0.5*histo->GetDelta(),histo->GetY()[i]*norm);
  }
  delete histo;
  fclose(f);
  //
  sprintf(fileName,"conv_GV_electron_angular_%s",(matCut->GetMaterial()->GetName()).c_str());
  f     = fopen(fileName,"w");
  histo = histo_electron_angle;
  for (int i=0; i<histo->GetNumBins(); ++i) {
   fprintf(f,"%d\t%.8g\t%.8g\n",i,histo->GetX()[i]+0.5*histo->GetDelta(),histo->GetY()[i]*norm);
  }
  delete histo;
  fclose(f);
  //
  sprintf(fileName,"conv_GV_positron_energy_%s",(matCut->GetMaterial()->GetName()).c_str());
  f     = fopen(fileName,"w");
  histo = histo_positron_energy;
  for (int i=0; i<histo->GetNumBins(); ++i) {
   fprintf(f,"%d\t%.8g\t%.8g\n",i,histo->GetX()[i]+0.5*histo->GetDelta(),histo->GetY()[i]*norm);
  }
  delete histo;
  fclose(f);
  //
  sprintf(fileName,"conv_GV_positron_angular_%s",(matCut->GetMaterial()->GetName()).c_str());
  f     = fopen(fileName,"w");
  histo = histo_positron_angle;
  for (int i=0; i<histo->GetNumBins(); ++i) {
   fprintf(f,"%d\t%.8g\t%.8g\n",i,histo->GetX()[i]+0.5*histo->GetDelta(),histo->GetY()[i]*norm);
  }
  delete histo;
  fclose(f);
  //
  std::cout<< "   Histogram is written  into files ................................................"<< std::endl;
  std::cout<< "   -------------------------------------------------------------------------------- "<<std::endl;
  //*******************************************************************************************//

  // end
  std::cout << "   ================================================================================ "
            << std::endl << std::endl;

  // delete the EMModel objects
  delete emModel;

  PhysicsParameters::Clear();
  // clear the ELossTableManager(will alos delete all ELossTable-s) and ELossTableRegister
  ELossTableManager::Instance().Clear();
  ELossTableRegister::Instance().Clear();
  MaterialCuts::ClearAll();
  Material::ClearAllMaterials(); // will delete all Elements and Isotoes as well

return 0;
}


void help() {
  std::cout<<"\n "<<std::setw(120)<<std::setfill('=')<<""<<std::setfill(' ')<<std::endl;
  std::cout<<"  Model-level GeantV test for testing GeantV Bethe-Heitler model for e-/e+"
           <<"  pair production by photons."
           << std::endl;
  std::cout<<"\n  Usage: conversionTest_GV [OPTIONS] \n"<<std::endl;
  for (int i = 0; options[i].name != NULL; i++) {
    printf("\t-%c  --%s\n", options[i].val, options[i].name);
  }
  std::cout<<"\n "<<std::setw(120)<<std::setfill('=')<<""<<std::setfill(' ')<<std::endl;
}



//*******************************************************************************************//
//************                 THIS CONTAINS MODEL SPECIFIC PARTS                 ***********//
//
// implementation of the final state distribution sampling
double sampleDistribution(double numSamples, double primaryEnergy, const MaterialCuts *matCut,
                          Particle *primParticle, EMModel *emModel, Hist *h1, Hist *h2, Hist *h3,
                          Hist *h4) {
  double ekin       = primaryEnergy;
  double dirx       = 0.0;   // direction
  double diry       = 0.0;
  double dirz       = 1.0;
  int    gvcode     = primParticle->GetInternalCode();  // internal code of the primary particle i.e. gamma

  // Set up a dummy Geant::GeantTaskData and its geantphysics::PhysicsData member: they are needed in the final state
  // sampling
  Geant::GeantTaskData *td = new Geant::GeantTaskData(1,1,1);
  PhysicsData         *phd = new PhysicsData();
  td->fPhysicsData         = phd;
  // Set up a the primary light track for brem.
  LightTrack primaryLT;
  // init time
  clock_t  start_time = clock();
  for (long int i=0; i<numSamples; ++i) {
     // we will use members:
     //  fMaterialCutCoupleIndex <==>  // current MaterialCuts index
     //  fKinE         <==>  fE-fMass  // kinetic energy;  will be set to the new kinetic energy
     //  fGVcode       <==>  fGVcode   // internal particle code
     //  fIntLen       <==>  fIntLen   // pre-step lambda for accounting energy loss i.e. to see if it is a delta inter.
     //  fXdir         <==>  fXdir     // direction vector x comp. will be set to the new direction x comp.
     //  fYdir         <==>  fYdir     // direction vector y comp. will be set to the new direction y comp.
     //  fZdir         <==>  fZdir     // direction vector z comp. will be set to the new direction z comp.
     primaryLT.SetMaterialCutCoupleIndex(matCut->GetIndex());
     primaryLT.SetKinE(ekin);
     primaryLT.SetGVcode(gvcode);
//       primaryLT.SetTrackIndex(0); // not important now
     primaryLT.SetDirX(dirx);
     primaryLT.SetDirY(diry);
     primaryLT.SetDirZ(dirz);
//       primaryLT.SetTotalMFP(1.0); // not important now
     //
     // clean the number of secondary tracks used (in PhysicsData)
     td->fPhysicsData->SetNumUsedSecondaries(0);
     //
     // invoke the interaction
     int numSecs = emModel->SampleSecondaries(primaryLT,td);
     // get the secondary tracks i.e. the e- and the e+
     if (numSecs==2) {
       std::vector<LightTrack> &secLTracks = td->fPhysicsData->GetListOfSecondaries();
       // get reduced e- total energy and cost
       double epsEl = (secLTracks[0].GetKinE()+geant::kElectronMassC2)/ekin;
       h1->Fill(epsEl,1.0);
       double costEl = secLTracks[0].GetDirZ();
       double elZ = 0.5*(1.-costEl);
       if (elZ>0.0) {
         double dum = std::log10(elZ);
         if (dum>-12) {
           h2->Fill(dum,1.0);
         }
       }
       // do the same for the e+ i.e. e+ total energy and cost
       double epsPos  = (secLTracks[1].GetKinE()+geant::kElectronMassC2)/ekin;
       h3->Fill(epsPos,1.0);
       double costPos = secLTracks[1].GetDirZ();
       double posZ    = 0.5*(1.-costPos);
       if (posZ>0.0) {
         double dum = std::log10(posZ);
         if (dum>-12) {
           h4->Fill(dum,1.0);
         }
       }
     } // end if there is secondary
   }
   clock_t end_time = clock();
   return (end_time-start_time)/(double(CLOCKS_PER_SEC));
}
//*******************************************************************************************//
