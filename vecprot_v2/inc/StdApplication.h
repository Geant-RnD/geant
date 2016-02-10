//===--- StdApplication.h - Geant-V ------------------------------*- C++ -*-===//
//
//                     Geant-V Prototype               
//
//===----------------------------------------------------------------------===//
/**
 * @file StdApplication.h
 * @brief Implementation of simple scoring for CMS geometry 
 * @author Andrei Gheata 
 */
//===----------------------------------------------------------------------===//
#ifndef GEANT_StdApplication
#define GEANT_StdApplication

#include <map>
#include <mutex>
#ifndef GEANT_VAPPLICATION
#include "GeantVApplication.h"
#endif

#ifndef GEANTV_MIC
class TH1F;
class TH1D;
class TProfile;
#endif
#include "GeantFwd.h"

/** @brief StdApplication class */
class StdApplication : public GeantVApplication {
public:
enum EScoreType {
  kNoScore = 0,
  kScore
};  

private:
  bool fInitialized;                            /** Initialized flag */
 #ifndef GEANTV_MIC
  TH1F     *fHeta;                                /** Eta distribution */
  TH1F     *fHpt;                                 /** Pt distribution */
  TH1D     *fHStep;                               /** Step size distribution */
  TProfile *fStepSize;                            /** Step size profile with eta */
  TProfile *fStepCnt;                             /** Number of steps profile with eta */
#endif
  std::mutex fMHist;                              /** Mutex for concurrent histogram filling */
  EScoreType fScore;                              /** Entity for scoring */
  
  /**
   * @brief Copy constructor
   */
  StdApplication(const StdApplication &);

  /**
   * @brief Operator=
   */
  StdApplication &operator=(const StdApplication &);
public:

  /** @brief Constructor StdApplication */
  StdApplication();

  /** @brief Destructor StdApplication */
  virtual ~StdApplication() {}

  /** @brief Initialization function */
  virtual bool Initialize();

  /** @brief Create and fill a uniform log scale bin limits array to pass to TH1D 
   * @param nbins Number of bins
   * @param lmin Low axis limit (positive)
   * @param lmax High axis limit (greater than lmin)
   */
  static double *MakeUniformLogArray(int nbins, double lmin, double lmax);

  /** @brief Set scoring type */
  void SetScoreType(EScoreType type) { fScore = type; }
  
  /**
   * @brief Callback function for user scoring 
   * 
   * @param tid Thread id.
   * @param npart Number of tracks
   * @param tracks GeantV track container
   */
  virtual void StepManager(int npart, const GeantTrack_v &tracks, GeantTaskData *td);

  /**
   * @brief Function of digitization
   * 
   * @param event Event that should be digitized
   */
  virtual void Digitize(int event);

  /** @brief User FinishRun function */
  virtual void FinishRun();
#ifndef GEANTV_MIC
  ClassDef(StdApplication, 1) // Standard scoring application
#endif
};
#endif
