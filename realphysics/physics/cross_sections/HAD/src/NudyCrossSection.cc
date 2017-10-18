//============================================================
// @file NudyCrossSection.cc
// @author Abhijit Bhattacharyya
// @brief This file makes and interface of NUDY to GV
//============================================================

#include <iostream>
#include "NudyCrossSection.h"

using namespace geantphysics;
using namespace Nudy;
using namespace NudyPhysics;

// namespace geantphysics {

NudyCrossSection::NudyCrossSection(): fIsoName("Fe"), fTargetA(56), fTargetZ(26), fProjEnergy(4.0) {}

NudyCrossSection::NudyCrossSection( const std::string isoName, const int Aval,
	const int Zval, const double projectileKE, const geantphysics::HadronicProcessType pType
) : fIsoName(isoName), fTargetA(Aval), fTargetZ(Zval), fProjEnergy(projectileKE) {}

NudyCrossSection::NudyCrossSection( const NudyCrossSection &other ) :
fIsoName(other.fIsoName), fTargetA(other.fTargetA), fTargetZ(other.fTargetZ),
fProjEnergy(other.fProjEnergy) {}

NudyCrossSection& NudyCrossSection::operator=( const NudyCrossSection &other ) {
  if ( this != &other ) {
    fIsoName = other.fIsoName;
    fTargetA = other.fTargetA;
		fTargetZ = other.fTargetZ;
		fProjEnergy = other.fProjEnergy;
  }
  return *this;
}

NudyCrossSection::~NudyCrossSection() {}

bool NudyCrossSection::IsApplicable(const int projCode, const double projKE ) {
  bool isOK = true;
  if (projCode != 2112 && projKE > 20.0 * geant::MeV) isOK = false;  // neutron 3 || 2112 ? and KE < 20 MeV
	return isOK;
}

double NudyCrossSection::GetIsotopeCrossSection(int projCode, double projKE,
	double temp, std::string isoName, int tZ, int tA, HadronicProcessType pType){
		double XSvalue = 0.0;
		temp = (temp == -99.99) ? 293.60608 : temp;

		NudyInterface nudyInt;

		XSvalue =  nudyInt.GetXS( projCode, projKE, temp,  isoName, tZ, tA, pType );
		return XSvalue;
	}


// }  // namespace
