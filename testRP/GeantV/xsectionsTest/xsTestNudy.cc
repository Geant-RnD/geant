#include "GlauberGribovTotalXsc.h"
#include "GlauberGribovInelasticXsc.h"
#include "GlauberGribovElasticXsc.h"
#include "Particle.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Proton.h"
#include "Neutron.h"
#include "KaonMinus.h"
#include "Electron.h"

#include "SystemOfUnits.h"
#include "PhysicalConstants.h"

#include "NudyInterface.h"

using geantphysics::Particle;
using geantphysics::Proton;
using geantphysics::Neutron;
using geantphysics::KaonMinus;

using namespace Nudy;
using NudyPhysics::NudyInterface;
using geantphysics::HadronicProcess;
using geantphysics::HadronicProcessType;



template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}


int main(int /*argc*/, char** /*argv*/) {

  std::ofstream writef("sumXsc.dat", std::ios::out ) ;

  writef.setf( std::ios::scientific, std::ios::floatfield );

  int projectileCode = 2112;
  std::string eleName =  "Pu";
  int AtomicNumber  =  94; // 4; //
  int MassNumber = 241; // 7;   //
  double temperature = 293.60608;
  /*  This is showing energy as 0.004 and not as 4.0E=6 and so
  for the time being I am testing using raw number.
  double EnergyValue = 4.0 * geant::MeV;
  */
  double EnergyValue = 1.0e+5; // 2.0e+6;  // in terms of eV    // 1.0 * geant::MeV;

  geantphysics::HadronicProcessType pType ;
  //pType = geantphysics::HadronicProcessType::kElastic;  // kFission;
  // pType = geantphysics::HadronicProcessType::kFission;  // kElastic;
  //pType = geantphysics::HadronicProcessType::kCapture;
  pType = geantphysics::HadronicProcessType::kRadioactiveDecay;

// @brief Here we provide code for projectile say 2112 for neutron, energy of projectile say 1.0 MeV
// @brief Then we provide temperature which is required for fission etc.
// @brief Then we provide element name like Pu , U etc. for which cross section is required
// @brief Z for element name
// @brief A for Mass number for the element.

NudyPhysics::NudyInterface *nudyxs = new NudyPhysics::NudyInterface(
  projectileCode, EnergyValue, temperature, eleName, AtomicNumber, MassNumber, pType
);

double xs = nudyxs->GetXS(projectileCode, EnergyValue, temperature,
  eleName, AtomicNumber, MassNumber, pType);
std::cout << "Neutron Energy = " << EnergyValue << " Isotope "
          << AtomicNumber << "-" << eleName << "-" << MassNumber << "  type: "
          << pType << " cross section = " << xs << std::endl;

/*
  double kinEnergy = 10.* geant::MeV;
  double maxEnergy = 1000 * geant::GeV;
  int particlePDG = -321;
  //int particlePDG = 2212;
  int Z = 82;
  // number of nucleons
  int N =207;
  double txs, ixs, exs;

  while (kinEnergy < maxEnergy)
    {

    txs = totxs.GetIsotopeCrossSection(geantphysics::Particle::GetParticleByPDGCode(particlePDG)->GetInternalCode(),
			       kinEnergy, geantphysics::Particle::GetParticleByPDGCode(particlePDG)->GetPDGMass(), Z, N) / geant::millibarn;

    ixs = inexs.GetIsotopeCrossSection(geantphysics::Particle::GetParticleByPDGCode(particlePDG)->GetInternalCode(),
			       kinEnergy, geantphysics::Particle::GetParticleByPDGCode(particlePDG)->GetPDGMass(), Z, N) / geant::millibarn;

    exs = elaxs.GetIsotopeCrossSection(geantphysics::Particle::GetParticleByPDGCode(particlePDG)->GetInternalCode(),
			       kinEnergy, geantphysics::Particle::GetParticleByPDGCode(particlePDG)->GetPDGMass(), Z, N) / geant::millibarn;

    std::cout <<"energyKin " << kinEnergy <<  " total " << txs << " elastic " << exs << " inelastic " << ixs <<  std::endl;

    writef << kinEnergy/geant::MeV <<"  \t" << ixs << "  \t"
     << exs << "  \t"<< 0 << "  \t"<< txs << std::endl;

    kinEnergy *= 1.001;
  }
*/
  return 0;
}
