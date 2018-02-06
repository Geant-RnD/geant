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
#include "TARCapp.h"
#include "TARCData.h"
#include "TARCGeometryConstruction.h"
#include "TARCPrimaryGenerator.h"
#include "TARCPhysicsList.h"


// Parameters
// Detector based
std::string  parGeomFileName             = "./tarc_geometry.gdml";    // "tarc_geometry.root";
// primary generator/particle based
std::string  parGunPrimaryParticleName   = "";
int          parGunPrimaryPerEvent       = 0;
double       parGunPrimaryKE             = 0.0;
double       parGunPrimaryDir[3]         = {0.0, 0.0, 0.0};
double       parGunPosition[3]           = {0.0, 0.0, 0.0};
// run config based
int          parConfigNumBufferedEvt     = 4;    // Num of events to be transported on the same time (buffered)
int          parConfigNumRunEvt          = 10;   // Num of events to be transported during the run
int          parConfigNumPropagators     = 1;    // Number of propagators per working threads
int          parConfigNumThreads         = 4;    // Number of working threads;
int          parConfigNumTracksPerBasket = 16;   // default Num of tracks per Basket
int          parConfigIsPerformance      = 0;    // run without any user action
int          parConfigVectorizedGeom     = 0;    // activate geometry basketizing
int          parConfigExternalLoop       = 0;    // activate external loop mode


static struct option options[] = {
         {"gun-set-primary-energy"             , required_argument, 0, 'a'},
         {"gun-set-primary-type"               , required_argument, 0, 'b'},
         {"gun-set-primary-per-event"          , required_argument, 0, 'c'},
         {"gun-set-primary-direction"          , required_argument, 0, 'd'},
         {"det-set-gdml"                       , required_argument, 0, 'e'},
         {"config-number-of-buffered-events"   , required_argument, 0, 'm'},
         {"config-total-number-of-events"      , required_argument, 0, 'n'},
         {"config-number-of-threads"           , required_argument, 0, 'p'},
         {"config-number-of-propagators"       , required_argument, 0, 'q'},
         {"config-tracks-per-basket"           , required_argument, 0, 'r'},
         {"config-run-performance"             , required_argument, 0, 's'},
         {"config-vectorized-geom"             , required_argument, 0, 't'},
         {"config-external-loop"               , required_argument, 0, 'u'},
         {"gun-set-position"                   , required_argument, 0, 'x'},
         {"help", no_argument, 0, 'h'},
         {0, 0, 0, 0}
};

enum PRIMARYDIR_OPTIONS { PRIMARYDIR_X_OPT = 0, PRIMARYDIR_Y_OPT, PRIMARYDIR_Z_OPT};
enum POSITION_OPTIONS { POSITION_X_OPT = 0, POSITION_Y_OPT, POSITION_Z_OPT};
char *const primarydir_token[] = {
   [PRIMARYDIR_OPTIONS::PRIMARYDIR_X_OPT]   = (char* const)"x",
   [PRIMARYDIR_OPTIONS::PRIMARYDIR_Y_OPT]   = (char* const)"y",
   [PRIMARYDIR_OPTIONS::PRIMARYDIR_Z_OPT]   = (char* const)"z",
   NULL
};
char *const position_token[] = {
   [POSITION_OPTIONS::POSITION_X_OPT]   = (char* const)"x",
   [POSITION_OPTIONS::POSITION_Y_OPT]   = (char* const)"y",
   [POSITION_OPTIONS::POSITION_Z_OPT]   = (char* const)"z",
   NULL
};


void GetArguments          (int argc, char** argv);
void SetupDetectorConst    (tarcapp::TARCGeometryConstruction*  tarcGeom);
void SetupPrimaryGenerator (tarcapp::TARCPrimaryGenerator*      tarcGUN);
void SetupApplication      (tarcapp::TARCapp*                      app);

Geant::GeantRunManager* RunManager();

int main(int argc, char** argv) {
  GetArguments(argc, argv);   // read user arguments here

  Geant::GeantRunManager* runMgr = RunManager();

  // Here setup the TARC Geometry
  tarcapp::TARCGeometryConstruction *tarcGeom = new tarcapp::TARCGeometryConstruction(runMgr);
  SetupDetectorConst(tarcGeom);
  runMgr->SetDetectorConstruction(tarcGeom);

  // Here setup primary generator
  tarcapp::TARCPrimaryGenerator *primaryGenerator = new tarcapp::TARCPrimaryGenerator();
  SetupPrimaryGenerator(primaryGenerator);
  runMgr->SetPrimaryGenerator(primaryGenerator);

  // Here setup physics List
  geantphysics::PhysicsListManager::Instance().RegisterPhysicsList(new tarcapp::TARCPhysicsList());

  // Here setup TARC application
  //               tarcapp::TARCapp *app = new tarcapp::TARCapp(runManager, tarcGeom, primaryGenerator);
  tarcapp::TARCapp *app = new tarcapp::TARCapp(runMgr, primaryGenerator);
  SetupApplication(app);
  //Setuptarc(app);
  runMgr->SetUserApplication(app);
  tarcapp::TARCPrimaryGenerator::Print();

  // run the simulation
  if (parConfigExternalLoop) {
    userfw::Framework fw(parConfigNumPropagators*parConfigNumThreads, parConfigNumRunEvt, runMgr, runMgr->GetPrimaryGenerator());
    fw.Run();
  } else {
    runMgr->RunSimulation();   // delete the run manager at the end of the simulation
    delete runMgr;
  }


  runMgr->RunSimulation();
  delete runMgr;
  return 0;
}

static struct option options[] = {
         {"gun-set-primary-energy"             , required_argument, 0, 'a'},
         {"gun-set-primary-type"               , required_argument, 0, 'b'},
         {"gun-set-primary-per-event"          , required_argument, 0, 'c'},
         {"gun-set-primary-direction"          , required_argument, 0, 'd'},
         {"det-set-gdml"                       , required_argument, 0, 'e'},
         {"config-number-of-buffered-events"   , required_argument, 0, 'm'},
         {"config-total-number-of-events"      , required_argument, 0, 'n'},
         {"config-number-of-threads"           , required_argument, 0, 'p'},
         {"config-number-of-propagators"       , required_argument, 0, 'q'},
         {"config-tracks-per-basket"           , required_argument, 0, 'r'},
         {"config-run-performance"             , required_argument, 0, 's'},
         {"config-vectorized-geom"             , required_argument, 0, 't'},
         {"config-external-loop"               , required_argument, 0, 'u'},
         {"help", no_argument, 0, 'h'},
         {0, 0, 0, 0}
};

enum PRIMARYDIR_OPTIONS { PRIMARYDIR_X_OPT = 0, PRIMARYDIR_Y_OPT, PRIMARYDIR_Z_OPT};
char *const primarydir_token[] = {
   [PRIMARYDIR_OPTIONS::PRIMARYDIR_X_OPT]   = (char* const)"x",
   [PRIMARYDIR_OPTIONS::PRIMARYDIR_Y_OPT]   = (char* const)"y",
   [PRIMARYDIR_OPTIONS::PRIMARYDIR_Z_OPT]   = (char* const)"z",
   NULL
};

// Here define the functions used in this application

void help() {
  printf("\nUsage: runTARC [OPTIONS] INPUT_FILE\n\n");
  for (int i = 0; options[i].name != NULL; i++) {
    printf("\t-%c  --%s\t%s\n", options[i].val, options[i].name, options[i].has_arg ? options[i].name : "");
  }
  printf("\n\n");
}


void GetArguments(int argc, char** argv) {
  char *subopts, *value;
  int errfind = 0;
  while(true) {
    int optionindex = 0;
    int c;
    c = getopt_long (argc, argv, "", options, &optionindex);

    if (c == -1) exit(1);   // break;     // exit if no option.
    switch(c) {
      case   0:
              c = options[optionindex].val;
      case 'a':
              parGunPrimaryKE = strtod(optarg, NULL);
              if (parGunPrimaryKE <= 0.0)
                  errx(1, "Primary particle energy must be positive non zero.");
              break;
      case 'b':
              parGunPrimaryParticleName = optarg;
              break;
      case 'c':
              parGunPrimaryPerEvent     = (int)strtol(optarg, NULL, 10);
              break;
      case 'd':
              subopts = optarg;
              while (*subopts != '\0' && !errfind) {
                switch(getsubopt(&subopts, primarydir_token, &value)){
                  case PRIMARYDIR_OPTIONS::PRIMARYDIR_X_OPT:
                                                            parGunPrimaryDir[0] = strtod(value, NULL);
                                                            break;
                  case PRIMARYDIR_OPTIONS::PRIMARYDIR_Y_OPT:
                                                            parGunPrimaryDir[1] = strtod(value, NULL);
                                                            break;
                  case PRIMARYDIR_OPTIONS::PRIMARYDIR_Z_OPT:
                                                            parGunPrimaryDir[2] = strtod(value, NULL);
                                                            break;
                  default                                  :
                                                            fprintf(stderr, "No match found for token: [%s] in options.\n", value);
                                                            errfind = 1;
                                                            exit(0);
                                                            break;
                }
              }
              break;
      case 'e':
              parGeomFileName = optarg;
              break;
      case 'm':
              parConfigNumBufferedEvt = (int)strtol(optarg, NULL, 10);
              break;
      case 'n':
              parConfigNumRunEvt = (int)strtol(optarg, NULL, 10);
              break;
      case 'p':
              parConfigNumThreads = (int)strtol(optarg, NULL, 10);
              break;
      case 'q':
              parConfigNumPropagators = (int)strtol(optarg, NULL, 10);
              break;
      case 'r':
              parConfigNumTracksPerBasket = (int)strtol(optarg, NULL, 10);
              break;
      case 's':
              parConfigIsPerformance = (int)strtol(optarg, NULL, 10);
              break;
      case 't':
              parConfigVectorizedGeom = (int)strtol(optarg, NULL, 10);
              break;
      case 'u':
              parConfigExternalLoop = (int)strtol(optarg, NULL, 10);
              break;
      case 'x':
              subopts = optarg;
              while (*subopts != '\0' && !errfind) {
                switch(getsubopt(&subopts, position_token, &value)){
                  case POSITION_OPTIONS::POSITION_X_OPT:
                                                    parGunPosition[0] = strtod(value, NULL);
                                                    break;
                  case POSITION_OPTIONS::POSITION_Y_OPT:
                                                    parGunPosition[1] = strtod(value, NULL);
                                                    break;
                  case POSITION_OPTIONS::POSITION_Z_OPT:
                                                    parGunPosition[2] = strtod(value, NULL);
                                                    break;
                  default                              :
                                                    fprintf(stderr, "No match found for token: [%s] in options.\n", value);
                                                    errfind = 1;
                                                    exit(0);
                                                    break;
                }
              }
              break;
      case 'h':
              help();
              exit(0);
      default:
              help();
              errx(1, "Unknown option %c", c);
    }
  }
}


//  RunManager
Geant::GeantRunManager* RunManager() {
  Geant::GeantConfig*       runConfig   = new Geant::GeantConfig();
  Geant::GeantRunManager*   runManager  = new Geant::GeantRunManager(parConfigNumPropagators, parConfigNumThreads, runConfig);

  runManager->SetPhysicsInterface(new geantphysics::PhysicsProcessHandler());  // real physics interface object

  // Setting parameters for GeantConfig object
  runConfig->fNtotal            = parConfigNumRunEvt;
  runConfig->fNbuff             = parConfigNumBufferedEvt;
  runConfig->fUseV3             = true;                     //  THIS IS TRUE as Present GV uses V3
  runConfig->fNminThreshold     = 5 * parConfigNumThreads;  // Sets threshold for tracks to be used in same volume.
  runConfig->fNminReuse         = 100000;
  runConfig->fNperBasket        = parConfigNumTracksPerBasket;
  runConfig->fUseVectorizedGeom = parConfigVectorizedGeom; // Activate Vectorized Geometry.

  return runManager;
}

// Setup detector
void SetupDetectorConst(tarcapp::TARCGeometryConstruction* tarcGeom) {
  std::cout << parGeomFileName << std::endl;
  if (parGeomFileName != "")
    tarcGeom->SetGDMLFile(parGeomFileName);
  /*
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
  */
}

//Setup primary generator
void SetupPrimaryGenerator(tarcapp::TARCPrimaryGenerator* tarcGUN) {
  if (parGunPrimaryPerEvent > 0)
    tarcGUN->SetNumberOfPrimaryPerEvent(parGunPrimaryPerEvent);
  if (parGunPrimaryParticleName != "")
    tarcGUN->SetPrimaryName(parGunPrimaryParticleName);
  if (parGunPrimaryKE > 0.0)
    tarcGUN->SetPrimaryEnergy(parGunPrimaryKE);
  if ( (parGunPosition[0] || parGunPosition[1] || parGunPosition[2]))
    tarcGUN->SetGunPosition(parGunPosition);
  if ( (parGunPrimaryDir[0] || parGunPrimaryDir[1] || parGunPrimaryDir[2]))
    tarcGUN->SetPrimaryDirection(parGunPrimaryDir);
}

// Setup application
void SetupApplication(tarcapp::TARCapp *app) {
  if (parConfigIsPerformance)
    app->SetPerformanceMode(true);
}
