
#include "CutConverterForElectron.h"

#include "Material.h"
#include "MaterialProperties.h"
#include "Element.h"

#include <cmath>
#include <iostream>

namespace geantphysics {

CutConverterForElectron::CutConverterForElectron(int numebins, double mincutenergy, double maxcutenergy)
: CutConverter(1, numebins, mincutenergy, maxcutenergy) {
  if (fMinCutEnergy >= fMaxCutEnergy) {
    std::cerr << "  *** ERROR in CutConverterForElectron::CutConverterForElectron() " << std::endl
              << "       minenergy = "<< mincutenergy/geant::units::GeV
              << " [GeV] >= maxenergy = "
              << maxcutenergy/geant::units::GeV << " [GeV]"
              << std::endl;
    exit(-1);
  }
  Initialise();
}


// must be called before using the Convert method if new element has been inserted into the Element table!
void CutConverterForElectron::Initialise() {
  CutConverter::Initialise();
  BuildElossOrAbsXsecTable();
}


CutConverterForElectron::~CutConverterForElectron() {}


double CutConverterForElectron::ComputeELossOrAbsXsecPerAtom(double zet, double ekin) {
  const double cbr1 = 0.02;
  const double cbr2 = -5.7e-5;
  const double cbr3 = 1.;
  const double cbr4 = 0.072;
  const double tlow = 10.*geant::units::keV;
  const double thigh = 1.*geant::units::GeV;
  const double mass = geant::units::kElectronMassC2;
  const double taul = tlow/mass;
  const double cpot = 1.6e-5*geant::units::MeV;
  const double fact = geant::units::kTwoPi*geant::units::kElectronMassC2*geant::units::kClassicElectronRadius*geant::units::kClassicElectronRadius;

  double ionpot     = cpot*std::exp(0.9*std::log(zet))/mass;
  double ionpotlog  = std::log(ionpot);

  // calculate approximated dE/dx for electrons
  double tau  = ekin/mass;
  double dEdx = 0.0;
  if (tau<taul) {
    double t1    = taul+1.;
    double t2    = taul+2.;
    double tsq   = taul*taul;
    double beta2 = taul*t2/(t1*t1);
    double f     = 1.-beta2+std::log(tsq/2.)+(0.5+0.25*tsq+(1.+2.*taul)*std::log(0.5))/(t1*t1);
    dEdx         = (std::log(2.*taul+4.)-2.*ionpotlog+f)/beta2;
    dEdx         = fact*zet*dEdx;
    double clow  = dEdx*std::sqrt(taul);
    dEdx         = clow/std::sqrt(tau);
  } else {
    double t1    = tau+1.;
    double t2    = tau+2.;
    double tsq   = tau*tau;
    double beta2 = tau*t2/(t1*t1);
    double f     = 1.-beta2+std::log(tsq/2.)+(0.5+0.25*tsq+(1.+2.*tau)*std::log(0.5))/(t1*t1);
    dEdx         = (std::log(2.*tau+4.)-2.*ionpotlog+f)/beta2;
    dEdx         = fact*zet*dEdx;
    // loss from bremsstrahlung follows
    double cbrem = (cbr1+cbr2*zet)*(cbr3+cbr4*std::log(ekin/thigh));
    cbrem        = 0.1*zet*(zet+1.)*cbrem*tau/beta2;
    dEdx        += fact*cbrem;
  }
  return dEdx;
}


} // namespace geantphysics