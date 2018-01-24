/*********************************************
 * @file NudyProcess.h
 * @author Abhijit Bhattacharyya
 * @brief Base class for all physics processes those Nudy may cater
 *        We are following HadronicProcess.h file written by MN and AR as reference
 *********************************************/
#ifndef NUDY_PROCESS_H
#define NUDY_PROCESS_H

#include <iostream>
#include <string>
#include <vector>

#include "PhysicsProcess.h"
// #include "HadronicProcess.h"
#include "LightTrack.h"
// #include "HadronicCrossSectionStore.h"
// #include "HadronicFinalStateModelStore.h"
// #include "HadronicFinalStateModel.h"
#include "NudyCrossSection.h"
#include "NudyCrossSectionStore.h"
#include "NudyFinalStateModel.h"
#include "NudyFinalStateModelStore.h"

#include "Isotope.h"
#include "Material.h"
#include "MaterialProperties.h"
#include "MaterialCuts.h"

/*
namespace NudyPhysics {
   class NudyInterface;
}
*/

namespace geantphysics {
   class LightTrack;
   class LightTrack_v;
   // class HadronicCrossSection;
   // class HadronicCrossSectionStore;
   // class HadronicFinalStateModelStore;
   // class HadronicFinalStateModel;
   // enum class HadronicProcessType;
   inline namespace GEANT_IMPL_NAMESPACE {
     class Isotope;
     class Material;
     class Element;
   }

   class NudyCrossSection;
   class NudyCrossSectionStore;
   class NudyFinalStateModelStore;
   class NudyFinalStateModel;
   enum class NudyProcessType{
     kNotDefined,
     kElastic,
     kFission,
     kInelastic,
     kCapture,
     kRadioActiveDecay,
     kQuasiElastic,
     kLeptoNuclear,
     kUserDefined
   };


// class NudyProcess : public PhysicsProcess {
class NudyProcess : public PhysicsProcess{
public:
  // constructor
  NudyProcess();
  NudyProcess( const std::string &name );
  NudyProcess( const std::string &name, const std::vector<int> &particlecodevec,
    // const HadronicProcessType pType, const bool isatrest,
    const NudyProcessType pType, const bool isatrest,
    NudyCrossSectionStore* xsecStore, NudyFinalStateModelStore* modelStore
    // HadronicFinalStateModelStore* modelStore
  );

  // destructor
  virtual ~NudyProcess();

public:
  virtual double ComputeMacroscopicXSection(
    const MaterialCuts *matCuts, double kEnergy,  const Particle *particle,
    NudyProcessType pType  //HadronicProcessType pType
  ) const;

  virtual double GetAtomicCrossSection(
    const int projCode, const double projectileEnergy, const Element* targetElement,  NudyProcessType pType
    // HadronicProcessType pType
  ) const;

  // Isotope* SampleTarget ( LightTrack &track, HadronicProcessType pType ) const;
  Isotope* SampleTarget ( LightTrack &track, NudyProcessType pType ) const;
  void AddModel( NudyFinalStateModel *model );
  void AddCrossSection( NudyCrossSection *xsection );
  // void AddModel( HadronicFinalStateModel *model );

  //HadronicProcessType GetProcHType() const { return fProcHType; }
  NudyProcessType GetProcType() const { return fProcNType; }

  const std::vector< int >& GetParticleCodeVec() const { return fParticleCodeVec; }
  const NudyCrossSectionStore* GetCrossSectionStore() const { return fXsecStore; }
  //const HadronicFinalStateModelStore* GetFinalHadronicStateModelStore() const { return fModelHStore; }
  const NudyFinalStateModelStore* GetFinalNudyStateModelStore() const { return fModelNStore; }

  void SetParticleCodeVec( const std::vector< int > &particleCodeVec ) {
    fParticleCodeVec.clear();
    for (size_t i = 0; i < particleCodeVec.size(); i++ ) {
      fParticleCodeVec.push_back(particleCodeVec[i]);
    }
  }

  void SetCrossSectionStoreStore ( NudyCrossSectionStore* xsecstore ) { fXsecStore = xsecstore; }
  //void SetFinalHadronicStateModelStoreStore( HadronicFinalStateModelStore* modelStore ) { fModelHStore = modelStore; }
  void SetFinalNudyStateModelStoreStore( NudyFinalStateModelStore* modelStore ) { fModelNStore = modelStore; }

private:
  std::vector< int > fParticleCodeVec;
  //HadronicProcessType fProcHType;
  NudyProcessType fProcNType;
  NudyCrossSectionStore* fXsecStore;
  //HadronicFinalStateModelStore* fModelHStore;
  NudyFinalStateModelStore* fModelNStore;

};   // end of Class NudyProcess

} // namespace

#endif
