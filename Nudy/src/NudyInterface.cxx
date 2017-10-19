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
    SetProjectileCode (projCode);
    SetProjectileKE (projKE);
    SetTemp (temp);
    SetIsotopeName (isoName);
    SetZ (tZ);
    SetA (tA);
    SetProcessType ( ProcessType );
    SetMTValues (ProcessType);
  };

NudyInterface::~NudyInterface() {}

////////////////////////////////////
double NudyInterface::GetXS( int projCode, double projKE, double temp,
  std::string isoName, int tZ, int tA, geantphysics::HadronicProcessType pType
) {
  SetProcessType (pType);
  SetMTValues ( pType );
  SetA (tA);
  SetZ (tZ);
  SetProjectileKE (projKE);
  SetTemp(temp);
  SetIsotopeName(isoName);
  SetIsFissKey(false); // initializing to false first

  Nudy::TNudyENDF *proc;

  //  Fix the name of the ENDF, ROOT and ENDFSUB filenames here
  std::string fileENDF1 = SetDataFileNameENDF(projCode, isoName, projKE);
  SetEndfDataFileName(fileENDF1.c_str());
  std::string fileENDF2 = SetDataFileNameROOT(isoName);
  SetRootFileName (fileENDF2.c_str());
  std::string fileENDF3 = SetDataFileNameENDFSUB( isoName);
  SetEndfSubDataFileName (fileENDF3.c_str());

/*
  std::cout << "1. ENDF      --> " << fEndfDataFileName    << "\n"
            << "2. ENDFSUB   --> " << fEndfSubDataFileName << "\n"
            << "3. ROOT      --> " << fRootFileName        << std::endl;

*/
  // Create and process with NUDY with keywords
  proc = new Nudy::TNudyENDF (fEndfDataFileName, fRootFileName, "recreate");
  proc->Process();
  bool LFIval = proc->GetLFI();
  SetIsFissKey(LFIval);
  proc->SetEndfSub(fEndfSubDataFileName);
  proc->Process();

  //SetProjIDFn(projCode, projKE,"");
  double XSvalue = ComputeCrossSection();  // call routines from Nudy
  return XSvalue;
}

std::string NudyInterface::SetDataFileNameENDF( int projCode, std::string isoName, double projKE){
  std::string fstyle = "ENDF";
  SetProjIDFn( projCode, projKE, fstyle);
  std::string DataFileNameString = findENDFFileName( isoName);
  std::string fileENDFName1 = GetDataFileName(fProjID, DataFileNameString);
  std::cout << DataFileNameString << "  " << fileENDFName1 << std::endl;

  return fileENDFName1;
}

std::string NudyInterface::SetDataFileNameROOT( std::string isoName){
  std::string DataFileNameString = findENDFFileName(isoName);
  std::string fileENDFName2 = FixRootDataFile(DataFileNameString);
  return fileENDFName2;
}

// This method is to be modified
void NudyInterface::SetProjIDFn(int projCode, double projKE, std::string fstyle) {
  bool isChFiss = GetFisCha(fMTValue);
  if ( projKE < 0.03*geant::eV && projCode == 2112 ) SetProjID("thermal_scatt");
  if ( projKE > 0.03*geant::eV && projCode == 2112 ) SetProjID("neutrons");
  if (isChFiss && fstyle.compare("ENDFSUB")==0) SetProjID("nfy");
}

std::string NudyInterface::SetDataFileNameENDFSUB( std::string isoName){
  std::string fileENDFName3;
  std::string DataFileNameString;
  SetProjID("nfy");
  std::string prjId = GetProjID();
  DataFileNameString = findENDFFileName(isoName);
  fileENDFName3 = GetDataFileName(prjId, DataFileNameString);
  return fileENDFName3;
}

// Actual Nudy CrossSection computation method
double NudyInterface::ComputeCrossSection() {
  int iElementID = 0;  //<------------- confusing testing by Abhijit 419 ?
  double xsvalue = 0.0;
  //double iSigDiff = 0.001;   // trial value for test documentation reqd.

  NudyPhysics::TNudyEndfSigma();
  TNudyEndfSigma  xsec;
  //xsec = new TNudyEndfSigma(fRootFileName, iSigDiff);
  //xsec.GetData(fRootFileName, iSigDiff);


//NudyPhysics::TNudyEndfRecoPoint();
  NudyPhysics::TNudyEndfRecoPoint *recoPoint; // = new NudyPhysics::TNudyEndfRecoPoint(iElementID, fRootFileName);
  recoPoint = new TNudyEndfRecoPoint(iElementID, fRootFileName);
  // This is  under testing to check Harphool code for interfacing to GV :: Abhijit

  for ( unsigned int crsp = 0; crsp < recoPoint->MtValues[iElementID].size(); crsp++) {
    int mtNow = recoPoint->MtValues[iElementID][crsp];
    if ( mtNow == fMTValue) { 
      xsvalue = recoPoint->GetSigmaPartial(iElementID, crsp, fProjKE) ;
      break;
    }
/*
    else if (mtNow == fMTValue) {
      xsvalue = recoPoint->GetSigmaTotal(iElementID, fProjKE) ;
      break;
    }
    */
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
    std::cout << " Please set environment ENDFDATADIR pointing to root of ENDF data directory ." << std::endl;
    exit(99);
  }
  return EndfDataPath;
}

// selects name of root data file in the current working directory
std::string NudyInterface::FixRootDataFile(std::string str1){
  std::string cwdPath = GetCWD();
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

std::string NudyInterface::findENDFFileName(std::string elementName){
  std::stringstream ss;
  std::string fName = "";
  if (fProjID == "thermal_scatt") {
    fName = "tsl-";
  } else if (fProjID == "neutrons" ) {
    fName = "n-";
  } else {
    fName = fProjID + "-";
  }

  ss << ftZ;
  std::string stZ = ss.str();
  ss.str("");
  ss << ftA;
  std::string stA = ss.str();
  ss.str("");

  switch(stZ.length()) {
    case 0: return "";
    case 1: stZ = "00" + stZ;   break;
    case 2: stZ = "0" + stZ;    break;
    case 3: stZ = stZ;
  }

  if (ftZ == 12) {
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
  SetProcessType ( pType );
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
  fChannelFiss.begin(), NudyInterface::fChannelFiss.end(), inKey ) != fChannelFiss.end();
  return isIn;
}
