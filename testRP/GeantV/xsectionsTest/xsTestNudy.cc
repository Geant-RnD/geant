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
using geantphysics::NudyProcess;
using geantphysics::NudyProcessType;
//using geantphysics::HadronicProcess;
//using geantphysics::HadronicProcessType;


NudyPhysics::NudyInterface  nudyxs;
geantphysics::NudyProcessType pType ;

template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

void dumpVerb(){
  // This simply transforms the raw ENDF data file to raw ROOT file
  // without linearization etc.
  std::string fInENDF="";
  std::string fOutROOT="";

  std::cout << "Enter ENDF data file name: ";
  std::getline (std::cin,fInENDF);
  std::cout << "Enter ROOT file name to produce: ";
  std::getline(std::cin, fOutROOT);
  if (fInENDF.length() < 1) nudyxs.printE2RErr();

  nudyxs.ConvertENDF2ROOT(fInENDF, fOutROOT);
}

void DumpProcE2R() {
    ////////////////////////////////////
    // This is an example to show dumping of ENDF data file
    // to ROOT file after computing resonance parameters, linearization etc.
      // DumpEndf2Root(std::string fIN, std::string fOUT, std::string fSUBName,
      // int tA, int tZ, double temp, std::string isotopeN)
    std::string fEndfIN = "";
    std::string fRootOUT = "";
    std::string fEndfSubIN = "";
    double temp = 293.60608;
    std::cout << " ENDF data file name as INPUT: ";
    std::getline(std::cin, fEndfIN);
    std::cout << " ROOT data file name as OUTPUT: ";
    std::getline(std::cin,fRootOUT);
    std::cout << " ENDFSUB file name < If does not exist, please ENTER <ZZ>: ";
    std::getline(std::cin, fEndfSubIN);
    //std::cout << " ENDFSUB data file name for fission data as INPUT: ";
    //std::cin >> fEndfSubIN;
    std::cout << "Temperature: "; std::cin >> temp;
    temp = (temp == 0.0) ? 293.60608 : temp;
    nudyxs.DumpEndf2Root(fEndfIN, fEndfSubIN, fRootOUT, temp);
    //////////////////////////////////
}

void CalXS() {
    int projectileCode = 2112;
    std::string eleName =  "Pu";
    int AtomicNumber  =  94; // 4; //
    int MassNumber = 241; // 7;   //
    double temperature = 293.60608;
    double EnergyValue = 14.0 * geant::MeV; // 1.0e+5; // 2.0e+6;  // in terms of eV    // 1.0 * geant::MeV;

    //pType = geantphysics::HadronicProcessType::kElastic;  // kFission;
    //pType = geantphysics::HadronicProcessType::kFission;  // kElastic;
    //pType = geantphysics::HadronicProcessType::kCapture;
    //pType = geantphysics::HadronicProcessType::kRadioactiveDecay;

  // @brief Here we provide code for projectile say 2112 for neutron, energy of projectile say 1.0 MeV
  // @brief Then we provide temperature which is required for fission etc.
  // @brief Then we provide element name like Pu , U etc. for which cross section is required
  // @brief Z for element name
  // @brief A for Mass number for the element.

  double Eval = EnergyValue/geant::eV;

  pType = geantphysics::NudyProcessType::kElastic;
  double xse = nudyxs.GetXS(projectileCode, Eval, temperature, eleName, AtomicNumber, MassNumber, pType);
  pType = geantphysics::NudyProcessType::kFission;
  double xsf = nudyxs.GetXS(projectileCode, Eval, temperature, eleName, AtomicNumber, MassNumber, pType);
  pType = geantphysics::NudyProcessType::kCapture;
  double xsc = nudyxs.GetXS(projectileCode, Eval, temperature, eleName, AtomicNumber, MassNumber, pType);
  std::cout << "Neutron Energy = " << EnergyValue/geant::MeV << " MeV " << " Isotope "
            << AtomicNumber << "-" << eleName << "-" << MassNumber
            << " Elastic :: XS = " << xse << " Fission :: XS = " << xsf << " Capture :: XS = " << xsc
            << std::endl;
}


int main(int /*argc*/, char** /*argv*/) {
std::cout << " Select choise:----"                                                   << std::endl
          << "\t 1. Dump ENDF data file to ROOT verbatim. "                          << std::endl
		      << "\t 2. Dump ENDF data file to ROOT after processing, linearizing etc. " << std::endl
		      << "\t 3. Compute cross section example for 94-Pu-241 for 14 Mev neutron." << std::endl
		      << "\t 4. Quit."                                                           << std::endl
          << "\t \t ENTER CHOICE:--->";
int cInput;
do {
  std::cin >> cInput ;
} while (cInput < 1 || cInput > 4);

std::cout << std::endl;
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

switch (cInput) {
  case 1: dumpVerb();
          exit(0);
  case 2: DumpProcE2R();
          exit(0);
  case 3: CalXS();
  case 4: exit(0);
}




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
