
#include "TestNudy1PhysicsList.h"

#include "Geant/PhysicalConstants.h"
#include "Geant/SystemOfUnits.h"

#include "Geant/PhysicsProcess.h"

#include "Geant/Particle.h"
#include "Geant/Electron.h"
#include "Geant/Positron.h"
#include "Geant/Gamma.h"

#include "Geant/Proton.h"
#include "Geant/Neutron.h"
#include "Geant/PionPlus.h"
#include "Geant/PionMinus.h"
#include "Geant/PionZero.h"
#include "Geant/KaonPlus.h"
#include "Geant/KaonMinus.h"
#include "Geant/KaonZero.h"
#include "Geant/KaonShort.h"
#include "Geant/KaonLong.h"

#include "Geant/ElectronIonizationProcess.h"
#include "Geant/MollerBhabhaIonizationModel.h"

#include "Geant/ElectronBremsstrahlungProcess.h"
#include "Geant/SeltzerBergerBremsModel.h"
#include "Geant/RelativisticBremsModel.h"

#include "Geant/PositronAnnihilationProcess.h"

#include "Geant/ComptonScatteringProcess.h"
#include "Geant/KleinNishinaComptonModel.h"

#include "Geant/GammaConversionProcess.h"
#include "Geant/BetheHeitlerPairModel.h"
#include "Geant/RelativisticPairModel.h"

#include "Geant/GammaPhotoElectricProcess.h"
#include "Geant/SauterGavrilaPhotoElectricModel.h"

#include "Geant/MSCProcess.h"
#include "Geant/MSCModel.h"
#include "Geant/GSMSCModel.h"

#include "Geant/ElasticScatteringProcess.h"
#include "Geant/CaptureProcess.h"
#include "Geant/FissionProcess.h"
#include "Geant/InelasticProcess.h"
#include "Geant/DiffuseElasticModel.h"
#include "Geant/GlauberGribovElasticXsc.h"
#include "Geant/NeutronNudyElasticModel.h"
#include "Geant/NeutronNudyInelasticModel.h"
#include "Geant/NeutronNudyFissionModel.h"
#include "Geant/NeutronNudyCaptureModel.h"
#include "Geant/NeutronNudyElasticXsec.h"
#include "Geant/NeutronNudyInelasticXsec.h"
#include "Geant/NeutronNudyFissionXsec.h"
#include "Geant/NeutronNudyCaptureXsec.h"

namespace userapplication {

TestNudy1PhysicsList::TestNudy1PhysicsList(const std::string &name) : geantphysics::PhysicsList(name)
{
  fMSCSteppingAlgorithm = geantphysics::MSCSteppingAlgorithm::kUseSaftey; // opt0 step limit type
  fStepMaxValue         = geantphysics::PhysicsProcess::GetAVeryLargeValue();
}

TestNudy1PhysicsList::~TestNudy1PhysicsList() {}

void TestNudy1PhysicsList::Initialize()
{
  // get the partcile table and loop over it
  std::vector<geantphysics::Particle *> pTable = geantphysics::Particle::GetTheParticleTable();
  for (unsigned int i = 0; i < pTable.size(); ++i) {
    geantphysics::Particle *particle = pTable[i];
    if (particle == geantphysics::Electron::Definition()) {
      // std::cout<<"  ELECTRON" <<std::endl;
      //
      // create ionization process for e- with 1 model:
      //
      geantphysics::EMPhysicsProcess *eIoniProc = new geantphysics::ElectronIonizationProcess("e-Ioni");
      // create the Moller-Bhabha model for ionization i.e. for e- + e- -> e- + e- intercation
      geantphysics::EMModel *eMBModel = new geantphysics::MollerBhabhaIonizationModel(true);
      // set min/max energies of the model
      eMBModel->SetLowEnergyUsageLimit(1.0 * geant::units::keV);
      eMBModel->SetHighEnergyUsageLimit(100.0 * geant::units::TeV);
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
      geantphysics::EMModel *eSBModel = new geantphysics::SeltzerBergerBremsModel(true);
      // set min/max energies of the model
      eSBModel->SetLowEnergyUsageLimit(1.0 * geant::units::keV);
      eSBModel->SetHighEnergyUsageLimit(1.0 * geant::units::GeV);
      // how to inactivate this model in a given region i.e. region with index 1
      // active regions for a model are set based on their process active regions + user requested inactive regions
      // eSBModel->AddToUserRequestedInActiveRegions(1);
      //
      // add this model to the process
      eBremProc->AddModel(eSBModel);
      //
      // create a RelativisticBremsModel for e-
      geantphysics::EMModel *eRelBModel = new geantphysics::RelativisticBremsModel();
      // set min/max energies of the model
      eRelBModel->SetLowEnergyUsageLimit(1.0 * geant::units::GeV);
      eRelBModel->SetHighEnergyUsageLimit(100.0 * geant::units::TeV);
      // add this model to the process
      eBremProc->AddModel(eRelBModel);
      //
      // add the process to the e- particle
      AddProcessToParticle(particle, eBremProc);
      //
      // create MSC process
      geantphysics::EMPhysicsProcess *eMSCProc = new geantphysics::MSCProcess("e-msc");
      // create GS-msc model, set min/max usage limits
      geantphysics::GSMSCModel *gsMSCModel = new geantphysics::GSMSCModel();
      gsMSCModel->SetRangeFactor(0.06);
      gsMSCModel->SetMSCSteppingAlgorithm(fMSCSteppingAlgorithm);
      gsMSCModel->SetLowEnergyUsageLimit(100. * geant::units::eV);
      gsMSCModel->SetHighEnergyUsageLimit(100. * geant::units::TeV);
      eMSCProc->AddModel(gsMSCModel);
      // add process to particle
      AddProcessToParticle(particle, eMSCProc);
    }
    if (particle == geantphysics::Proton::Definition() || particle == geantphysics::Neutron::Definition() ||
        particle == geantphysics::PionPlus::Definition() || particle == geantphysics::PionMinus::Definition() ||
        particle == geantphysics::PionZero::Definition() || particle == geantphysics::KaonPlus::Definition() ||
        particle == geantphysics::KaonMinus::Definition() || particle == geantphysics::KaonZero::Definition() ||
        particle == geantphysics::KaonShort::Definition() || particle == geantphysics::KaonLong::Definition()) {
      // create hadronic elastic process for proton:
      //
      geantphysics::HadronicProcess *helProc = new geantphysics::ElasticScatteringProcess();
      // create the diffuse elastic model for elastic scattering
      //
      // create the ENDF based elastic model for elastic scattering
      geantphysics::HadronicFinalStateModel *nudyElModel = new geantphysics::NeutronNudyElasticModel();
      // create the cross sections
      geantphysics::HadronicCrossSection *nElasticXS = new geantphysics::NeutronNudyElasticXsec();

      // set min/max energies of the model
      nudyElModel->SetLowEnergyUsageLimit(1E-5 * geant::units::eV);
      nudyElModel->SetHighEnergyUsageLimit(20.0 * geant::units::MeV);
      // add the model to the process
      helProc->AddModel(nudyElModel);
      // add the cross-sections to the process
      helProc->AddCrossSection(nElasticXS);
      // add the process to the hadrons
      AddProcessToParticle(particle, helProc);
      // create hadronic Capture process for proton:
      //
      geantphysics::HadronicProcess *hcapProc = new geantphysics::CaptureProcess();

      // create the ENDF based Capture model for n,g
      geantphysics::HadronicFinalStateModel *nudyCapModel = new geantphysics::NeutronNudyCaptureModel();
      // create the cross sections
      geantphysics::HadronicCrossSection *nCaptureXS = new geantphysics::NeutronNudyCaptureXsec();

      // set min/max energies of the model
      nudyCapModel->SetLowEnergyUsageLimit(1E-5 * geant::units::eV);
      nudyCapModel->SetHighEnergyUsageLimit(20.0 * geant::units::MeV);
      // add the model to the process
      hcapProc->AddModel(nudyCapModel);
      hcapProc->AddCrossSection(nCaptureXS);
      // add the process to the hadrons
      AddProcessToParticle(particle, hcapProc);
      // create hadronic Fission process for proton:
      //
      geantphysics::HadronicProcess *hfisProc = new geantphysics::FissionProcess();
      //
      // create the ENDF based Fission model for n,fission
      geantphysics::HadronicFinalStateModel *nudyFisModel = new geantphysics::NeutronNudyFissionModel();
      // create the cross sections
      geantphysics::HadronicCrossSection *nFissionXS = new geantphysics::NeutronNudyFissionXsec();

      // set min/max energies of the model
      nudyFisModel->SetLowEnergyUsageLimit(1E-5 * geant::units::eV);
      nudyFisModel->SetHighEnergyUsageLimit(20.0 * geant::units::MeV);
      // add the model to the process
      hfisProc->AddModel(nudyFisModel);
      hfisProc->AddCrossSection(nFissionXS);
      // add the process to the hadrons
      AddProcessToParticle(particle, hfisProc);
      //
      // create hadronic Inelastic process for proton:
      //
      geantphysics::HadronicProcess *hinelProc = new geantphysics::InelasticProcess();
      // create the ENDF based Inelastic reaction model for Inelastic scattering and reactions
      geantphysics::HadronicFinalStateModel *nudyInelModel = new geantphysics::NeutronNudyInelasticModel();
      // create the cross sections
      geantphysics::HadronicCrossSection *nInelasticXS = new geantphysics::NeutronNudyInelasticXsec();

      // set min/max energies of the model
      nudyInelModel->SetLowEnergyUsageLimit(1E-5 * geant::units::eV);
      nudyInelModel->SetHighEnergyUsageLimit(20.0 * geant::units::MeV);
      // add the model to the process
      hinelProc->AddModel(nudyInelModel);
      hinelProc->AddCrossSection(nInelasticXS);
      //
      // add the process to the hadrons
      AddProcessToParticle(particle, hinelProc);
    }
  }
}

void TestNudy1PhysicsList::SetMSCStepLimit(geantphysics::MSCSteppingAlgorithm stepping)
{
  fMSCSteppingAlgorithm = stepping;
}

void TestNudy1PhysicsList::SetStepMaxValue(double val)
{
  fStepMaxValue = val;
}

} // namespace userapplication
