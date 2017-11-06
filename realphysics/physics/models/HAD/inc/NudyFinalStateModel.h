/************************************************************
 * @file NudyFinalStateModel.h
 * @author Abhijit Bhattacharyya
 * @brief State model on cross section
 ************************************************************/

#ifndef NUDY_FINALSTATE_MODEL_H
#define NUDY_FINALSTATE_MODEL_H

#include <string>
#include <vector>

//#include "HadronicProcess.h"
#include "NudyProcess.h"
#include "NudyCrossSectionStore.h"
#include "GeantTaskData.h"

namespace NudyPhysics {
  class NudyInterface;
}

namespace geantphysics {
  class LightTrack;
  class NudyCrossSectionStore;
  class NudyCrossSectionStore;
  inline namespace GEANT_IMPL_NAMESPACE {
    class Isotope;
    class Material;
    class Element;
  }
  //enum class HadronicProcessType;
  enum class NudyModelType {
    kNotDefined,
    kElastic,
    kFission,
    kInelastic,
    kCapture,
    kRadioActiveDecay,
    kUserDefined
  };

  class NudyFinalStateModel {
  public:
    NudyFinalStateModel();
    NudyFinalStateModel(
      const std::string name, const std::vector<int> &particlecodevec,
      const NudyModelType pType, const double minEnergyLim, const double maxEnergyLim,
      const double minTargetZ, const double maxTargetZ,
      const double minTargetA, const double maxTargetA
    );
    NudyFinalStateModel( const NudyFinalStateModel &other );
    NudyFinalStateModel& operator=( const NudyFinalStateModel &other );
    virtual ~NudyFinalStateModel();

    const std::vector< int >& GetProjectileCodeVec() const { return fProjectileCodeVec; }
    std::string GetName() const { return fName; }
    NudyModelType GetModelNType() const { return fModelNType; }
    double GetLowEnergyUsageLimit() const { return fLowEnergyLimit; }
    double GetHighEnergyUsageLimit() const { return fHighEnergyLimit; }
    double GetMinTargetZ() const { return fMinTargetZ; }
    double GetMinTargetA() const { return fMinTargetA; }
    double GetMaxTargetZ() const { return fMaxTargetZ; }
    double GetMaxTargetA() const { return fMaxTargetA; }

    void SetName( const std::string &name ) { fName = name; }
    void SetModelNType( const NudyModelType type ) { fModelNType = type; }
    void SetLowEnergyUsageLimit( const double minE ) { fLowEnergyLimit = minE; }
    void SetHighEnergyUsageLimit( const double maxE ) { fHighEnergyLimit = maxE; }
    void SetMinTargetZ( const double minZ ) { fMinTargetZ = minZ; }
    void SetMinTargetA( const double minA ) { fMinTargetA = minA; }
    void SetMaxTargetZ( const double maxZ ) { fMaxTargetZ = maxZ; }
    void SetMaxTargetA( const double maxA ) { fMaxTargetA = maxA; }

    virtual bool IsApplicable(
      const int projectilecode, const double projectileKE,
      const Isotope* targetisotope
    );

    void SetProjectileCodeVec( const std::vector< int > &projectileCodeVec ) {
      fProjectileCodeVec.clear();
      for ( unsigned int i = 0; i < projectileCodeVec.size(); i++ ) {
        fProjectileCodeVec.push_back( projectileCodeVec[i] );
      }
    }

  private:
    std::string fName;
    std::vector< int > fProjectileCodeVec;
    //HadronicProcessType fProcHType;
    NudyProcessType fProcNType;
    NudyModelType fModelNType;
    double fLowEnergyLimit;
    double fHighEnergyLimit;
    double fMinTargetZ;
    double fMaxTargetZ;
    double fMinTargetA;
    double fMaxTargetA;
  };

} // namespace

#endif
