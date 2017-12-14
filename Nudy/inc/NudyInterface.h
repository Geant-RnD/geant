//-----------------------------------------------------------------
// @file NudyInterface.h
// @brief prototype Nudy interface for GV
// @author Abhijit Bhattacharyya
//----------------------------------------------------------------
#ifndef NUDY_INTERFACE_H
#define NUDY_INTERFACE_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <iterator>

#include "Material.h"
#include "Element.h"

#include "SystemOfUnits.h"
#include "PhysicalConstants.h"
//#include "HadronicProcess.h"
#include "NudyProcess.h"
#include "NudyCrossSection.h"


#ifdef USE_ROOT
#include "TNudyDB.h"
#include "TNudyAlias.h"
#include "TNudyElementTable.h"
#include "TNudyENDF.h"
#include "TNudyEndfCont.h"
#include "TNudyEndfFile.h"
#include "TNudyEndfINTG.h"
#include "TNudyEndfList.h"
#include "TNudyEndfMat.h"
#include "TNudyEndfTape.h"
#include "TNudyEndfRecord.h"
#include "TNudyEndfSec.h"
#include "TNudyEndfTab1.h"
#include "TNudyEndfTab2.h"
#include "TNudyLibrary.h"
#include "TNudyManager.h"
#include "TNudySubLibrary.h"
#include "TVNudyModel.h"
#include "TNudyEndfEnergy.h"
#include "TNudyEndfSigma.h"
#include "TNudyEndfRecoPoint.h"
#include "TNudyEndfTape.h"
#include "TNudyEndfAng.h"
//#include "NudyXSProcess.h"

#include "TSystem.h"
#include "TFile.h"
#include "TList.h"
#include "TKey.h"
class TList;
#endif


using Nudy::TNudyAlias;
using Nudy::TNudyCore;
using Nudy::TNudyDB;
using Nudy::TNudyElementTable;
using Nudy::TNudyEndfFile;
using Nudy::TNudyENDF;
using Nudy::TNudyEndfMat;
using Nudy::TNudyEndfList;
using Nudy::TNudyEndfSec;
using Nudy::TNudyEndfCont;
using Nudy::TNudyEndfRecord;
using Nudy::TNudyEndfTape;
using Nudy::TNudyEndfTab1;
using Nudy::TNudyEndfTab2;
using Nudy::TNudyLibrary;
using Nudy::TNudyManager;
using Nudy::TVNudyModel;
using Nudy::TNudyEndfINTG;

using NudyPhysics::TNudyEndfDoppler;
using NudyPhysics::TNudyEndfEnergyAng;
using NudyPhysics::TNudyEndfEnergy;
using NudyPhysics::TNudyEndfFissionYield;
using NudyPhysics::TNudyEndfNuPh;
using NudyPhysics::TNudyEndfPhAng;
using NudyPhysics::TNudyEndfPhEnergy;
using NudyPhysics::TNudyEndfPhProd;
using NudyPhysics::TNudyEndfPhYield;
using NudyPhysics::TNudyEndfRecoPoint;
using NudyPhysics::TNudyEndfSigma;
//using NudyPhysics::NudyXSProcess;


namespace geantphysics {
  class LightTrack;
  //class HadronicCrossSection;
  //class HadronicProcess;
//  class HadronicProcessType;
  class NudyCrossSection;
  class NudyProcess;
  enum class NudyProcessType;

  // class HadronicProcess;
  inline namespace GEANT_IMPL_NAMESPACE {
    class Isotope;
    class Material;
    class Element;
  }
}

namespace NudyPhysics{

  class NudyInterface {
  public:
    NudyInterface ();
    NudyInterface( const int projCode, const double projKE, const double temp, const std::string isoName,
      const int tZ, const int tA, geantphysics::NudyProcessType  processType );
    virtual ~NudyInterface ();

  public:
    double GetXS( int projCode, double projKE, double temp, std::string isoName,
      int tZ, int tA, geantphysics::NudyProcessType pType
    ) ;
    std::string  SetDataFileNameENDF( int projCode, std::string isoName); //, int tZ, int tA );
    std::string findENDFFileName( std::string ele);//, int tZ, int tA ) ;
    std::string GetDataFileName( std::string str1, std::string str2 );
    std::string FixRootDataFile( std::string str1 );                   // ENDF filename without path and extension
    std::string SetDataFileNameROOT( std::string isoName); //, int tZ, int tA );
    std::string SetDataFileNameENDFSUB( std::string isoName); //, int tZ, int tA );
    std::string GetCWD();
    bool GetFisCha(int inKey);
    void SetProjIDFn( int projCode, std::string style);
    double ComputeCrossSection( );
    void SetMTValues(geantphysics::NudyProcessType pType);
    void setFileNames(std::string fIN, std::string fOUT, std::string fSubName);
    void DumpEndf2Root(std::string fIN, std::string fOUT, std::string fSubName, int tA, int tZ, double temp, std::string isotopeN);
	  void ConvertENDF2ROOT(std::string fENDF, std::string rENDF);
	  void printE2RErr();
  public:
    inline std::string GetIsotopeName ();
    inline int GetProjectileCode ();
    inline int GetZ ();
    inline int GetA ();
    inline double GetProjectileKE ();
    inline double GetTemp();
    inline double GetCrossSection ();
    inline bool GetIsFissKey();
    inline std::string GetProjID();
    //inline std::vector<double> GetXSTable();

    inline void SetIsotopeName (const std::string &isoName );
    inline void SetProjectileCode ( const int projCode );
    inline void SetZ ( const int tZValue );
    inline void SetA ( const int tAvalue );
    inline void SetProjectileKE ( const double projKE );
    inline void SetTemp ( const double temp );
    inline void SetCrossSection ( const double XSvalue );
    inline void SetEndfDataFileName ( std::string  fileName );
    inline void SetEndfSubDataFileName ( std::string fileName );
    inline void SetRootFileName ( std::string fileName );
    inline void SetIsFissKey( const bool theKey);
    inline void SetProjID (const std::string &theID);
    inline void AppendXS ( const double xsvalue );
    inline void SetProcessType ( const geantphysics::NudyProcessType ptype );


  private :

    unsigned int fNumberOfReactionChannels = 895;
    geantphysics::NudyProcessType fProcType;
    std::string fIsoName;
    int fMTValue;
    int fProjCode;
    int ftZ;
    int ftA;
    int MTChargeFlag[10];
    double fProjKE;
    double fTemperature;
    double fXS;
    std::string fProjID;
    const char* fEndfFileN;
    const char* fEndfSubDataFileName;
    const char* fRootFileName;
    std::vector<int> fFissionFragmentsMass;
    std::vector<int> fFissionFragmentCharge;
    std::vector<double>  fChannelXSArray;
    bool fIsFiss;
    std::vector<int> fChannelFiss {18, 19, 20, 21, 38, 151, 452, 454, 455, 456, 457, 458, 459};
  };

  //--------- GETTERS -------
  inline std::string NudyInterface::GetIsotopeName () { return fIsoName; }
  inline int NudyInterface::GetProjectileCode () { return fProjCode; }
  inline int NudyInterface::GetZ () { return ftZ; }
  inline int NudyInterface::GetA () { return ftA; }
  inline double NudyInterface::GetProjectileKE () { return fProjKE; }
  inline double NudyInterface::GetTemp () { return fTemperature; }
  inline bool NudyInterface::GetIsFissKey() { return fIsFiss; }
  inline std::string NudyInterface::GetProjID() { return fProjID; }
  //inline std::vector<double> NudyInterface::GetXSTable() { return fChannelXSArray; }

  //--------- SETTERS ---------
  inline void NudyInterface::SetIsotopeName ( const std::string &isoName ) { fIsoName = isoName; }
  inline void NudyInterface::SetProjectileCode ( const int projCode ) { fProjCode = projCode; }
  inline void NudyInterface::SetZ ( const int tZValue ) { ftZ = tZValue; }
  inline void NudyInterface::SetA ( const int tAvalue ) { ftA = tAvalue; }
  inline void NudyInterface::SetProjectileKE ( const double projKE ) { fProjKE = projKE; }
  inline void NudyInterface::SetTemp (const double temp ) { fTemperature = temp; }
  inline void NudyInterface::SetCrossSection ( const double XSvalue ) { fXS = XSvalue; }
  inline void NudyInterface::SetEndfDataFileName ( std::string fileName ) { fEndfFileN = fileName.c_str(); }
  inline void NudyInterface::SetEndfSubDataFileName ( std::string fileName ) { fEndfSubDataFileName = fileName.c_str(); }
  inline void NudyInterface::SetRootFileName ( std::string fileName ) { fRootFileName = fileName.c_str(); }
  inline void NudyInterface::SetIsFissKey( const bool theKey ) { fIsFiss = theKey; }
  inline void NudyInterface::SetProjID (const std::string &theID ) { fProjID = theID; }
  //inline void NudyInterface::AppendXS ( const double xsvalue ) { fChannelFiss.push_back(xsvalue); }
  inline void NudyInterface::SetProcessType ( const geantphysics::NudyProcessType ptype ) { fProcType = ptype; }
 } // namespace NudyPhysics

#endif
