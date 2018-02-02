/***********************************************************
 * @file TARC.cxx
 * @author Abhijit Bhattacharyya
 * @brief tarc main code.
 * @date Oct 2017
 ***********************************************************/

#include "TARC.h"

using namespace vecgeom;
using namespace geantphysics;
using namespace Geant;
using namespace tarc;

  TARC::TARC(Geant::GeantRunManager *runmgr, TARCGeometryConstruction *geom, TARCPrimaryGenerator *gun)
  : Geant::GeantVApplication(runmgr), fGeomSetup(geom), fPrimaryGun(gun){
    fInitialized           = false;
    fTargetLogicalVolumeID = -1;
    fNumPrimaryPerEvent    = -1;
    fNumBufferedEvents     = -1;
    fPrimaryParticleCharge = -1.0;
  }

  TARC::~TARC(){}

  bool TARC::Initialize() {
    if (fInitialized) return true;
    std::cout << " Initializing......" << std::endl;
    if (!fGeomSetup) {
      Geant::Error("TARC::Initialize", "Geometry is not available!");
      return false;
    }

    RetrieveLogicalVolumesFromGDML();
    RetrievePlacedVolumesFromGDML();
    fixGun();

    fInitialized = true;

    std::cout <<"Fixing the Gun position at :--> " << fGunPos <<  " OK " << std::endl;
    for (auto iLV : fLVolumeList) {
      std::cout << iLV->GetLabel() << "   " << iLV->id() << std::endl;
      // fGeomSetup->SetLVList(iLV);
    }

    if (!fPrimaryGun) {
      Geant::Error("TARC::Initialize","PrimaryGenerator not available!");
      return false;
    }

    fPrimaryParticleCharge = fPrimaryGun->GetPrimaryParticle()->GetPDGCharge();
    fNumPrimaryPerEvent    = fRunMgr->GetConfig()->fNaverage;
    fNumBufferedEvents     = fRunMgr->GetConfig()->fNbuff;

    fInitialized = true;

    exit(0);
    return true;
  }

  void TARC::fixGun() {
    Vector3D<double> minVal(0.0, 0.0, 0.0);
    Vector3D<double> maxVal(0.0, 0.0, 0.0);
    Vector3D<double> myGunPoint(0., 0., 0.);
    double maxX = -999999.99, minX = -maxX, maxY = maxX, minY = minX, maxZ = maxX, minZ = minZ;
    VPlacedVolume *beamLine;
    Transformation3D const *transUsed;
    for (auto iPV : fPVolumeList) {
      std::string placedName = iPV->GetLabel();
      if (placedName.find("beam")!=std::string::npos){
        iPV->GetUnplacedVolume()->Extent(minVal, maxVal);
        myGunPoint.x() = 0.5 * (maxVal.x() + minVal.x());
        myGunPoint.y() = 0.5 * (maxVal.y() + minVal.y());
        myGunPoint.z() = 0.5 * (maxVal.z() + minVal.z());
        beamLine = vecgeom::GeoManager::Instance().FindPlacedVolume(placedName.c_str());
        transUsed = beamLine->GetTransformation();
        SetGun(
         transUsed->InverseTransform(myGunPoint).x(),
         transUsed->InverseTransform(myGunPoint).y(),
         transUsed->InverseTransform(myGunPoint).z()
       );

       maxZ = std::max(maxZ, fGunPos.z());
       minZ = std::min(minZ, fGunPos.z());
     }
     fGunPos.z() = minZ;
     fGunPos.z() -= 1000.0*geant::mm;
   }
 }
