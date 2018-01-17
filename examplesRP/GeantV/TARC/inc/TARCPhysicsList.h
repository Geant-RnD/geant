/*****************************************************
 * @file TARCPhysicsList.h
 * @author Abhijit Bhattacharyya
 * @brief This file treats physics List
 ***********************************************************/

#ifndef TARC_PHYSICSLIST_H
#define TARC_PHYSICSLIST_H

#include "PhysicsList.h"
#include "MSCModel.h"
#include "PhysicsProcess.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"


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
#include "GammaConversionProcess.h"
#include "BetheHeitlerPairModel.h"
#include "MSCProcess.h"
#include "MSCModel.h"
#include "GSMSCModel.h"
#include "StepMaxProcess.h"
//#include "ElasticScatteringProcess.h"
//#include "DiffuseElasticModel.h"
//#include "GlauberGribovElasticXsc.h"


namespace userapplication {
  class TARCPhysicsList : public geantphysics::PhysicsList {
  public:
    TARCPhysicsList(const std::string &name);
    ~TARCPhysicsList();

    void SetStepMaxValue(double aVal);
    virtual void Initialize();

  private:
  double fStepMaxValue;

};


} // namespace ends

#endif
