/*****************************************************
 * @file TARCPhysicsList.h
 * @author Abhijit Bhattacharyya
 * @brief This file treats physics List
 ***********************************************************/

#ifndef TARC_PHYSICSLIST_H
#define TARC_PHYSICSLIST_H

#include "PhysicsList.h"
#include "MSCModel.h"
#include "MSCProcess.h"
#include "PhysicsProcess.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "GSMSCModel.h"
#include "StepMaxProcess.h"


#include "Particle.h"
#include "Proton.h"
#include "Neutron.h"
#include "Electron.h"
#include "Positron.h"
#include "Gamma.h"
#include "PionPlus.h"
#include "PionMinus.h"
#include "PionZero.h"
#include "KaonPlus.h"
#include "KaonMinus.h"
#include "KaonZero.h"
#include "KaonShort.h"
#include "KaonLong.h"

#include "ElectronIonizationProcess.h"
#include "MollerBhabhaIonizationModel.h"
#include "ElectronBremsstrahlungProcess.h"
#include "SeltzerBergerBremsModel.h"
#include "RelativisticBremsModel.h"
#include "ComptonScatteringProcess.h"
#include "KleinNishinaComptonModel.h"
#include "BetheHeitlerPairModel.h"
#include "PositronAnnihilationProcess.h"
#include "RelativisticPairModel.h"
#include "GammaConversionProcess.h"
#include "GammaPhotoElectricProcess.h"
#include "SauterGavrilaPhotoElectricModel.h"
#include "StepMaxProcess.h"
// #include "ElasticScatteringProcess.h"
#include "DiffuseElasticModel.h"
#include "GlauberGribovElasticXsc.h"



namespace tarcapp {
  class TARCPhysicsList : public geantphysics::PhysicsList {
  public:
    TARCPhysicsList(const std::string &name = "TARC-PhysicsList");
    virtual~TARCPhysicsList();
    void SetMSCSteppingAlgorithm(geantphysics::MSCSteppingAlgorithm);
    void SetStepMaxValue(double);
    void SetMSCStepLimit(geantphysics::MSCSteppingAlgorithm);
    virtual void Initialize();

  private:
    geantphysics::MSCSteppingAlgorithm fMSCSteppingAlgorithm;
    double fStepMaxValue;

};


} // namespace ends

#endif
