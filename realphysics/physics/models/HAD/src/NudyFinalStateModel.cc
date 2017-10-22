#include "NudyFinalStateModel.h"
#include "Isotope.h"

using namespace geantphysics;

NudyFinalStateModel::NudyFinalStateModel() :
     fName(""), fModelType(NudyModelType::kNotDefined), fLowEnergyLimit(0.0),
     fHighEnergyLimit(1.0e+10), fMinTargetZ(1), fMaxTargetZ(120),
     fMinTargetA(1), fMaxTargetA(300) {}


NudyFinalStateModel::NudyFinalStateModel(
  const std::string name, const std::vector< int > &projectilecodevec,
  const NudyModelType modelType, const double minE, const double maxE,
  const double minZ, const double maxZ, const double minA, const double maxA
) : fName(name), fModelType(modelType), fLowEnergyLimit(minE), fHighEnergyLimit(maxE),
fMinTargetZ(minZ), fMaxTargetZ(maxZ), fMinTargetA(minA), fMaxTargetA(maxA)
{
    SetProjectileCodeVec( projectilecodevec );
}

NudyFinalStateModel::NudyFinalStateModel( const NudyFinalStateModel &other ):
fName( other.fName ), fModelType( other.fModelType ),
fLowEnergyLimit( other.fLowEnergyLimit ), fHighEnergyLimit( other.fHighEnergyLimit ),
fMinTargetZ( other.fMinTargetZ ), fMaxTargetZ( other.fMaxTargetZ ),
fMinTargetA( other. fMinTargetA ), fMaxTargetA( other.fMaxTargetA )
{
  SetProjectileCodeVec(other.fProjectileCodeVec);
}

NudyFinalStateModel& NudyFinalStateModel::operator=( const NudyFinalStateModel &other ) {
  if ( this != &other ) {
    SetProjectileCodeVec( other.fProjectileCodeVec );
    fName = other.fName;
    fModelType = other.fModelType;
    fLowEnergyLimit = other.fLowEnergyLimit;
    fHighEnergyLimit = other.fHighEnergyLimit;
    fMinTargetZ = other.fMinTargetZ;
    fMinTargetA = other.fMinTargetA;
    fMaxTargetZ = other.fMaxTargetZ;
    fMaxTargetA = other.fMaxTargetA;
  }
  return *this;
}

NudyFinalStateModel::~NudyFinalStateModel() {}

bool NudyFinalStateModel::IsApplicable(
  const int projectilecode, const double projectileKE,
  const Isotope* targetisotope ) {
    bool isOK = false;
    for ( size_t i = 0; i < fProjectileCodeVec.size(); i++ ) {
      if ( fProjectileCodeVec[i] == projectilecode ) {
        isOK = true;
        break;
      }
    }
    if ( isOK ) {
      if ((projectileKE < fLowEnergyLimit)  ||  (projectileKE > fHighEnergyLimit )) {
        isOK = false;
      } else {
        int targetZ = 0;
        int targetA = 0;
        if ( targetisotope ) {
          targetZ = static_cast< int >( targetisotope->GetZ() );
          targetA = targetisotope->GetA();
        }
        if ( (targetZ < fMinTargetZ)  || (targetZ > fMaxTargetZ)  ||
        (targetA < fMinTargetA)  || (targetA > fMaxTargetA)) {
          isOK = false;
        }
      }
    }
    return isOK;
  }
