/********************************************
 * @file NudyCrossSection.h
 ********************************************/
#include "NudyCrossSectionStore.h"
#include "NudyCrossSection.h"
#include "Isotope.h"
#include "Element.h"
#include "Material.h"
#include "MaterialProperties.h"

using namespace geantphysics;
using namespace NudyPhysics;

NudyCrossSectionStore::NudyCrossSectionStore () : fChannelName("") {}

NudyCrossSectionStore::NudyCrossSectionStore (const std::string name ) : fChannelName( name ) {}

NudyCrossSectionStore::NudyCrossSectionStore( const NudyCrossSectionStore &other ) :
  fChannelName( other.fChannelName )
{
  for ( size_t i = 0; i < other.fNudyXsecVec.size(); i++ ) {
    fNudyXsecVec.push_back( other.fNudyXsecVec[i] );
  }
}

NudyCrossSectionStore& NudyCrossSectionStore::operator=( const NudyCrossSectionStore &other ) {
  if ( this != &other ) {
    fNudyXsecVec.clear();
    for ( size_t i = 0; i < other.fNudyXsecVec.size(); i++ ) {
      fNudyXsecVec.push_back( other.fNudyXsecVec[i] );
    }
    fChannelName = other.fChannelName;
  }
  return *this;
}

NudyCrossSectionStore::~NudyCrossSectionStore () {
  for ( size_t i = 0; i < fNudyXsecVec.size(); i++ ) {
    delete fNudyXsecVec[i];
  }
}

void NudyCrossSectionStore::RegisterNudyCrossSection( NudyCrossSection* ptrnudyxsec ) {
  if ( ptrnudyxsec ) {
    fNudyXsecVec.push_back( ptrnudyxsec );
  }
}

int NudyCrossSectionStore::GetIndexFirstApplicableXsec (
  const int projectileCode, const double projectileEnergy
  //, const Element* targetElement, const Material* targetMaterial
) {
    int index = -1;
    for (int i = fNudyXsecVec.size() -1; i >= 0; i-- ) {
      if ( fNudyXsecVec[i] && fNudyXsecVec[i]->IsApplicable( projectileCode, projectileEnergy ) ) {
          index = i;
          break;
        }
    }
    return index;
}

double NudyCrossSectionStore::GetIsotopeCrossSection( const int projectileCode,
  const double projEnergy, const Isotope* targetIsotope,  NudyProcessType pType
  // const double projEnergy, const Isotope* targetIsotope,  HadronicProcessType pType
) {
  double xsec = -1.0;
  int index = GetIndexFirstApplicableXsec ( projectileCode, projEnergy );
  if ( index >= 0 ) {
    xsec = fNudyXsecVec[index]->GetIsotopeCrossSection ( projectileCode,
      projEnergy, -99.99, targetIsotope->GetName(), targetIsotope->GetZ(), targetIsotope->GetA(), pType );
  }
  return xsec;
}


double NudyCrossSectionStore::GetElementCrossSection(
  // const int projectileCode, const double projEnergy, const Element* targetElement, HadronicProcessType pType
  const int projectileCode, const double projEnergy, const Element* targetElement, NudyProcessType pType

) {
  double xsec = -1.0;
  int index = GetIndexFirstApplicableXsec(
    projectileCode, projEnergy
  );

  if ( index >= 0 ) {
    const Vector_t< geantphysics::Isotope* > isotopeVector = targetElement->GetIsotopeVector();
      const double* abundanceIsotopeVector = targetElement->GetRelativeAbundanceVector();
      xsec = 0.0;
      for ( size_t i = 0; i < isotopeVector.size(); i++ ) {
        double isotopeXsec = fNudyXsecVec[index]->GetIsotopeCrossSection(
          projectileCode, projEnergy, -99.99, targetElement->GetName(),  // temp =-99.99 means default
          targetElement->GetZ(), targetElement->GetA(), pType); // geantphysics::HadronicProcessType pType

        if ( isotopeXsec < 0.0 ) {
          xsec = -1.0;
          break;
        }
        xsec += abundanceIsotopeVector[i] * isotopeXsec;
    }
  }
  return xsec;
}

double NudyCrossSectionStore::GetMacroscopicCrossSection(
  const int projectileCode, const double projectileEnergy,
  const Material* targetMaterial, NudyProcessType pType
  // const Material* targetMaterial, HadronicProcessType pType
) {
  double xsec = -1.0;
  if ( targetMaterial ) {
    Vector_t< Element* > elementVector = targetMaterial->GetElementVector();
    const double* numOfAtomsPerVolumeVector =  targetMaterial->GetMaterialProperties()->GetNumOfAtomsPerVolumeVect();
    xsec = 0.0;
    for ( size_t i = 0; i < elementVector.size(); i++ ) {
      double elementXsec = GetElementCrossSection( projectileCode,
        projectileEnergy, elementVector[i], pType );
        if ( elementXsec < 0.0 ) {
          xsec = -1.0;
          continue;
        }
        xsec += elementXsec * numOfAtomsPerVolumeVector[i];
    }
  }
  return xsec;
}

std::pair< int, int > NudyCrossSectionStore::SampleTarget(
  const int projectileCode, const double projectileEnergy,
  const Material* targetMaterial, NudyProcessType pType
  // const Material* targetMaterial, HadronicProcessType pType
) {
  int tZ = 0;
  int tA = 0;
  if ( targetMaterial ) {
    Vector_t<Element*> elementVector = targetMaterial->GetElementVector();
    const double* numOfAtomsPerVolumeVector = targetMaterial->GetMaterialProperties()->GetNumOfAtomsPerVolumeVect();
    std::vector<double> sumElementXsecVector;
    double xsec = 0.0;
    for ( size_t i = 0; i < elementVector.size(); i++ ) {
      double elementXsec = GetElementCrossSection( projectileCode,
        projectileEnergy, elementVector[i],
         pType );
      if ( elementXsec < 0.0 ) break;
        xsec += elementXsec * numOfAtomsPerVolumeVector[i];
        sumElementXsecVector.push_back( xsec );
      }
      if ( xsec > 0.0  &&  sumElementXsecVector.size() == elementVector.size() ) {
        double randomNumber1 = 0.5;  //***LOOKHERE*** TO-BE-REPLACED with a call to a random number generator.
        size_t iEle = 0;
        while ( iEle < elementVector.size()  &&  randomNumber1 > sumElementXsecVector[iEle]/xsec ) {
          iEle++;
        }
        tZ = static_cast< int >( elementVector[iEle]->GetZ() );

        // Now select the isotope, i.e. the number of nucleons N
        Vector_t< Isotope* > isotopeVector = elementVector[iEle]->GetIsotopeVector();
        const double* abundanceIsotopeVector = elementVector[iEle]->GetRelativeAbundanceVector();
        std::vector< double > sumIsotopeXsecVector;
        xsec = 0.0;
        for ( size_t i = 0; i < isotopeVector.size(); i++ ) {
          double isotopeXsec = GetIsotopeCrossSection( projectileCode,
            projectileEnergy, isotopeVector[i], pType
          );
          if ( isotopeXsec < 0.0 ) break;
          xsec += abundanceIsotopeVector[i] * isotopeXsec;
          sumIsotopeXsecVector.push_back( xsec );
        }
        if ( xsec > 0.0  &&  sumIsotopeXsecVector.size() == isotopeVector.size() ) {
          double randomNumber2 = 0.5;  //***LOOKHERE*** TO-BE-REPLACED with a call to a random number generator.
          size_t iIso = 0;
          while ( iIso < isotopeVector.size()  &&  randomNumber2 > sumIsotopeXsecVector[iEle]/xsec ) {
            iIso++;
          }
          tA = isotopeVector[iIso]->GetN();
        }
      }
    }
    return std::pair< int, int >( tZ, tA );
  }
