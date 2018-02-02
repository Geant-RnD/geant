/****************************************************************
 *  @file: runTARC.cc
 *  @author: Abhijit Bhattacharyya
 *  @brief: This file is created to run simulation for TARC.
 *          The geometry file has been provided by Alexander Howard.
 ****************************************************************/

#include <err.h>
#include <getopt.h>
#include <iostream>
#include <unistd.h>

#ifdef USE_ROOT
#include "Rtypes.h"
#include "TGeoManager.h"
#endif

#ifdef USE_ROOT
class TGeoManager;
class TGeoMaterial;
#endif

// GeantV include
#include "GeantVDetectorConstruction.h"
#include "GeantRunManager.h"
#include "ExternalFramework.h"

// real-physics includes
#include "PhysicsProcessHandler.h"
#include "PhysicsListManager.h"
#include "MSCModel.h"

// application includes
#include "TARC.h"
#include "TARCGeometryConstruction.h"
#include "TARCPrimaryGenerator.h"
#include "TARCPhysicsList.h"


// Parameters
// Detector based
std::string  GeomFileName                = "tarc_geometry.root";
// primary generator/particle based
std::string  parGunPrimaryParticleName   = "";
int          ParGunPrimaryPerEvent       = 0;
double       ParGunPrimaryKE             = 0.0;
double       ParGunPrimaryDir[3]         = {0.0, 0.0, 0.0};
// run config based
int          parConfigNumBufferedEvt     = 4;    // Num of events to be transported on the same time (buffered)
int          parConfigNumRunEvt          = 10;   // Num of events to be transported during the run
int          parConfigNumPropagators     = 1;    // Number of propagators per working threads
int          parConfigNumThreads         = 4;    // Number of working threads;
int          parConfigNumTracksPerBasket = 16;   // default Num of tracks per Basket
int          parConfigIsPerformance      = 0;    // run without any user action
int          parConfigVectorizedGeom     = 0;    // activate geometry basketizing
int          parConfigExternalLoop       = 0;    // activate external loop mode


void GetArguments(int argc, char **argv);
void SetupUserDetector         (tarc::TARCGeometryConstruction*  tarcGeom);
void SetupUserPrimaryGenerator (tarc::TARCPrimaryGenerator*      tarcGUN);
void SetupUserPhysicsList      (tarc::TARCPhysicsList*           tarcPhys);
void Setuptarc                 (tarc::TARC*                      app);

Geant::GeantRunManager* RunManager();

int main(int argc, char** argv) {
  Geant::GeantRunManager *runManager = RunManager();

  // Here setup the TARC Geometry
  tarc::TARCGeometryConstruction *tarcGeom = new tarc::TARCGeometryConstruction(runManager);
  SetupUserDetector(tarcGeom);
  runManager->SetDetectorConstruction(tarcGeom);

  // Here setup primary generator
  tarc::TARCPrimaryGenerator *primaryGenerator = new tarc::TARCPrimaryGenerator();
  SetupUserPrimaryGenerator(primaryGenerator);
  runManager->SetPrimaryGenerator(primaryGenerator);

  // Here setup physics List
  tarc::TARCPhysicsList *tarcPhysList       = new tarc::TARCPhysicsList("test");
  SetupUserPhysicsList(tarcPhysList);
  geantphysics::PhysicsListManager::Instance().RegisterPhysicsList(tarcPhysList);

  // Here setup TARC application
  tarc::TARC *tarcApplication = new tarc::TARC(runManager, tarcGeom, primaryGenerator);
  Setuptarc(tarcApplication);
  runManager->Settarc(tarcApplication);

  // run the simulation
  runManager->RunSimulation();

  // delete the run manager at the end of the simulation
  delete runManager;
  return 0;
}

// Here define the functions used in this application

//  RunManager
Geant::GeantRunManager* RunManager() {
  Geant::GeantConfig*       runConfig   = new Geant::GeantConfig();
  Geant::GeantRunManager*   runManager  = new Geant::GeantRunManager(
    parConfigNumPropagators, parConfigNumThreads, runConfig
  );
  runManager->SetPhysicsInterface(new geantphysics::PhysicsProcessHandler());
  return runManager;
}

// Setup detector
void SetupUserDetector(tarc::TARCGeometryConstruction* tarcGeom) {
  Geant::GeantVDetectorConstruction *gvDetNew;
  std::string rootGeomFilePath = "";
  if (std::getenv("TARCGDMLPATH") == "" ) {
  	rootGeomFilePath = std::getenv("pwd");
  } else {
  	rootGeomFilePath = std::getenv("TARCGDMLPATH");
  }
  rootGeomFilePath += "/" + GeomFileName;
  const char* rootGeomFile = rootGeomFilePath.c_str();
  std::cout << rootGeomFile << std::endl;

  // bool wasLoaded = GeantVDetectorConstruction::LoadGeometry(GeomFileName);
  bool wasLoaded = gvDetNew->LoadGeometry(rootGeomFile);
  if (!wasLoaded){
    std::cout << "Error in loading the GDML file." << std::endl;
    return;
  } else {
    std::cout << GeomFileName << " loaded." <<std::endl;
  }

  std::cout << " Visited SetupUserDetector function." << std::endl;
}

//Setup primary generator
void SetupUserPrimaryGenerator(tarc::TARCPrimaryGenerator* tarcGUN) {
  std::cout << " Inside primary Generator setting." << std::endl;
}

// Setup Physics List
void SetupUserPhysicsList(tarc::TARCPhysicsList* tarcPhys) {
  std::cout << " Visited Phys List." << std::endl;
}

// Setup application
void Setuptarc(tarc::TARC* app) {
  std::cout << " Visited Setuptarc function." << std::endl;
}
