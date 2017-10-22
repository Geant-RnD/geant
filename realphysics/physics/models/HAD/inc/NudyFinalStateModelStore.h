/*********************************************************
 * @file NudyFinalStateModelStore.h
 * @author Abhijit Bhattacharyya
 * @brief Collection of Nudy final state models as pointers to object
 * from the abstract base class NudyFinalStateModel
 *******************************************************/

#ifndef NUDY_FINALSTATE_MODEL_STORE_H
#define NUDY_FINALSTATE_MODEL_STORE_H

#include <string>
#include <vector>
#include "Geant/Config.h"
#include "NudyFinalStateModel.h"

namespace geantphysics {
  inline namespace GEANT_IMPL_NAMESPACE {
    class Isotope;
  }

  class NudyFinalStateModel;

  class NudyFinalStateModelStore {
  public:
    NudyFinalStateModelStore();
    NudyFinalStateModelStore( const std::string name );
    NudyFinalStateModelStore( const NudyFinalStateModelStore &other );
    NudyFinalStateModelStore& operator=( const NudyFinalStateModelStore &other );
    virtual ~NudyFinalStateModelStore();

    // register nudy final state model to store
    // ordering is not important for registering
    void RegisterNudyFinalStateModel ( NudyFinalStateModel* ptrnudyfs );

    int GetIndexChosenFinalStateModel( const int projectilecode, const double projKE,
      const Isotope* targetisotope
    ) const;

    const std::vector<NudyFinalStateModel*>& GetNudyFinalStateModelVec() const { return fNudyFSVec; }

    std::string GetName() { return fName; }
    void SetName( const std::string &name ) { fName = name; }

  private:
    std::vector< NudyFinalStateModel* > fNudyFSVec;
    std::string fName;
  };
}

#endif
