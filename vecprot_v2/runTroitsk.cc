/****************************************************************
 *  @file: runTroitsk.cc
 *  @author: Abhijit Bhattacharyya
 *  @brief: This file is created to run simulation for Troitsk.
 *          The geometry file has been provided by Vladimir Grichine
 ****************************************************************/

#include <err.h>
#include <getopt.h>
#include <iostream>
#include <unistd.h>

// GeantV include
#include "GeantRunManager.h"

// real-physics includes
#include "PhysicsProcessHandler.h"
#include "PhysicsListManager.h"
#include "MSCModel.h"

// application includes
#include "Troitsk.h"
#include "TroitskGeometryConstruction.h"
#include "TroitskPrimaryGenerator.h"

//#include "UserPhysicsList.h"

// Parameters
int parConfigNumPropagators    = 1;    // Number of propagators per working threads
int parConfigNumThreads        = 4;    // Number of working threads;

void SetupUserDetector             (userapplication::TroitskGeometryConstruction*     troitskGeom);
void SetupUserPrimaryGenerator     (userapplication::TroitskPrimaryGenerator*         troitskGUN);
void SetupUserApplication          (userapplication::Troitsk*                              app);
Geant::GeantRunManager* RunManager();

int main(int argc, char* argv[]) {

  Geant::GeantRunManager *runManager = RunManager();

  // Here setup the Troitsk Geometry
  userapplication::TroitskGeometryConstruction *troitskGeom = new userapplication::TroitskGeometryConstruction(runManager);
  SetupUserDetector(troitskGeom);
  runManager->SetDetectorConstruction(troitskGeom);

  // Here setup primary generator
  userapplication::TroitskPrimaryGenerator *primGenerator = new userapplication::TroitskPrimaryGenerator(troitskGeom);
  SetupUserPrimaryGenerator(primGenerator);
  runManager->SetPrimaryGenerator(primGenerator);

  // Here setup Troitsk application
  userapplication::Troitsk *troitskApplication = new userapplication::Troitsk(runManager, troitskGeom, primGenerator);
  SetupUserApplication(troitskApplication);
  runManager->SetUserApplication(troitskApplication);

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
void SetupUserDetector(userapplication::TroitskGeometryConstruction* troitskGeom) {
  std::cout << " Visited SetupUserDetector function." << std::endl;
}

//Setup primary generator
void SetupUserPrimaryGenerator(userapplication::TroitskPrimaryGenerator* troitskGUN) {
  std::cout << " Inside primary Generator setting." << std::endl;
}

// Setup application
void SetupUserApplication(userapplication::Troitsk* app) {
  std::cout << " Visited SetupUserApplication function." << std::endl;
}
