
#include "Geant/GSMSCTable.h"

#include "Geant/SystemOfUnits.h"
#include "Geant/PhysicalConstants.h"

// from material
#include "Geant/Types.h"

#include "Geant/Material.h"
#include "Geant/MaterialProperties.h"
#include "Geant/Element.h"
#include "Geant/MaterialCuts.h"

#include "Geant/GSMottCorrection.h"

// from geantV
#include "Geant/TaskData.h"

#include <fstream>
#include "Geant/math_wrappers.h"
//#include <cstdlib>
#include <cmath>
#include <iostream>

namespace geantphysics {

// perecomputed GS angular distributions, based on the Screened-Rutherford DCS
// are the same for e- and e+ so make sure we load them only onece
bool GSMSCTable::gIsInitialised = false;
//
std::vector<GSMSCTable::GSMSCAngularDtr *> GSMSCTable::gGSMSCAngularDistributions1;
std::vector<GSMSCTable::GSMSCAngularDtr *> GSMSCTable::gGSMSCAngularDistributions2;
//
std::vector<double> GSMSCTable::gMoliereBc;
std::vector<double> GSMSCTable::gMoliereXc2;

GSMSCTable::GSMSCTable(bool iselectron) : fIsElectron(iselectron)
{
  // set initial values: final values will be set in the Initialize method
  fLogLambda0        = 0.;
  fLogDeltaLambda    = 0.;
  fInvLogDeltaLambda = 0.;
  fInvDeltaQ1        = 0.;
  fDeltaQ2           = 0.;
  fInvDeltaQ2        = 0.;
  //
  fLowEnergyLimit  = 0.1 * geant::units::keV;   // will be set properly at init.
  fHighEnergyLimit = 100.0 * geant::units::MeV; // will be set properly at init.
  //
  fIsMottCorrection = false; // will be set properly at init.
  fIsPWACorrection  = false; // will be set properly at init.
  fMottCorrection   = nullptr;
  //
  fNumSPCEbinPerDec = 3;
}

GSMSCTable::~GSMSCTable()
{
  for (size_t i = 0; i < gGSMSCAngularDistributions1.size(); ++i) {
    if (gGSMSCAngularDistributions1[i]) {
      delete[] gGSMSCAngularDistributions1[i]->fUValues;
      delete[] gGSMSCAngularDistributions1[i]->fParamA;
      delete[] gGSMSCAngularDistributions1[i]->fParamB;
      delete gGSMSCAngularDistributions1[i];
    }
  }
  gGSMSCAngularDistributions1.clear();
  for (size_t i = 0; i < gGSMSCAngularDistributions2.size(); ++i) {
    if (gGSMSCAngularDistributions2[i]) {
      delete[] gGSMSCAngularDistributions2[i]->fUValues;
      delete[] gGSMSCAngularDistributions2[i]->fParamA;
      delete[] gGSMSCAngularDistributions2[i]->fParamB;
      delete gGSMSCAngularDistributions2[i];
    }
  }
  gGSMSCAngularDistributions2.clear();
  //
  if (fMottCorrection) {
    delete fMottCorrection;
    fMottCorrection = nullptr;
  }
  // clear scp correction data
  for (size_t imc = 0; imc < fSCPCPerMatCuts.size(); ++imc) {
    if (fSCPCPerMatCuts[imc]) {
      fSCPCPerMatCuts[imc]->fVSCPC.clear();
      delete fSCPCPerMatCuts[imc];
    }
  }
  fSCPCPerMatCuts.clear();
  fActiveRegionsVector.clear();
  //
  gIsInitialised = false;
}

void GSMSCTable::Initialize(double lownergylimit, double highenergylimit, const std::vector<bool> &activeregionv)
{
  fLowEnergyLimit    = lownergylimit;
  fHighEnergyLimit   = highenergylimit;
  double lLambdaMin  = Math::Log(gLAMBMIN);
  double lLambdaMax  = Math::Log(gLAMBMAX);
  fLogLambda0        = lLambdaMin;
  fLogDeltaLambda    = (lLambdaMax - lLambdaMin) / (gLAMBNUM - 1.);
  fInvLogDeltaLambda = 1. / fLogDeltaLambda;
  fInvDeltaQ1        = 1. / ((gQMAX1 - gQMIN1) / (gQNUM1 - 1.));
  fDeltaQ2           = (gQMAX2 - gQMIN2) / (gQNUM2 - 1.);
  fInvDeltaQ2        = 1. / fDeltaQ2;
  //
  fActiveRegionsVector = activeregionv;
  // load precomputed angular distributions and set up several values used during the sampling
  // these are particle independet => they go to static container: load them only onece
  if (!gIsInitialised) {
    // load pre-computed GS angular distributions (computed based on Screened-Rutherford DCS)
    LoadMSCData();
    gIsInitialised = true;
  }
  InitMoliereMSCParams();
  // Mott-correction: particle(e- or e+) dependet so init them
  if (fIsMottCorrection) {
    if (!fMottCorrection) {
      fMottCorrection = new GSMottCorrection(fIsElectron);
    }
    fMottCorrection->Initialise(activeregionv);
  }
  // init scattering power correction data; used only together with Mott-correction
  // (Moliere's parameters must be initialised before)
  if (fMottCorrection) {
    InitSCPCorrection(activeregionv);
  }
}

// samplig multiple scattering angles cos(theta) and sin(thata)
//  - including no-scattering, single, "few" scattering cases as well
//  - Mott-correction will be included if it was requested by the user (i.e. if fIsMottCorrection=true)
// lambdaval : s/lambda_el
// qval      : s/lambda_el G1
// scra      : screening parameter
// cost      : will be the smapled cos(theta)
// sint      : will be the smapled sin(theta)
// lekin     : logarithm of the current kinetic energy
// beta2     : the corresponding beta square
// matindx   : index of the current material
// returns true if it was msc
bool GSMSCTable::Sampling(double lambdaval, double qval, double scra, double &cost, double &sint, double lekin,
                          double beta2, int matindx, GSMSCAngularDtr **gsDtr, int &mcekini, int &mcdelti,
                          double &transfPar, geant::TaskData *td, bool isfirst)
{
  double rand0 = td->fRndm->uniform();
  double expn  = Math::Exp(-lambdaval);
  //
  // no scattering case
  if (rand0 < expn) {
    cost = 1.0;
    sint = 0.0;
    return false;
  }
  //
  // single scattering case : sample from the single scattering PDF
  // - Mott-correction will be included if it was requested by the user (i.e. if fIsMottCorrection=true)
  if (rand0 < (1. + lambdaval) * expn) {
    // cost is sampled in SingleScattering()
    cost = SingleScattering(lambdaval, scra, lekin, beta2, matindx, td);
    // add protections
    cost = std::max(cost, -1.0);
    cost = std::min(cost, 1.0);
    //    if (cost<-1.0) cost = -1.0;
    //    if (cost>1.0)  cost =  1.0;
    // compute sin(theta) from the sampled cos(theta)
    double dum0 = 1. - cost;
    sint        = std::sqrt(dum0 * (2.0 - dum0));
    return false;
  }
  //
  // handle this case:
  //      -lambdaval < 1 i.e. mean #elastic events along the step is < 1 but
  //       the currently sampled case is not 0 or 1 scattering. [Our minimal
  //       lambdaval (that we have precomputed, transformed angular distributions
  //       stored in a form of equally probabe intervalls together with rational
  //       interp. parameters) is 1.]
  //      -probability of having n elastic events follows Poisson stat. with
  //       lambdaval parameter.
  //      -the max. probability (when lambdaval=1) of having more than one
  //       elastic events is 0.2642411 and the prob of having 2,3,..,n elastic
  //       events decays rapidly with n. So set a max n to 10.
  //      -sampling of this cases is done in a one-by-one single elastic event way
  //       where the current #elastic event is sampled from the Poisson distr.
  if (lambdaval < 1.0) {
    double prob, cumprob;
    prob = cumprob = expn;
    double curcost, cursint;
    // init cos(theta) and sin(theta) to the zero scattering values
    cost = 1.0;
    sint = 0.0;
    for (int iel = 1; iel < 10; ++iel) {
      // prob of having iel scattering from Poisson
      prob *= lambdaval / (double)iel;
      cumprob += prob;
      //
      // sample cos(theta) from the singe scattering pdf:
      // - Mott-correction will be included if it was requested by the user (i.e. if fIsMottCorrection=true)
      curcost     = SingleScattering(lambdaval, scra, lekin, beta2, matindx, td);
      double dum0 = 1. - curcost;
      cursint     = dum0 * (2.0 - dum0); // sin^2(theta)
      //
      // if we got current deflection that is not too small
      // then update cos(theta) sin(theta)
      if (cursint > 1.0e-20) {
        cursint       = std::sqrt(cursint);
        double curphi = geant::units::kTwoPi * td->fRndm->uniform();
        cost          = cost * curcost - sint * cursint * Math::Cos(curphi);
        sint          = std::sqrt(std::max(0.0, (1.0 - cost) * (1.0 + cost)));
      }
      //
      // check if we have done enough scattering i.e. sampling from the Poisson
      if (rand0 < cumprob) {
        return false;
      }
    }
    // if reached the max iter i.e. 10
    return false;
  }
  //
  // multiple scattering case with lambdavalue >= 1:
  //   - use the precomputed and transformed Goudsmit-Saunderson angular
  //     distributions to sample cos(theta)
  //   - Mott-correction will be included if it was requested by the user (i.e. if fIsMottCorrection=true)
  cost = SampleCosTheta(lambdaval, qval, scra, lekin, beta2, matindx, gsDtr, mcekini, mcdelti, transfPar, td, isfirst);
  // add protections
  cost = std::max(cost, -1.0);
  cost = std::min(cost, 1.0);
  //  if (cost<-1.0)  cost = -1.0;
  //  if (cost> 1.0)  cost =  1.0;
  // compute cos(theta) and sin(theta) from the sampled 1-cos(theta)
  double dum0 = 1.0 - cost;
  sint        = std::sqrt(dum0 * (2.0 - dum0));
  // return true if it was msc
  return true;
}

double GSMSCTable::SampleCosTheta(double lambdaval, double qval, double scra, double lekin, double beta2, int matindx,
                                  GSMSCAngularDtr **gsDtr, int &mcekini, int &mcdelti, double &transfPar,
                                  geant::TaskData *td, bool isfirst)
{
  double cost = 1.;
  // determine the base GS angular distribution if it is the first call (when sub-step sampling is used)
  if (isfirst) {
    *gsDtr = GetGSAngularDtr(scra, lambdaval, qval, transfPar, td);
  }
  // sample cost from the GS angular distribution (computed based on Screened-Rutherford DCS)
  cost = SampleGSSRCosTheta(*gsDtr, transfPar, td);
  // Mott-correction if it was requested by the user
  if (fIsMottCorrection && *gsDtr) { // no Mott-correction in case of izotropic theta
    static const int nlooplim = 1000;
    int nloop                 = 0; // rejection loop counter
                                   //    int    ekindx   = -1; // evaluate only in the first call
                                   //    int    deltindx = -1 ; // evaluate only in the first call
    double val = fMottCorrection->GetMottRejectionValue(lekin, beta2, qval, cost, matindx, mcekini, mcdelti, td);
    while (td->fRndm->uniform() > val && ++nloop < nlooplim) {
      // sampling cos(theta)
      cost = SampleGSSRCosTheta(*gsDtr, transfPar, td);
      val  = fMottCorrection->GetMottRejectionValue(lekin, beta2, qval, cost, matindx, mcekini, mcdelti, td);
    };
  }
  return cost;
}

// returns with cost sampled from the GS angular distribution computed based on Screened-Rutherford DCS
double GSMSCTable::SampleGSSRCosTheta(const GSMSCAngularDtr *gsDtr, double transfpar, geant::TaskData *td)
{
  // check if isotropic theta (i.e. cost is uniform on [-1:1])
  if (!gsDtr) {
    return 1. - 2.0 * td->fRndm->uniform();
  }
  //
  // sampling form the selected distribution
  double ndatm1 = gsDtr->fNumData - 1.;
  double delta  = 1.0 / ndatm1;
  // determine lower cumulative bin inidex
  double rndm = td->fRndm->uniform();
  int indxl   = (int)(rndm * ndatm1);
  double aval = rndm - indxl * delta;
  double dum0 = delta * aval;
  //
  double dum1   = (1.0 + gsDtr->fParamA[indxl] + gsDtr->fParamB[indxl]) * dum0;
  double dum2   = delta * delta + gsDtr->fParamA[indxl] * dum0 + gsDtr->fParamB[indxl] * aval * aval;
  double sample = gsDtr->fUValues[indxl] + dum1 / dum2 * (gsDtr->fUValues[indxl + 1] - gsDtr->fUValues[indxl]);
  // transform back u to cos(theta) :
  // this is the sampled cos(theta) = (2.0*para*sample)/(1.0-sample+para)
  return 1. - (2.0 * transfpar * sample) / (1.0 - sample + transfpar);
}

// determine the GS angular distribution we need to sample from: will set other things as well ...
GSMSCTable::GSMSCAngularDtr *GSMSCTable::GetGSAngularDtr(double scra, double &lambdaval, double &qval,
                                                         double &transfpar, geant::TaskData *td)
{
  GSMSCAngularDtr *dtr = nullptr;
  bool first           = false;
  // isotropic cost above gQMAX2 (i.e. dtr stays nullptr)
  if (qval < gQMAX2) {
    int lamIndx = -1; // lambda value index
    int qIndx   = -1; // lambda value index
    // init to second grid Q values
    int numQVal    = gQNUM2;
    double minQVal = gQMIN2;
    double invDelQ = fInvDeltaQ2;
    double pIndxH  = 0.; // probability of taking higher index
    // check if first or second grid needs to be used
    if (qval < gQMIN2) { // first grid
      first = true;
      // protect against qval<gQMIN1
      if (qval < gQMIN1) {
        qval  = gQMIN1;
        qIndx = 0;
        // pIndxH = 0.;
      }
      // set to first grid Q values
      numQVal = gQNUM1;
      minQVal = gQMIN1;
      invDelQ = fInvDeltaQ1;
    }
    // make sure that lambda = s/lambda_el is in [gLAMBMIN,gLAMBMAX)
    // lambda<gLAMBMIN=1 is already handeled before so lambda>= gLAMBMIN for sure
    if (lambdaval >= gLAMBMAX) {
      lambdaval = gLAMBMAX - 1.e-8;
      lamIndx   = gLAMBNUM - 1;
    }
    double lLambda = Math::Log(lambdaval);
    //
    // determine lower lambda (=s/lambda_el) index: linear interp. on log(lambda) scale
    if (lamIndx < 0) {
      pIndxH  = (lLambda - fLogLambda0) * fInvLogDeltaLambda;
      lamIndx = (int)(pIndxH);    // lower index of the lambda bin
      pIndxH  = pIndxH - lamIndx; // probability of taking the higher index distribution
      if (td->fRndm->uniform() < pIndxH) {
        ++lamIndx;
      }
    }
    //
    // determine lower Q (=s/lambda_el G1) index: linear interp. on Q
    if (qIndx < 0) {
      pIndxH = (qval - minQVal) * invDelQ;
      qIndx  = (int)(pIndxH); // lower index of the Q bin
      pIndxH = pIndxH - qIndx;
      if (td->fRndm->uniform() < pIndxH) {
        ++qIndx;
      }
    }
    // set indx
    int indx = lamIndx * numQVal + qIndx;
    if (first) {
      dtr = gGSMSCAngularDistributions1[indx];
    } else {
      dtr = gGSMSCAngularDistributions2[indx];
    }
    // dtr might be nullptr that indicates isotropic cot distribution because:
    // - if the selected lamIndx, qIndx correspond to L(=s/lambda_el) and Q(=s/lambda_el G1) such that G1(=Q/L) > 1
    //   G1 should always be < 1 and if G1 is ~1 -> the dtr is isotropic (this can only happen in case of the 2. grid)
    //
    // compute the transformation parameter
    if (lambdaval > 10.0) {
      transfpar = 0.5 * (-2.77164 + lLambda * (2.94874 - lLambda * (0.1535754 - lLambda * 0.00552888)));
    } else {
      transfpar = 0.5 * (1.347 + lLambda * (0.209364 - lLambda * (0.45525 - lLambda * (0.50142 - lLambda * 0.081234))));
    }
    transfpar *= (lambdaval + 4.0) * scra;
  }
  // return with the selected GS angular distribution that we need to sample cost from (if nullptr => isotropic cost)
  return dtr;
}

void GSMSCTable::LoadMSCData()
{
  // get the path to the main physics data directory
  char *path = std::getenv("GEANT_PHYSICS_DATA");
  if (!path) {
    std::cerr << "******   ERROR in GSMSCTable::LoadMSCData() \n"
              << "         GEANT_PHYSICS_DATA is not defined! Set the GEANT_PHYSICS_DATA\n"
              << "         environmental variable to the location of Geant data directory!\n"
              << std::endl;
    exit(1);
  }
  //
  gGSMSCAngularDistributions1.resize(gLAMBNUM * gQNUM1, nullptr);
  for (int il = 0; il < gLAMBNUM; ++il) {
    char fname[512];
    sprintf(fname, "%s/msc_GS/GSGrid_1/gsDistr_%d", path, il);
    std::ifstream infile(fname, std::ios::in);
    if (!infile.is_open()) {
      std::string strfname(fname);
      std::cerr << "******   ERROR in GSMSCTable::LoadMSCData() \n"
                << "         Cannot open file: " << fname << " \n"
                << std::endl;
      exit(1);
    }
    for (int iq = 0; iq < gQNUM1; ++iq) {
      GSMSCAngularDtr *gsd = new GSMSCAngularDtr();
      infile >> gsd->fNumData;
      gsd->fUValues = new double[gsd->fNumData]();
      gsd->fParamA  = new double[gsd->fNumData]();
      gsd->fParamB  = new double[gsd->fNumData]();
      double ddummy;
      infile >> ddummy;
      infile >> ddummy;
      for (int i = 0; i < gsd->fNumData; ++i) {
        infile >> gsd->fUValues[i];
        infile >> gsd->fParamA[i];
        infile >> gsd->fParamB[i];
      }
      gGSMSCAngularDistributions1[il * gQNUM1 + iq] = gsd;
    }
    infile.close();
  }
  //
  // second grid
  gGSMSCAngularDistributions2.resize(gLAMBNUM * gQNUM2, nullptr);
  for (int il = 0; il < gLAMBNUM; ++il) {
    char fname[512];
    sprintf(fname, "%s/msc_GS/GSGrid_2/gsDistr_%d", path, il);
    std::ifstream infile(fname, std::ios::in);
    if (!infile.is_open()) {
      std::string strfname(fname);
      std::cerr << "******   ERROR in GSMSCTable::LoadMSCData() \n"
                << "         Cannot open file: " << fname << " \n"
                << std::endl;
      exit(1);
    }
    for (int iq = 0; iq < gQNUM2; ++iq) {
      int numData;
      infile >> numData;
      if (numData > 1) {
        GSMSCAngularDtr *gsd = new GSMSCAngularDtr();
        gsd->fNumData        = numData;
        gsd->fUValues        = new double[gsd->fNumData]();
        gsd->fParamA         = new double[gsd->fNumData]();
        gsd->fParamB         = new double[gsd->fNumData]();
        double ddummy;
        infile >> ddummy;
        infile >> ddummy;
        for (int i = 0; i < gsd->fNumData; ++i) {
          infile >> gsd->fUValues[i];
          infile >> gsd->fParamA[i];
          infile >> gsd->fParamB[i];
        }
        gGSMSCAngularDistributions2[il * gQNUM2 + iq] = gsd;
      } else {
        gGSMSCAngularDistributions2[il * gQNUM2 + iq] = nullptr;
      }
    }
    infile.close();
  }
}

// samples cost in single scattering based on Screened-Rutherford DCS
// (with Mott-correction if it was requested)
double GSMSCTable::SingleScattering(double /*lambdaval*/, double scra, double lekin, double beta2, int matindx,
                                    geant::TaskData *td)
{
  double rand1 = td->fRndm->uniform();
  // sample cost from the Screened-Rutherford DCS
  double cost = 1. - 2.0 * scra * rand1 / (1.0 - rand1 + scra);
  // Mott-correction if it was requested by the user
  if (fIsMottCorrection) {
    static const int nlooplim = 1000; // rejection loop limit
    int nloop                 = 0;    // loop counter
    int ekindx                = -1;   // evaluate only in the first call
    int deltindx              = 0;    // single scattering case
    double q1                 = 0.;   // not used when deltindx = 0;
    // computing Mott rejection function value
    double val = fMottCorrection->GetMottRejectionValue(lekin, beta2, q1, cost, matindx, ekindx, deltindx, td);
    while (td->fRndm->uniform() > val && ++nloop < nlooplim) {
      // sampling cos(theta) from the Screened-Rutherford DCS
      rand1 = td->fRndm->uniform();
      cost  = 1. - 2.0 * scra * rand1 / (1.0 - rand1 + scra);
      // computing Mott rejection function value
      val = fMottCorrection->GetMottRejectionValue(lekin, beta2, q1, cost, matindx, ekindx, deltindx, td);
    };
  }
  return cost;
}

void GSMSCTable::GetMottCorrectionFactors(double logekin, double beta2, int matindx, double &mcToScr, double &mcToQ1,
                                          double &mcToG2PerG1)
{
  if (fIsMottCorrection) {
    fMottCorrection->GetMottCorrectionFactors(logekin, beta2, matindx, mcToScr, mcToQ1, mcToG2PerG1);
  }
}

// compute material dependent Moliere MSC parameters at initialisation
void GSMSCTable::InitMoliereMSCParams()
{
  constexpr double const1   = 7821.6;         // [cm2/g]
  constexpr double const2   = 0.1569;         // [cm2 MeV2 / g]
  constexpr double finstrc2 = 5.325135453E-5; // fine-structure const. square

  const Vector_t<Material *> &theMaterialTable = Material::GetTheMaterialTable();
  // get number of materials in the table
  size_t numMaterials = theMaterialTable.size();
  // make sure that we have long enough vectors
  if (gMoliereBc.size() < numMaterials) {
    gMoliereBc.resize(numMaterials);
    gMoliereXc2.resize(numMaterials);
  }
  double xi = 1.0;
  int maxZ  = 200;
  if (fIsMottCorrection || fIsPWACorrection) {
    // xi   = 1.0;  <= always set to 1 from now on
    maxZ = GSMottCorrection::GetMaxZet();
  }
  //
  for (size_t imat = 0; imat < numMaterials; ++imat) {
    const Material *theMaterial            = theMaterialTable[imat];
    const Vector_t<Element *> &theElemVect = theMaterial->GetElementVector();
    const double *theNbAtomsPerVolVect     = theMaterial->GetMaterialProperties()->GetNumOfAtomsPerVolumeVect();
    const double theTotNbAtomsPerVol       = theMaterial->GetMaterialProperties()->GetTotalNumOfAtomsPerVol();
    int numelems                           = theElemVect.size();
    //
    double zs = 0.0;
    double zx = 0.0;
    double ze = 0.0;
    double sa = 0.0;
    //
    for (int ielem = 0; ielem < numelems; ielem++) {
      double zet = theElemVect[ielem]->GetZ();
      if (zet > maxZ) {
        zet = (double)maxZ;
      }
      double iwa = theElemVect[ielem]->GetA() * geant::units::mole / geant::units::g;
      double ipz = theNbAtomsPerVolVect[ielem] / theTotNbAtomsPerVol;
      double dum = ipz * zet * (zet + xi);
      zs += dum;
      ze += dum * (-2.0 / 3.0) * Math::Log(zet);
      zx += dum * Math::Log(1.0 + 3.34 * finstrc2 * zet * zet);
      sa += ipz * iwa;
    }
    double density = theMaterial->GetDensity() * geant::units::cm3 / geant::units::g; // [g/cm3]
    //
    gMoliereBc[theMaterial->GetIndex()] = const1 * density * zs / sa * Math::Exp(ze / zs) / Math::Exp(zx / zs); //[1/cm]
    gMoliereXc2[theMaterial->GetIndex()] = const2 * density * zs / sa; // [MeV2/cm]
    // change to internal units of 1/length and energ2/length
    gMoliereBc[theMaterial->GetIndex()] *= 1.0 / geant::units::cm;
    gMoliereXc2[theMaterial->GetIndex()] *= geant::units::MeV * geant::units::MeV / geant::units::cm;
  }
}

double GSMSCTable::ComputeScatteringPowerCorrection(const MaterialCuts *matcut, double ekin)
{
  int imc          = matcut->GetIndex();
  double corFactor = 1.0;
  if (!(fActiveRegionsVector[matcut->GetRegionIndex()]) || ekin <= fSCPCPerMatCuts[imc]->fPrCut) {
    return corFactor;
  }
  // get the scattering power correction factor
  double lekin     = Math::Log(ekin);
  double remaining = (lekin - fSCPCPerMatCuts[imc]->fLEmin) * fSCPCPerMatCuts[imc]->fILDel;
  int lindx        = (int)remaining;
  remaining -= lindx;
  int imax = fSCPCPerMatCuts[imc]->fVSCPC.size() - 1;
  if (lindx >= imax) {
    corFactor = fSCPCPerMatCuts[imc]->fVSCPC[imax];
  } else {
    corFactor = fSCPCPerMatCuts[imc]->fVSCPC[lindx] +
                remaining * (fSCPCPerMatCuts[imc]->fVSCPC[lindx + 1] - fSCPCPerMatCuts[imc]->fVSCPC[lindx]);
  }
  return corFactor;
}

void GSMSCTable::InitSCPCorrection(const std::vector<bool> &activeregionv)
{
  // get the material-cuts table
  const std::vector<MaterialCuts *> &thePCTable = MaterialCuts::GetTheMaterialCutsTable();
  size_t numMatCuts                             = thePCTable.size();
  // clear container if any
  for (size_t imc = 0; imc < fSCPCPerMatCuts.size(); ++imc) {
    if (fSCPCPerMatCuts[imc]) {
      fSCPCPerMatCuts[imc]->fVSCPC.clear();
      delete fSCPCPerMatCuts[imc];
      fSCPCPerMatCuts[imc] = nullptr;
    }
  }
  //
  // set size of the container and create the corresponding data structures
  fSCPCPerMatCuts.resize(numMatCuts, nullptr);
  // loop over the material-cuts and create scattering power correction data structure for each
  for (size_t imc = 0; imc < numMatCuts; ++imc) {
    const MaterialCuts *matCut = thePCTable[imc];
    if (!(activeregionv[matCut->GetRegionIndex()])) {
      continue;
    }
    // get e- production cut in the current material-cuts in energy
    double limit;
    double ecut = matCut->GetProductionCutsInEnergy()[1];
    if (fIsElectron) {
      limit = 2. * ecut;
    } else {
      limit = ecut;
    }
    double min = std::max(limit, fLowEnergyLimit);
    double max = fHighEnergyLimit;
    if (min >= max) {
      fSCPCPerMatCuts[imc]         = new SCPCorrection();
      fSCPCPerMatCuts[imc]->fIsUse = false;
      fSCPCPerMatCuts[imc]->fPrCut = min;
      continue;
    }
    int numEbins         = fNumSPCEbinPerDec * std::lrint(Math::Log10(max / min));
    numEbins             = std::max(numEbins, 3);
    double lmin          = Math::Log(min);
    double ldel          = Math::Log(max / min) / (numEbins - 1.0);
    fSCPCPerMatCuts[imc] = new SCPCorrection();
    fSCPCPerMatCuts[imc]->fVSCPC.resize(numEbins, 1.0);
    fSCPCPerMatCuts[imc]->fIsUse = true;
    fSCPCPerMatCuts[imc]->fPrCut = min;
    fSCPCPerMatCuts[imc]->fLEmin = lmin;
    fSCPCPerMatCuts[imc]->fILDel = 1. / ldel;
    for (int ie = 0; ie < numEbins; ++ie) {
      double ekin    = Math::Exp(lmin + ie * ldel);
      double scpCorr = 1.0;
      // compute correction factor: I.Kawrakow NIMB 114(1996)307-326 (Eqs(32-37))
      if (ie > 0) {
        double tau    = ekin / geant::units::kElectronMassC2;
        double tauCut = ecut / geant::units::kElectronMassC2;
        // Moliere's screening parameter
        int matindx = matCut->GetMaterial()->GetIndex();
        double A    = GetMoliereXc2(matindx) / (4.0 * tau * (tau + 2.) * GetMoliereBc(matindx));
        double gr   = (1. + 2. * A) * Math::Log(1. + 1. / A) - 2.;
        double dum0 = (tau + 2.) / (tau + 1.);
        double dum1 = tau + 1.;
        double gm   = Math::Log(0.5 * tau / tauCut) +
                    (1. + dum0 * dum0) * Math::Log(2. * (tau - tauCut + 2.) / (tau + 4.)) -
                    0.25 * (tau + 2.) * (tau + 2. + 2. * (2. * tau + 1.) / (dum1 * dum1)) *
                        Math::Log((tau + 4.) * (tau - tauCut) / tau / (tau - tauCut + 2.)) +
                    0.5 * (tau - 2 * tauCut) * (tau + 2.) * (1. / (tau - tauCut) - 1. / (dum1 * dum1));
        if (gm < gr) {
          gm = gm / gr;
        } else {
          gm = 1.;
        }
        double z0 = matCut->GetMaterial()->GetMaterialProperties()->GetEffectiveZ();
        scpCorr   = 1. - gm * z0 / (z0 * (z0 + 1.));
      }
      fSCPCPerMatCuts[imc]->fVSCPC[ie] = scpCorr;
    }
  }
}

} // namespace geantphysics
