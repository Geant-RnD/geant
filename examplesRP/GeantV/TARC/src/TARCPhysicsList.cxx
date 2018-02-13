/**********************************************************************
 * @file       TARCPhysicsList.cxx
 * @author     Abhijit Bhattacharyya
 * @brief      This file is for setting PhysicsList
 *********************************************************************/

 #include "TARCPhysicsList.h"
using namespace tarcapp;


  TARCPhysicsList::TARCPhysicsList(const std::string &name) : geantphysics::PhysicsList(name) {

  }

  TARCPhysicsList::~TARCPhysicsList() {}

  void TARCPhysicsList::Initialize() {
    std::vector<geantphysics::Particle*> pTable = geantphysics::Particle::GetTheParticleTable(); // Getting the particle table
    for (unsigned i=0; i < pTable.size(); ++i) {
      geantphysics::Particle *particle = pTable[i];
      if (particle==geantphysics::Electron::Definition()) {
        //
        // create ionization process for e- with 1 model:
        //
        geantphysics::EMPhysicsProcess *eIoniProc = new geantphysics::ElectronIonizationProcess("e-Ioni");
        // create the Moller-Bhabha model for ionization i.e. for e- + e- -> e- + e- intercation
        geantphysics::EMModel          *eMBModel  = new geantphysics::MollerBhabhaIonizationModel(true);
        // set min/max energies of the model
        eMBModel->SetLowEnergyUsageLimit (  1.0*geant::keV);
        eMBModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
        // turn off using sampling tables
        eMBModel->SetUseSamplingTables(false);
        // add the model to the process
        eIoniProc->AddModel(eMBModel);
        //
        // add the process to the e- particle
        AddProcessToParticle(particle, eIoniProc);
        //
        // create bremsstrahlung process for e- with 2 models:
        //
        geantphysics::EMPhysicsProcess *eBremProc = new geantphysics::ElectronBremsstrahlungProcess("e-Brem");
        // create a SeltzerBergerBremsModel for e-
        geantphysics::EMModel          *eSBModel  = new geantphysics::SeltzerBergerBremsModel(true);
        // set min/max energies of the model
        eSBModel->SetLowEnergyUsageLimit (1.0*geant::keV);
        eSBModel->SetHighEnergyUsageLimit(1.0*geant::GeV);
        // turn off using sampling tables
        eSBModel->SetUseSamplingTables(false);
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
        // turn off using sampling tables
        eRelBModel->SetUseSamplingTables(false);
        // add this model to the process
        eBremProc->AddModel(eRelBModel);
        //
        // add the process to the e- particle
        AddProcessToParticle(particle, eBremProc);
        //
        // create MSC process
        geantphysics::EMPhysicsProcess *eMSCProc  = new geantphysics::MSCProcess("e-msc");
        // create GS-msc model, set min/max usage limits
        geantphysics::GSMSCModel       *gsMSCModel = new geantphysics::GSMSCModel();
        gsMSCModel->SetRangeFactor(0.06);
        gsMSCModel->SetMSCSteppingAlgorithm(geantphysics::MSCSteppingAlgorithm::kUseSaftey);
        gsMSCModel->SetLowEnergyUsageLimit(100.*geant::eV);
        gsMSCModel->SetHighEnergyUsageLimit(100.*geant::TeV);
        eMSCProc->AddModel(gsMSCModel);
        // add process to particle
        AddProcessToParticle(particle, eMSCProc);
      }
      if (particle==geantphysics::Positron::Definition()) {
        //std::cout<<"  Positron" <<std::endl;
        //
        // create ionization process for e+ with 1 model:
        //
        geantphysics::EMPhysicsProcess *eIoniProc = new geantphysics::ElectronIonizationProcess("e+Ioni");
        // create the Moller-Bhabha model for ionization i.e. for e+ + e- -> e+ + e- intercation
        geantphysics::EMModel          *eMBModel  = new geantphysics::MollerBhabhaIonizationModel(false);
        // set min/max energies of the model
        eMBModel->SetLowEnergyUsageLimit (  1.0*geant::keV);
        eMBModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
        // turn off using sampling tables
        eMBModel->SetUseSamplingTables(false);
        // add the model to the process
        eIoniProc->AddModel(eMBModel);
        // add the process to the e+ particle
        AddProcessToParticle(particle, eIoniProc);
        //
        // create bremsstrahlung process for e+ with 2 models:
        //
        geantphysics::EMPhysicsProcess *eBremProc = new geantphysics::ElectronBremsstrahlungProcess("e+Brem");
        // create a SeltzerBergerBremsModel for e-
        geantphysics::EMModel          *eSBModel  = new geantphysics::SeltzerBergerBremsModel(false);
        // set min/max energies of the model
        eSBModel->SetLowEnergyUsageLimit (1.0*geant::keV);
        eSBModel->SetHighEnergyUsageLimit(1.0*geant::GeV);
        // turn off using sampling tables
        eSBModel->SetUseSamplingTables(false);
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
        // turn off using sampling tables
        eRelBModel->SetUseSamplingTables(false);
        // add this model to the process
        eBremProc->AddModel(eRelBModel);
        //
        // add the process to the e+ particle
        AddProcessToParticle(particle, eBremProc);
        //
        // create MSC process
        geantphysics::EMPhysicsProcess *eMSCProc   = new geantphysics::MSCProcess("e+msc");
        // create GS-msc model, set min/max usage limits
        geantphysics::GSMSCModel       *gsMSCModel = new geantphysics::GSMSCModel(false); // for e+
        gsMSCModel->SetRangeFactor(0.06);
        gsMSCModel->SetMSCSteppingAlgorithm(geantphysics::MSCSteppingAlgorithm::kUseSaftey);
        gsMSCModel->SetLowEnergyUsageLimit(100.*geant::eV);
        gsMSCModel->SetHighEnergyUsageLimit(100.*geant::TeV);
        eMSCProc->AddModel(gsMSCModel);
        // add process to particle
        AddProcessToParticle(particle, eMSCProc);
        //
        // create e+ electron annihilation into 2 gamma process
        geantphysics::PositronAnnihilationProcess *pAnhProc = new geantphysics::PositronAnnihilationProcess();
        AddProcessToParticle(particle, pAnhProc);
      }
      if (particle==geantphysics::Gamma::Definition()) {
        // create compton scattering process for gamma with 1 model:
        //
        geantphysics::EMPhysicsProcess *comptProc = new geantphysics::ComptonScatteringProcess();
        // create the Klein-Nishina model for Compton scattering i.e. for g + e- -> g + e- intercation
        geantphysics::EMModel          *kncModel  = new geantphysics::KleinNishinaComptonModel();
        // set min/max energies of the model
        kncModel->SetLowEnergyUsageLimit (100.0*geant::eV);
        kncModel->SetHighEnergyUsageLimit(100.0*geant::TeV);
        // turn off using sampling tables
        kncModel->SetUseSamplingTables(false);
        // add the model to the process
        comptProc->AddModel(kncModel);
        //
        // add the process to the gamma particle
        AddProcessToParticle(particle, comptProc);
        //
        // create gamma conversion process for gamma with 1 model:
        //
        geantphysics::EMPhysicsProcess *convProc = new geantphysics::GammaConversionProcess();
        // create the Bethe-Heitler model for pair production i.e. for g + A -> e- + e+ interaction
        geantphysics::EMModel           *bhModel = new geantphysics::BetheHeitlerPairModel();
        // set min/max energies of the model
        bhModel->SetLowEnergyUsageLimit (  2.0*geant::kElectronMassC2);
        bhModel->SetHighEnergyUsageLimit( 80.0*geant::GeV);
        // turn off using sampling tables
        bhModel->SetUseSamplingTables(false);
        // add the model to the process
        convProc->AddModel(bhModel);
        //
        // create the relativistic model(with LPM) for pair production i.e. for g + A -> e- + e+ interaction
        geantphysics::EMModel           *relModel = new geantphysics::RelativisticPairModel();
        // set min/max energies of the model
        relModel->SetLowEnergyUsageLimit (  80.0*geant::GeV);
        relModel->SetHighEnergyUsageLimit( 100.0*geant::TeV);
        // turn off using sampling tables
        relModel->SetUseSamplingTables(false);
        // add the model to the process
        convProc->AddModel(relModel);
        //
        // add the process to the gamma particle
        AddProcessToParticle(particle, convProc);
        //
        // create photoelectric effect process for gamma with 1 model:
        //
        geantphysics::EMPhysicsProcess *photoelectricProc = new geantphysics::GammaPhotoElectricProcess();
        // create the Sauter-Gavrila model for photoelectric effect
        geantphysics::EMModel           *sgModel = new geantphysics::SauterGavrilaPhotoElectricModel();
        // set min/max energies of the model
        sgModel->SetLowEnergyUsageLimit ( 1.0*geant::eV);
        sgModel->SetHighEnergyUsageLimit( 1.0*geant::TeV);
        // turn off using sampling tables
        sgModel->SetUseSamplingTables(false);
        // add the model to the process
        photoelectricProc->AddModel(sgModel);
        //
        // add the process to the gamma particle
        AddProcessToParticle(particle, photoelectricProc);
      }
      if (  particle == geantphysics::Proton::Definition()
         || particle == geantphysics::Neutron::Definition()
         || particle == geantphysics::PionPlus::Definition()
         || particle == geantphysics::PionMinus::Definition()
         || particle == geantphysics::PionZero::Definition()
         || particle == geantphysics::KaonPlus::Definition()
         || particle == geantphysics::KaonMinus::Definition()
         || particle == geantphysics::KaonZero::Definition()
         || particle == geantphysics::KaonShort::Definition()
         || particle == geantphysics::KaonLong::Definition()
       ){
          geantphysics::HadronicProcess *hadElasticProc = new geantphysics::ElasticScatteringProcess();  // hadronic elastic process for proton
          geantphysics::HadronicFinalStateModel *diffuseElasticModel = new geantphysics::DiffuseElasticModel(); // diffuse elastic model.
          geantphysics::HadronicCrossSection *ggElasticXS = new geantphysics::GlauberGribovElasticXsc(); // create cross section. ?? compare with NUDY ?

          diffuseElasticModel->SetLowEnergyUsageLimit(0.001*geant::eV); // minimum energy of the model
          diffuseElasticModel->SetHighEnergyUsageLimit(100.0 * geant::TeV); // maximum energy of model

          hadElasticProc->AddModel(diffuseElasticModel);    // add model to the process
          hadElasticProc->AddCrossSection(ggElasticXS);

          AddProcessToParticle(particle, hadElasticProc); // Add process to particle
      }
    }
  }

  void TARCPhysicsList::SetMSCStepLimit(geantphysics::MSCSteppingAlgorithm stepping) {
    fMSCSteppingAlgorithm = stepping;
  }

  void TARCPhysicsList::SetStepMaxValue(double aVal){
    fStepMaxValue = aVal;
  }
