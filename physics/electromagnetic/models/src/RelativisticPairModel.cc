
#include "RelativisticPairModel.h"

#include "Geant/PhysicalConstants.h"
// for Vector_t
#include "Types.h"

#include "Material.h"
#include "MaterialProperties.h"
#include "MaterialCuts.h"
#include "Element.h"
#include "ElementProperties.h"

#include "LightTrack.h"
#include "PhysicsData.h"

#include "Particle.h"
#include "Gamma.h"
#include "Electron.h"
#include "Positron.h"

#include "AliasTable.h"
#include "GLIntegral.h"

// from geantV
#include "TaskData.h"

namespace geantphysics {

// use these elastic and inelatic form factors for light elements instead of TFM
// under the complete screening approximation
// Tsai Table.B2.
const double RelativisticPairModel::gFelLowZet  [] = {0.0, 5.310, 4.790, 4.740, 4.710, 4.680, 4.620, 4.570};
const double RelativisticPairModel::gFinelLowZet[] = {0.0, 6.144, 5.621, 5.805, 5.924, 6.012, 5.891, 5.788};

const double RelativisticPairModel::gLPMFactor     = 0.25*geant::units::kFineStructConst*geant::units::kElectronMassC2*geant::units::kElectronMassC2/(geant::units::kPi*geant::units::kHBarPlanckCLight);

RelativisticPairModel::LPMFuncs  RelativisticPairModel::gLPMFuncs;
std::vector<RelativisticPairModel::ElementData*>  RelativisticPairModel::gElementData(gMaxZet,nullptr);

RelativisticPairModel::RelativisticPairModel(const std::string &modelname) : EMModel(modelname) {
  fIsUseTsaisScreening              = false;   // for xsec computation and rejection sampling only
  fIsUseLPM                         = true;    // photon energies above fLPMEnergyLimit energy

  fNGL                              = 64;

  fElectronInternalCode             = -1;      // will be set at init
  fPositronInternalCode             = -1;      // will be set at init

  fLPMEnergyLimit                   = 100.0*geant::units::GeV;  // photon energy limit above which LPM is active

  fSTNumPhotonEnergiesPerDecade     = 12;      // used only in case of sampling tables
  fSTNumDiscreteEnergyTransferVals  = 84;      // used only in case of sampling tables
  fSTNumPhotonEnergies              = -1;      // will be set at init

  fSTLogMinPhotonEnergy             = -1.;     // will be set at init in case of sampling tables
  fSTILDeltaPhotonEnergy            = -1.;     // will be set at init in case of sampling tables

  fAliasSampler                     = nullptr;

  fGL                               = nullptr;
}


RelativisticPairModel::~RelativisticPairModel() {
  // clear ElementData
  for (size_t i=0; i<gElementData.size(); ++i) {
    if (gElementData[i]) {
      delete gElementData[i];
    }
  }
  gElementData.clear();
  //
  // clear LPMFunctions (if any)
  if (fIsUseLPM) {
    gLPMFuncs.fLPMFuncG.clear();
    gLPMFuncs.fLPMFuncPhi.clear();
  }
  // clear sampling tables if any
  if (GetUseSamplingTables()) {
    ClearSamplingTables();
    delete fAliasSampler;
  }
  if (fGL) {
    delete fGL;
  }
}

void RelativisticPairModel::Initialize() {
  EMModel::Initialize();  // will set the PhysicsParameters member
  fElectronInternalCode = Electron::Definition()->GetInternalCode();
  fPositronInternalCode = Positron::Definition()->GetInternalCode();
  if (GetLowEnergyUsageLimit()<50.*geant::units::MeV) {
    std::cerr<< "  *** ERROR: RelativisticPairModel::Initialize()  \n"
             << "      The model should not be used below 50 [MeV]   "
             << std::endl;
    exit(-1);
  }
  InitialiseElementData();
  if (fIsUseLPM) {
    InitLPMFunctions();
  }
  // create one GL integral
  if (!fGL) {
    fGL = new GLIntegral(fNGL,0.0,1.0);
  }
  if (GetUseSamplingTables()) {
    InitSamplingTables();
  } else {
    InitialiseElementSelectors(this,Gamma::Definition(),true);
  }
}


double RelativisticPairModel::ComputeMacroscopicXSection(const MaterialCuts *matcut, double kinenergy,
                                                         const Particle *part) {
  double xsec = 0.0;
  if (kinenergy<GetLowEnergyUsageLimit() || kinenergy>GetHighEnergyUsageLimit()) {
    return xsec;
  }
  // compute the macroscopic cross section as the sum of the atomic cross sections weighted by the number of atoms in
  // in unit volume.
  const Material *mat =  matcut->GetMaterial();
  const double egamma = kinenergy;
  // we will need the element composition of this material
  const Vector_t<Element*> &theElements   = mat->GetElementVector();
  const double* theAtomicNumDensityVector = mat->GetMaterialProperties()->GetNumOfAtomsPerVolumeVect();
  size_t numElems = theElements.size();
  for (size_t iel=0; iel<numElems; ++iel) {
    xsec += theAtomicNumDensityVector[iel]*ComputeXSectionPerAtom(theElements[iel], matcut, egamma, part);
  }
  return xsec;
}


double RelativisticPairModel::ComputeXSectionPerAtom(const Element *elem, const MaterialCuts* matcut, double kinenergy,
                                                     const Particle*) {
   double xsec  = 0.0;
   if (kinenergy<GetLowEnergyUsageLimit() || kinenergy>GetHighEnergyUsageLimit()) {
     return xsec;
   }
   // compute the parametrized atomic cross section: depends only on target Z and gamma energy.
   xsec = ComputeAtomicCrossSection(elem, matcut->GetMaterial(), kinenergy);
   return xsec;
}


int    RelativisticPairModel::SampleSecondaries(LightTrack &track, geant::TaskData *td) {
  int    numSecondaries = 0;
  const double ekin     = track.GetKinE();
  // check if kinetic energy is below fLowEnergyUsageLimit (its minimum is 50 [MeV]) and do nothing if yes;
  // check if kinetic energy is above fHighEnergyUsageLimit andd o nothing if yes
  if (ekin<GetLowEnergyUsageLimit() || ekin>GetHighEnergyUsageLimit()) {
    return numSecondaries;
  }
  // interaction is possible so sample the sample the reduced total energy transfered to one of the secondary
  // particles i.e. either e- or e+ using sampling tables (if it was requested before initialisation) or with
  // rejection
  const MaterialCuts *matCut = MaterialCuts::GetTheMaterialCutsTable()[track.GetMaterialCutCoupleIndex()];
  const Material        *mat = matCut->GetMaterial();
  double eps = 0.0;
  if (GetUseSamplingTables()) {  // Sampling eps from table
    double *rndArray  = td->fDblArray;
    td->fRndm->uniform_array(3, rndArray);
    eps = SampleTotalEnergyTransfer(ekin, mat->GetIndex(), rndArray[0], rndArray[1], rndArray[2]);
  } else {                       // Sampling eps with rejection
    const double lpmEnergy = mat->GetMaterialProperties()->GetRadiationLength()*gLPMFactor;
    // sample target element
    const Vector_t<Element*> &theElements = mat->GetElementVector();
    double targetElemIndx = 0;
    if (theElements.size()>1) {
      targetElemIndx = SampleTargetElementIndex(matCut, ekin, td->fRndm->uniform());
    }
    const double  zet     = theElements[targetElemIndx]->GetZ();
    const int    izet     = std::min(std::lrint(zet),gMaxZet-1);
    eps = SampleTotalEnergyTransfer(ekin, lpmEnergy, izet, td);
  }
  //
  // create the secondary partcicles:
  // 1. the total elengy of e-/e+
  double electronTotE;
  double positronTotE;
  if (td->fRndm->uniform()>0.5) {
    electronTotE = (1.-eps)*ekin;
    positronTotE = eps*ekin;
  } else {
    electronTotE = eps*ekin;
    positronTotE = (1.-eps)*ekin;
  }
  //
  // 2. sample the direction: theta is sampled based on Laszlo's approximation to Tsai-s dcs
  // note: we should investigate the possibility to use the leading term of the dcs and sample cos(theta(-+))
  double *rndArray  = td->fDblArray;
  td->fRndm->uniform_array(4, rndArray);
  double uvar = -std::log(rndArray[0]*rndArray[1]);
  if (9.>36.*rndArray[2]) {
    uvar *= 1.6;
  } else {
    uvar *= 0.53333;
  }
  const double thetaElectron = uvar*geant::units::kElectronMassC2/electronTotE;
  const double sintEle       = std::sin(thetaElectron);
  const double thetaPositron = uvar*geant::units::kElectronMassC2/positronTotE;
  const double sintPos       = -std::sin(thetaPositron);
  const double phi           = geant::units::kTwoPi*rndArray[3];
  const double sinphi        = std::sin(phi);
  const double cosphi        = std::cos(phi);
  // e- direction
  double eleDirX = sintEle*cosphi;
  double eleDirY = sintEle*sinphi;
  double eleDirZ = std::cos(thetaElectron);
  // e+ direction
  double posDirX = sintPos*cosphi;
  double posDirY = sintPos*sinphi;
  double posDirZ = std::cos(thetaPositron);
  //
  // 3. kill the primary photon and create the secondaries
  track.SetKinE(0.0);
  track.SetTrackStatus(LTrackStatus::kKill);
  // 4. compute kinetic energy of e-/e+
  const double ekinElectron = std::max((electronTotE-geant::units::kElectronMassC2),0.);
  const double ekinPositron = std::max((positronTotE-geant::units::kElectronMassC2),0.);
  // 5. rotate direction back to the lab frame: current directions are relative to the photon dir as z-dir
  RotateToLabFrame(eleDirX, eleDirY, eleDirZ, track.GetDirX(), track.GetDirY(), track.GetDirZ());
  RotateToLabFrame(posDirX, posDirY, posDirZ, track.GetDirX(), track.GetDirY(), track.GetDirZ());
  //
  // 6. insert the secondary e-/e+ into the secondary list:
  numSecondaries = 2;
  // current capacity of secondary track container
  int curSizeOfSecList = td->fPhysicsData->GetSizeListOfSecondaries();
  // currently used secondary tracks in the container
  int curNumUsedSecs   = td->fPhysicsData->GetNumUsedSecondaries();
  if (curSizeOfSecList-curNumUsedSecs<numSecondaries) {
    td->fPhysicsData->SetSizeListOfSecondaries(2*curSizeOfSecList);
  }
  int secIndx     = curNumUsedSecs;
  curNumUsedSecs += numSecondaries;
  td->fPhysicsData->SetNumUsedSecondaries(curNumUsedSecs);
  std::vector<LightTrack>& sectracks = td->fPhysicsData->GetListOfSecondaries();
  // first set the e-
  sectracks[secIndx].SetDirX(eleDirX);
  sectracks[secIndx].SetDirY(eleDirY);
  sectracks[secIndx].SetDirZ(eleDirZ);
  sectracks[secIndx].SetKinE(ekinElectron);
  sectracks[secIndx].SetGVcode(fElectronInternalCode);
  sectracks[secIndx].SetMass(geant::units::kElectronMassC2);
  sectracks[secIndx].SetTrackIndex(track.GetTrackIndex()); // parent Track index
  // then set the e+
  ++secIndx;
  sectracks[secIndx].SetDirX(posDirX);
  sectracks[secIndx].SetDirY(posDirY);
  sectracks[secIndx].SetDirZ(posDirZ);
  sectracks[secIndx].SetKinE(ekinPositron);
  sectracks[secIndx].SetGVcode(fPositronInternalCode);
  sectracks[secIndx].SetMass(geant::units::kElectronMassC2);
  sectracks[secIndx].SetTrackIndex(track.GetTrackIndex()); // parent Track index

  return numSecondaries;
}





double RelativisticPairModel::SampleTotalEnergyTransfer(const double egamma, const int matindx, const double r1,
                                                         const double r2, const double r3) {
  // determine electron energy lower grid point
  const double legamma = std::log(egamma);
  //
  int indxEgamma = fSTNumPhotonEnergies-1;
  if (egamma<GetHighEnergyUsageLimit()) {
    const double val       = (legamma-fSTLogMinPhotonEnergy)*fSTILDeltaPhotonEnergy;
    indxEgamma             = (int)val;  // lower electron energy bin index
    const double pIndxHigh = val-indxEgamma;
    if (r1<pIndxHigh)
      ++indxEgamma;
  }
  // sample the transformed variable
  const RatinAliasData *als = fSamplingTables[matindx]->fRatinAliasData[indxEgamma];
  //
  const int    izet     = fSamplingTables[matindx]->fILowestZ;
  const double eps0     = geant::units::kElectronMassC2/egamma;
  const double deltaMax = gElementData[izet]->fDeltaMaxTsai;
  const double epsp     = 0.5-0.5*std::sqrt(1.-4.*eps0*gElementData[izet]->fDeltaFactor/deltaMax);
  const double epsMin   = std::max(eps0,epsp);
  // we could put a static assert here to make sure that als is not nullptr
  //
  // sample the transformed variable xi=... (such that linear aprx will be used in the first interval)
  const double  xi = fAliasSampler->SampleRatin(&(als->fXdata[0]), &(als->fCumulative[0]), &(als->fParaA[0]),
                                                &(als->fParaB[0]), &(als->fAliasW[0]), &(als->fAliasIndx[0]),
                                                fSTNumDiscreteEnergyTransferVals, r2, r3, 0);
  // transform back xi to eps = E_total_energy_transfer/E_{\gamma}
  return epsMin*std::exp(xi*std::log(0.5/epsMin));
}


// Sample totel energy (fraction) transfered to one of the e-/e+ pair with REJECTION: i.e. first the target atom
// needs to be sampled and there is an option if Tsai's screening is used or not: if Tsai's screening is used then
// epsmin, and so on must be evaluated with Tsai's screening
double RelativisticPairModel::SampleTotalEnergyTransfer(const double egamma, const double lpmenergy, const int izet,
                                                        const geant::TaskData *td) {
    const bool  isLPM     = (fIsUseLPM && egamma>fLPMEnergyLimit);
    const double eps0     = geant::units::kElectronMassC2/egamma;
    const double deltaFac = gElementData[izet]->fDeltaFactor;
    const double deltaMin = 4.*eps0*deltaFac;
    double deltaMax = gElementData[izet]->fDeltaMax;
    if (fIsUseTsaisScreening) {
      deltaMax = gElementData[izet]->fDeltaMaxTsai;
    }
    const double epsp     = 0.5-0.5*std::sqrt(1.-deltaMin/deltaMax);
    const double epsMin   = std::max(eps0,epsp);
    const double epsRange = 0.5-epsMin;
    const double fz       = gElementData[izet]->fFz;
    //
    double F10,F20;
    ScreenFunction12(F10,F20,deltaMin,fIsUseTsaisScreening);
    F10 -= fz;
    F20 -= fz;
    const double NormF1   = std::max(F10*epsRange*epsRange,0.);
    const double NormF2   = std::max(1.5*F20,0.);
    const double NormCond = NormF1/(NormF1+NormF2);
    //
    double *rndArray = td->fDblArray;
    double greject   = 0.0;
    double eps       = 0.0;
    do {
      td->fRndm->uniform_array(3, rndArray);
      if (NormCond>rndArray[0]) {
      	eps = 0.5 - epsRange*std::pow(rndArray[1],1./3.);
	      const double delta = deltaFac*eps0/(eps*(1.-eps));
        if (isLPM) {
          double lpmPhiS, lpmGS, lpmXiS, phi1, phi2;
          ComputeScreeningFunctions(phi1, phi2, delta, fIsUseTsaisScreening);
          ComputeLPMfunctions(lpmXiS, lpmGS, lpmPhiS, lpmenergy, eps, egamma, izet);
          greject = lpmXiS*((lpmGS+2.*lpmPhiS)*phi1 - lpmGS*phi2 - lpmPhiS*fz)/F10;
        } else {
	        greject = (ScreenFunction1(delta,fIsUseTsaisScreening) - fz)/F10;
        }
      } else {
	      eps = epsMin + epsRange*rndArray[1];
        const double delta = deltaFac*eps0/(eps*(1.-eps));
        if (isLPM) {
          double lpmPhiS, lpmGS, lpmXiS, phi1, phi2;
          ComputeScreeningFunctions(phi1, phi2, delta, fIsUseTsaisScreening);
          ComputeLPMfunctions(lpmXiS, lpmGS, lpmPhiS, lpmenergy, eps, egamma, izet);
          greject = lpmXiS*((0.5*lpmGS+lpmPhiS)*phi1 + 0.5*lpmGS*phi2 - 0.5*(lpmGS+lpmPhiS)*fz)/F20;
        } else {
	        greject = (ScreenFunction2(delta,fIsUseTsaisScreening) - fz)/F20;
        }
      }
    } while (greject<rndArray[2]);
  return eps;
}


double RelativisticPairModel::ComputeAtomicCrossSection(const Element *elem, const Material *mat, const double egamma) {
  const double xsecFactor  = geant::units::kFineStructConst*geant::units::kClassicElectronRadius*geant::units::kClassicElectronRadius;
  const double eps0        = geant::units::kElectronMassC2/egamma;
  const double zet         = elem->GetZ();
  const int   izet         = std::lrint(zet);
  const bool  isLPM        = (fIsUseLPM && egamma>fLPMEnergyLimit);
  double deltaMax = gElementData[izet]->fDeltaMax;
  if (fIsUseTsaisScreening) {
    deltaMax = gElementData[izet]->fDeltaMaxTsai;
  }
  const double epsp   = 0.5-0.5*std::sqrt(1.-4.*gElementData[izet]->fDeltaFactor*eps0/deltaMax);
  const double epsMin = std::max(eps0,epsp);
  const double eta    = gElementData[izet]->fEtaValue;
  //
//  int ngl = 64;
//  GLIntegral *gl = new GLIntegral(ngl,0.0,1.0);
  const std::vector<double> &glW = fGL->GetWeights();
  const std::vector<double> &glX = fGL->GetAbscissas();
  double xsec = 0.0;
  for (int i=0; i<fNGL; ++i) {
    const double xi  = glX[i];
    if (isLPM) {
      const double lpmEnergy = mat->GetMaterialProperties()->GetRadiationLength()*gLPMFactor;
      xsec += glW[i]*ComputeLPMDXsectionPerAtom(epsMin, egamma, xi, lpmEnergy, izet, fIsUseTsaisScreening); // istsai = false by def.
    } else {
      xsec += glW[i]*ComputeDXsectionPerAtom(epsMin, egamma, xi, izet, fIsUseTsaisScreening); // istsai = false by def.
    }
  }
//  delete gl;
  xsec = std::max(0.0,2.*xsecFactor*zet*(zet+eta)*xsec);
  return xsec;
}


// transformd no - LPM
double RelativisticPairModel::ComputeDXsectionPerAtom(const double epsmin, const double egamma, const double xi,
                                                      const int izet, bool istsai) {
  const double twoThird       = 0.666666666666;
  const double lHalfPerEpsMin = std::log(0.5/epsmin);
  const double eps            = epsmin*std::exp(xi*lHalfPerEpsMin);
  const double meps           = 1.-eps;
  const double halfFz         = 0.5*gElementData[izet]->fFz;
  const double delta          = gElementData[izet]->fDeltaFactor*geant::units::kElectronMassC2/(egamma*eps*meps);
  double phi1, phi2;
  ComputeScreeningFunctions(phi1,phi2,delta,istsai);
  double dxsec = (eps*eps+meps*meps)*(phi1-halfFz) + twoThird*eps*meps*(phi2-halfFz);
  dxsec *= (eps*lHalfPerEpsMin);
  return std::max(dxsec,0.0);
}

// transformd with - LPM   // double df, double fz, double z23
double RelativisticPairModel::ComputeLPMDXsectionPerAtom(const double epsmin, const double egamma, const double xi,
                                                         const double lpmenergy, const int izet,  bool istsai) {
  const double twoThird       = 0.666666666666;
  const double lHalfPerEpsMin = std::log(0.5/epsmin);
  const double eps            = epsmin*std::exp(xi*lHalfPerEpsMin);
  const double meps           = 1.-eps;
  const double halfFz         = 0.5*gElementData[izet]->fFz;
  const double delta          = gElementData[izet]->fDeltaFactor*geant::units::kElectronMassC2/(egamma*eps*meps);
  double phi1, phi2, lpmPhiS, lpmGS, lpmXiS;
  ComputeScreeningFunctions(phi1,phi2,delta,istsai);
  ComputeLPMfunctions(lpmXiS, lpmGS, lpmPhiS, lpmenergy, eps, egamma, izet);
  double dxsec = ((lpmGS + 2.*lpmPhiS)/3.)*(eps*eps+meps*meps)*(phi1-halfFz) + twoThird*lpmGS*eps*meps*(phi2-halfFz);
  dxsec *= (eps*lHalfPerEpsMin*lpmXiS);
  return std::max(dxsec,0.0);
}


double RelativisticPairModel::ComputeDXsection(const Material *mat, const double egamma, const double epsmin,
                                               const double xi, bool istsai) {
  const bool  isLPM = (fIsUseLPM && egamma>fLPMEnergyLimit);
  const Vector_t<Element*> &theElements   = mat->GetElementVector();
  const double* theAtomicNumDensityVector = mat->GetMaterialProperties()->GetNumOfAtomsPerVolumeVect();
  size_t numElems = theElements.size();
  double dxsec    = 0.0;
  for (size_t iel=0; iel<numElems; ++iel) {
    double xsec       = 0.0;
    const double zet  = theElements[iel]->GetZ();
    const int izet    = std::min(std::lrint(zet),gMaxZet-1);
    if (isLPM) {
      const double lpmEnergy  = mat->GetMaterialProperties()->GetRadiationLength()*gLPMFactor;
      xsec = ComputeLPMDXsectionPerAtom(epsmin, egamma, xi, lpmEnergy, izet, istsai);
    } else {
      xsec = ComputeDXsectionPerAtom(epsmin, egamma, xi, izet, istsai);
    }
    xsec  *= zet*(zet+gElementData[izet]->fEtaValue);
    dxsec += theAtomicNumDensityVector[iel]*xsec;
  }
  return std::max(dxsec,0.);
}


void RelativisticPairModel::InitialiseElementData() {
  size_t numMatCuts = MaterialCuts::GetTheMaterialCutsTable().size();
  // get list of active region
  std::vector<bool> &isActiveInRegion = GetListActiveRegions();
  for (size_t imc=0; imc<numMatCuts; ++imc) {
    const MaterialCuts *matCut = MaterialCuts::GetTheMaterialCutsTable()[imc];
    // if this MaterialCuts belongs to a region where this model is active:
    if (isActiveInRegion[matCut->GetRegionIndex()]) {
      // get the list of elements
      const Vector_t<Element*> &theElemVect = matCut->GetMaterial()->GetElementVector();
      size_t numElems = theElemVect.size();
      for (size_t ie=0; ie<numElems; ++ie) {
        const Element *elem = theElemVect[ie];
        double zet = elem->GetZ();
        int   izet = std::min(std::lrint(zet),gMaxZet-1);
        if (!gElementData[izet]) {
          ElementData *elemData   = new ElementData();
          elemData->fCoulombCor   = elem->GetElementProperties()->GetCoulombCorrection();
          elemData->fDeltaFactor  = 136./elem->GetElementProperties()->GetZ13(); // 136/pow(z,1/3)
          elemData->fFz           = 8.*elem->GetElementProperties()->GetLogZ13()+8*elemData->fCoulombCor;
          elemData->fDeltaMax     = std::exp((42.24-elemData->fFz)/8.368)-0.952;
          elemData->fDeltaMaxTsai = 1.36*std::sqrt( std::exp(0.5*16.863-0.25*elemData->fFz)-1. )/0.55846;
          double Fel   = 0.;
          double Finel = 0.;
          if (izet<5) {
            Fel   = gFelLowZet[izet];
            Finel = gFinelLowZet[izet];
          } else {
            Fel   = std::log(184.15)  -    elem->GetElementProperties()->GetLogZ()/3.;
            Finel = std::log(1194.)   - 2.*elem->GetElementProperties()->GetLogZ()/3.;
          }
          elemData->fEtaValue    = Finel/(Fel-elemData->fCoulombCor);
          //
          elemData->fVarS1Cond   = std::sqrt(2.)*elem->GetElementProperties()->GetZ23()/(184.15*184.15);
          elemData->fILVarS1Cond = 1./std::log(elemData->fVarS1Cond);
          gElementData[izet]     = elemData;
        }
      }
    }
  }
}



void RelativisticPairModel::ComputeScreeningFunctions(double &phi1, double &phi2, const double delta, const bool istsai) {
  if (istsai) {
    const double gamma   = delta*0.735294; // 0.735 = 1/1.36 <= gamma = delta/1.36
    const double gamma2  = gamma*gamma;
    phi1   = 16.863-2.0*std::log(1.0+0.311877*gamma2)+2.4*std::exp(-0.9*gamma)+1.6*std::exp(-1.5*gamma);
    phi2   = phi1-2.0/(3.0+19.5*gamma+18.0*gamma2);  // phi1-phi2
  } else {
    if (delta>1.) {
      phi1 = 21.12 - 4.184*std::log(delta+0.952);
      phi2 = phi1;
    } else {
      const double delta2 = delta*delta;
      phi1 = 20.867 - 3.242*delta + 0.625*delta2;
      phi2 = 20.209 - 1.93 *delta - 0.086*delta2;
    }
  }
}

// val1 =  3xPhi_1 - Phi_2: used in case of rejection (either istsai or not)
// val2 =  1.5*Phi_1 + 0.5*Phi_2: used in case of rejection (either istsai or not)
void RelativisticPairModel::ScreenFunction12(double &val1, double &val2, const double delta, const bool istsai) {
  if (istsai) {
    const double gamma  = delta*0.735294; // 0.735 = 1/1.36 <= gamma = delta/1.36
    const double gamma2 = gamma*gamma;
    const double dum1   = 33.726 - 4.*std::log(1.0+0.311877*gamma2) + 4.8*std::exp(-0.9*gamma) + 3.2*std::exp(-1.5*gamma);
    const double dum2   = 2./(3.+19.5*gamma+18.*gamma2);
    val1 = dum1 +     dum2;
    val2 = dum1 - 0.5*dum2;
  } else {
    if (delta>1.) {
      val1 = 42.24 - 8.368*std::log(delta+0.952);
      val2 = val1;
    } else {
      val1 = 42.392 - delta*(7.796 - 1.961*delta);
      val2 = 41.405 - delta*(5.828 - 0.8945*delta);
    }
  }
}


// 3xPhi_1 - Phi_2: used in case of rejection (either istsai or not)
double RelativisticPairModel::ScreenFunction1(const double delta, const bool istsai) {
  double val;
  if (istsai) {
    const double gamma   = delta*0.735294; // 0.735 = 1/1.36 <= gamma = delta/1.36
    const double gamma2  = gamma*gamma;
    val = 33.726 - 4.*std::log(1.0+0.311877*gamma2) + 4.8*std::exp(-0.9*gamma) + 3.2*std::exp(-1.5*gamma)
          + 2./(3.+19.5*gamma+18.*gamma2);
  } else {
    if (delta>1.) {
      val = 42.24 - 8.368*std::log(delta+0.952);
    } else {
      val = 42.392 - delta*(7.796 - 1.961*delta);
    }
  }
  return val;
}

// 1.5*Phi_1 + 0.5*Phi_2: used in case of rejection (either istsai or not)
double RelativisticPairModel::ScreenFunction2(const double delta, const bool istsai) {
  double val;
  if (istsai) {
    const double gamma   = delta*0.735294; // 0.735 = 1/1.36 <= gamma = delta/1.36
    const double gamma2  = gamma*gamma;
    val = 33.726 - 4.*std::log(1.0+0.311877*gamma2) + 4.8*std::exp(-0.9*gamma) + 3.2*std::exp(-1.5*gamma)
          - 1./(3.+19.5*gamma+18.*gamma2);
  } else {
    if (delta>1.) {
      val = 42.24 - 8.368*std::log(delta+0.952);
    } else {
      val = 41.405 - delta*(5.828 - 0.8945*delta);
    }
  }
  return val;
}


void RelativisticPairModel::ComputeLPMfunctions(double &funcXiS, double &funcGS, double &funcPhiS, const double lpmenergy,
                                                const double eps, const double egamma, const int izet) {
  //  1. y = E_+/E_{\gamma} with E_+ being the total energy transfered to one of the e-/e+ pair
  //     s'  = \sqrt{ \frac{1}{8} \frac{1}{y(1-y)}   \frac{E^{KL}_{LPM}}{E_{\gamma}}  }
  const double varSprime = std::sqrt(0.125*lpmenergy/(eps*egamma*(1.0-eps)));
  const double condition = gElementData[izet]->fVarS1Cond;
  funcXiS = 2.0;
  if (varSprime>1.0) {
    funcXiS = 1.0;
  } else if (varSprime>condition) {
    const double funcHSprime = std::log(varSprime)*gElementData[izet]->fILVarS1Cond;
    funcXiS = 1.0+funcHSprime-0.08*(1.0-funcHSprime)*funcHSprime*(2.0-funcHSprime)*gElementData[izet]->fILVarS1Cond;
  }
  //  2. s=\frac{s'}{\sqrt{\xi(s')}}
  const double varShat   = varSprime/std::sqrt(funcXiS);
  GetLPMFunctions(funcGS, funcPhiS, varShat);
//  ComputeLPMGsPhis(funcGS, funcPhiS, varShat);
  //
  //MAKE SURE SUPPRESSION IS SMALLER THAN 1: due to Migdal's approximation on xi
  if (funcXiS*funcPhiS>1. || varShat>0.57) {
    funcXiS=1./funcPhiS;
  }
}


void RelativisticPairModel::ComputeLPMGsPhis(double &funcGS, double &funcPhiS, const double varShat) {
  if (varShat<0.01) {
    funcPhiS = 6.0*varShat*(1.0-geant::units::kPi*varShat);
    funcGS   = 12.0*varShat-2.0*funcPhiS;
  } else {
    double varShat2 = varShat*varShat;
    double varShat3 = varShat*varShat2;
    double varShat4 = varShat2*varShat2;
    if (varShat<0.415827397755) { // use Stanev approximation: for \psi(s) and compute G(s)
      funcPhiS = 1.0-std::exp(-6.0*varShat*(1.0+varShat*(3.0-geant::units::kPi)) + varShat3/(0.623+0.796*varShat+0.658*varShat2));
      // 1-\exp \left\{  -4s  - \frac{8s^2}{1+3.936s+4.97s^2-0.05s^3+7.5s^4} \right\}
      const double funcPsiS = 1.0-std::exp(-4.0*varShat - 8.0*varShat2/(1.0+3.936*varShat+4.97*varShat2-0.05*varShat3+7.5*varShat4));
      // G(s) = 3 \psi(s) - 2 \phi(s)
      funcGS = 3.0*funcPsiS - 2.0*funcPhiS;
    } else if (varShat<1.55) {
      funcPhiS = 1.0-std::exp(-6.0*varShat*(1.0+varShat*(3.0-geant::units::kPi)) + varShat3/(0.623+0.796*varShat+0.658*varShat2));
      const double dum0 = -0.16072300849123999+3.7550300067531581*varShat-1.7981383069010097*varShat2+0.67282686077812381*varShat3-0.1207722909879257*varShat4;
      funcGS = std::tanh(dum0);
    } else {
      funcPhiS = 1.0-0.01190476/varShat4;
      if (varShat<1.9156) {
        const double dum0 = -0.16072300849123999+3.7550300067531581*varShat-1.7981383069010097*varShat2+0.67282686077812381*varShat3-0.1207722909879257*varShat4;
        funcGS = std::tanh(dum0);
      } else {
        funcGS   = 1.0-0.0230655/varShat4;
      }
    }
  }
}


void RelativisticPairModel::InitLPMFunctions() {
  if (!gLPMFuncs.fIsInitialized) {
//    gLPMFuncs.fSLimit = 2.;
//    gLPMFuncs.fSDelta = 0.01;
    const int num = gLPMFuncs.fSLimit/gLPMFuncs.fSDelta+1;
    gLPMFuncs.fLPMFuncG.resize(num);
    gLPMFuncs.fLPMFuncPhi.resize(num);
    for (int i=0; i<num; ++i) {
      const double s=i*gLPMFuncs.fSDelta;
      ComputeLPMGsPhis(gLPMFuncs.fLPMFuncG[i],gLPMFuncs.fLPMFuncPhi[i],s);
    }
    gLPMFuncs.fIsInitialized = true;
  }
}


void RelativisticPairModel::GetLPMFunctions(double &lpmGs, double &lpmPhis, const double s) {
  if (s<gLPMFuncs.fSLimit) {
    double     val = s/gLPMFuncs.fSDelta;
    const int ilow = int(val);
    val    -= ilow;
    lpmGs   = (gLPMFuncs.fLPMFuncG[ilow+1]-gLPMFuncs.fLPMFuncG[ilow])*val + gLPMFuncs.fLPMFuncG[ilow];
    lpmPhis = (gLPMFuncs.fLPMFuncPhi[ilow+1]-gLPMFuncs.fLPMFuncPhi[ilow])*val + gLPMFuncs.fLPMFuncPhi[ilow];
  } else {
    double ss = s*s;
    ss *= ss;
    lpmPhis = 1.0-0.01190476/ss;
    lpmGs   = 1.0-0.0230655/ss;
  }
}


void RelativisticPairModel::ClearSamplingTables() {
  size_t num = fSamplingTables.size();
  for (size_t im=0; im<num; ++im) {
    RatinAliasDataPerMaterial *matData = fSamplingTables[im];
    if (matData) {
      size_t nTables = matData->fRatinAliasData.size();
      for (size_t it=0; it<nTables; ++it) {
        RatinAliasData *tb = matData->fRatinAliasData[it];
        tb->fXdata.clear();
        tb->fCumulative.clear();
        tb->fParaA.clear();
        tb->fParaB.clear();
        tb->fAliasW.clear();
        tb->fAliasIndx.clear();
        delete tb;
      }
      matData->fRatinAliasData.clear();
      delete matData;
    }
  }
  fSamplingTables.clear();
}


void RelativisticPairModel::InitSamplingTables() {
  // 1. clear sampling tables if any
  ClearSamplingTables();
  // 2. generate primary gamma energy grid
  const double minEprim  = GetLowEnergyUsageLimit();
  const double maxEprim  = GetHighEnergyUsageLimit();
  fSTNumPhotonEnergies   = fSTNumPhotonEnergiesPerDecade*std::lrint(std::log10(maxEprim/minEprim))+1;
  fSTNumPhotonEnergies   = std::max(fSTNumPhotonEnergies,3);
  // set up the initial gamma energy grid
  const double delta     = std::log(maxEprim/minEprim)/(fSTNumPhotonEnergies-1.0);
  fSTLogMinPhotonEnergy  = std::log(minEprim);
  fSTILDeltaPhotonEnergy = 1./delta;
  std::vector<double> primEVect(fSTNumPhotonEnergies);
  primEVect[0]                      = minEprim;
  primEVect[fSTNumPhotonEnergies-1] = maxEprim;
  for (int i=1; i<fSTNumPhotonEnergies-1; ++i) {
    primEVect[i] = std::exp(fSTLogMinPhotonEnergy+i*delta);
  }
  // 3. create an AliasTable object
  if (fAliasSampler) {
    delete fAliasSampler;
  }
  fAliasSampler = new AliasTable();
  // 4. set up the container that stores sampling tables for all the materials (init to nullptr-s)
  fSamplingTables.resize(Material::GetTheMaterialTable().size(),nullptr);
  // 5. build the sampling tables:
  // get all MaterialCuts; loop over them and if they belongs to a region where this model is active:
  //  - get the material that the MaterialCuts belongs to
  //  - build RatinAliasDataPerMaterial for this material if it has not been done yet
  int numMatCuts = MaterialCuts::GetTheMaterialCutsTable().size();
  // get list of active region
  std::vector<bool> &isActiveInRegion = GetListActiveRegions();
  for (int i=0; i<numMatCuts; ++i) {
    const MaterialCuts *matCut = MaterialCuts::GetTheMaterialCutsTable()[i];
    // if this MaterialCuts belongs to a region where this model is active:
    if (isActiveInRegion[matCut->GetRegionIndex()]) {
      // get the material
      const Material *theMaterial =  matCut->GetMaterial();
      // if RatinAliasDataPerMaterial has not been built for this material yet => build
      if (!fSamplingTables[theMaterial->GetIndex()]) {
        BuildSamplingTablesForMaterial(theMaterial, primEVect);
      }
    }
  }
  primEVect.clear();
}


void RelativisticPairModel::BuildSamplingTablesForMaterial(const Material *mat, const std::vector<double> &primevect) {
  // prepare one RatinAliasDataPerMaterial structure
  const int numEprim = primevect.size();
  RatinAliasDataPerMaterial *perMat = new RatinAliasDataPerMaterial(numEprim);
  fSamplingTables[mat->GetIndex()]  = perMat;
  // set lowest Z
  const Vector_t<Element*> &theElements = mat->GetElementVector();
  const size_t numElems = theElements.size();
  double lowZ = 200.;
  for (size_t iel=0; iel<numElems; ++iel) {
    const double zet = theElements[iel]->GetZ();
    if (zet<lowZ) {
      lowZ = zet;
    }
  }
  perMat->fILowestZ = std::min(std::lrint(lowZ),gMaxZet-1);
  // allocate an array that will be used temporary:
  double *thePdfdata = new double[fSTNumDiscreteEnergyTransferVals]();
  // loop over the photon energy grid and build one RatinAliasData structure at each photon energy for this material
  for (int i=0; i<numEprim; ++i) {
    // note that egamma is for sure > 50 [MeV]
    const double egamma = primevect[i];
    // build one table for the given: egamma, material
    BuildOneRatinAlias(egamma, mat, thePdfdata, i, perMat->fILowestZ);
  }
  // delete the temporary array
  delete [] thePdfdata;
}


void RelativisticPairModel::BuildOneRatinAlias(const double egamma, const Material *mat, double *pdfarray,
                                                const int egammaindx, const int ilowestz) {
  // compute the theoretical minimum of the reduced total energy transfer to the e+ (or to the e-)
  const double eps0    = geant::units::kElectronMassC2/egamma;
  // deterime the lowest eps': that will belong to the lowest Z of the material
  const double dMax    = gElementData[ilowestz]->fDeltaMaxTsai;  // Tsai's screening apr. will be used for the tables
  const double dFactor = gElementData[ilowestz]->fDeltaFactor;
  const double dMin    = 4.*eps0*dFactor;  // 4*136*Z^(-1/3)*eps0
  const double eps1    = 0.5-0.5*std::sqrt(1.-dMin/dMax);
  // take the higest value between eps0 and eps'=eps1: eps will be in [epsMin,0.5]
  const double epsMin = std::max(eps1,eps0);
  const double invLHalfPerEpsMin = 1./std::log(0.5/epsMin);
  //
  // allocate one RatinAliasData structure for the current element,gamma-energy combination
  RatinAliasData *raData = new RatinAliasData(fSTNumDiscreteEnergyTransferVals);
  // insert possible points: at xi(epsMin)=0, xi(epsMax=0.5)=1, at the midle xi=0.5
  // and at xi(eps'(Z_i)) points if these eps'(Z_i)> epsMin
  int curNumData    = 1;
  raData->fXdata[0] = 0.0;
  const Vector_t<Element*> &theElements = mat->GetElementVector();
  const int numElems = theElements.size();
  for (int iel=0; iel<numElems; ++iel) {
    const int izet           = std::min(std::lrint(theElements[iel]->GetZ()),gMaxZet-1);
    const double deltaMax    = gElementData[izet]->fDeltaMaxTsai;  // Tsai's screening apr. will be used for the tables
    const double deltaFactor = gElementData[izet]->fDeltaFactor;
    const double deltaMin    = 4.*eps0*deltaFactor;  // 4*136*Z^(-1/3)*eps0
    const double eps1        = 0.5-0.5*std::sqrt(1.-deltaMin/deltaMax);
    if (eps1>epsMin) {
      raData->fXdata[curNumData] = std::log(eps1/epsMin)*invLHalfPerEpsMin;
      ++curNumData;
    }
  }
  // reorder
  for (int i=1; i<curNumData; ++i) {
    for (int j=i; j<curNumData; ++j) {
      if (raData->fXdata[j]<raData->fXdata[i]) {
        double a = raData->fXdata[i];
        raData->fXdata[i] = raData->fXdata[j];
        raData->fXdata[j] = a;
      }
    }
  }
  raData->fXdata[curNumData] = 0.5;
  raData->fXdata[curNumData+1] = 1.0;
  curNumData += 2;
  //
  //
  // create a 4 point GL integral to compute the interpolation error at each interval
  const int glnum = 4;
  //
  // fill in the discrete pdf array by inserting fSTNumDiscreteEnergyTransferVals discrete points such that the
  // interpolation error is minimized: one new discrete point will be inserted at the end of each
  // iteration of this while loop:
  while (curNumData<fSTNumDiscreteEnergyTransferVals) {
    // compute the pdf values at the current discrete sample points
    for (int i=0; i<curNumData; ++i) {
      pdfarray[i] = ComputeDXsection(mat, egamma, epsMin, raData->fXdata[i], true);
    }
    double maxerr      = 0.0;
    double thexval     = 0.0;
    int    maxErrIndex = 0;
    // prepare data for the approximation of the pdf based on the current discretization
    const double norm = fAliasSampler->PreparRatinForPDF(&(raData->fXdata[0]), pdfarray, &(raData->fCumulative[0]),
                                                         &(raData->fParaA[0]), &(raData->fParaB[0]), curNumData, true,
                                                         glnum);
    //
    // find the interval with the highest approximation error to the real pdf based on the current discretization
    for (int i=0; i<curNumData-1; ++i) {
      const double xx  = 0.5*(raData->fXdata[i]+raData->fXdata[i+1]);  // mid xi-point of the current(i.e. i-th) interval
      double err = 0.0;
      // we shuld compute the integrated error (by transforming the integral from [xi[i],xi[i+1] to [0,1])
      // but we will use sample points to estimate the error: divide the interval into isub sub intervals and compute
      // some error measure at each edge points
      const int isub   = 2;
      const double dd  = (raData->fXdata[i+1]-raData->fXdata[i])/((double)isub);
      for (int j=1; j<isub; ++j) {
        const double xval = raData->fXdata[i]+j*dd;
        double valAprx = 0.0;
        // - get the approximated pdf value based on the current discretization
        // - unless if the current interval is the first because we use linear appoximation in the first
        //   interval <= the pdf(xi[0])=0
        if (i>0) {
          valAprx = fAliasSampler->GetRatinForPDF1(xval, &(raData->fXdata[0]), &(raData->fCumulative[0]),
                                                   &(raData->fParaA[0]), &(raData->fParaB[0]), i);
        } else { //linear aprx in the first interval because pdf[0] = 0
          valAprx = norm*pdfarray[i+1]/(raData->fXdata[i+1]-raData->fXdata[i])*(xval-raData->fXdata[i]);
        }
        // get the real value
        const double valReal = norm*ComputeDXsection(mat, egamma, epsMin, xval, true);
        double curerr  = std::fabs((1.-valAprx/valReal));//*(valAprx-valReal));
        if (fSTNumDiscreteEnergyTransferVals-curNumData<5) {
          curerr  = std::fabs((1.-valAprx/valReal)*(valAprx-valReal));
        }
        err += curerr;
        //err += std::fabs((1.-valAprx/valReal));//*(valAprx-valReal));
      }
      err *=(raData->fXdata[i+1]-raData->fXdata[i]);
      // if the current interval gives the highest approximation error so far then store some values :
      // i.e. current maximum error value, the mid-point of the current interval and the lower edge index
      if (err>maxerr) {
        maxerr      = err;
        thexval     = xx;
        maxErrIndex = i;
      }
    } // end of the for loop i.e. the approximation error was checked in each of the currently used intervals
    //
    // after checking the approximation error in each interval we want to insert a new point into the midle of that
    // interval that gave the maximum approximation error:
    // 1. shift to the right all points above the maxErrIndex
    for (int i=curNumData; i>maxErrIndex+1; --i) {
      raData->fXdata[i] = raData->fXdata[i-1];
    }
    // 2. then insert the new discrete xi point
    raData->fXdata[maxErrIndex+1] = thexval;
    // 3. increase the number of currently used discrete sample points
    ++curNumData;
  }
  //
  // if all the available discrete xi points was filled, compute the corresponding values of the pdf at these discrete
  // points
  for (int i=0; i<curNumData; ++i) {
    pdfarray[i] = ComputeDXsection(mat, egamma, epsMin, raData->fXdata[i], true);
  }
  //
  // and prepare the final ratin-alias sampling table structure:
  fAliasSampler->PreparRatinTable(&(raData->fXdata[0]), pdfarray, &(raData->fCumulative[0]), &(raData->fParaA[0]),
                                  &(raData->fParaB[0]), &(raData->fAliasW[0]), &(raData->fAliasIndx[0]),
                                  fSTNumDiscreteEnergyTransferVals, true, glnum);
  // fill in
  const int matindx = mat->GetIndex();
  fSamplingTables[matindx]->fRatinAliasData[egammaindx]=raData;
}


}    // namespace geantphysics
