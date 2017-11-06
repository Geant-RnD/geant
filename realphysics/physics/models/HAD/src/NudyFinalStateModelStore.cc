#include "NudyFinalStateModel.h"
#include "NudyFinalStateModelStore.h"
#include "Isotope.h"

using namespace geantphysics;

NudyFinalStateModelStore::NudyFinalStateModelStore() : fName(""){}

NudyFinalStateModelStore::NudyFinalStateModelStore( const std::string name ) : fName(name){}

NudyFinalStateModelStore::NudyFinalStateModelStore( const NudyFinalStateModelStore &other ) : fName( other.fName ) {
  for ( size_t i = 0; i < other.fNudyFSVec.size(); i++ ){
    fNudyFSVec.push_back( other.fNudyFSVec[i] );
  }
}


NudyFinalStateModelStore& NudyFinalStateModelStore::operator=( const NudyFinalStateModelStore &other ) {
  if ( this != &other ) {
    fNudyFSVec.clear();
    for ( size_t i = 0; i < other.fNudyFSVec.size(); i++ ) {
      fNudyFSVec.push_back( other.fNudyFSVec[i] );
    }
    fName = other.fName;
  }
  return *this;
}

NudyFinalStateModelStore::~NudyFinalStateModelStore() {
  for ( size_t i = 0; i < fNudyFSVec.size(); i++ ) {
    delete fNudyFSVec[i];
  }
  fNudyFSVec.clear();
}

void NudyFinalStateModelStore::RegisterNudyFinalStateModel( NudyFinalStateModel* ptrnudyfs ) {
  if ( ptrnudyfs ) {
    fNudyFSVec.push_back( ptrnudyfs );
  }
}

int NudyFinalStateModelStore::
GetIndexChosenFinalStateModel( const int projcode, const double projKE,
                               const Isotope* targetisotope ) const {
  int index = -1;
  std::vector< int > indexApplicableModelVec;

  for ( size_t i = 0; i < fNudyFSVec.size(); i++ ) {
    if ( fNudyFSVec[i]  &&  fNudyFSVec[i]->IsApplicable( projcode, projKE,
                                                       targetisotope )) {
      indexApplicableModelVec.push_back( i );
    }
  }

  if ( indexApplicableModelVec.size() == 1 ) {
    index = indexApplicableModelVec[0];
  } else if ( indexApplicableModelVec.size() == 2 ) {
    // The "first" index corresponds to the model with the lowest minimal energy
    int first = indexApplicableModelVec[0];
    int second = indexApplicableModelVec[1];
    if ( fNudyFSVec[ first ]->GetLowEnergyUsageLimit() > fNudyFSVec[ second ]->GetLowEnergyUsageLimit() ) {
      first = second;
      second = indexApplicableModelVec[0];
    }
    if ( fNudyFSVec[ first ]->GetHighEnergyUsageLimit() >= fNudyFSVec[ second ]->GetHighEnergyUsageLimit() ) {
      std::cerr << "NudyFinalStateModelStore::GetIndexChosenFinalState : projectilecode=" << projcode
                << " ; projectilekineticenergy=" << projKE << " GeV; targetisotope: Z="
                << targetisotope->GetZ() << " N=" << targetisotope->GetA() << std::endl
                << "\t NOT allowed full overlapping between two models: " << fNudyFSVec[ first ]->GetName()
                << " , "  << fNudyFSVec[ second ]->GetName() << std::endl;
    } else {  // All if fine: first model applicable to lower energies than the second model
      // Select one of the two models with probability depending linearly on the projectilekineticenergy
      double probFirst = ( fNudyFSVec[ first ]->GetHighEnergyUsageLimit() - projKE ) /
                         ( fNudyFSVec[ first ]->GetHighEnergyUsageLimit() - fNudyFSVec[ second ]->GetLowEnergyUsageLimit() );
      double randomNumber = 0.5;  //***LOOKHERE*** TO-BE-REPLACED with a call to a random number generator.
      if ( randomNumber < probFirst ) {
        index = first;
      } else {
        index = second;
      }
    }
  } else {
    std::cerr << "NudyFinalStateModelStore::GetIndexChosenFinalState : projectilecode=" << projcode
              << " ; projectilekineticenergy=" << projKE << " GeV; targetisotope: Z="
              << targetisotope->GetZ() << " N=" << targetisotope->GetN() << std::endl
              << "\t wrong number of applicable final-state models: " << indexApplicableModelVec.size() << std::endl;
  }
  return index;
}
