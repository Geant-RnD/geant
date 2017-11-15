
#include <err.h>
#include <getopt.h>
#include <iostream>
#include <unistd.h>

#include "GeantRunManager.h"
#include "TaskBroker.h"
#include "WorkloadManager.h"
#include "GeantPropagator.h"

// realphysics
#include "PhysicsProcessHandler.h"
#include "PhysicsListManager.h"
#include "MSCModel.h"

// application
#include "CaloApp.h"
#include "CaloDetectorConstruction.h"
#include "CaloPrimaryGenerator.h"
#include "CaloPhysicsList.h"


// some helper methods to get the possible input arguments and configure the user defined components of the application,
// set up the run manager and run the simulation.
void GetArguments(int argc, char *argv[]);
void SetupCaloPhysicsList     (userapplication::CaloPhysicsList* physlist);
void SetupUserDetector        (userapplication::CaloDetectorConstruction* detector);
void SetupUserPrimaryGenerator(userapplication::CaloPrimaryGenerator* primarygun, int numprimsperevt);
void SetupUserApplication     (userapplication::CaloApp* caloapp);
void PrintRunInfo(userapplication::CaloPrimaryGenerator *gun, Geant::GeantRunManager *rmg);
void PreSet(int num);
Geant::GeantRunManager* RunManager();

// The main application: gets the possible input arguments, sets up the run-manager, physics-list, detector, primary
//                       generator, application and starts the simulation.
int main(int argc, char *argv[]) {
  //
  // Read in user arguments
  PreSet(userapplication::CaloDetectorConstruction::GetMaxNumberOfAbsorbers());
  GetArguments(argc, argv);
  //
  // Create and configure run manager
  Geant::GeantRunManager* runMgr = RunManager();

  // Create user defined physics list
  userapplication::CaloPhysicsList *userPhysList = new userapplication::CaloPhysicsList("CaloPhysicsList");
  SetupCaloPhysicsList(userPhysList);
  geantphysics::PhysicsListManager::Instance().RegisterPhysicsList(userPhysList);

  // Create Calo detector construction
  userapplication::CaloDetectorConstruction *calo = new userapplication::CaloDetectorConstruction(runMgr);
  SetupUserDetector(calo);
  runMgr->SetDetectorConstruction(calo);

  // Create Calo primary generator
  userapplication::CaloPrimaryGenerator *caloGun = new userapplication::CaloPrimaryGenerator(calo);
  SetupUserPrimaryGenerator(caloGun,runMgr->GetConfig()->fNaverage);
  runMgr->SetPrimaryGenerator(caloGun);

  // Create the real physics Calo application
  userapplication::CaloApp *caloApplication = new userapplication::CaloApp(runMgr,calo,caloGun);
  SetupUserApplication(caloApplication);
  runMgr->SetUserApplication(caloApplication);

  // Print basic parameters for the simulation
  PrintRunInfo(caloGun, runMgr);

  // Run the simulation
  runMgr->RunSimulation();

  // Delete the run manager
  delete runMgr;
  return 0;
}


//
// Optional input arguments that make possible the configuration of detector(parDet), primary generator(parGun), the
// application(parApp), run configuration(parConfig) and some physics processes(parProcess):
//
// detector parameters
int                      parDetNumberOfAbsorbers  =  0;   // i.e. default application value
int                      parDetNumberOfLayers     =  0;   // i.e. default application value
double                   parDetProductionCuts     =  0.;  // i.e. default application value
double                   parDetSizeYZ             =  0.;  // i.e. default application value
std::vector<double>      parDetAbsThicknesses;            // i.e. default application values
std::vector<std::string> parDetAbsMaterials;              // i.e. default application values
//
// primary generator parameters (primary particle gun)
std::string parGunPrimaryParticleName             =  "";  // i.e. default application value
double      parGunPrimaryKinEnergy                =  0.;  // i.e. default application value
//
// run configuration parameters
int         parConfigNumBufferedEvt   =     4;  // number of events taken to be transported on the same time (buffered)
int         parConfigNumRunEvt        =    10;  // total number of events to be transported during the run
int         parConfigNumPrimaryPerEvt =  1000;  // number of primary particles per event
int         parConfigNumThreads       =     4;  // number of working threads
int         parConfigNumPropagators   =     1;  // number of propagators per working threads
bool        parConfigIsPerformance    = false;  // run without any user actions
//
// physics process configuration parameters:
std::string parProcessMSCStepLimit    = "";    // i.e. default application value


void PreSet(int num) {
  parDetAbsThicknesses.resize(num,0.);
  parDetAbsMaterials.resize(num,"");
}


static struct option options[] = {
         {"det-number-of-absorbers"            , required_argument, 0, 'a'},
         {"det-number-of-layers"               , required_argument, 0, 'b'},
         {"det-set-absorber"                   , required_argument, 0, 'c'},
         {"det-set-sizeYZ"                     , required_argument, 0, 'd'},
         {"det-prod-cut-length"                , required_argument, 0, 'e'},

         {"gun-primary-energy"                 , required_argument, 0, 'f'},
				 {"gun-primary-type"                   , required_argument, 0, 'g'},

         {"config-number-of-buffered-events"    , required_argument, 0,  'm'},
         {"config-total-number-of-events"       , required_argument, 0,  'n'},
         {"config-number-of-primary-per-events" , required_argument, 0,  'o'},
         {"config-number-of-threads"            , required_argument, 0,  'p'},
         {"config-number-of-propagators"        , required_argument, 0,  'q'},
         {"config-run-performance"              , no_argument      , 0,  'r'},

				 {"particle-process-MSC-step-limit"    , required_argument, 0, 'A'},

         {"help", no_argument, 0, 'h'},
         {0, 0, 0, 0}
};

enum ABS_OPTIONS { ABS_INDEX_OPT = 0, ABS_MATNAME_OPT, ABS_THICK_OPT};
char *const abs_token[] = {
   [ABS_OPTIONS::ABS_INDEX_OPT]     = (char* const)"absorber-index",
   [ABS_OPTIONS::ABS_MATNAME_OPT]   = (char* const)"material-name",
   [ABS_OPTIONS::ABS_THICK_OPT]     = (char* const)"thickness",
   NULL
};



void help() {
  printf("\nUsage: caloApp [OPTIONS] INPUT_FILE\n\n");
  for (int i = 0; options[i].name != NULL; i++) {
    printf("\t-%c  --%s\t%s\n", options[i].val, options[i].name, options[i].has_arg ? options[i].name : "");
  }
  printf("\n\n");
}

void PrintRunInfo(userapplication::CaloPrimaryGenerator *gun, Geant::GeantRunManager *rmg) {
  // Print run information
  long int nevents    = rmg->GetConfig()->fNtotal;
  long int nprimpere  = rmg->GetConfig()->fNaverage;
  long int nprimtotal = nevents*nprimpere;
  std::cout<< "\n\n"
           << " ===================================================================================  \n"
           << "  primary              : " << gun->GetPrimaryParticleName()                << "       \n"
           << "  primary energy       : " << gun->GetPrimaryParticleEnergy()              << " [GeV] \n"
           << "  #events              : " << nevents                                      << "       \n"
           << "  #primaries per event : " << nprimpere                                    << "       \n"
           << "  total # primaries    : " << nprimtotal                                   << "       \n"
           << "  performance mode?    : " << parConfigIsPerformance                       << "       \n"
           << " ===================================================================================\n\n";
}

void GetArguments(int argc, char *argv[]) {
  std::cout << "Avoid ctest truncation of output: CTEST_FULL_OUTPUT" << std::endl;
  // for the set-absorber sub-options
  int    errfnd        =  0;
  int    absorberIndex = -1;
  char  *subopts;
  char  *value;

  while (true) {
    int c, optidx=0;
    //
    c = getopt_long(argc, argv, "", options, &optidx);
    //
    if (c==-1)
      break;
    //
    switch (c) {
      case 0:
        c = options[optidx].val; /* fall through */
      //---- Primary generator
      case 'f':
        parGunPrimaryKinEnergy = (double)strtod(optarg, NULL);
        if (parGunPrimaryKinEnergy<=0)
          errx(1, "primary particle energy must be positive");
        break;
      case 'g':
        parGunPrimaryParticleName = optarg;
        break;
      //---- Detector
      case 'a':
        parDetNumberOfAbsorbers = (int)strtol(optarg, NULL, 10);
    	  break;
      case 'b':
        parDetNumberOfLayers    = (int)strtol(optarg, NULL, 10);
        break;
      case 'c':
        subopts = optarg;
        while (*subopts!='\0' && !errfnd) {
           switch (getsubopt(&subopts, abs_token, &value)) {
           case ABS_OPTIONS::ABS_INDEX_OPT:
             absorberIndex = (int)strtol(value, NULL, 10);
             break;
           case ABS_OPTIONS::ABS_MATNAME_OPT:
             parDetAbsMaterials[absorberIndex] = value;
             break;
           case ABS_OPTIONS::ABS_THICK_OPT:
             parDetAbsThicknesses[absorberIndex] = strtod(value, NULL);
             break;
           default:
             std::cerr<< " No match found for token: [" << value << "] among ABS_OPTIONS" << std::endl;
             errfnd = 1;
             break;
           }
        }
        break;
      case 'd':
        parDetSizeYZ = (double)strtod(optarg, NULL);
        break;
      case 'e':
        parDetProductionCuts = (double)strtod(optarg, NULL);
        break;
       //---- Run configuration
       case 'm':
         parConfigNumBufferedEvt   = (double)strtod(optarg, NULL);
         break;
       case 'n':
         parConfigNumRunEvt        = (double)strtod(optarg, NULL);
         break;
       case 'o':
         parConfigNumPrimaryPerEvt = (double)strtod(optarg, NULL);
         break;
       case 'p':
         parConfigNumThreads       = (double)strtod(optarg, NULL);
         break;
       case 'q':
         parConfigNumPropagators   = (double)strtod(optarg, NULL);
         break;
       case 'r':
         parConfigIsPerformance    = true;
         break;
       //---- Physics
       case 'A':
         parProcessMSCStepLimit = optarg;
         break;
      //---- Help
      case 'h':
        help();
        exit(0);
        break;
      default:
        help();
        errx(1, "unknown option %c", c);
    }
  }
}


Geant::GeantRunManager* RunManager() {
  // create the GeantConfiguration object and the GeantRunManager object
  Geant::GeantConfig*     runConfig  = new Geant::GeantConfig();
  Geant::GeantRunManager* runManager = new Geant::GeantRunManager(parConfigNumPropagators, parConfigNumThreads, runConfig);
  // create the real physics main manager/interface object and set it in the GeantRunManager
  runManager->SetPhysicsInterface(new geantphysics::PhysicsProcessHandler());
  //
  // Set parameters of the GeantConfig object:
  runConfig->fNtotal        = parConfigNumRunEvt;
  runConfig->fNbuff         = parConfigNumBufferedEvt;
  runConfig->fNaverage      = parConfigNumPrimaryPerEvt;
  //
  // Some additional parameters that have values in this application different than their default
  //
  // this should be true by default from now on since we use only V3
  runConfig->fUseV3         = true;
  runConfig->fNminThreshold = 5*parConfigNumThreads;
  // Set threshold for tracks to be reused in the same volume
  runConfig->fNminReuse     = 100000;
  //
  // Activate standard scoring
  runConfig->fUseStdScoring = !parConfigIsPerformance;

  return runManager;
}


void SetupUserDetector(userapplication::CaloDetectorConstruction* det) {
  if (parDetNumberOfAbsorbers>0) {  det->SetNumberOfAbsorbersPerLayer(parDetNumberOfAbsorbers); }
  if (parDetNumberOfLayers   >0) {  det->SetNumberOfLayers(parDetNumberOfLayers);               }
  if (parDetProductionCuts   >0) {  det->SetProductionCut(parDetProductionCuts);                }
  if (parDetSizeYZ           >0) {  det->SetSizeYZ(parDetSizeYZ);                               }
  for (int i=0;i<parDetNumberOfAbsorbers; ++i) {
    std::string matName = parDetAbsMaterials[i];
    double      thick   = parDetAbsThicknesses[i];
    if (thick>0) {
      det->SetAbsorberThickness(i,thick);
    }
    if (matName>0) {
      det->SetAbsorberMaterialName(i,matName);
    }
  }
  det->DetectorInfo();
}


void SetupUserPrimaryGenerator(userapplication::CaloPrimaryGenerator* primarygun, int numprimsperevt) {
  // it needs to be consistent with GeantConfig::fNaverage i.e. number of primary particles per event !!!
  primarygun->SetNumberOfPrimaryParticlePerEvent(numprimsperevt);
  if (parGunPrimaryParticleName!="")
    primarygun->SetPrimaryParticleName(parGunPrimaryParticleName);
  if (parGunPrimaryKinEnergy>0.)
    primarygun->SetPrimaryParticleEnergy(parGunPrimaryKinEnergy);
}


void SetupCaloPhysicsList(userapplication::CaloPhysicsList *userPhysList){
  if (parProcessMSCStepLimit!="") {
    if (parProcessMSCStepLimit=="UseSafety") {
      userPhysList->SetMSCStepLimit(geantphysics::MSCSteppingAlgorithm::kUseSaftey);
    } else if (parProcessMSCStepLimit=="ErrorFree") {
      userPhysList->SetMSCStepLimit(geantphysics::MSCSteppingAlgorithm::kErrorFree);
    } else if (parProcessMSCStepLimit=="UseDistanceToBoundary") {
      userPhysList->SetMSCStepLimit(geantphysics::MSCSteppingAlgorithm::kUseDistanceToBoundary);
    } else {
      std::cerr<< " **** ERROR caloAppRP::SetupCaloPhysicsList() \n"
               << "   unknown MSC stepping algorithm = " << parProcessMSCStepLimit
               << std::endl;
      exit(-1);
    }
  }
}

void SetupUserApplication(userapplication::CaloApp *caloapp) {
  if (parConfigIsPerformance) {
    caloapp->SetPerformanceMode(true);
  }
}
