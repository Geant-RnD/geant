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
