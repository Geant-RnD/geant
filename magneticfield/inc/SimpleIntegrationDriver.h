//
// Driver for explicit Runge-Kutta methods
//
// Class description:
//
// Provides a driver that talks to the Integrator Stepper, and ensures
//  that the errors are within acceptable bounds.
// When multiple tracks are integrated, provide different ways to 
//   handle the early end of some 'lanes' - while others continue.
//
// History:
//
// Adaptations of template interface: J. Apostolakis,     Nov 2017
// First templated version:  Ananya, Feb/March 2016
//     ( commit 95e1316bcc156a04c876d6ea0fc9e60a15eeac4f )
// 
// Adapted from G4MagInt_Drv class of Geant4 (G4MagIntegratorDriver)
//
// - Contributors: Ananya, J.Apostolakis                    2015-2017
// --------------------------------------------------------------------

#ifndef SimpleIntegrationDriver_Def
#define SimpleIntegrationDriver_Def

// #include "TemplateFieldTrack.h"
#include "base/AlignedBase.h"
#include "FieldTrack.h"

// #include "TemplateGUVIntegrationStepper.h"
// #include "IntegrationStepper.h"

#include "base/Vector.h"

// Adding because adding scalar stepper for new constructor (KeepStepping)
#include "GUVIntegrationStepper.h"

// Adding to send in scalar driver to deal with 1/2 remaining lanes
#include "ScalarIntegrationDriver.h"
// #include "ScalarFieldTrack.h"

// Eventual config: 
// template <class T_Stepper, unsigned int Nvar>

template <class Real_v, class T_Stepper, unsigned int Nvar>
   class SimpleIntegrationDriver : public vecgeom::AlignedBase
{

  public:

    template <typename T>
       using Vector3D = vecgeom::Vector3D<T>;

    // using Real_v          = Backend;             // First adaptation
    // using Double_v        = Geant::Double_v;
    using Bool_v          = vecCore::Mask_v<Real_v>;
    using ThreeVectorSimd = Vector3D<Real_v>;
  
    SimpleIntegrationDriver( double     hminimum,  //same 
                             T_Stepper *pStepper,
                             int        numberOfComponents  = 6,
                             int        statsVerbosity = 1     );

    SimpleIntegrationDriver( const SimpleIntegrationDriver& );
    // Copy constructor used to create Clone method
    ~SimpleIntegrationDriver();
 
       // Above drivers for integrator (Runge-Kutta) with stepsize control. 
       // Integrates ODE starting values yInput 
       // from current s (s=s0) to s=s0+h with accuracy eps. 
       // On output yOutput is value at end of interval.
       // The concept is similar to the odeint routine from NRC 2nd edition p.721

    // 2nd AccurateAdvance
    //   Involves track insertion etc 
    // succeeded[] of length nTracks
    void AccurateAdvance( const  FieldTrack  yInput[],
                                 double      hstep[],
                          const  double      charge[],                            
                                 double      epsilon,
                                 FieldTrack  yOutput[],
                                 int         nTracks, 
                                 bool        succeeded[] );

/******    
    Bool_v  QuickAdvance( TemplateFieldTrack<Real_v>& y_posvel,        // INOUT
                          const  Real_v      dydx[],  
                                 Real_v      hstep,           // IN
                           // Real_v&     dchord_step, //take out
                                 Real_v&     dyerr_pos_sq,
                                 Real_v&     dyerr_mom_rel_sq ) ;
       // New QuickAdvance that also just tries one Step
       //    (so also does not ensure accuracy)
       //    but does return the errors in  position and
       //        momentum (normalised: Delta_Integration(p^2)/(p^2) )
  *******/

    // SimpleIntegrationDriver* Clone() const;
       // Create an independent copy of the current object -- including independent 'owned' objects
       // NOTE: Evaluate whether this method is needed - 2017.11.09
     
    // VEquationOfMotion<Real_v>* GetEquationOfMotion() { return fpStepper->GetEquationOfMotion(); }
    // const VEquationOfMotion<Real_v>* GetEquationOfMotion() const { return fpStepper->GetEquationOfMotion(); } 
     
    // Auxiliary methods
    inline double GetHmin()        const { return fMinimumStep; } 
    inline double GetSafety()      const { return fSafetyFactor; }
    inline double GetPowerShrink() const { return fPowerShrink; }
    inline double GetPowerGrow()   const { return fPowerGrow; } 
    inline double GetErrcon()      const { return fErrcon; }

    inline int    GetMaxNoSteps()  const { return fMaxNoSteps; }

/*****    
    inline void   GetDerivatives( const TemplateFieldTrack<Real_v> &y_curr,     // const, INput
                                        Real_v    charge, 
                                        Real_v    dydx[]   );  //       OUTput
 ******/
    inline const T_Stepper* GetStepper() const { return fpStepper; }
    inline       T_Stepper* GetStepper()       { return fpStepper; } 
        // Accessors.

    // Setting parameters ( few now )
    void SetMaxNoSteps(int val)   { fMaxNoSteps= val; }

     // Compute dependent parameters
     inline void ComputeAndSetErrcon();

     // Check
     void CheckParameters();      // Sanity check of 
     
#ifdef QUICK_ADV_ARRAY_IN_AND_OUT      
     Bool_v QuickAdvance(      Real_v     yarrin[],     // In
                         const Real_v     dydx[],  
                               Real_v     hstep,        
                               Real_v     yarrout[],    // Out
                               Real_v&    dchord_step,  // Out
                               Real_v&    dyerr );      // in length
#endif

     void ReportConditionLanes( vecCore::Mask_v<Real_v> problemLane,
                                Real_v x,
                                Real_v xnew,
                                Real_v h,
                                Real_v htry );

protected:
     // Implementation methods 
     void  OneGoodStep(const Real_v  yStart[], //  [N]
                       const Real_v  dydx[],   //  [N]
                       const Real_v  charge,                          
                             Real_v& x,        // InOut                    
                             Real_v  htry,
                             Real_v  epsilon,  // required relative accuracy
                             Real_v  yEnd[],   // [N]
                             Real_v& hdid,
                             Real_v& hnext ) ;
      // Integrate over the length htry 'together', until each lane succeeds
      //   if not for the initial lenght 'htry', then for a reduced size step
      //    ( reduced in order to likely succeed. )
      // In this version each lane stops as soon with its first success.

     Real_v ComputeNewStepSize( Real_v  errMaxNorm,    // normalised error
                                Real_v  hstepCurrent); // current step size
        // Taking the last step's normalised error, calculate
        // a step size for the next step.
        // Do not limit the next step's size within a factor of the
        // current one.

     Real_v ComputeNewStepSize_WithinLimits( Real_v  errMaxNorm,    // normalised error
                                             Real_v  hstepCurrent); // current step size
        // Taking the last step's normalised error, calculate
        // a step size for the next step.
        // Limit the next step's size within a range around the current one.
     
     int  InitializeLanes( const  FieldTrack  yInput[],
                           const  double      hstep [],
                       //  const  double      xStart [],
                           const  double      charge [],                                                  
                                  int         nTracks,         
                                  bool        badStepSize[], // Output - redundant
                                  Real_v      y[],
                                  Real_v    & hStepLane,
                                  Real_v    & chargeLane,                                                  
                                  Real_v    & startCurveLength,
                                  int       & numBadSize  );
      // Return number of filled lanes

  public: // For now
     bool TestInitializeLanes(); // (int numTracks)
      // Simple, unit-test like check.  Returns ok or not
     void CreateInput( FieldTrack  yInput[], int nTracks );
     bool CheckOutput( Real_v     yOutput[],                        
                       int         lane,     int         initialSlot,
                       std::string testName, std::string varName );
       // returns 'true' if ok

     void ReportInvalidStepInputs(double hStep[], int    nTracks);
     
private:

     SimpleIntegrationDriver& operator=(const SimpleIntegrationDriver&);
     // Private copy constructor and assignment operator.

     void StoreFinalValues( const                  Real_v & yWork, 
                            const                  Real_v & hValue,
                            const                  Real_v & epsSqVal,
                            const vecCore::Mask_v<Real_v> & storeFlag,                     
                                                   Real_v & yFinal,     
                                                   Real_v & hFinalStore,  
                                                   Real_v & epsSqStore );
     // Auxiliary method, to store results of some 'lanes'
     
private:

     // ---------------------------------------------------------------
     // DEPENDENT Objects
     T_Stepper *fpStepper;

     // ---------------------------------------------------------------     
     // Compilation constants 
     const bool partDebug= true;  // Enforce debugging output
     const  int ncompSVEC= FieldTrack::NumCompFT; // expect 6, later 8, eventually up to 12
     const bool useOneStep  = true; //  Algorithm selection - false for KeepStepping

     // ---------------------------------------------------------------
     //  INVARIANTS 

     double  fMinimumStep; //same 
     // Minimum Step allowed in a Step (in absolute units)
     double  fSmallestFraction= 1.0e-7;   // Expected value: larger than 1e-12 to 5e-15;
     // Smallest fraction of (existing) curve length - in relative units
     //  below this fraction the current step will be the last 

     // const int  fNoIntegrationVariables;  // Number of Variables in integration
     const int  fMinNoVars;               // Minimum number for TemplateFieldTrack<Real_v>
     const int  fNoVars;                  // Full number of variable

     int   fMaxNoSteps;
     static const int  fMaxStepBase;  

     const double fSafetyFactor;
     const double fPowerShrink;   //  exponent for shrinking
     const double fPowerGrow;    //  exponent for growth
     const double fErrcon;
     // Parameters used to grow and shrink trial stepsize.

     double fSurfaceTolerance; 

     //  Stepsize can increase by no more than 5.0
     //           and decrease by no more than x10. = 0.1
     static constexpr double fMaxSteppingIncrease = 5.0;
     static constexpr double fMaxSteppingDecrease = 0.1;
     // Maximum stepsize increase/decrease factors.

     int            fStatisticsVerboseLevel;
     unsigned long  fStepperCalls= 0UL;
     // ---------------------------------------------------------------
     //  STATE
 // public:


// Related to AccurateAdvance
     //Variables required for track insertion algorithm
     static constexpr unsigned int kVectorSize = vecgeom::kVectorSize; // TODO: template it on the backend
     int    fIndex[kVectorSize]; // vecgeom::kVectorSize]; // int *fIndex; 
     int    fNTracks = 0;        //  number under current integration -> ~ current array size
     // ScalarIntegrationStepper *fpScalarStepper= nullptr;  // --> call Stepper with scalar values (args)
     // ScalarIntegrationDriver  *fpScalarDriver = nullptr;  // --> (later) use this driver with scalar args
     // bool partDebug = false ; 
// ---
     
     int    fNoTotalSteps, fNoBadSteps, fNoSmallSteps, fNoInitialSmallSteps; 
     Real_v fDyerr_max, fDyerr_mx2;
     Real_v fDyerrPos_smTot, fDyerrPos_lgTot, fDyerrVel_lgTot; 
     Real_v fSumH_sm, fSumH_lg; 
     // Step Statistics

     int  fVerboseLevel;   // Verbosity level for printing (debug, ..)
     // Could be varied during tracking - to help identify issues
     
#ifdef GVFLD_STATS
     static const bool   fStatsEnabled= true;
     const char* fStatsStatus= " enabled ";
#else
     static const bool fStatsEnabled= false;
     const char* fStatsStatus= " disabled ";
#endif

}; // End of class definition -- SimpleIntegrationDriver


using std::cerr;
using std::cout;
using std::endl;

// #include "GUIntegratorDriver.icc"

/*****
template <class Real_v, class T_Stepper, unsigned int Nvar>
constexpr double SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>::fMaxSteppingIncrease;

template <class Real_v, class T_Stepper, unsigned int Nvar>
constexpr double SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>::fMaxSteppingDecrease;
 *****/

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
   ::ComputeAndSetErrcon()
{
  fErrcon = std::pow(fMaxSteppingIncrease/fSafetyFactor,1.0/fPowerGrow);
  // return fErrcon;
}

// ---------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::CheckParameters()
{
  constexpr double perMillion = 1.0e-6;
  
  double checkPowerShrink  = -1.0 / fpStepper->IntegratorOrder();
  if (    std::fabs(checkPowerShrink - fPowerShrink)             
          >=   perMillion *  std::fabs(fPowerShrink) ){
     std::cerr << "SimpleIntegrationDriver: ERROR in fPowerShrink" << std::endl;
     exit(1);
  }
  assert( std::fabs(checkPowerShrink - fPowerShrink)
          < perMillion *     std::fabs(fPowerShrink) );
  
  double checkPowerGrow    = -1.0 / (1.0 + fpStepper->IntegratorOrder());
  assert( std::fabs(checkPowerGrow - fPowerGrow)
          < perMillion *   std::fabs(fPowerGrow) );

  if (    std::fabs(checkPowerGrow - fPowerGrow)             
          >= perMillion *  std::fabs(fPowerGrow) ){
     std::cerr << "SimpleIntegrationDriver: ERROR in fPowerGrow" << std::endl;
     exit(1);     
  }
}

/*********
// ---------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::GetDerivatives(const TemplateFieldTrack<Real_v> &y_curr, // const, INput
                         Real_v  charge, 
                         Real_v  dydx[])  // OUTput
{ 
  Real_v  tmpValArr[ncompSVEC]; // TemplateFieldTrack<Real_v>::ncompSVEC];
  y_curr.DumpToArray( tmpValArr  );
  fpStepper -> RightHandSideVIS( tmpValArr , charge, dydx );
}
 ********/

template <class Real_v, class T_Stepper, unsigned int Nvar>
const int  SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>::fMaxStepBase = 250;  // Was 5000

#ifndef NO_FIELD_STATISTICS
#define GVFLD_STATS  1
#endif

#define GUDEBUG_FIELD 1

// To add much printing for debugging purposes, uncomment the following
// and set verbose level to 1 or higher value !
// #define  GUDEBUG_FIELD 1    

// ---------------------------------------------------------

//  Constructor
//
template <class Real_v, class T_Stepper, unsigned int Nvar>
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::SimpleIntegrationDriver( double     hminimum, 
                             T_Stepper *pStepper,
                             int        numComponents,
                             int        statisticsVerbose)
   : fMinimumStep( hminimum ),
     // fSmallestFraction( 1.0e-12 ), 
     // fNoIntegrationVariables(numComponents),  // ==> Nvar
     fMinNoVars(12),
     fNoVars( std::max( Nvar, fMinNoVars, numComponents )),
     fSafetyFactor(0.9),
     fPowerShrink(0.0),   //  exponent for shrinking
     fPowerGrow(0.0),    //  exponent for growth
     // fErrcon(0.0),
     fSurfaceTolerance( 1.0e-6),
     fStatisticsVerboseLevel(statisticsVerbose),
     fNoTotalSteps(0),  fNoBadSteps(0), fNoSmallSteps(0),
     fNoInitialSmallSteps(0), 
     fDyerr_max(0.0), fDyerr_mx2(0.0), 
     fDyerrPos_smTot(0.0), fDyerrPos_lgTot(0.0), fDyerrVel_lgTot(0.0), 
     fSumH_sm(0.0), fSumH_lg(0.0),
     fVerboseLevel(3)
{  
  // In order to accomodate "Laboratory Time", which is [7], fMinNoVars=8
  // is required. For proper time of flight and spin,  fMinNoVars must be 12
  assert( pStepper != nullptr );
  assert( Nvar <= ncompSVEC );  // Ensure that arrays are large enough for Integr.

  RenewStepperAndAdjust( pStepper );
  fMaxNoSteps = fMaxStepBase / fpStepper->IntegratorOrder();
#ifdef GUDEBUG_FIELD
  fVerboseLevel=2;
#endif

  if( (fVerboseLevel > 0) || (fStatisticsVerboseLevel > 1) ) {
     std::cout << "MagIntDriver version: Accur-Adv: "
               << "invE_nS, QuickAdv-2sqrt with Statistics " << fStatsStatus << std::endl;
        // ( fStatsEnabled ? "enabled" : "disabled" ) 
  }

  ComputeAndSetErrcon();
     
  CheckParameters();
  
#ifdef GVFLD_STATS
  /*****
  if( !gHistStepsLin )
       gHistStepsLin = new TH1F("hSteps", "Step size in Int-Driver", 100, 0, 100.0);
  if( !gHistStepsInit )
       gHistStepsInit = new TH1F("hSteps", "Input Step size in Int-Driver", 100, 0, 100.0);  
  if( !gHistStepsLog )
       gHistStepsLog = new TH1F("hSteps", "Log of Step size in Int-Driver", 40, -10., +10.0);
   *****/
#endif

  //For track insertion
  // fIndex = new int[kVectorSize];
}

// ---------------------------------------------------------

//  Copy Constructor - used by Clone
template <class Real_v, class T_Stepper, unsigned int Nvar>
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
::SimpleIntegrationDriver( const SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>& right ) 
 : fMinimumStep( right.fMinimumStep ),
   fSmallestFraction( right.fSmallestFraction ),
   // fNoIntegrationVariables( right.fNoIntegrationVariables ),
   fMinNoVars( right.fMinNoVars ),
   fNoVars( std::max( Nvar, fMinNoVars )),
   fSafetyFactor( right.fSafetyFactor ),
   fPowerShrink( right.fPowerShrink ),
   fPowerGrow(   right.fPowerGrow),
   fErrcon( right.fErrcon ),
   fSurfaceTolerance( right.fSurfaceTolerance ),
   fStatisticsVerboseLevel( right.fStatisticsVerboseLevel ),
   fNoTotalSteps(0),  fNoBadSteps(0), fNoSmallSteps(0),
   fNoInitialSmallSteps(0),
   fDyerr_max(0.0), fDyerr_mx2(0.0), 
   fDyerrPos_smTot(0.0), fDyerrPos_lgTot(0.0), fDyerrVel_lgTot(0.0), 
   fSumH_sm(0.0), fSumH_lg(0.0),
   fVerboseLevel( right.fVerboseLevel )
{  
  // In order to accomodate "Laboratory Time", which is [7], fMinNoVars=8
  // is required. For proper time of flight and spin,  fMinNoVars must be 12
   const T_Stepper *protStepper = right.GetStepper();
   fpStepper= protStepper->Clone();
   
   RenewStepperAndAdjust( fpStepper );
   fMaxNoSteps = fMaxStepBase / fpStepper->IntegratorOrder();

  if( (fVerboseLevel > 0) || (fStatisticsVerboseLevel > 1) )
  {
     std::cout << "MagIntDriver version: Accur-Adv: "
           << "invE_nS, QuickAdv-2sqrt with Statistics "
#ifdef GVFLD_STATS
           << " enabled "
#else
           << " disabled "
#endif
           << std::endl;
  }
}


// ---------------------------------------------------------

//  Destructor
template <class Real_v, class T_Stepper, unsigned int Nvar>
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::~SimpleIntegrationDriver()
{ 
  if( fStatisticsVerboseLevel > 1 )
  {
     // PrintStatisticsReport();
  }

  // delete[] fIndex;
  // delete fpScalarDriver;
  // delete fpScalarStepper;
  // delete fpStepper;
}

template <class Real_v, class T_Stepper, unsigned int Nvar>
void
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::OneGoodStep(const Real_v  yStart[],
                const Real_v  dydx[],
                const Real_v  charge,                
                      Real_v& x,         // InOut
                      Real_v  htry,
                      Real_v  eps_rel_max,
                      Real_v  yFinal[],  // Out-values
                      Real_v& hdid,      // Out - achieved length
                      Real_v& hnext   )  // Out - proposed next integration length
// This version:  J. Apostolakis,  13 November 2017. 
//   Lanes are integrated until all have either,
//     - succeeded with the initial interval (1st iteration),
//     -     >>    at a later iteration, with a reduced step size, or
//     - failed due to step-size underlow.
//  That is, no minimum step size exists (or it is not respected.)
//   
// A maximum of number of iterations is observed.
//
// Simplest method, meant as baseline, or for cases where all lanes are
//  expected to succeed in 1 step in a large fraction of cases.
// -------------------------------------------------------------------------
//   
// Derived from OneGoodStep
//
// Driver for one Runge-Kutta Step with monitoring of local truncation error
// to ensure accuracy and adjust stepsize.
//  Input are dependent variable array y[0,...,5] and its derivative dydx[0,...,5]
// at the starting value of the independent variable x . Also input are stepsize
// to be attempted htry, and the required accuracy eps. On output y and x
// are replaced by their new values, hdid is the stepsize that was actually
// accomplished, and hnext is the estimated next stepsize. 
// Similar to function rkqs from Numerical Recipes in C, 2nd Ed:p. 719
//
   
{
  // using std::cout;
  // using std::cerr;
  // using std::endl;  
  if (partDebug) { cout<<"\n"<<endl; }

  // const int kVectorSize = vecgeom::kVectorSize;
  
  Real_v errmax_sq;
  Real_v htemp, xnew ;

  // const int ncompSVEC = TemplateFieldTrack<Real_v>::ncompSVEC;

  Real_v yerr [ncompSVEC], 
         ytemp[ncompSVEC];

  Real_v h = htry ; // Set stepsize to the initial trial value
  // Renamed it to hStep

  Real_v invEpsilonRelSq = 1.0 / (eps_rel_max*eps_rel_max);

  static int tot_no_trials=0;  // Should be thread_local - or suppressed. Just statistics
  const int max_trials=100; 

  // int finishedArr[kVectorSize] = {0,0,0,0};
  Bool_v finished(false);
  // vecCore::Int_v  finishedInt = 0;
  
  Real_v hFinal, hnextFinal, xFinal, hdidFinal, errmax_sqFinal;
  // Real_v yFinal[ncompSVEC];
  Bool_v goodStep(false), stepSizeUnderflow(false);

  // for (iter = 0; iter < max_trials; iter++)
  unsigned int iter= 0;
  int itersLeft= max_trials;

  do
  {
     Real_v errpos_sq=0.0;    // square of displacement error
     Real_v errmom_sq=0.0;    // square of momentum vector difference

     Bool_v Active = ! finished;
     
     itersLeft--;
     iter++; 
     // if ( !vecCore::IsFull(stepSizeUnderflow || goodStep) )
     // {
     fpStepper-> StepWithErrorEstimate(yStart,dydx,charge,h,ytemp,yerr);  // CAREFUL -> changes for others ?
     fStepperCalls++;

     if (partDebug) {
        cout<< "----h is: " << h[0] << " at iter: " << iter << endl;
        // cout<< " yerr is: " << yerr[0] << endl;
        cout<< "----yerr is: " << yerr[0][0] <<" "<<yerr[1][0]<<" "<<yerr[2][0] << " "
            << yerr[3][0] << " " << yerr[4][0] << " " << yerr[5][0]  << endl;
     }

     Real_v epsPosition = eps_rel_max * Max(h, fMinimumStep);  // Uses remaining step 'h'
     // Could change it to use full step size ==> move it outside loop !!   2017.11.10 JA
     Real_v invEpsPositionSq = 1.0 / (epsPosition*epsPosition);
     
     // Evaluate accuracy
     errpos_sq =  yerr[0]*yerr[0] + yerr[1]*yerr[1] + yerr[2]*yerr[2] ; 
     errpos_sq *= invEpsPositionSq; // Scale relative to required tolerance

     // Accuracy for momentum
     Real_v magmom_sq=  ytemp[3]*ytemp[3] + ytemp[4]*ytemp[4] + ytemp[5]*ytemp[5];
     Real_v sumerr_sq=  yerr[3]*yerr[3] + yerr[4]*yerr[4] + yerr[5]*yerr[5]; 
     
     // vecCore::CondAssign(magmom_sq > 0.0, sumerr_sq/magmom_sq, sumerr_sq, &errmom_sq);
     constexpr double tinyValue= 1.0e-20; // Just to ensure there is no division by zero
     errmom_sq = sumerr_sq / ( magmom_sq + tinyValue );
     
     errmom_sq *= invEpsilonRelSq;
     errmax_sq =  Max( errpos_sq, errmom_sq ); // Square of maximum error

     if (partDebug)
     {
        cout<< "----epsPosition      = " << epsPosition[0] << endl;
        cout<< "----invEpsPositionSq = " << invEpsPositionSq[0] << endl;
        cout<< "----errmom_sq = " << errmom_sq[0] << endl;
        cout<< "----errpos_sq = " << errpos_sq[0] << endl;
        cout<< "----errmax_sq = " << errmax_sq[0] << endl;
     }

     goodStep = Active && ( errmax_sq <= 1.0 );

     Bool_v laneDone= (goodStep|finished);
     bool    allDone = vecCore::MaskFull(laneDone); // goodStep|finished);

     // Store (only) steps which have just finished
     if ( !vecCore::MaskEmpty(goodStep && !finished) )
     {
#if 1
        // Idea 1
        StoreFinalValues( ytemp, h, errmax_sq, goodStep, yFinal, hFinal, errmax_sqFinal );
        finished = finished || goodStep;
        // Can replacement of logic below using multiple MaskedAssigns
#else              
        for (int i = 0; i < kVectorSize; ++i)
        {
           if ( goodStep[i] ==1 && ! finished[i] )
           {
              cout << "Storing lane " << i << " :  " << " h = " << h[i]
                   << " errMaxSq = " << errmax_sq[i] << endl;

              // finishedArr[i] = true;
              Set( finished, i, true);  //  finished[i] = true;
              hFinal        [i] = h[i];
              errmax_sqFinal[i] = errmax_sq[i];
              for (int j = 0; j < ncompSVEC; ++j)
              {
                 yFinal[j][i] = ytemp[j][i];
                 Set( yFinal[j],i,  Get( ytemp[j], i));
              }
           }
        }
#endif              
     }

     if ( allDone ) // All (or remaining) steps succeeded.
     {
        // Idea 2
        // StoreFinalValues( ytemp, h, errmax_sq, goodStep, yFinal, hFinal, errmax_sqFinal );
        // finished = finished || goodstep;        
        break;
     } 

     // Some of the steps failed
     // Compute the size of retrial Step.
     
     // htemp = fSafetyFactor *h* vecCore::Pow( errmax_sq, 0.5*fPowerShrink );
     Real_v errPower = exp( (0.5*fPowerShrink) * Log(errmax_sq) ); 
     htemp = fSafetyFactor * h * errPower;
     // Does it make sense to calculate these also for succesful steps
     //  ( when some have failed - landing you here )
     //   Or are these operations too costly ?  
     // The alternative is to loop (as below) to avoid calling power (or log/exp)
     //   when not needed, as they are (very) expensive operation.
     /*******
      for (int i = 0; i < kVectorSize; ++i) {
         if ( ! finished[i] )
         {
            htemp[i] = fSafetyFactor *h[i]* std::pow(errmax_sq[i], 0.5*fPowerShrink);
         }
      }******/

      // using Max = vecCore::Math::Max;
      
      h = Max( htemp, 0.1 * h );
      
      xnew = x + h;

      stepSizeUnderflow = Active && (xnew == x);

#if 1     
      StoreFinalValues( ytemp,      h,        errmax_sq,
                        stepSizeUnderflow, // && !finished,
                        yFinal,     hFinal,   errmax_sqFinal );
      finished = finished || stepSizeUnderflow;

/********
      //  'Idea 2' - Store only once (except on loop exit) 
      StoreFinalValues( ytemp,      h,        errmax_sq,
                        stepSizeUnderflow | goodLane, 
                        yFinal,     hFinal,   errmax_sqFinal );
 *******/
#else      
      if ( !vecCore::MaskEmpty(stepSizeUnderflow) )
      {
         for (int i = 0; i < kVectorSize; ++i)
         {
            // Probably could use several MaskedAssigns as well
            if ( stepSizeUnderflow[i] && !finished[i] )
            {
               /* StoreFinalValues() */
               finished      [i] = true;
               hFinal        [i] = h[i];
               errmax_sqFinal[i] = errmax_sq[i];
               for (int j = 0; j < ncompSVEC; ++j)
               {
                  yFinal[j][i] = ytemp[j][i];
               }
            }
         }
      }
#endif
      // Interim Report of problem lanes -- For debugging only !!
      Bool_v problemLanes = stepSizeUnderflow && Active ;
      if(! vecCore::MaskEmpty(problemLanes)) {
         std::cerr << "GVIntegratorDriver::OneStep:" << std::endl
                   << "  Stepsize underflow in Stepper ( Report 1 - in loop )" << std::endl ;
         Bool_v problemLanes= stepSizeUnderflow && !finished;
         
         ReportConditionLanes( problemLanes, x, xnew, h, htry );
      }

  } while (    itersLeft > 0
            && ( ! vecCore::MaskFull( stepSizeUnderflow || goodStep ) )
          );

  tot_no_trials += iter;

  if( ! vecCore::MaskEmpty(stepSizeUnderflow) )
  {
     // int numUnder= NumberTrue( stepSizeUnderlow );
     std::cerr << "== Check after iteration loop: found " // << numUnder
               << "underflow lanes." << std::endl;
     ReportConditionLanes( stepSizeUnderflow, x, h, htry );     
  }
  
#ifdef PARTDEBUG
  if (partDebug)
  {
    cout << "SimpleIntDrv: 1--step - Loop done at iter = " << iter << " with htry= " << htry <<std::endl;
  }
#endif 

  h         = hFinal;
  errmax_sq = errmax_sqFinal;

  // Compute size of next Step
  Real_v errStretch = fSafetyFactor * vecCore::math::Pow(errmax_sq, 0.5*fPowerGrow);
  //                = fSafetyFactor * vecCore::math::Exp( (0.5*fPowerGrow)* vecCore::math::Log(errmax_sq));
  hnext = vecCore::math::Min( errStretch,  fMaxSteppingIncrease) * h;

  // Could use check against fErrcon to avoid calling power ... if few/none are 'under'
  // Bool_v  errUnderThreshold = errmax_sq <= fErrcon*fErrcon;
  // ...

  x += (hdid = h);

  // for(int k=0;k<Nvar ;k++) { y[k] = yFinal[k]; }

#ifdef PARTDEBUG
  if (partDebug)
  {
    cout << " hdid= " << hdid << " and hnext= " << hnext <<  std::endl;
  }
#endif
  return;
}   // end of  OneStep .............................

// ---------------------------------------------------------
template <class Real_v, class T_Stepper, unsigned int Nvar>
void
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>::
   StoreFinalValues( const                  Real_v & yWork, 
                     const                  Real_v & hValue,
                     const                  Real_v & epsSqValue,
                     const vecCore::Mask_v<Real_v> & storeFlag,                     
                                            Real_v & yFinal,     
                                            Real_v & hFinalStore,  
                                            Real_v & epsSqStore
      )
   // yWork,  hValue,      epsSqVal   represent the output variables   
   // yFinal, hFinalStore, epsSqStore represent the output variables
{
   for (int j = 0; j < Nvar; ++j)
      MaskedAssign( yFinal[j],   storeFlag, yWork[j] );

   MaskedAssign( hFinalStore, storeFlag, hValue );
   MaskedAssign( epsSqStore,  storeFlag, epsSqValue );
}

// ---------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
void
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>::
  ReportConditionLanes( vecCore::Mask_v<Real_v> problemLane, Real_v x, Real_v xnew, Real_v h, Real_v htry )
{
   using std::cerr;
   using std::endl;   
   
   for (int i = 0; i < vecgeom::kVectorSize; ++i)
   {
      if( problemLane[i] ) {
         double xnewCheck = x[i] + h[i];
         double diffX = xnew[i] - x[i];
         cerr.precision(16);
         cerr << " WARNING (SimpleIntegrationDriver::OneGoodStep()> Underflow in lane " << i << endl
              << "   Step's start and end are equal !  Values: " << endl
              << "      Start x = " << std::setw(20) << x[i]    << endl
              << "      End   x = " << std::setw(20) << xnew[i] << " check= " << xnewCheck << endl;
         cerr.precision(6);
         cerr << "      Diff    = " << diffX << endl;
         cerr.precision(9);
         cerr <<"     Step-size = " << h[i]    << endl
              << "   Input step = " << htry[i] << endl;
      }
   }
}

#if 0
//  Auxiliary methods - should be encapsulated into a separate helper class
template< typename Real_v >
void
ReportRowOfDoubles( std::str varName, Real_v var, int lenName, int lenValue )
{
   cout << setw( lenName ) << varName << " : "
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {   
      cout << " " << setw( lenValue ) << Get( var, i ) << " | ";      
   }
   cout << endl;
}

template< typename Real_v >
void
ReportBoolRow( std::str varName, vecCore::Mask_v<Real_v> var, int lenName, int lenValue)
{
   cout << setw( lenName ) << varName << " : "
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      cout << " " << setw( lenValue ) << Get( var, i ) << " | ";
   }
   cout << endl;
}

template< typename Real_v >
void
ReportRowOfDoublesIf( std::str varName, Real_v var, int lenName, int lenValue, vecCore::Mask_v<Real_v> cond )
{
   cout << setw( lenName ) << varName << " : "
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      if( Get( cond, i ) )
         cout << " " << setw( lenValue ) << Get(var, i) << " | ";      
   }
   cout << endl;
}


template <class Real_v, class T_Stepper, unsigned int Nvar>
void
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
ReportResults()
{
   // Method to report intermediate state of integration, including
   //   - error ratios
   //   - status of finishing, underflow, storing, ..
   //   - integrated variables 
   
   const int chName = 10;
   cout << setw( chName ) << "h" << " : ";
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      cout << " " << setw( chValue ) << hFinal[i]         << " | ";
   }
   cout << endl;
   
   ReportRowOfDoubles( "hFinal", hFinal );
   //
   cout << setw( chName ) << "errMaxSq" << " : "
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      cout << " " << setw( chValue ) << errmax_sqFinal[i] << " | ";      
   }
   cout << endl;

   ReportRowOfDoubles( "errMaxSq", errmax_sqFinal );
}

#endif

// ----------------------------------------------------------------------------------

void ReportArray( const std::string&   methodName,
                  const std::string& variableName,                  
                  int numTracks,
                  double Arr[] )
{
   using std::cout;
   using std::endl;

   const int precisionVal = 8;
   
   cout << " " << methodName << " called.  Value of " << variableName << " : " << endl;
   cout.precision( precisionVal );
   for (int i = 0; i < numTracks; ++i)
   {
      cout << Arr[i] << " ";
   }
   cout<< std::endl;
}

// ------------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
int
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::InitializeLanes(   const  FieldTrack yInput[],
                       const  double     hstep [],
                       const  double     charge [],                       
                       // const  double     xStart [], // --> Now in FieldTrack
                              int        nTracks,
                              bool       badStepSize[],  // Output - redundant
                              Real_v     y[],
                              Real_v   & hStepLane,
                              Real_v   & chargeLane,                       
                              Real_v   & startSlen,
                              int      & numBadSize )
// Converts input scalar stream to acceptable form of Vc vectors
// for vector processing in OneStep
{
  if( partDebug )  std::cout << "----Initializing Lanes ----" << std::endl;
  const int arrLen= FieldTrack::NumCompFT;
  double yStartScalar[arrLen]; // Size depends on needs of DumpToArray
  
  numBadSize = 0; // Ensure it starts at Zero !?
  int     j=0;
  int slot= 0;
  do
  {
     double hval= hstep[j];
     bool   hNotPositive = ( hval <= 0.0 ); 

     badStepSize[j] =  hNotPositive;

     if( hNotPositive )
     {
        numBadSize++;
     }
     else
     {
        fIndex[j] = slot;
        yInput[j].DumpToArray(yStartScalar);
        
        //   hStepLane   []  = hval;
        Set( hStepLane,  slot, hval );
        //   chargeLane  []  = charge[j];
        Set( chargeLane, slot, charge[j] );
        //  startSlen  [j] = xStart[j] ); // yInput[j].GetCurveLength();
        Set( startSlen,  slot, // xStart[j] );
                               yInput[j].GetCurveLength() );
        for (int i = 0; i < fNoVars; ++i)
        {
           //   y[i] [j] = yStartScalar[i];
           Set( y[i], slot, yStartScalar[i] );
        }
     }
     
     slot++;
  } while (     slot < vecgeom::kVectorSize
            &&     j < nTracks
     );

  return slot;
} // End of InitializeLanes function

// ------------------------------------------------------------

template< typename Real_v, typename scalar_t > 
bool CheckLaneValue( Real_v varLanes, int lane, scalar_t expected, std::string testName, std::string nameVal )
{
   double current = Get( varLanes, lane );
   bool ok= ( current != expected );
   if( !ok ) { 
      std::cerr << testName << " : ERROR in comparison of " << nameVal
                << " [ lane = "  << lane << " ] "
                << " current = " << current << " VS expected = " << expected 
                << "  diff = " << current - expected << std::endl;
   }
   return ok;
}

// ------------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
bool 
SimpleIntegrationDriver<<Real_v, T_Stepper, Nvar>>
  ::InsertNewTrack( const FieldTrack  yInput[],
                    const double      hstep[],
                    const double      charge[],
                    const int         slot,
                          int&        trackNextInput,
                          bool        succeeded[],
                          Real_v      y[],
                          Real_v    & hStepLane,
                          Real_v    & chargeLane,
                          Real_v    & startCurveLength )
2// Inserts a new track whenever a lane is finished. 
// returns isDoneLane = true for h<=0 case, false otherwise
// because in former case, no further work is required
{
#ifdef DEBUG
  std::cout<<"----Inserting New Track "<< trackNextInput << " at position "<< slot <<std::endl;
#endif 

  bool filled = false; // to get the while loop starting
  while(trackNextInput < fNTracks && ! filled )
  {
    // Ensure that hstep > 0
    double hStepNext = hstep[trackNextInput];
    if (hStepNext <= 0)
    {
      if (hStepNext == 0 )
      {
        std::cerr << "Proposed step is zero; hstep = " << hStepNext << " !" << std::endl;
        // Success step
        // succeeded is initialized with true, hence no assignment needed here
      }
      else
      {
        std::cerr << "Invalid run condition." << std::endl
                  << "Proposed step is negative; hstep = " << hStepNext << "." << std::endl;
        succeeded[trackNextInput] = false; //the final bool array to be returned 
      }
    }

    else
    {
      double yScalar[fNoVars];
      yInput[trackNextInput].DumpToArray(yScalar);
      for (int i = 0; i < fNoVars; ++i)
      {
         //  y[i] [slot] = yScalar[i];
        Set( y[i], slot,   yScalar[i] );
        
      }
      fIndex          [slot] = trackNextInput;
      //   hStepLane  [slot] = hstep[trackNextInput];
      Set( hStepLane,  slot,   hstep[trackNextInput] );
      //   chargeLane [slot] = charge[trackNextInput] );
      Set( chargeLane, slot,   charge[trackNextInput] );

      double slen = yInput[trackNextInput].GetCurveLength();      
      //    startCurveLength [slot] = slen;
      Set ( startCurveLength, slot, slen );
      
      filled = true;
    }

    trackNextInput++;
    
  }
  
  return filled;

} // End of InsertNewTrack function

template <class Real_v, class T_Stepper, unsigned int Nvar>
void 
SimpleIntegrationDriver<<Real_v, T_Stepper, Nvar>>
  ::StoreOutput(const Real_v       y[],
                const Real_v       x,
                      FieldTrack   yOutput[],
                      int          currIndex,
                      double       hstep[],
                      bool         succeeded[] )
// Called whenever a lane is finished.
// Stores value of succeeded in the bool[nTracks]
// Stores final curve length and end position and momentum
// in yOutput (scalar form)
// currIndex is the index of finished lane in Vc vector 
// hstep argument given because no storage needed for 
// h<=0 case 
{
#ifdef DEBUG
  cout<<"----Storing Output at position: "<< currIndex << std::endl;
#endif 

  int    absoluteIndex = fIndex[currIndex]; // might be sent directly to StoreOutput as well
  double hStepOriginal = hstep [absoluteIndex];

  if(hStepOriginal == 0.0)
  {
    succeeded[absoluteIndex] = true;
  }
  else if (hStepOriginal < 0.0)
  {
    succeeded[absoluteIndex] = false;
  }
  else
  {
    // need to get a yEnd : scalar array
    double yEnd[fNoVars]; // Confirm size //ncompSVEC? : Ananya 
    for (int i = 0; i < fNoIntegrationVariables; ++i)
    {
      yEnd[i] = y[i][currIndex]; // Constant col no., varying row no. for required traversal
    }
    yOutput[absoluteIndex].LoadFromArray ( yEnd, fNoIntegrationVariables);
    yOutput[absoluteIndex].SetCurveLength( x[currIndex]); // x is a double_v variable
  }

} // End of StoreOutput function



// ------------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
void
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::AccurateAdvance(const FieldTrack yInput[],
                          double     hstep[],
                    const double     charge[],
                          double     epsilon,   // Can be scalar or varying 
                          FieldTrack yOutput[],
                          int        nTracks,
                          bool       stillOK[])
{
  // Built on original AccurateAdvance. Takes buffer stream of nTracks
  // Converts them to Vc vectors for processing
  // Inserts new track when processing for a lane is finished.

  // Driver for Runge-Kutta integration with adaptive stepsize control.
  // Integrate starting 'vector' y_current, over length 'hstep'
  // maintaining integration error so that relative accuracy is better
  // than 'epsilon'.
  // NOTE: The number of trial steps is limited by 'fMaxNoSteps'. Integration will 
  //       stop if this maximum is reached, and the return value will be false.
  // On return
  //  - 'yOutput' provides the values at the end of the integration interval;
  //  - the return value is 'true' if integration succeeded to the end of the interval,
  //    and 'false' otherwise.

  const std::string methodName= "SID::AccurateAdvance";
  using ThreeVector = vecgeom::Vector3D<Real_v>;

  fNTracks = nTracks;  // Keep in state (pass easily)

  Real_v x, hnext, hdid, h;

  // Real_v ySubStepStart[ncompSVEC];

  if (partDebug)
     ReportArray( methodName, "hstep",  nTracks, hstep );

  bool badStepSize[nTracks];      // Step size is illegal, ie. h <= 0
  
  std::fill_n( stillOK, nTracks,     1);   //  Assume success, flag failure ... Strange/JA
  std::fill_n( badStepSize,  nTracks, false);

  // Working variables for integration
  Real_v     y [ncompSVEC];  // Working array 1
  Real_v  yNext[ncompSVEC],  // Working array 2
           dydx[ncompSVEC];
  
  Real_v  x1, x2;
  Bool_v  lastStepOK;
  Real_v  xStartLane;
  Real_v  chargeLane;

  // ThreadLocal int  noGoodSteps =0 ;  // Bad = chord > curve-len 

  Real_v  hStepLane; 
  Real_v  hTotalDoneSoFar(0.); // To keep track of hDone in KeepStepping

  Bool_v  succeededLane(false);
  Bool_v  isDoneLane(false); // set true when there is a return statement 
  Bool_v  isLastStepLane(false);

  int  numBadSize= 0;  // How may tracks have invalide step size
  int  idNext = InitializeLanes( yInput, hstep,     charge, /* xStart, */ nTracks, badStepSize,
                                 y,      hStepLane, chargeLane, xStartLane, numBadSize);
  // OLD: 
  // int     idNext = kVectorSize; // ASSUMES that nTracks >= kVectorSize !!! FIX
  
  if( idNext > kVectorSize ) // Some lanes had hstep <= 0.0 
  {
    for (int i = 0; i < kVectorSize; ++i)
    {
      if ( hstep[i] <= 0.0 )
      {
         stillOK[i] = ( hstep[i] == 0.0 );
      }
    }
  }

  succeededLane= ( hStepLane <= 0.0 );

  // Assume that all lanes are currently Full ===>>> TEST it !!
  
  x1= xStartLane;
  x2= x1 + hStepLane; // x2 also needs to be lane specific

  h = hStepLane; // = x2 -x1 ; or x2 - x

  x = x1;

  // Why both y and ystart? Ask John : Ananya
  // for (i=0; i<fNoVars; i++)  { y[i] = ystart[i]; }

  Real_v nstp(1);  // Should be Int_v with size compatible with Real_v ( likely 64-bit )

  // Real_v StartPosAr[3];
  // StartPosAr[0] = y[0];   StartPosAr[1] = y[1];  StartPosAr[2] = y[2];
  
  // isDoneLane needed. In end, other conditions might keep changing
  // even if processing for that lane is finished.
  // Need a way to store the fact that the lane is done.
  // Either make a new single condition that combines isDoneLane
  // and all other conditions or some conditions at least
  // For now, just adding isDoneLane : needs to be && or || with the first 3 
  // and keep nTracks condition in final ||
  // Say continue if isDoneLane is not 1111 and rest all conditions are not 0000
  // while ( !vecgeom::IsEmpty((nstp<=fMaxNoSteps) && (x < x2) && (!isLastStepLane)) || idNext < nTracks  )
  
  while ( (!vecCore::MaskFull(isDoneLane) && 
           !vecCore::MaskEmpty((nstp<=fMaxNoSteps) && (x < x2) && (!isLastStepLane)) ) || 
           idNext < nTracks  )
  {
     if( partDebug ) std::cout<<"----hStepLane is: "<< hStepLane << std::endl;

     // if( h > fMinimumStep ) { QuickAdvance .. } else { .. below  //  ( Sequential code  )
     // if (useOneStep) {
     fpStepper->RightHandSideVIS( y, chargeLane, dydx );       // TODO: change to inline
     OneGoodStep( y, dydx, charge, x, h, epsilon, yNext, hdid, hnext);
     // } else KeepStepping( y, dydx, x, h, epsilon, hdid, hnext, hStepLane, hTotalDoneSoFar) ;

     if( partDebug ) {
        int i = 0;
        cout << " y-Start = " <<  y  [i] << endl; 
        cout << " dydx    = " << dydx[i] << endl;
        cout << " hdid    = " <<  hdid   << endl;     
        cout << " y-Now   = " << yNext[i] << endl; 
     }
     
     lastStepOK = (hdid == h);   
     fNoTotalSteps++;

     // ThreeVector EndPos( y[0], y[1], y[2] );

     // Check the endpoint
     const Real_v edx= yNext[0] - y[0]; // StartPosAr[0];
     const Real_v edy= yNext[1] - y[1]; // StartPosAr[1];
     const Real_v edz= yNext[2] - y[2]; // StartPosAr[2];
     Real_v endPointDist2= vecgeom::Sqrt(edx*edx+edy*edy+edz*edz) ;

     // Note: xStartLane must be positive. ( expect starting value = 0 )
     Bool_v avoidNumerousSmallSteps = h < vecCore::math::Min( epsilon * hStepLane ,
                                                              fSmallestFraction * (xStartLane+hdid) );
     // If it is always true for h<=0 --> lastStep is true, hence the lane will be sent to StoreOutput.

     isLastStepLane = avoidNumerousSmallSteps || isLastStepLane;

     Real_v xOld= x;

     x += hdid;
     Real_v xremains = (x2 - x);
     // For rest, check the proposed next stepsize 

     cout << " x-Start = " << xOld<< endl; 
     cout << " hdid    = " << hdid<< endl;
     cout << " x-Now   = " << x<< endl; 
     
     hnext = vecgeom::Max(hnext, fMinimumStep);
       // Note: This has potential for instability i.e. if MinStep is 'too long'

     // Ensure that the next step does not overshoot
     hnext = vecgeom::Min(xremains , hnext);

     h = hnext;
      
  #ifdef PARTDEBUG
     if (partDebug)      
     {
        Real_v      hRemainsAgn = x2 - x;
        std::cout<< " x2 -x :  "<< hRemainsAgn << std::endl;
        std::cout<< "diff is: " << diff << std::endl;
        // h = x2 - x; 
        std::cout<<"AccurateAdvance: hnext is: "<<hnext<<" and h is : "<<h<<std::endl; 
     }
  #endif 
    // When stepsize overshoots, decrease it!
    // Must cope with difficult rounding-error issues if hstep << x2

     isLastStepLane = (h==0) || isLastStepLane ;

     if (partDebug) std::cout<<" lastStep : "<< isLastStepLane << std::endl;

     nstp++;

     succeededLane = (x>=x2); // If it was a "forced" last step ?
     
     Bool_v laneContinues =    ( nstp<=fMaxNoSteps )
                              && !succeededLane
                              && !isLastStepLane;

     if ( ! vecCore::MaskFull( laneContinues ) )  // At least one lane is finished 
     {
        Bool_v finishedLane = ! laneContinues;
 #ifdef DEBUG
        if (partDebug) {
           cout << "SiD At least one lane finished " << std::endl;
           cout<<" finishedLane:     "<< finishedLane     << std::endl;
           cout<<" CondNoOfSteps:    "<< nstp<=fMaxNoSteps  /* CondNoOfSteps */ << std::endl;
           cout<<" CondXLessThanx2:  "<< x < x2 /* CondXLessThanx2 */  << std::endl;
           cout<<" CondIsNotLastStep:"<< !isLastStepLane << std::endl;
        }
#endif
        Bool_v renewedLanes( false ); // To be 'set' only in the slots in which new values are inserted
        
// #if SINGLE_INSERT
        // Use a single method (to be written) to insert all new tracks ... avoid multiple 'calls' ?
        // InsertSeveralTracks( yInput, hstep,     charge,     i, idNext, stillOK,
        //                      y,      hStepLane, chargeLane, xStartLane );  .......
// #else // SINGLE_INSERT
        for (int i = 0; i < kVectorSize; ++i)
        {
           cout << " [ "<< i << " ]  nstp = " << Get(nstp, i) << " <= ? ( = fMaxNoSteps ) "
   << x < x2;
           
           if (finishedLane[i] == 1 &&  fIndex[i] != -1)
           {
              stillOK[fIndex[i]] = succeededLane[i];           // First
              StoreOutput( y, x, yOutput, i, hstep, stillOK ); // Second - can change 'succeeded'
              // Ananya: Do not pass succeededLane to StoreOutput (preference?), so 
              //         'stillOK' should *not* be absorbed in StoreOutput.
              
              if (idNext<nTracks)
              {
                 isDoneLane[i] = InsertNewTrack( yInput, hstep,     charge,     i, idNext, stillOK,
                                                 y,      hStepLane, chargeLane, xStartLane ); 
                 //   nstp [i] = 1;
                 Set( nstp, i,   1);

                 Set( renewedLanes, i,   true);

                 //   isLastStepLane  [i] = false;
                 Set( isLastStepLane,  i,   false);

                 //   x1[i] = xStartLane[i];
                 Set( x1, i  = Get( xStartLane, i );

                 // x   [i] = x1[i];        // ?? Needed? Find something to set x<x2
                 Set( x, i   = Get( x1, i );
                      
                 // h    [i] = hStepLane[i]; // Can absorb in InsertNewTrack - but too many arguments ?
                 Set( h, i  , hStepLane[i]);
                 x2   [i] = x[i] + hStepLane[i];

                 hTotalDoneSoFar[i] = 0.; // Setting to 0 for every new track inserted. 
              }
              else
              {
                 isDoneLane[i] = true;
                 fIndex    [i] = -1;
              }
           } 
        } // for
#endif  // SINGLE_INSERT        
     }

     //  2nd way to reset working variables for lanes -- cleaner !?
     MaskedAssign( x1, renewedLanes, xStartLane );
     MaskedAssign( x,  renewedLanes, x1 );
     MaskedAssign( x2, renewedLanes, x1 + hStepLane );
     MaskedAssign( hTotalDoneSoFar,  renewedLanes, 0.0 );
     MaskedAssign( isLastStepLane,   renewedLanes, false);
        
#ifdef DEBUG
    if (partDebug)
    {
      cout<<"Value of lastStep is: "<< isLastStepLane <<std::endl;
      cout<<"isDoneLane is:        "<< isDoneLane <<std::endl;
    }
#endif 
       
/*    Bool_v leftLanes = (nstp<=fMaxNoSteps) && (x < x2) && (!isLastStepLane) ;
    int countLeftLanes=0;
    int indLastLane;
    // cout << " leftLanes is: " << leftLanes << std::endl;
    if( !vecCore::MaskEmpty(leftLanes) )
    {
      for (int i = 0; i < kVectorSize; ++i)
      {
        if (leftLanes[i] == 1)
        {
          countLeftLanes++;
          indLastLane = i;
          // cout << indLastLane << std::endl;
        }
      }
    }

    // cout<< "countLeftLanes is: "<<countLeftLanes << std::endl;

    if (countLeftLanes == 1)
    {
      // double hstepOneLane = hStepLane[indLastLane] - hTotalDoneSoFar[indLastLane];
      vecgeom::Vector3D<double> Pos, Mom;
      for (int i = 0; i < 3; ++i)
       {
         Pos[i] = y[i][indLastLane];
         Mom[i] = y[i+3][indLastLane];
       } 
      ScalarFieldTrack y_input(Pos, Mom); 
      ScalarFieldTrack y_output(Pos, Mom);
      // y_input.SetCurveLength( hTotalDoneSoFar[indLastLane] ) ;
      fpScalarDriver->AccurateAdvance(y_input, hstep[ fIndex[indLastLane] ] - hTotalDoneSoFar[indLastLane], epsilon, y_output );

      isDoneLane[indLastLane] == true;
      // Store Output
      double y_output_arr[12];
      y_output.DumpToArray(y_output_arr);
      yOutput[fIndex[indLastLane]].LoadFromArray(y_output_arr);
    }*/

  } // end of while loop

}  // end of AccurateAdvance (flexible / vector )  .....................



//----------------------------------------------------------------------

/*********************************
#define SQR(a)   ((a)*(a))

// QuickAdvance just tries one Step - it does not ensure accuracy
template <class Real_v, class T_Stepper, unsigned int Nvar>//
   typename vecCore::Mask_v<Real_v>
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::QuickAdvance( TemplateFieldTrack<Real_v>&       y_posvel,         // INOUT
                  const Real_v  dydx[],  
                        Real_v  hstep,       // In
                  // Real_v& dchord_step,
                        Real_v& dyerr_pos_sq,
                        Real_v& dyerr_mom_rel_sq )  
{
  // typedef typename Real_v Real_v;
//  typedef typename Mask<Real_v>      Bool_v;
  // Real_v dyerr_pos_sq, dyerr_mom_rel_sq;  
  Real_v yerr_vec[TemplateFieldTrack<Real_v>::ncompSVEC],
           yarrin  [TemplateFieldTrack<Real_v>::ncompSVEC], 
           yarrout [TemplateFieldTrack<Real_v>::ncompSVEC]; 
  Real_v s_start;
  Real_v dyerr_mom_sq, vel_mag_sq, inv_vel_mag_sq;

  static int no_call=0;  // thread_local 
  no_call ++; 

  // Move data into array
  y_posvel.DumpToArray( yarrin );      //  yarrin  <== y_posvel 
  s_start = y_posvel.GetCurveLength();

  // Do an Integration Step
  fpStepper-> StepWithErrorEstimate(yarrin, dydx, hstep, yarrout, yerr_vec) ; 
  //          *********************

#ifdef USE_DCHORD  
  // Estimate curve-chord distance
  dchord_step= fpStepper-> DistChord();
  //                       *********
#endif

  // Put back the values.  yarrout ==> y_posvel
  y_posvel.LoadFromArray( yarrout, Nvar ); // fNoIntegrationVariables );
  y_posvel.SetCurveLength( s_start + hstep );

#ifdef  GUDEBUG_FIELD
  if(fVerboseLevel>2)
  {
    cout << "G4MagIntDrv: Quick Advance" << std::endl;
    PrintStatus( yarrin, s_start, yarrout, s_start+hstep, hstep,  1); 
  }
#endif

  // A single measure of the error   
  //      TO-DO :  account for  energy,  spin, ... ? 
  vel_mag_sq   = ( SQR(yarrout[3])+SQR(yarrout[4])+SQR(yarrout[5]) );
  inv_vel_mag_sq = 1.0 / vel_mag_sq; 
  dyerr_pos_sq = ( SQR(yerr_vec[0])+SQR(yerr_vec[1])+SQR(yerr_vec[2]));
  dyerr_mom_sq = ( SQR(yerr_vec[3])+SQR(yerr_vec[4])+SQR(yerr_vec[5]));
  dyerr_mom_rel_sq = dyerr_mom_sq * inv_vel_mag_sq;

#ifdef RETURN_A_NEW_STEP_LENGTH
  // The following step cannot be done here because "eps" is not known.
  dyerr_len = Sqrt( dyerr_len_sq );   // vecgeom::
  dyerr_len_sq /= epsilon ;

  // Set suggested new step
  hstep= ComputeNewStepSize( dyerr_len, hstep);
#endif

  return true;
}
 *********************************/

// --------------------------------------------------------------------------
#ifdef QUICK_ADV_ARRAY_IN_AND_OUT
template <class Real_v, class T_Stepper, unsigned int Nvar>
typename Mask<Real_v>  
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::QuickAdvance(       Real_v     yarrin[],    // In
                  const Real_v     dydx[],  
                        Real_v     hstep,       // In
                        Real_v     yarrout[],
                        Real_v&    dchord_step,
                        Real_v&    dyerr )      // In length
{
   std::cerr << "ERROR in SimpleIntegrationDriver::QuickAdvance()" << std::endl;
   std::cerr << "      Method is not yet implemented." << std::endl;

   //            FatalException, "Not yet implemented.");
   dyerr = dchord_step = hstep * yarrin[0] * dydx[0];
   yarrout[0]= yarrin[0];
   exit(1); 
}
#endif 

// --------------------------------------------------------------------------

//  This method computes new step sizes - but does not limit changes to
//  within  certain factors
// 
template <class Real_v, class T_Stepper, unsigned int Nvar>
Real_v 
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::ComputeNewStepSize( Real_v  errMaxNorm,    // max error  (normalised)
                        Real_v  hStepCurrent)  // current step size
{
  Bool_v goodStep = (errMaxNorm <= 1.0);
  Real_v powerUse = vecCore::Blend( goodStep, fPowerShrink, fPowerGrow);
  Real_v stretch = fSafetyFactor * vecgeom::Pow(errMaxNorm,powerUse); 
  Real_v hNew    = stretch * hStepCurrent;
  return hNew;
}

// ---------------------------------------------------------------------------

// This method computes new step sizes limiting changes within certain factors
// 
// It shares its logic with AccurateAdvance.
// They are kept separate currently for optimisation.
//
template <class Real_v, class T_Stepper, unsigned int Nvar>
Real_v 
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  ::ComputeNewStepSize_WithinLimits( Real_v  errMaxNorm,    // max error  (normalised)
                                     Real_v  hStepCurrent)  // current step size
{
  Bool_v goodStep = (errMaxNorm <= 1.0);
  Real_v powerUse = vecCore::Blend( goodStep, fPowerShrink, fPowerGrow);
  
  Real_v stretch = fSafetyFactor * vecgeom::Pow(errMaxNorm,powerUse); 

  Real_v stemp;
  stemp   = vecCore::math::Max( stretch,  fMaxSteppingDecrease );
  stretch = vecCore::math::Min( stemp,    fMaxSteppingIncrease );

  Real_v hNew    = stretch * hStepCurrent;

/*
  // Compute size of next Step for a failed step
  if (errMaxNorm > 1.0 )
  {
    // Step failed; compute the size of retrial Step.
    hnew = fSafetyFactor * hstepCurrent * std::pow(errMaxNorm,fPowerShrink) ;
    
    hnew = std::min( hnew, fMaxSteppingDecrease * hstepCurrent );
                         // reduce stepsize, but no more
                         // than this factor (value= 1/10)
    }
  }
  else
  {
    // Compute size of next Step for a successful step
    if (errMaxNorm > fErrcon)
     { hnew = fSafetyFactor * hstepCurrent * std::pow(errMaxNorm,fPowerGrow); }
    else  // No more than a factor of 5 increase
     { hnew = fMaxSteppingIncrease * hstepCurrent; }
  }*/

  return hNew;
}


// ---------------------------------------------------------------------------
template <class Real_v, class T_Stepper, unsigned int Nvar>
void SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
   ::ReportInvalidStepInputs(double hStepArr[],
                             int    nTracks)
{
   for (int i = 0; i < nTracks; ++i)
   {
      double hStep= hStepArr[i];
      if (hStep<=0.0)
      {
         if (hStep==0.0)
         {
            std::cerr << "Proposed step of track " << i << " is zero; hstep = " << hStep << " !";
         }
         else
         {
            std::cerr << "Invalid run condition." << std::endl
                      << "Proposed step is negative; (i= " << i << " ) hstep = " << hStep << "." << std::endl;
         }
      }
   }
}

// ------------------------------------------------------------


// ####################  Testing method(s) ####################

#include <cassert>


template <class Real_v, class T_Stepper, unsigned int Nvar>
void SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>::
// void   
   CreateInput(  FieldTrack yInput[], int nTracks )
{
   constexpr int Ncomp = FieldTrack::NumCompFT;
   double PositionMomArr[ Ncomp ];

   assert( Nvar <= Ncomp );  // Ensure that arrays are large enough for Integr.
   
   double length;
   for( int itr = 0; itr < nTracks; itr++ )
   {
      for( int j = 0; j < Nvar; j++ )
      {
         PositionMomArr [j] = 10. * itr + j;
      }
      length = 0.1 * itr;
      // yInput[j] = FieldTrack( PositionMomArr, length );
      yInput[itr].LoadFromArray(PositionMomArr, Nvar );
      yInput[itr].SetCurveLength(length);
   }
}

// ------------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
bool
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
   :: CheckOutput( Real_v      Output[], 
                   int         lane,     int         initialSlot,
                   std::string testName, std::string varName )
{
   bool allGood= true;
   for( int j = 0; j < Nvar; j++ )
   {
      double current  = Get( Output[j], lane );
      double expected = 10. * j + initialSlot;
      double diff     = current - expected ;
      if( std::fabs(diff) > 1.e-9 * std::fabs( expected ) )
      {
         std::cerr << testName << " : ERROR in Output " << varName
                   << " [lane= " << lane << " ] "
                   << " [iVar= " << j    << " ] "
                   << " current = " << current << " VS expected = " << expected 
                   << "  diff = " << current - expected << std::endl;
         allGood= false;
      }
   }
   return allGood;
}

// ------------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
bool
SimpleIntegrationDriver<Real_v, T_Stepper, Nvar>
  :: TestInitializeLanes() // int numTracks)
{
   using std::cerr;
   using std::endl;
   bool allOk = true;

   constexpr int numTracks=8;
   FieldTrack      yInput[numTracks];
   bool       badStepSize[numTracks];
   // double xStart[numTracks] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 };
   double chargeArr[numTracks] = { 1.0, -2.0, 3.0, -4.0, 5.1, -6.1, 7.1, -8.1 };
   // ------------------------ Test case 1 ------------------------
   Real_v  yOutput[Nvar], hStepLane, chargeLane, sBegin;
   double hStep[numTracks] = { 0.1, 1.1, 2.1, 3.1, 4.1, 5.1, 6.1, 7.1 };
   //     ======   
   int    numBadSize= -1;

   CreateInput( yInput, numTracks );

   int nFilled1= 
      InitializeLanes( yInput,  hStep, chargeArr, /* xStart, */ numTracks, badStepSize,
                       
                       yOutput, hStepLane, chargeLane, sBegin, numBadSize );

   std::string testName( "Test of InitAccurateAdvance # " );
   int testId = 1;

   if( nFilled1 != kVectorSize ) {  // Can cope with 2, 4 or 8 
      cerr << testName << testId << " ERROR> Found less than all lanes filled: Number = "
           << nFilled1 <<  endl;
   }
   // assert( nFilled1 == kVectorSize );
   
   if( numBadSize != 0 ) {
      cerr << testName << testId << " ERROR> Found non-zero bad size lanes." << endl;
   }
   assert( numBadSize== 0 );

   std::string varNameOutput("yOutput");
   for( int iSlot= 0; iSlot < kVectorSize; iSlot++) {
      bool ok= CheckOutput( yOutput, iSlot, iSlot, testName, varNameOutput );
      allOk = allOk && ok;
   }
   
   // ------------------------ Test case 2 ------------------------
   Real_v  yOutput2[Nvar]; // hStepLane2, sBegin2; ==> To check clean (not overwrite)
   testId = 2;
   double hStep2[numTracks] = { 0.0, -1.0, 2.0, -3.0, 4.0, -5.0, 6.0, -7.0 };
   //     ======
   int nFilled2=
      InitializeLanes( yInput,   hStep2,    chargeArr, /* xStart, */ numTracks,  badStepSize,
                       yOutput2, hStepLane, chargeLane, sBegin,    numBadSize );

   int expectFill = std::min( (int)kVectorSize, 3 );
   if( nFilled2 != expectFill )
   {
      cerr << testName << testId << " ERROR> Found less than all lanes filled: Number = "
           << nFilled1 <<  endl;
   }
   // assert( nFilled2 == std::min( kVectorSize, 3 ) );
   
   assert( kVectorSize <= 8);  // Tests below assume it
   if( nFilled2 != (kVectorSize / 2) - 1 )
      cerr << testName << testId << " ERROR> Found " << nFilled2 << " filled lanes."
           << " versus " << (kVectorSize / 2) - 1  << " for kVectorSize= " << kVectorSize << endl;
      
   if( numBadSize != (kVectorSize / 2) + 1  )
      cerr << testName << testId << " ERROR> Found " << numBadSize << " initial lanes."
           << " versus " << (kVectorSize / 2) + 1  << " expected for VecSize = " << kVectorSize
           << endl;

   int lane=0; 
   double hLane0 = Get( hStepLane, lane );
   if( hLane0 != 2.0 ) {
      cerr << testName << testId << " ERROR> hStep[ " << lane << " initial lanes." << endl;
   }

   std::string nameHstep("hStep");
   CheckLaneValue( hStepLane, 0, 2.0, testName, nameHstep );
   CheckLaneValue( hStepLane, 1, 4.0, testName, nameHstep );
   CheckLaneValue( hStepLane, 2, 6.0, testName, nameHstep );
   
   assert( std::fabs( Get( hStepLane, lane=0 ) - 2.0 ) < 1.0e-8 );
   // double hLane1 = Get( hStepLane, 1 );
   assert( std::fabs( Get( hStepLane, lane=1 ) - 4.0 ) < 1.0e-8 );
   if( kVectorSize > 2 ) {
      assert( std::fabs( Get( hStepLane, lane=2 ) - 4.0 ) < 1.0e-8 );   
   }

   int slot=-1;
   bool ok1, ok2, ok3; 
   ok1= CheckOutput( yOutput2, lane=0, slot=2, testName, varNameOutput );
   ok2= CheckOutput( yOutput2, lane=1, slot=4, testName, varNameOutput );
   ok3= CheckOutput( yOutput2, lane=2, slot=6, testName, varNameOutput );
   allOk = allOk && ok1 && ok2 && ok3;
   
   return allOk;
}





#endif /* SimpleIntegrationDriver_Def */
