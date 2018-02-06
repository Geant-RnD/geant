/***********************************************************
 * @file TARC.cxx
 * @author Abhijit Bhattacharyya
 * @brief tarc main code.
 * @date Oct 2017
 ***********************************************************/

#include "TARCapp.h"

using namespace vecgeom;
using namespace geantphysics;
using namespace Geant;
using namespace tarcapp;

// TARCapp::TARCapp(Geant::GeantRunManager *runmgr, TARCGeometryConstruction *geom, TARCPrimaryGenerator *gun)
TARCapp::TARCapp(Geant::GeantRunManager *runmgr, TARCPrimaryGenerator *gun)
  : Geant::GeantVApplication(runmgr), fPrimaryGun(gun){
    fInitialized           = false;
    //fTargetLogicalVolumeID = -1;
    fNumPrimaryPerEvent    = TARCPrimaryGenerator::GetMaxNumberOfPrimariesPerEvent();
    fNumBufferedEvents     = -1;
    fPrimaryParticleCharge = -1.0;
    fIsPerformance         = false;
    fDataHandlerEvents     = nullptr;
    fData                  = nullptr;
}

TARCapp::~TARCapp(){
    if (fData)
      delete fData;
}


  void TARCapp::AttachUserData(Geant::GeantTaskData *td) {
    if (fIsPerformance)
      return;
    TARCThreadDataEvents *eventData = new TARCThreadDataEvents(fNumBufferedEvents, fNumPrimaryPerEvent);
    fDataHandlerEvents->AttachUserData(eventData, td);
  }

  bool TARCapp::Initialize() {

    if (fInitialized || fIsPerformance) return true;

    /*
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
*/

  // Get Number of primaries per events. number of event-slots from Geant::GeantConfig
  int maxPrimaryPerEvent = fPrimaryGun->GetNumberOfPrimaryPerEvent();
  if (maxPrimaryPerEvent > 0)   fNumPrimaryPerEvent = maxPrimaryPerEvent;

  fNumBufferedEvents     = fRunMgr->GetConfig()->fNbuff;
  fNumPrimaryPerEvent    = fRunMgr->GetConfig()->fNaverage;

  // Register thread local data and get handler for them.
  fDataHandlerEvents     = fRunMgr->GetTDManager()->RegisterUserData<TARCThreadDataEvents>("TARCThreadDataEvents");

  // create unique, global data structure that will be used to store cumulated per primary data during simulation.
  fData                  = new TARCData(TARCPrimaryGenerator::GetNumberOfPrimaryTypes());
  fInitialized = true;
  return true;
}


void TARCapp::SteppingActions(Geant::GeantTrack &track, Geant::GeantTaskData *td){
  if (fIsPerformance) return;
  const geantphysics::Particle *part = geantphysics::Particle::GetParticleByInternalCode(track.GVcode());
  int pdgCode   = part->GetPDGCode();
  double charge = part->GetPDGCharge();

  // Get user defined thread local data structure per-primary particle for event-slot index.
  // that defines per-event data structure
  // and
  // primary index that defines per-primary data structure within that per-event data structure
  // Each track stores event-slot, primary particle index
  TARCDataPerPrimary &dataPerPrimary =
        (*fDataHandlerEvents)(td).GetDataPerEvent(track.EventSlot()).GetDataPerPrimary(track.PrimaryParticleIndex());

  // Scoring: Collect charged/neutral steps done in target without counting secondary tracks.
  if (track.Status() !=Geant::kNew) {
    if (charge == 0.0) {
      dataPerPrimary.AddNeutralStep();
      dataPerPrimary.AddNeutralTrackL(track.GetStep());
    } else {
      dataPerPrimary.AddChargedStep();
      dataPerPrimary.AddChargedTrackL(track.GetStep());
    }
    dataPerPrimary.AddEdep(track.Edep());
  }

  // Now collect secondary particle Stats
  if (track.Status() == Geant::kNew) {
    switch(pdgCode) {
      case   22: dataPerPrimary.AddGamma();    break;     // Gamma
      case   11: dataPerPrimary.AddElectron(); break;     // Electron
      case  -11: dataPerPrimary.AddPositron(); break;     // Positron
      case 2212: dataPerPrimary.AddProton();   break;     // Proton
      case 2112: dataPerPrimary.AddNeutron();  break;     // Neutron
    }
  }
}

void TARCapp::FinishEvent(Geant::GeantEvent *event) {
  if (fIsPerformance) return;

  // merge thread local data (filled in SteppingActions and distributed now in different threads) of transported event
  TARCThreadDataEvents *data = fRunMgr->GetTDManager()->MergeUserData(event->GetSlot(), *fDataHandlerEvents);

  // After merge write data into user defined global data structure
  // Since more than one thread can write to this global data structure, protect the object with a lock.
  TARCDataPerEvent &dataPerEvent = data->GetDataPerEvent(event->GetSlot());
  fMutex.lock();

  // Print event
  int nPrims = event->GetNprimaries();
  std::cout << "=====================================================================\n"
            << " *** FinishEvent --- event = " << event->GetEvent() << " with " << nPrims << " primary:\n";
  for (int ip=0; ip < nPrims; ++ip) {
    Geant::GeantTrack *primTrack     = event->GetPrimary(ip);
    int                primGVCode    = primTrack->GVcode();
    const std::string &primName      = geantphysics::Particle::GetParticleByInternalCode(primGVCode)->GetName();
    int                primTypeIndex = TARCPrimaryGenerator::GetPrimaryTypeIndex(primName);
    double             primEkin      = primTrack->T();
    double             xdir          = primTrack->Dx();
    double             ydir          = primTrack->Dy();
    double             zdir          = primTrack->Dz();
    fData->AddDataPerPrimaryType(dataPerEvent.GetDataPerPrimary(ip), primTypeIndex);
    std::cout << " Primary particle: " << ip       << " (type index = " << primTypeIndex << ")\n"
              << " Name      = "       << primName                                       << "\n"
              << " Energy    = "       <<primEkin / geant::MeV                           << " [MeV]\n"
              << " Direction = "       << xdir     << ", "  << ydir << ", " << zdir      << ")\n";
    dataPerEvent.GetDataPerPrimary(ip).Print();
  }
  fMutex.unlock();  // unlock the object
  data->Clear(event->GetSlot());     // clear currently added (Master) thread local data
  return;
}

void TARCapp::FinishRun(){
  TARCPrimaryGenerator::Print();   // print run conditions
  if (fIsPerformance) return;

  int numPrimTypes = fData->GetNumberOfPrimaryTypes();
  int numPrimaries = 0;
  for (int ipt = 0; ipt < numPrimaries; ++ipt){
    numPrimaries += fData->GetDataPerPrimaryType(ipt).GetNumPrimaries();
  }
  std::ios::fmtflags mode = std::cout.flags();
  int prec = std::cout.precision(2);
  std::cout << "\n =============================== Run Summary ========================================\n";
  std::cout << std::setprecision(4);
  std::cout << " Total number of primary = " << numPrimaries << std::endl;

  for (int ipt = 0; ipt < numPrimTypes; ++ipt) {
    const TARCDataPerPrimaryType& runData = fData->GetDataPerPrimaryType(ipt);
    int nPrimaries                        = runData.GetNumPrimaries();
    double norm                           = static_cast<double>(nPrimaries);
    norm = (norm > 0.0) ? 1.0 / norm : norm;
    std::string primName = TARCPrimaryGenerator::GetPrimaryName(ipt);
    double meanEdep            = norm * runData.GetEdep();
    double rmsEdep             = std::sqrt(std::abs(norm * runData.GetEdep2() - SQR(meanEdep)));
    double meanChargedTrackLen = norm * runData.GetChargedTrackL();
    double rmsChargedTrackLen  = std::sqrt(std::abs(norm * runData.GetChargedTrackL2() - SQR(meanChargedTrackLen)));
    double meanNeutralTrackLen = norm * runData.GetNeutralTrackL();
    double rmsNeutralTrackLen  = std::sqrt(std::abs(norm * runData.GetNeutralTrackL2() - SQR(meanNeutralTrackLen)));

    double meanNGamma          = norm * runData.GetGammas();
    double rmsNGamma           = std::sqrt(std::abs(norm * runData.GetGammas2() - SQR(meanNGamma)));
    double meanNElectron       = norm * runData.GetElectrons();
    double rmsNElectron        = std::sqrt(std::abs(norm * runData.GetElectrons2() - SQR(meanNElectron)));

    std::cout << " Number of Primaries = " << nPrimaries << "  " << primName << std::endl;
    std::cout << " Total energy deposited per primary = "       << meanEdep            << " +- " << rmsEdep            << " [MeV]" << std::endl;
    std::cout << " Total track length (charged) per primary = " << meanChargedTrackLen << " +- " << rmsChargedTrackLen << " [cm]"  << std::endl;
    std::cout << " Total track length (Neutral) per primary = " << meanNeutralTrackLen << " +- " << rmsNeutralTrackLen << " [cm]"  << std::endl;
    std::cout <<                                                                                                                      std::endl;
    std::cout << " Number of secondaries per primary: "                                                                            << std::endl;
    std::cout << "         Gammas = "                           << meanNGamma          << " +- " << rmsNGamma                      << std::endl;
    std::cout << "         Electrons = "                        << meanNElectron       << " +- " << rmsNElectron                   << std::endl;
  }
  std::cout.setf(mode, std::ios::floatfield);
  std::cout.precision(prec);
}


  void TARCapp::fixGun() {
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
