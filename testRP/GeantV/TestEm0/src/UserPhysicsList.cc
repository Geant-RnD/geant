
#include "UserPhysicsList.h"

#include "PhysicalConstants.h"
#include "SystemOfUnits.h"

#include "PhysicsProcess.h"

#include "Particle.h"
#include "Electron.h"
#include "Positron.h"
#include "Gamma.h"

#include "ElectronIonizationProcess.h"
#include "MollerBhabhaIonizationModel.h"

#include "ElectronBremsstrahlungProcess.h"
#include "SeltzerBergerBremsModel.h"
#include "RelativisticBremsModel.h"

#include "ComptonScatteringProcess.h"
#include "KleinNishinaComptonModel.h"

#include "GammaConversionProcess.h"
#include "BetheHeitlerPairModel.h"
#include "RelativisticPairModel.h"

#include "GammaPhotoElectricProcess.h"
#include "SauterGavrilaPhotoElectricModel.h"

#include "GammaPhotoElectricProcess.h"
#include "SauterGavrilaPhotoElectricModel.h"


namespace userapplication {

UserPhysicsList::UserPhysicsList(const std::string &name) : geantphysics::PhysicsList(name) {}
UserPhysicsList::~UserPhysicsList() {}

void UserPhysicsList::Initialize() {
  // get the partcile table and loop over it
  std::vector<geantphysics::Particle*> pTable = geantphysics::Particle::GetTheParticleTable();
  for (unsigned int i=0; i<pTable.size(); ++i) {
    geantphysics::Particle *particle = pTable[i];
    if (particle==geantphysics::Electron::Definition()) {
      //std::cout<<"  ELECTRON" <<std::endl;
      //
      // create ionization process for e- with 1 model:
     //
      geantphysics::EMPhysicsProcess *eIoniProc = new geantphysics::ElectronIonizationProcess("eIoni");
      // create the Moller-Bhabha model for ionization i.e. for e- + e- -> e- + e- intercation
      geantphysics::EMModel          *eMBModel  = new geantphysics::MollerBhabhaIonizationModel(true);
      // set min/max energies of the model
      eMBModel->SetLowEnergyUsageLimit (  1.0*geant::keV);
      eMBModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
      // add the model to the process
      eIoniProc->AddModel(eMBModel);
      //
      // add the process to the e- particle
      AddProcessToParticle(particle, eIoniProc);
      //
      // create bremsstrahlung process for e- with 2 models:
      //
      geantphysics::EMPhysicsProcess *eBremProc = new geantphysics::ElectronBremsstrahlungProcess("eBrem");
      // create a SeltzerBergerBremsModel for e-
      geantphysics::EMModel          *eSBModel  = new geantphysics::SeltzerBergerBremsModel(true);
      // set min/max energies of the model
      eSBModel->SetLowEnergyUsageLimit (1.0*geant::keV);
      eSBModel->SetHighEnergyUsageLimit(1.0*geant::GeV);
      // how to inactivate this model in a given region i.e. region with index 1
      // active regions for a model are set based on their process active regions + user requested inactive regions
      //eSBModel->AddToUserRequestedInActiveRegions(1);
      //
      // add this model to the process
      eBremProc->AddModel(eSBModel);
      //
      // create a RelativisticBremsModel for e-
      geantphysics::EMModel          *eRelBModel = new geantphysics::RelativisticBremsModel();
      // set min/max energies of the model
      eRelBModel->SetLowEnergyUsageLimit (  1.0*geant::GeV);
      eRelBModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
      // add this model to the process
      eBremProc->AddModel(eRelBModel);
      //
      // add the process to the e- particle
      AddProcessToParticle(particle, eBremProc);
    }
    if (particle==geantphysics::Positron::Definition()) {
      //std::cout<<"  Positron" <<std::endl;
      //
      // create ionization process for e+ with 1 model:
      //
      geantphysics::EMPhysicsProcess *eIoniProc = new geantphysics::ElectronIonizationProcess("eIoni");
      // create the Moller-Bhabha model for ionization i.e. for e+ + e- -> e+ + e- intercation
      geantphysics::EMModel          *eMBModel  = new geantphysics::MollerBhabhaIonizationModel(false);
      // set min/max energies of the model
      eMBModel->SetLowEnergyUsageLimit (  1.0*geant::keV);
      eMBModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
      // add the model to the process
      eIoniProc->AddModel(eMBModel);
      // add the process to the e+ particle
      AddProcessToParticle(particle, eIoniProc);
      //
      // create bremsstrahlung process for e+ with 2 models:
      //
      geantphysics::EMPhysicsProcess *eBremProc = new geantphysics::ElectronBremsstrahlungProcess("eBrem");
      // create a SeltzerBergerBremsModel for e-
      geantphysics::EMModel          *eSBModel  = new geantphysics::SeltzerBergerBremsModel(false);
      // set min/max energies of the model
      eSBModel->SetLowEnergyUsageLimit (1.0*geant::keV);
      eSBModel->SetHighEnergyUsageLimit(1.0*geant::GeV);
      // how to inactivate this model in a given region i.e. region with index 1
      // active regions for a model are set based on their process active regions + user requested inactive regions
      //eSBModel->AddToUserRequestedInActiveRegions(1);
      //
      // add this model to the process
      eBremProc->AddModel(eSBModel);
      //
      // create a RelativisticBremsModel for e+
      geantphysics::EMModel          *eRelBModel = new geantphysics::RelativisticBremsModel();
      // set min/max energies of the model
      eRelBModel->SetLowEnergyUsageLimit (  1.0*geant::GeV);
      eRelBModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
      // add this model to the process
      eBremProc->AddModel(eRelBModel);
      //
      // add the process to the e+ particle
      AddProcessToParticle(particle, eBremProc);
    }
    if (particle==geantphysics::Gamma::Definition()) {
      //
      // create photoelectric effect process for gamma with 1 model:
      //
      geantphysics::EMPhysicsProcess *photoelectricProc = new geantphysics::GammaPhotoElectricProcess("phot");
      // create the Sauter-Gavrila model for photoelectric effect
      geantphysics::EMModel           *sgModel = new geantphysics::SauterGavrilaPhotoElectricModel();
      // set min/max energies of the model
      sgModel->SetLowEnergyUsageLimit ( 1.0*geant::eV);
      sgModel->SetHighEnergyUsageLimit( 1.0*geant::TeV);
      // add the model to the process
      photoelectricProc->AddModel(sgModel);
      //
      // add the process to the gamma particle
      AddProcessToParticle(particle, photoelectricProc);
      // create compton scattering process for gamma with 1 model:
      //
      geantphysics::EMPhysicsProcess *comptProc = new geantphysics::ComptonScatteringProcess("compt");
      // create the Klein-Nishina model for Compton scattering i.e. for g + e- -> g + e- intercation
      geantphysics::EMModel          *kncModel  = new geantphysics::KleinNishinaComptonModel();
      // set min/max energies of the model
      kncModel->SetLowEnergyUsageLimit (100.0*geant::eV);
      kncModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
      // add the model to the process
      comptProc->AddModel(kncModel);
      //
      // add the process to the gamma particle
      AddProcessToParticle(particle, comptProc);
      //
      // create gamma conversion process for gamma with 1 model:
      //
      geantphysics::EMPhysicsProcess *convProc = new geantphysics::GammaConversionProcess("conv");
      // create the Bethe-Heitler model for pair production i.e. for g + A -> e- + e+ interaction
      geantphysics::EMModel           *bhModel = new geantphysics::BetheHeitlerPairModel();
      // set min/max energies of the model
      bhModel->SetLowEnergyUsageLimit (  2.0*geant::kElectronMassC2);
      bhModel->SetHighEnergyUsageLimit( 80.0*geant::GeV);
      // add the model to the process
      convProc->AddModel(bhModel);
      //
      // create the relativistic model(with LPM) for pair production i.e. for g + A -> e- + e+ interaction
      geantphysics::EMModel           *relModel = new geantphysics::RelativisticPairModel();
      // set min/max energies of the model
      relModel->SetLowEnergyUsageLimit (  80.0*geant::GeV);
      relModel->SetHighEnergyUsageLimit( 100.0*geant::TeV);
      // add the model to the process
      convProc->AddModel(relModel);
      //
      // add the process to the gamma particle
      AddProcessToParticle(particle, convProc);
        
        
      //
      // create photoelectric effect process for gamma with 1 model:
      //
      geantphysics::EMPhysicsProcess *photoelectricProc = new geantphysics::GammaPhotoElectricProcess("gPhotoElectric");
      // create the Sauter-Gavrila model for photoelectric effect
      geantphysics::EMModel           *sgModel = new geantphysics::SauterGavrilaPhotoElectricModel();
      // set min/max energies of the model
      sgModel->SetLowEnergyUsageLimit (  2.0*geant::keV);
      // the parametrized cross sections works only up t0 80-90 GeV but we will use it now up to 1 TeV
        
      sgModel->SetHighEnergyUsageLimit(  1.0*geant::TeV);
      // add the model to the process
      photoelectricProc->AddModel(sgModel);
      //
      // add the process to the gamma particle
      AddProcessToParticle(particle, photoelectricProc);
        
    }
  }
}


}  // userapplication
