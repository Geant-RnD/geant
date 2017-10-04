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
#include "NudyXSProcess.h"

#include "TSystem.h"
//class TFile;
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
using NudyPhysics::NudyXSProcess;


namespace geantphysics {
  class LightTrack;
  class HadronicCrossSection;
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
    NudyInterface () ;
    NudyInterface( const int projCode, const double projKE, const double temp, const std::string isoName,
      const int tZ, const int tN, const std::string reactType );
    virtual ~NudyInterface ();

    double GetNudyXS( int projCode, double projKE, double temp, std::string isoName, int tZ, int tN, std::string reactType ) ;

  public:
    inline std::string GetIsotopeName ();
    inline int GetProjectileCode ();
    inline int GetZ ();
    inline int GetN ();
    inline double GetProjectileKE ();
    inline double GetTemp();
    inline double GetCrossSection ();
    inline std::string GetReactionType();

    inline void SetIsotopeName (const std::string &isoName );
    inline void SetProjectileCode ( const int projCode );
    inline void SetZ ( const int tZValue );
    inline void SetN ( const int tNValue ) ;
    inline void SetProjectileKE ( const double projKE );
    inline void SetTemp ( const double temp );
    inline void SetCrossSection ( const double XSvalue );
    inline void SetReactionType ( const std::string react );


  private :
    std::string fIsoName;
    std::string XSType;
    int fProjCode;
    int ftZ;
    int ftN;
    double fProjKE;
    double fTemperature;
    double fXS;
    const char* fEndfDataFileName;
    const char* fRootFileName;
    std::string fReactType;
  };

  //--------- GETTERS -------
  inline std::string NudyInterface::GetIsotopeName () { return fIsoName; }
  inline int NudyInterface::GetProjectileCode () { return fProjCode; }
  inline int NudyInterface::GetZ () { return ftZ; }
  inline int NudyInterface::GetN () { return ftN; }
  inline double NudyInterface::GetProjectileKE () { return fProjKE; }
  inline double NudyInterface::GetTemp () { return fTemperature; };
  inline double NudyInterface::GetCrossSection () { return fXS; }
  inline std::string NudyInterface::GetReactionType () { return fReactType; }

  //--------- SETTERS ---------
  inline void NudyInterface::SetIsotopeName ( const std::string &isoName ) { fIsoName = isoName; }
  inline void NudyInterface::SetProjectileCode ( const int projCode ) { fProjCode = projCode; }
  inline void NudyInterface::SetZ ( const int tZValue ) { ftZ = tZValue; }
  inline void NudyInterface::SetN ( const int tNValue ) { ftN = tNValue; }
  inline void NudyInterface::SetProjectileKE ( const double projKE ) { fProjKE = projKE; }
  inline void NudyInterface::SetTemp (const double temp ) { fTemperature = temp; }
  inline void NudyInterface::SetCrossSection ( const double XSvalue ) { fXS = XSvalue; }
  inline void NudyInterface::SetReactionType ( const std::string reactType ) { fReactType = reactType; }


 } // namespace NudyPhysics

#endif
