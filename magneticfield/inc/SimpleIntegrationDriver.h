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

#include "TemplateFieldTrack.h"
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

// #define NEWACCURATEADVANCE

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

    // template <class Real_v>  // Eventual config
    Bool_v  AccurateAdvance( const TemplateFieldTrack<Backend>& yInput,
                                   Real_v    hstep,
                                   Real_v    eps,                  // Requested y_err/hstep
                                   TemplateFieldTrack<Backend>& yOutput   );                            
                                   // Real_v  hinitial=0.0   );  // Suggested 1st interval
                                                                   // (e.g. from previous step) 
       // Above drivers for integrator (Runge-Kutta) with stepsize control. 
       // Integrates ODE starting values yInput 
       // from current s (s=s0) to s=s0+h with accuracy eps. 
       // On output yOutput is value at end of interval.
       // The concept is similar to the odeint routine from NRC 2nd edition p.721

     void  OneStepEach(   Real_v  ystart, // Like old RKF45step()
                    const Real_v  dydx,
                          Real_v& x,
                          Real_v  htry,
                          Real_v  eps,
                          Real_v& hdid,
                          Real_v& hnext ) ;

     Bool_v  QuickAdvance( TemplateFieldTrack<Backend>& y_posvel,        // INOUT
                           const Real_v      dydx[],  
                                 Real_v      hstep,           // IN
                           // Real_v&     dchord_step, //take out
                                 Real_v&     dyerr_pos_sq,
                                 Real_v&     dyerr_mom_rel_sq ) ;
       // New QuickAdvance that also just tries one Step
       //    (so also does not ensure accuracy)
       //    but does return the errors in  position and
       //        momentum (normalised: Delta_Integration(p^2)/(p^2) )

     // SimpleIntegrationDriver* Clone() const;
       // Create an independent copy of the current object -- including independent 'owned' objects
       // NOTE: Evaluate whether this method is needed - 2017.11.09
     
     // VEquationOfMotion<Backend>* GetEquationOfMotion() { return fpStepper->GetEquationOfMotion(); }
     // const VEquationOfMotion<Backend>* GetEquationOfMotion() const { return fpStepper->GetEquationOfMotion(); } 
     
     // Auxiliary methods
     inline double GetHmin()        const { return fMinimumStep; } 
     inline double GetSafety()      const { return fSafetyFactor; }
     inline double GetPowerShrink() const { return fPowerShrink; }
     inline double GetPowerGrow()   const { return fPowerGrow; } 
     inline double GetErrcon()      const { return fErrcon; }

     inline int    GetMaxNoSteps()  const { return fMaxNoSteps; }
     
     inline void   GetDerivatives( const TemplateFieldTrack<Backend> &y_curr,     // const, INput
                                        Real_v    charge, 
                                        Real_v    dydx[]   );  //       OUTput

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

     void ReportUnderflowLanes( veccore::Mask_v<Real_v> problemLane, Real_v x, Real_v h, Real_v htry );
     
   private:

     SimpleIntegrationDriver& operator=(const SimpleIntegrationDriver&);
     // Private copy constructor and assignment operator.

   private:

     // ---------------------------------------------------------------
     // DEPENDENT Objects
     T_Stepper *fpStepper;
     
     // ---------------------------------------------------------------
     //  INVARIANTS 

     double  fMinimumStep; //same 
     // Minimum Step allowed in a Step (in absolute units)
     double  fSmallestFraction= 1.0e-7;   // Expected value: larger than 1e-12 to 5e-15;
     // Smallest fraction of (existing) curve length - in relative units
     //  below this fraction the current step will be the last 

     const int  fNoIntegrationVariables;  // Number of Variables in integration
     const int  fMinNoVars;               // Minimum number for TemplateFieldTrack<Backend>
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

     int    fStatisticsVerboseLevel;

     // ---------------------------------------------------------------
     //  STATE
    public:
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

// #include "GUIntegratorDriver.icc"

/*****
template <class Real_v, class T_Stepper, unsigned int Nvar>
constexpr double SimpleIntegrationDriver<Backend, T_Stepper, Nvar>::fMaxSteppingIncrease;

template <class Real_v, class T_Stepper, unsigned int Nvar>
constexpr double SimpleIntegrationDriver<Backend, T_Stepper, Nvar>::fMaxSteppingDecrease;
 *****/

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
   ::ComputeAndSetErrcon()
{
  fErrcon = std::pow(fMaxSteppingIncrease/fSafetyFactor,1.0/fPowerGrow);
  // return fErrcon;
}

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
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


// START Methods to modify parameters ----- Suppressed
#ifdef NON_CONST_PARAMS
template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Backend, T_Stepper, unsigned int Nvar>
  ::ReSetParameters(double new_safety)
{
  fSafetyFactor = new_safety;
  fPowerShrink  = -1.0 / fpStepper->IntegratorOrder();
  fPowerGrow    = -1.0 / (1.0 + fpStepper->IntegratorOrder());
  ComputeAndSetErrcon();
}

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Backend, T_Stepper, unsigned int Nvar>
  ::SetSafety(double val)
{ 
  fSafetyFactor=val;
  ComputeAndSetErrcon();
}

/*******************8
template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Backend, T_Stepper, unsigned int Nvar>
  ::SetPowerGrow(double  val)
{ 
  fPowerGrow=val;
  ComputeAndSetErrcon(); 
}

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Backend, T_Stepper, unsigned int Nvar>
  ::SetErrcon(double val)
{ 
  fErrcon=val;
}

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Backend, T_Stepper, unsigned int Nvar>
  ::RenewStepperAndAdjust(T_Stepper *pStepper)
{  
  fpStepper = pStepper; 
  ReSetParameters();
}
******************/

#endif
// END:   Methods to modify parameters ----- Suppressed

template <class Real_v, class T_Stepper, unsigned int Nvar>
inline
void SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
  ::GetDerivatives(const TemplateFieldTrack<Backend> &y_curr, // const, INput
                         Real_v  charge, 
                         Real_v  dydx[])  // OUTput
{ 
  Real_v  tmpValArr[TemplateFieldTrack<Backend>::ncompSVEC];
  y_curr.DumpToArray( tmpValArr  );
  fpStepper -> RightHandSideVIS( tmpValArr , charge, dydx );
}


template <class Real_v, class T_Stepper, unsigned int Nvar>
const int  SimpleIntegrationDriver<Backend, T_Stepper, Nvar>::fMaxStepBase = 250;  // Was 5000

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
SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
  ::SimpleIntegrationDriver( double  hminimum, 
                                 T_Stepper *pStepper,
                                 int     numComponents,
                                 int     statisticsVerbose)
   : fMinimumStep( hminimum ),
     // fSmallestFraction( 1.0e-12 ), 
     fNoIntegrationVariables(numComponents),
     fMinNoVars(12),
     fNoVars( std::max( fNoIntegrationVariables, fMinNoVars )),
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


//  Copy Constructor - used by Clone
template <class Real_v, class T_Stepper, unsigned int Nvar>
SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
::SimpleIntegrationDriver( const SimpleIntegrationDriver<Backend, T_Stepper, Nvar>& right ) 
 : fMinimumStep( right.fMinimumStep ),
   fSmallestFraction( right.fSmallestFraction ),
   fNoIntegrationVariables( right.fNoIntegrationVariables ),
   fMinNoVars( right.fMinNoVars ),
   fNoVars( std::max( fNoIntegrationVariables, fMinNoVars )),
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
SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
  ::~SimpleIntegrationDriver()
{ 
  if( fStatisticsVerboseLevel > 1 )
  {
    PrintStatisticsReport();
  }

  // delete[] fIndex;
  // delete fpScalarDriver;
  // delete fpScalarStepper;
  // delete fpStepper;
}

template <class Real_v, class T_Stepper, unsigned int Nvar>
void
SimpleIntegrationDriver<<Real_v, T_Stepper, Nvar>>
  ::LockStep(      Real_v  y[],       // InOut
             const Real_v  dydx[],
                   Real_v& x,         // InOut
                   Real_v  htry,
                   Real_v  eps_rel_max,
                   Real_v& hdid,      // Out
                   Real_v& hnext   )  // Out
// This version:  J. Apostolakis,  10 November 2017. 
//   Lanes are integrated until all have either
//     - finished the interval, or
//     - failed due to underlow.
// A maximum of number of iterations is observed.
//
// Simplest method, meant as baseline, or for cases where all lanes are
//  expected to succeed in 1 (or 2) steps in a large fraction of cases.
// -------------------------------------------------------------------------
//   
// Derived from OneGoodStep
//
// Driver for one Runge-Kutta Step with monitoring of local truncation error
// to ensure accuracy and adjust stepsize. Input are dependent variable
// array y[0,...,5] and its derivative dydx[0,...,5] at the
// starting value of the independent variable x . Also input are stepsize
// to be attempted htry, and the required accuracy eps. On output y and x
// are replaced by their new values, hdid is the stepsize that was actually
// accomplished, and hnext is the estimated next stepsize. 
// This is similar to the function rkqs from the book:
// Numerical Recipes in C:p. 719

{
#ifdef PARTDEBUG
  using cout = std::cout;
  using cerr = std::cerr;
  using endl = std::endl;  
  if (partDebug) { std::cout<<"\n"<<endl; }
#endif 

  Real_v errmax_sq;
  Real_v h, htemp, xnew ;

  int ncompSVEC = TemplateFieldTrack<Real_v>::ncompSVEC;

  Real_v yerr [ncompSVEC], 
         ytemp[ncompSVEC];

  h = htry ; // Set stepsize to the initial trial value

  Real_v invEpsilonRelSq = 1.0 / (eps_rel_max*eps_rel_max);

  static int tot_no_trials=0;  // Should be thread_local - or suppressed. Just statistics
  const int max_trials=100; 

  // int finished[kVectorSize] = {0,0,0,0};
  // veccore::Int_v  finished = 0;
  Bool_v finished(false);

  Real_v hFinal, hnextFinal, xFinal, hdidFinal, errmax_sqFinal;
  Real_v yFinal[ncompSVEC];
  Bool_v goodStep(false), stepSizeUnderflow(false);

  // for (iter = 0; iter < max_trials; iter++)
  unsigned int iter= 0;
  int itersLeft= max_trials;
  do
  {
     Real_v errpos_sq=0.0;    // square of displacement error
     Real_v errmom_sq=0.0;    // square of momentum vector difference
     
     itersLeft--;
     iter++; 
     // if ( !vecgeom::IsFull(stepSizeUnderflow || goodStep) )
     // {
     fpStepper-> StepWithErrorEstimate(y,dydx,h,ytemp,yerr);
     fStepperCalls++;

#ifdef PARTDEBUG
     if (debug) {
        cout<< "----h is: " << h[0] << " at iter: " << iter << endl;
        // std::cout<< " yerr is: " << yerr[0] << endl;
        cout<< "----yerr is: " << yerr[0][0] <<" "<<yerr[1][0]<<" "<<yerr[2][0] << " "
            << yerr[3][0] << " " << yerr[4][0] << " " << yerr[5][0]  << endl;
     }
#endif 
     Real_v epsPosition = eps_rel_max * vecgeom::Max(h, fMinimumStep);  // Uses remaining step 'h'
     // Could change it to use full step size ==> move it outside loop !!   2017.11.10 JA
     Real_v invEpsPositionSq = 1.0 / (epsPosition*epsPosition);
     
     // Evaluate accuracy
     errpos_sq =  yerr[0]*yerr[0] + yerr[1]*yerr[1] + yerr[2]*yerr[2] ; 
     errpos_sq *= invEpsPositionSq; // Scale relative to required tolerance

     // Accuracy for momentum
     Real_v magmom_sq=  y[3]*y[3] + y[4]*y[4] + y[5]*y[5];
     Real_v sumerr_sq=  yerr[3]*yerr[3] + yerr[4]*yerr[4] + yerr[5]*yerr[5]; 
     
     // vecgeom::CondAssign(magmom_sq > 0.0, sumerr_sq/magmom_sq, sumerr_sq, &errmom_sq);
     constexpr double tinyValue= 1.0e-20; // Just to ensure there is no division by zero
     errmom_sq = sumerr_sq / ( magmom_sq + tinyValue );
     
     errmom_sq *= invEpsilonRelSq;
     errmax_sq = vecgeom::Max( errpos_sq, errmom_sq ); // Square of maximum error

#ifdef PARTDEBUG
     if (debug)
     {
        cout<< "----epsPosition      = " << epsPosition[0] << endl;
        cout<< "----invEpsPositionSq = " << invEpsPositionSq[0] << endl;
        cout<< "----errmom_sq = " << errmom_sq[0] << endl;
        cout<< "----errpos_sq = " << errpos_sq[0] << endl;
        cout<< "----errmax_sq = " << errmax_sq[0] << endl;
     }
#endif 
     goodStep = ( errmax_sq <= 1.0 );

     bool allDone = vecgeom::IsFull(goodStep|finished);
     
     // Store (only) steps which have just finished 
     if ( !vecgeom::IsEmpty(goodStep && !finished) )
     {
        for (int i = 0; i < kVectorSize; ++i)
        {
           // Probably could use several MaskedAssigns as well
           if ( goodStep[i] ==1 && ! finished[i] )
           {
              cout << "Storing lane " << i << " :  "
                   << " h = " << h[i]
                   << " errMaxSq = " << errmax_sq[i]
                   << endl;
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

     if ( allDone )  { break; } // Step succeeded. 

     // Some of the steps failed
     // Compute the size of retrial Step.
     
     // htemp = fSafetyFactor *h* vecgeom::Pow( errmax_sq, 0.5*fPowerShrink );
     Real_v errPower = Vc::exp( (0.5*fPowerShrink)*vecgeom::Log(errmax_sq) ); 
     htemp = fSafetyFactor *h* errPower;
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

      h = vecgeom::Max( htemp, 0.1 * h );
      
      xnew = x + h;

      stepSizeUnderflow = (xnew == x);
      if ( !vecgeom::IsEmpty(stepSizeUnderflow) )
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

      if(vecgeom::IsFull(StepSizeUnderflow|finished))
      {
        void ReportUnderflowLane( Bool_v underflowing, Real_v x, Real_v h, Real_v htry );
        std::cerr << "GVIntegratorDriver::OneStep:" << std::endl
                  << "  Stepsize underflow in Stepper " << std::endl ;
        Bool_v problemLanes= stepSizeUnderflow && !finished;
// #if  1        
        ReportUnderflowLanes( problemLanes, x, h, htry );
// #else        
        for (int i = 0; i < kVectorSize; ++i)
        {
           if( problemLanes[i] ) {
              std::cerr << " WARNING (SimpleIntegrationDriver::LockStep()> Underflow in lane " << i << endl
                        << "  Step's start x=" << x[i] << " and end x= " << xnew[i]
                        << " are equal !! "    << endl
                        <<"  Due to step-size= " << h[i]
                        << " . Note that input step was " << htry[i] << std::endl;
           }
        }
// #endif        
        break;
      }
  } while (    itersLeft > 0
            && ( ! vecgeom::IsFull( stepSizeUnderflow || goodStep ) )
          );

  tot_no_trials += iter;

  
  if( vecgeom::Any(StepSizeUnderflow) )
  {
     std::cerr << "== Check after iteration loop: found these underflow lanes."
     ReportUnderflowLanes( stepSizeUnderflow, x, h, htry );     
  }
  
#ifdef PARTDEBUG
  if (partDebug)
  {
    cout << "FlexibleIntDrv: 1--step - Loop done at iter = " << iter << " with htry= " << htry <<std::endl;
  }
#endif 

  h         = hFinal;
  errmax_sq = errmax_sqFinal;

  // Compute size of next Step
  Real_v errPower = Vc::exp( (0.5*GetPowerGrow())* vecgeom::Log(errmax_sq));
  hnext = GetSafety() * h * errPower;
  // hnext = GetSafety()*vecgeom::Pow(errmax_sq, 0.5*GetPowerGrow());
  vecgeom::MaskedAssign(errmax_sq <= fErrcon*fErrcon, fMaxSteppingIncrease*h, &hnext); // No more than a factor of 5 increase

  x += (hdid = h);

  for(int k=0;k<fNoIntegrationVariables;k++) { y[k] = yFinal[k]; }

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
SimpleIntegrationDriver<Backend, T_Stepper, Nvar>::
   ReportUnderflowLanes( veccore::Mask_v<Real_v> problemLane, Real_v x, Real_v h, Real_v htry )
{
   std::cerr << "GVIntegratorDriver::OneStep:" << std::endl
             << "  Stepsize underflow in Stepper " << std::endl;
   for (int i = 0; i < kVectorSize; ++i)
   {
      if( problemLane[i] ) {
         std::cerr << " WARNING (SimpleIntegrationDriver::LockStep()> Underflow in lane " << i << endl
                   << "  Step's start x=" << x[i] << " and end x= " << xnew[i]
                   << " are equal !! "    << endl
                   <<"  Due to step-size= " << h[i]
                   << " . Note that input step was " << htry[i] << std::endl;
      }
   }
}

#if 0
void
ReportResults()
{
   // Method to report intermediate state of integration, including
   //   - error ratios
   //   - status of finishing, underflow, storing, ..
   //   - integrated variables 
   
   const int chName = 10;
   cout << setw( chName ) << "h" << " : ";
   for (int i = 0; i < kVectorSize; ++i) {
      cout << " " << setw( chValue ) << hFinal[i]         << " | ";
   }
   cout << endl;
   
   ReportDoubleRow( "errMaxSq", errmax_sqFinal );
   //
   cout << setw( chName ) << "errMaxSq" << " : "
   for (int i = 0; i < kVectorSize; ++i) {
      cout << " " << setw( chValue ) << errmax_sqFinal[i] << " | ";      
   }
   cout << endl;

   ReportDoubleRow( "errMaxSq", errmax_sqFinal );
}

template< typename Real_v >
void
ReportDoubleRow( std::str varName, Real_v var, int lenName, int lenValue )
{
   cout << setw( lenName ) << varName << " : "
   for (int i = 0; i < kVectorSize; ++i) {   
      cout << " " << setw( lenValue ) << var[i] << " | ";      
   }
   cout << endl;
}

template< typename Real_v >
void
ReportBoolRow( std::str varName, veccore::Mask_v<Real_v> var, int lenName, int lenValue)
{
   cout << setw( lenName ) << varName << " : "
   for (int i = 0; i < kVectorSize; ++i) {   
      cout << " " << setw( lenValue ) << var[i] << " | ";      
   }
   cout << endl;
}
#endif

// ----------------------------------------------------------------------------------

template <class Real_v, class T_Stepper, unsigned int Nvar>
bool 
SimpleIntegrationDriver<double>  // scalar
  ::AccurateAdvance(const TemplateFieldTrack<double>& yInput,  // scalar
                          double                      hstep,
                          double                      epsilon,
                          TemplateFieldTrack<double>& yOutput )
                          // Real_v  hinitial)
{
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

  typedef vecgeom::Vector3D<double>   ThreeVector;

  // std::cout<<"\n " << std::endl;
#ifdef COLLECT_STATISTICS
  constexpr double perMillion  = 1.0e-6;
  constexpr double perThousand = 1.0e-3;
  int no_warnings = 0;
#endif 

  int nstp, i;
  double x, hnext, hdid, h;

  constexpr int ncompSVEC = TemplateFieldTrack<double>::ncompSVEC; // 12 ? 

#ifdef GUDEBUG_FIELD
  static int dbg=1;
  static int nStpPr=50;   // For debug printing of long integrations
  double ySubStepStart[ncompSVEC];

  // std::cout << " AccurateAdvance called with hstep= " << hstep << std::endl;
#endif

  double  y     [ncompSVEC], 
            dydx  [ncompSVEC];
  double  ystart[ncompSVEC], 
            yEnd  [ncompSVEC]; 
  double  x1, x2;
  Bool_v succeeded(true), lastStepSucceeded;

  double startCurveLength;

  int  noFullIntegr=0, noSmallIntegr = 0 ;
  // G4ThreadLocal
  static int  noGoodSteps =0 ;  // Bad = chord > curve-len 
  const  int  nvar= fNoVars;


  //  Ensure that hstep > 0
  if( hstep <= 0.0 )
  { 
    if(hstep==0.0)
    {
      std::cerr << "Proposed step is zero; hstep = " << hstep << " !" << std::endl;
      return succeeded; 
    }
    else
    { 
      std::cerr << "Invalid run condition." << std::endl
                << "Proposed step is negative; hstep = " << hstep << "." << std::endl;
      return false;
    }
  }

  yInput.DumpToArray( ystart );

  startCurveLength= yInput.GetCurveLength();
  x1= startCurveLength; 
  x2= x1 + hstep; // = x + hstep

  h = hstep; // = x2 -x1 ; or x2 - x

  x = x1;


  for (i=0; i<fNoVars; i++)  { y[i] = ystart[i]; }

  bool lastStep = false;
  nstp=1;


  double StartPosAr[3];
  double charge(-1.);

  while ( ((nstp++)<=fMaxNoSteps) && (x < x2) && (!lastStep) )
  {
    StartPosAr[0]= y[0]; StartPosAr[1]= y[1];  StartPosAr[2]= y[2]; 
    
    fpStepper->RightHandSideVIS( y, charge, dydx );   // TODO: change to inline

#ifdef COLLECT_STATISTICS
    fNoTotalSteps++;
#endif 

    // Perform the Integration
    if( h > fMinimumStep )
    { 
      OneGoodStep(y,dydx,x,h,epsilon,hdid,hnext) ;

      //--------------------------------------
      lastStepSucceeded= (hdid == h);   

    }

#ifdef COLLECT_STATISTICS
    if (lastStepSucceeded)  { noFullIntegr++; }
    else                    { noSmallIntegr++; }
#endif 

    ThreeVector EndPos( y[0], y[1], y[2] );

    // Check the endpoint
    const double edx= y[0] - StartPosAr[0];
    const double edy= y[1] - StartPosAr[1];
    const double edz= y[2] - StartPosAr[2];
    double endPointDist2= vecgeom::Sqrt(edx*edx+edy*edy+edz*edz) ; // (EndPos-StartPos).Mag(); 

    //Ananya: discuss. What exactly is happening here?
    bool avoidNumerousSmallSteps = (h < epsilon * hstep) || (h < fSmallestFraction * startCurveLength);
    if (avoidNumerousSmallSteps ==1)
    {
      lastStep = true;
    }
    // vecgeom::MaskedAssign(avoidNumerousSmallSteps, true, &lastStep);

    //For rest, check the proposed next stepsize 
    h = vecgeom::Max(hnext, fMinimumStep);

    // Ensure that the next step does not overshoot
    if ((x+h) > x2 )
    {
      h = x2 -x ;
    }

    if (h==0)
    {
      lastStep = true;
    }
  } 
     // Have we reached the end ?
     // --> a better test might be x-x2 > an_epsilon

  succeeded=  (x>=x2);  // If it was a "forced" last step

  for (i=0;i<nvar;i++)  { yEnd[i] = y[i]; }

  // Put back the values.
  yOutput.LoadFromArray( yEnd, fNoIntegrationVariables );
  yOutput.SetCurveLength( x );

  return succeeded;
}  // end of AccurateAdvance ...........................


//----------------------------------------------------------------------

#define SQR(a)   ((a)*(a))

// QuickAdvance just tries one Step - it does not ensure accuracy
template <class Real_v, class T_Stepper, unsigned int Nvar>//
typename Mask<Backend>
SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
  ::QuickAdvance( TemplateFieldTrack<Backend>&       y_posvel,         // INOUT
                  const Real_v  dydx[],  
                        Real_v  hstep,       // In
                  // Real_v& dchord_step,
                        Real_v& dyerr_pos_sq,
                        Real_v& dyerr_mom_rel_sq )  
{
  // typedef typename Backend Real_v;
//  typedef typename Mask<Backend>      Bool_v;
  // Real_v dyerr_pos_sq, dyerr_mom_rel_sq;  
  Real_v yerr_vec[TemplateFieldTrack<Backend>::ncompSVEC],
           yarrin  [TemplateFieldTrack<Backend>::ncompSVEC], 
           yarrout [TemplateFieldTrack<Backend>::ncompSVEC]; 
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
  y_posvel.LoadFromArray( yarrout, fNoIntegrationVariables );
  y_posvel.SetCurveLength( s_start + hstep );

#ifdef  GUDEBUG_FIELD
  if(fVerboseLevel>2)
  {
    std::cout << "G4MagIntDrv: Quick Advance" << std::endl;
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
  dyerr_len = vecgeom::Sqrt( dyerr_len_sq ); 
  dyerr_len_sq /= epsilon ;

  // Set suggested new step
  hstep= ComputeNewStepSize( dyerr_len, hstep);
#endif

  return true;
}

// --------------------------------------------------------------------------
#ifdef QUICK_ADV_ARRAY_IN_AND_OUT
template <class Real_v, class T_Stepper, unsigned int Nvar>
typename Mask<Backend>  
SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
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
SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
  ::ComputeNewStepSize( Real_v  errMaxNorm,    // max error  (normalised)
                        Real_v  hstepCurrent)  // current step size
{
  Real_v hnew;

  // Compute size of next Step for a failed step
  hnew = GetSafety()*hstepCurrent*vecgeom::Pow(errMaxNorm,GetPowerShrink()) ;
  
  vecgeom::MaskedAssign(errMaxNorm <= 1.0 /*&& errMaxNorm >0.0*/, GetSafety()*hstepCurrent*vecgeom::Pow(errMaxNorm,GetPowerGrow()), &hnew );

  vecgeom::MaskedAssign(errMaxNorm <= 0.0, fMaxSteppingIncrease * hstepCurrent, &hnew);

  hnew = fMaxSteppingIncrease * hstepCurrent; 


/*  if(errMaxNorm > 1.0 )
  {
    hnew = GetSafety()*hstepCurrent*std::pow(errMaxNorm,GetPowerShrink()) ;
  } 
  else if(errMaxNorm > 0.0 ) {
    hnew = GetSafety()*hstepCurrent*std::pow(errMaxNorm,GetPowerGrow()) ;
  } 
  else {
    hnew = fMaxSteppingIncrease * hstepCurrent; 
  }*/

  return hnew;
}

// ---------------------------------------------------------------------------

// This method computes new step sizes limiting changes within certain factors
// 
// It shares its logic with AccurateAdvance.
// They are kept separate currently for optimisation.
//
template <class Real_v, class T_Stepper, unsigned int Nvar>
Real_v 
SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
  ::ComputeNewStepSize_WithinLimits( Real_v  errMaxNorm,    // max error  (normalised)
                                     Real_v  hstepCurrent)  // current step size
{
  Real_v hnew, htemp;

  hnew  = fMaxSteppingIncrease * hstepCurrent;

  htemp = GetSafety()*hstepCurrent*vecgeom::Pow(errMaxNorm,GetPowerShrink()) ; 
  //Size of next step for failed step
  vecgeom::MaskedAssign(errMaxNorm>1.0 && htemp > hnew, htemp, &hnew);

  htemp= GetSafety()*hstepCurrent*vecgeom::Pow(errMaxNorm,GetPowerGrow());
  //Size of next step for successful step
  vecgeom::MaskedAssign(errMaxNorm<=1.0 && errMaxNorm > fErrcon, htemp, &hnew);


/*
  // Compute size of next Step for a failed step
  if (errMaxNorm > 1.0 )
  {
    // Step failed; compute the size of retrial Step.
    hnew = GetSafety()*hstepCurrent*std::pow(errMaxNorm,GetPowerShrink()) ;
  
    if (hnew < fMaxSteppingDecrease*hstepCurrent)
    {
      hnew = fMaxSteppingDecrease*hstepCurrent ;
                         // reduce stepsize, but no more
                         // than this factor (value= 1/10)
    }
  }
  else
  {
    // Compute size of next Step for a successful step
    if (errMaxNorm > fErrcon)
     { hnew = GetSafety()*hstepCurrent*std::pow(errMaxNorm,GetPowerGrow()); }
    else  // No more than a factor of 5 increase
     { hnew = fMaxSteppingIncrease * hstepCurrent; }
  }*/

  return hnew;
}

 
// ---------------------------------------------------------------------------
template <class Real_v, class T_Stepper, unsigned int Nvar>
void SimpleIntegrationDriver<Backend, T_Stepper, Nvar>
  ::SetSmallestFraction(double newFraction)
{
  if( (newFraction > 1.e-16) && (newFraction < 1e-8) )
  {
    fSmallestFraction= newFraction;
  }
  else
  { 
    std::cerr << "Warning: SmallestFraction not changed. " << std::endl
           << "  Proposed value was " << newFraction << std::endl
           << "  Value must be between 1.e-8 and 1.e-16" << std::endl;
  }
}

#endif /* SimpleIntegrationDriver_Def */
