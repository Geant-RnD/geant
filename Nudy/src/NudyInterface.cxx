/**********************************************
 * @file NudyInterface.cxx
 * @author Abhijit Bhattacharyya
 * @brief This interface links to the GV through NUDYCrossSection of realphysics/physics/HAD/CrossSection
 **********************************************/
#include <iostream>
#include "NudyInterface.h"

using namespace geantphysics;
using namespace Nudy;
using namespace NudyPhysics;


NudyInterface::NudyInterface() //:
// fProjCode(2112), fProjKE(4.0e+6), fTemperature(293.60608), fIsoName("Fe"), ftZ(26), ftN(56),
// fEndfDataFileName(""), fEndfSubDataFileName(""), fRootFileName("")
{};


NudyInterface::NudyInterface(
  const int projCode, const double projKE, double temp, const std::string isoName,
  const int tZ, const int tA, geantphysics::HadronicProcessType ProcessType
)
  // : fProjCode(projCode), fProjKE(projKE), fIsoName(isoName), ftZ(tZ), ftN(tN) {
  {
    std::string newReact;
    NudyInterface::SetProjectileCode (projCode);
    NudyInterface::SetProjectileKE (projKE);
    NudyInterface::SetTemp (temp);
    NudyInterface::SetIsotopeName (isoName);
    NudyInterface::SetZ (tZ);
    NudyInterface::SetA (tA);
    NudyInterface::SetProcessType ( ProcessType );
    NudyInterface::SetMTValues (ProcessType);
  };

NudyInterface::~NudyInterface() {}

////////////////////////////////////
double NudyInterface::GetXS( int projCode, double projKE, double temp,
  std::string isoName, int tZ, int tA, geantphysics::HadronicProcessType pType
) {
  NudyInterface::SetProcessType (pType);
  NudyInterface::SetMTValues ( pType );
  NudyInterface::SetA (tA);
  NudyInterface::SetProjectileKE (projKE);
  NudyInterface::SetTemp(temp);
  NudyInterface::SetIsotopeName(isoName);
  NudyInterface::SetIsFissKey(false); // initializing to false first

  Nudy::TNudyENDF *proc;

  //  Fix the name of the ENDF, ROOT and ENDFSUB filenames here
  std::string fileENDF1 = NudyInterface::SetDataFileNameENDF(projCode, isoName, projKE, tZ, tA);
  SetEndfDataFileName(fileENDF1.c_str());
  std::string fileENDF2 = NudyInterface::SetDataFileNameROOT(isoName, tZ, tA);
  SetRootFileName (fileENDF2.c_str());
  std::string fileENDF3 = NudyInterface::SetDataFileNameENDFSUB( isoName, tZ, tA);
  NudyInterface::SetEndfSubDataFileName (fileENDF3.c_str());

  // Create and process with NUDY with keywords
  proc = new Nudy::TNudyENDF (fEndfDataFileName, fRootFileName, "recreate");
  proc->Process();
  bool LFIval = proc->GetLFI();
  SetIsFissKey(LFIval);
  proc->SetEndfSub(fEndfSubDataFileName);
  proc->Process();

  SetProjIDFn(projCode, projKE);
  double XSvalue = NudyInterface::ComputeCrossSection();  // call routines from Nudy

  return XSvalue;
}

std::string NudyInterface::SetDataFileNameENDF( int projCode, std::string isoName, double projKE, int tZ, int tA ){
  NudyInterface::SetProjIDFn( projCode, projKE);
  std::string DataFileNameString = NudyInterface::findENDFFileName( isoName, tZ, tA);
  std::string fileENDFName1 = NudyInterface::GetDataFileName(fProjID, DataFileNameString);
  return fileENDFName1;
}

std::string NudyInterface::SetDataFileNameROOT( std::string isoName, int tZ, int tA ){
  std::string DataFileNameString = NudyInterface::findENDFFileName(isoName, tZ, tA);
  std::string fileENDFName2 = NudyInterface::FixRootDataFile(DataFileNameString);
  return fileENDFName2;
}

// This method is to be modified
void NudyInterface::SetProjIDFn(int projCode, double projKE) {
  bool isChFiss = NudyInterface::GetFisCha(fMTValue);
  if ( projKE < 0.03*geant::eV && projCode == 2112 ) NudyInterface::SetProjID("thermal_scatt");
  if ( projKE > 0.03*geant::eV && projCode == 2112 ) NudyInterface::SetProjID("neutrons");
  if (isChFiss) NudyInterface::SetProjID("nfy");
}

std::string NudyInterface::SetDataFileNameENDFSUB( std::string isoName, int tZ, int tA ){
  std::string fileENDFName3;
  std::string DataFileNameString;
  NudyInterface::SetProjID("nfy");
  std::string prjId = NudyInterface::GetProjID();
  DataFileNameString = NudyInterface::findENDFFileName(isoName, tZ, tA);
  fileENDFName3 = NudyInterface::GetDataFileName(prjId, DataFileNameString);
  return fileENDFName3;
}

// Actual Nudy CrossSection computation method
double NudyInterface::ComputeCrossSection() {
  int iElementID = 0;  //<------------- confusing testing by Abhijit 419 ?
  double xsvalue = 0.0;
  double iSigDiff = 0.001;   // trial value for test documentation reqd.

  NudyPhysics::TNudyEndfSigma();
  NudyPhysics::TNudyEndfSigma xsec;
  xsec.GetData(fRootFileName, iSigDiff);

  NudyPhysics::TNudyEndfRecoPoint *recoPoint = new NudyPhysics::TNudyEndfRecoPoint(iElementID, fRootFileName);
  // This is  under testing to check Harphool code for interfacing to GV :: Abhijit

  for ( unsigned int crsp = 0; crsp < recoPoint->MtValues[iElementID].size(); crsp++) {
    if ( fMTValue == recoPoint->MtValues[iElementID][crsp] ) {
      xsvalue = recoPoint->GetSigmaTotal(iElementID, fProjKE);
      break;
    }
  }
  return xsvalue;
}

// selects the data file name for ENDF data
std::string NudyInterface::GetDataFileName(std::string str1, std::string str2) {
  std::string EndfDataPath="";
  if (std::getenv("ENDFDATADIR")!=NULL){
    EndfDataPath = std::getenv("ENDFDATADIR");
    std::string ENDFDataString = EndfDataPath + "/" + str1 + "/" + str2 + ".endf";
    return ENDFDataString;
  } else {
    std::cout << " Please set environment ENDFDATADIR pointing to ENDF data directory." << std::endl;
    exit(99);
  }
  return EndfDataPath;
}

// selects name of root data file in the current working directory
std::string NudyInterface::FixRootDataFile(std::string str1){
  std::string cwdPath = NudyInterface::GetCWD();
  std::string rootENDFFile = cwdPath + "/" + str1 + ".root";
  return rootENDFFile;
}

// store root data file in current working directory
// This is a bad technique. Using for the testing as quick solution.
std::string NudyInterface::GetCWD() {
  char * tempDIRarray = new char[1024];
  std::string cwdPath= (getcwd(tempDIRarray, 1024)) ? std::string(tempDIRarray) : std::string("");
  delete [] tempDIRarray;
  return cwdPath;
}

std::string NudyInterface::findENDFFileName(std::string elementName, int tZ, int tA) {
  std::stringstream ss;
  std::string fName = "";
  if (fProjID == "thermal_scatt") {
    fName = "tsl-";
  } else if (fProjID == "neutrons" ) {
    fName = "n-";
  } else {
    fName = fProjID + "-";
  }

  ss << tZ;
  std::string stZ = ss.str();
  ss.str("");
  ss << tA;
  std::string stA = ss.str();
  ss.str("");

  switch(stZ.length()) {
    case 0: return "";
    case 1: stZ = "00" + stZ;   break;
    case 2: stZ = "0" + stZ;    break;
    case 3: stZ = stZ;
  }

  if (tZ == 12) {
    stA = "000";
  } else {
    switch(stA.length()) {
      case 0: return "";
      case 1: stA = "00" + stA;  break;
      case 2: stA = "0" + stA;   break;
      case 3: stA = stA;
    }
  }
  fName = fName + stZ + "_" + elementName + "_" + stA;
  return fName;
}

// Depending on the process type set the MT value for cross section
void NudyInterface::SetMTValues (geantphysics::HadronicProcessType pType) {
  NudyInterface::SetProcessType ( pType );
  switch (pType) {
    case geantphysics::HadronicProcessType::kElastic          : fMTValue = 2;   break;
    case geantphysics::HadronicProcessType::kInelastic        : fMTValue = 3;   break;
    case geantphysics::HadronicProcessType::kFission          : fMTValue = 18;  break;
    case geantphysics::HadronicProcessType::kCapture          : fMTValue = 102; break;
    case geantphysics::HadronicProcessType::kRadioactiveDecay : fMTValue = 457; break;
    case geantphysics::HadronicProcessType::kNotDefined       :
    case geantphysics::HadronicProcessType::kQuasiElastic     :
    case geantphysics::HadronicProcessType::kUserDefined      :
    case geantphysics::HadronicProcessType::kLeptoNuclear     : fMTValue = 2;   // to be changed later
  }
}

bool NudyInterface::GetFisCha(int inKey) {
  bool isIn = false;
  isIn = std::find(
  NudyInterface::fChannelFiss.begin(), NudyInterface::fChannelFiss.end(), inKey ) != NudyInterface::fChannelFiss.end();
  return isIn;
}
