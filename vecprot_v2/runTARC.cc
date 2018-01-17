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

// real-physics includes
#include "PhysicsProcessHandler.h"
#include "PhysicsListManager.h"
#include "MSCModel.h"

// application includes
#include "TARC.h"
#include "TARCGeometryConstruction.h"
#include "TARCPrimaryGenerator.h"
#include "TARCPhysicsList.h"

//#include "UserPhysicsList.h"

// Parameters
int parConfigNumPropagators    = 1;    // Number of propagators per working threads
int parConfigNumThreads        = 4;    // Number of working threads;

void SetupUserDetector             (userapplication::TARCGeometryConstruction*     tarcGeom);
void SetupUserPrimaryGenerator     (userapplication::TARCPrimaryGenerator*          tarcGUN);
void SetupUserPhysicsList          (userapplication::TARCPhysicsList*              tarcPhys);
void SetupUserApplication          (userapplication::TARC*                              app);

Geant::GeantRunManager* RunManager();

int main(int argc, char** argv) {
  Geant::GeantRunManager *runManager = RunManager();

  // Here setup the TARC Geometry
  userapplication::TARCGeometryConstruction *tarcGeom = new userapplication::TARCGeometryConstruction(runManager);
  SetupUserDetector(tarcGeom);
  runManager->SetDetectorConstruction(tarcGeom);

  // Here setup primary generator
  userapplication::TARCPrimaryGenerator *primGenerator = new userapplication::TARCPrimaryGenerator(tarcGeom);
  SetupUserPrimaryGenerator(primGenerator);
  runManager->SetPrimaryGenerator(primGenerator);

  // Here setup physics List
  userapplication::TARCPhysicsList *tarcPhysList       = new userapplication::TARCPhysicsList("test");
  SetupUserPhysicsList(tarcPhysList);
  geantphysics::PhysicsListManager::Instance().RegisterPhysicsList(tarcPhysList);

  // Here setup TARC application
  userapplication::TARC *tarcApplication = new userapplication::TARC(runManager, tarcGeom, primGenerator);
  SetupUserApplication(tarcApplication);
  runManager->SetUserApplication(tarcApplication);

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
void SetupUserDetector(userapplication::TARCGeometryConstruction* tarcGeom) {
  std::string GeomFileName = "tarc_geometry.root";
  std::string rootGeomFilePath = std::getenv("TARCGDMLPATH");
  rootGeomFilePath += "/" + GeomFileName;
  const char* rootGeomFile = rootGeomFilePath.c_str();
  // bool wasLoaded = GeantVDetectorConstruction::LoadGeometry(GeomFileName);
  bool wasLoaded = Geant::GeantVDetectorConstruction::LoadGeometry(rootGeomFile);
  if (!wasLoaded){
    std::cout << "Error in loading the GDML file." << std::endl;
    return;
  } else {
    std::cout << GeomFileName << " loaded." <<std::endl;
  }

  std::cout << " Visited SetupUserDetector function." << std::endl;
}

//Setup primary generator
void SetupUserPrimaryGenerator(userapplication::TARCPrimaryGenerator* tarcGUN) {
  std::cout << " Inside primary Generator setting." << std::endl;
}

// Setup Physics List
void SetupUserPhysicsList(userapplication::TARCPhysicsList* tarcPhys) {
  std::cout << " Visited Phys List." << std::endl;
}

// Setup application
void SetupUserApplication(userapplication::TARC* app) {
  std::cout << " Visited SetupUserApplication function." << std::endl;
}
