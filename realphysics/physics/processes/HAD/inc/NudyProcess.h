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
#include "HadronicProcess.h"
#include "LightTrack.h"
#include "HadronicCrossSectionStore.h"
#include "HadronicFinalStateModelStore.h"
#include "HadronicFinalStateModel.h"
#include "NudyCrossSection.h"
#include "NudyCrossSectionStore.h"

#include "Isotope.h"
#include "Material.h"
#include "MaterialProperties.h"
#include "MaterialCuts.h"

namespace NudyPhysics {
   class NudyInterface;
}

namespace geantphysics {
   class LightTrack;
   class LightTrack_v;
   class HadronicCrossSection;
   class HadronicCrossSectionStore;
   class HadronicFinalStateModelStore;
   class HadronicFinalStateModel;
   enum class HadronicProcessType;
   inline namespace GEANT_IMPL_NAMESPACE {
     class Isotope;
     class Material;
     class Element;
   }

   class NudyCrossSection;
   class NudyCrossSectionStore;

// class NudyProcess : public PhysicsProcess {
class NudyProcess : public PhysicsProcess{
public:
  // constructor
  NudyProcess();
  NudyProcess( const std::string &name );
  NudyProcess( const std::string &name, const std::vector<int> &particlecodevec,
    const HadronicProcessType pType, const bool isatrest,
    NudyCrossSectionStore* xsecStore, HadronicFinalStateModelStore* modelStore
  );

  // destructor
  virtual ~NudyProcess();

public:
  virtual double ComputeMacroscopicXSection(
    const MaterialCuts *matCuts, double kEnergy,  const Particle *particle,
    HadronicProcessType pType
  ) const;

  virtual double GetAtomicCrossSection(
    const int projCode, const double projectileEnergy, const Element* targetElement,  HadronicProcessType pType
  ) const;

  Isotope* SampleTarget ( LightTrack &track, HadronicProcessType pType ) const;
  void AddModel( HadronicFinalStateModel *model );
  void AddCrossSection( NudyCrossSection *xsection );

  HadronicProcessType GetType() const { return fType; }

  const std::vector< int >& GetParticleCodeVec() const { return fParticleCodeVec; }
  const NudyCrossSectionStore* GetCrossSectionStore() const { return fXsecStore; }
  const HadronicFinalStateModelStore* GetFinalStateModelStore() const { return fModelStore; }

  void SetParticleCodeVec( const std::vector< int > &particleCodeVec ) {
    fParticleCodeVec.clear();
    for (size_t i = 0; i < particleCodeVec.size(); i++ ) {
      fParticleCodeVec.push_back(particleCodeVec[i]);
    }
  }

  void SetCrossSectionStoreStore ( NudyCrossSectionStore* xsecstore ) { fXsecStore = xsecstore; }
  void SetFinalStateModelStoreStore( HadronicFinalStateModelStore* modelStore ) { fModelStore = modelStore; }

private:
  std::vector< int > fParticleCodeVec;
  HadronicProcessType fType;
  NudyCrossSectionStore* fXsecStore;
  HadronicFinalStateModelStore* fModelStore;

};   // end of Class NudyProcess

} // namespace

#endif
