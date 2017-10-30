//
// Embedded explicit Runge-Kutta Stepper using Cash Karp's RK tableau
//
// Adapted to remove 'backend' template interface: J. Apostolakis, Oct 2017
// Templated version:  Ananya, Feb/March 2016
//     ( commit 95e1316bcc156a04c876d6ea0fc9e60a15eeac4f )
// 
// Adapted from 'GUTCashKarpRKF45' by John Apostolakis, Nov 2015
//
// Adapted from 'GUTCashKarpRKF45' by Qieshen Xie, GSoC 2014
//         (derived from G4CashKarpRKF45)
//
//     
#ifndef GUVVECTORCASHKARPRKF45_H
#define GUVVECTORCASHKARPRKF45_H

#include "GUVectorLineSection.h"
#include "GUVVectorIntegrationStepper.h"

// #include "TMagErrorStepper.h" //for sake of GUIntegrationNms::NumVars
#include "TemplateTMagErrorStepper.h"
#include "AlignedBase.h"

#define INLINERHS 1
// #define DEBUGAnanya
#define RemoveAuxStepper
// #define NoPointers

#ifdef INLINERHS
#define REALLY_INLINE   inline __attribute__((always_inline))
#else
#define REALLY_INLINE   inline
#endif

template
<class T_Equation, unsigned int Nvar>
class VectorCashKarpRKF45 : public GUVVectorIntegrationStepper<Backend>, public AlignedBase
{
  // typedef                   typename Backend::precision_v  Double_v;
  // typedef vecgeom::Vector3D<typename Backend::precision_v> ThreeVectorSimd; 
  template <typename T>
  using Vector3D = vecgeom::Vector3D<T>;
  
  using Double_v        = Geant::Double_v;
  using ThreeVectorSimd = Vector3D<Double_v>;
  
  public:
    static constexpr unsigned int sOrderMethod= 4;
    static constexpr unsigned int sNstore = (GUIntegrationNms::NumVarBase > Nvar) ? GUIntegrationNms::NumVarBase : Nvar;
    // std::max( GUIntegrationNms::NumVarBase,  Nvar);
    // static const IntegratorCorrection = 1./((1<<4)-1);
    inline double IntegratorCorrection() { return 1./((1<<4)-1); }

  public:
    inline
    VectorCashKarpRKF45( T_Equation *EqRhs,
                              unsigned int numStateVariables=0,
                              bool primary=true);

    VectorCashKarpRKF45( const VectorCashKarpRKF45& );
    
    virtual ~VectorCashKarpRKF45();

    GUVVectorIntegrationStepper<Backend>* Clone() const override;

    REALLY_INLINE
    #ifdef NoPointers
    void StepWithErrorEstimate(const Double_v yInput[],    // Consider __restrict__
                               const Double_v  dydx[],
                                     Double_v  Step,
                                     Double_v  yOut[],
                                     Double_v  yErr[]) override;
   #else 
    void StepWithErrorEstimate(const Double_v* yInput,    // Consider __restrict__
                               const Double_v*  dydx,
                                     Double_v   Step,
                                     Double_v*  yOut,
                                     Double_v*  yErr) override;
    #endif 

    // Double_v DistChord() ;// override;  
    Double_v DistChord() const override ;

    REALLY_INLINE
    void RightHandSideInl(Double_v y[], Double_v dydx[]) 
    {fEquation_Rhs->T_Equation::RightHandSide(y, dydx);}

    void SetEquationOfMotion(T_Equation* equation);
    
    private:

    
      VectorCashKarpRKF45& operator=(const VectorCashKarpRKF45&) = delete;
        //private assignment operator.

    private:
        // 'Invariant' during integration - the pointers must not change
        // -----------
        T_Equation* fEquation_Rhs;
        bool        fOwnTheEquation; //enquire it's nature. If Bool_v , need to change if -> MaskedAssign
        #ifndef RemoveAuxStepper
        VectorCashKarpRKF45* fAuxStepper;
        #endif
        // State -- intermediate values used during RK step
        // -----        
        Double_v ak2[sNstore];
        Double_v ak3[sNstore];
        Double_v ak4[sNstore];
        Double_v ak5[sNstore];
        Double_v ak6[sNstore];
        Double_v ak7[sNstore];
        Double_v yTemp2[sNstore];  // Separate temporaries per step - to aid compiler
        Double_v yTemp3[sNstore];
        Double_v yTemp4[sNstore];
        Double_v yTemp5[sNstore];
        
        Double_v yIn[sNstore];
        // scratch space

        // State -- values used for subsequent call to DistChord
        // -----
        
        Double_v  fLastStepLength;
      #ifdef NoPointers
        Double_v  fLastInitialVector[sNstore];
        Double_v  fLastFinalVector[sNstore];
        Double_v  fLastDyDx[sNstore];
        Double_v  fMidVector[sNstore];
        Double_v  fMidError[sNstore];
      #endif 
        // for DistChord calculations
       #ifndef NoPointers
        Double_v* fLastInitialVector;
        Double_v* fLastFinalVector;
        Double_v* fLastDyDx;
        Double_v* fMidVector;
        Double_v* fMidError;
       #endif
        bool  fDebug= false;
};

template <class T_Equation, unsigned int Nvar>
inline
VectorCashKarpRKF45<T_Equation,Nvar>::
   VectorCashKarpRKF45( T_Equation *EqRhs,
                             unsigned int numStateVariables,
                             bool primary                  )
   : GUVVectorIntegrationStepper( EqRhs,    
                                  sOrderMethod,
                                  Nvar, //8, //Ananya
                                  ((numStateVariables>0) ? numStateVariables : sNstore) ),
     fEquation_Rhs(EqRhs),
     fOwnTheEquation(primary),
    #ifndef RemoveAuxStepper
     fAuxStepper(0),
    #endif
     fLastStepLength(0.)
{
   if( fDebug ) {
      std::cout<<"\n----Entered constructor of VectorCashKarpRKF45 "<<std::endl;
      std::cout<<"----In VectorCashKarpRKF45 constructor, Nvar is: "<<Nvar<<std::endl;
   }
   assert( dynamic_cast<TemplateGUVEquationOfMotion<Backend>*>(EqRhs) != 0 );


   assert( (numStateVariables == 0) || (numStateVariables >= Nvar) );
  
  #ifndef NoPointers
   fLastInitialVector = new Double_v[sNstore] ;
   fLastFinalVector   = new Double_v[sNstore] ;
   fLastDyDx          = new Double_v[sNstore];
   
   fMidVector = new Double_v[sNstore];
   fMidError  = new Double_v[sNstore];
  #endif

  #if 0
    std::cout<<"----Before Auxiliary stepper if condition"<<std::endl;
  #endif

  #ifndef RemoveAuxStepper
   if( primary )
   {
      // Reuse the Equation of motion in the Auxiliary Stepper      
      VectorCashKarpRKF45<T_Equation,Nvar> fAuxStepper2(EqRhs, numStateVariables, false);
      fAuxStepper = &fAuxStepper2;
      // fAuxStepper = new VectorCashKarpRKF45<T_Equation,Nvar>(EqRhs, numStateVariables, false);
      #ifdef DEBUGAnanya
      std::cout<<"----VectorCashKarpRKF45 Auxiliary Stepper being made"<<std::endl;
      #endif 
   }
  #endif 

   #if 0
    std::cout<<"----end of constructor of VectorCashKarpRKF45"<<std::endl;
   #endif
}

template <class T_Equation, unsigned int Nvar>
void VectorCashKarpRKF45<T_Equation,Nvar>::
  SetEquationOfMotion(T_Equation* equation)
{
   fEquation_Rhs= equation;
   this->GUVIntegrationStepper::SetABCEquationOfMotion(fEquation_Rhs);
}

//  Copy - Constructor
// 
template <class T_Equation,unsigned int Nvar>
inline
VectorCashKarpRKF45<T_Equation,Nvar>::
   VectorCashKarpRKF45( const VectorCashKarpRKF45& right )
   : GUVVectorIntegrationStepper( (GUVVectorEquationOfMotion*) nullptr,
                                              sOrderMethod,
                                              Nvar,
                                              right.GetNumberOfStateVariables() ),
     fEquation_Rhs( (T_Equation*) nullptr ),
     fOwnTheEquation(true),

     #ifndef RemoveAuxStepper
     fAuxStepper(0),   //  May overwrite below
     #endif 

     fLastStepLength(0.)
     // fPrimary( right.fPrimary )
{
   if( fDebug ) {
    std::cout<<"----Entered constructor of VectorCashKarpRKF45 "<<std::endl;
   }
   // if( primary )
   SetEquationOfMotion( new T_Equation( *(right.fEquation_Rhs)) );
   fOwnTheEquation=true;
    // fEquation_Rhs= right.GetEquationOfMotion()->Clone());
   
   assert( dynamic_cast<GUVVectorEquationOfMotion*>(fEquation_Rhs) != 0 );  
   assert( this->GetNumberOfStateVariables() >= Nvar);
  
  #ifndef NoPointers
   fLastInitialVector = new Double_v[sNstore] ;
   fLastFinalVector   = new Double_v[sNstore] ;
   fLastDyDx          = new Double_v[sNstore];
   
   fMidVector = new Double_v[sNstore];
   fMidError  = new Double_v[sNstore];
  #endif 

   if( fDebug )
      std::cout << " VectorCashKarpRKF45 - copy constructor: " << std::endl
                << " Nvar = " << Nvar << " Nstore= " << sNstore 
                << " Own-the-Equation = " << fOwnTheEquation << std::endl;

  #ifndef RemoveAuxStepper
   if( right.fAuxStepper )
   {
      // Reuse the Equation of motion in the Auxiliary Stepper
      fAuxStepper = new VectorCashKarpRKF45(fEquation_Rhs, this->GetNumberOfStateVariables(), false);
      std::cout<<"Auxiliary stepper made"<<std::endl;
   }
  #endif
}



template <class T_Equation,unsigned int Nvar>
REALLY_INLINE
VectorCashKarpRKF45<T_Equation,Nvar>::~VectorCashKarpRKF45()
{
#if 0
  std::cout<<"----- CashKarp destructor0"<<std::endl;
#endif


#ifndef NoPointers
   delete[] fLastInitialVector;
   delete[] fLastFinalVector;
   delete[] fLastDyDx;
   delete[] fMidVector;
   delete[] fMidError;
#if 0
  std::cout<<"----- CashKarp destructor0.5"<<std::endl;
#endif

  #ifndef RemoveAuxStepper
  std::cout<<fAuxStepper<<std::endl;
   delete fAuxStepper;
  #endif 

#if 0
  std::cout<<"----- CashKarp destructor1"<<std::endl;
#endif

   if( fOwnTheEquation )
      delete fEquation_Rhs; // Expect to own the equation, except if auxiliary (then sharing the equation)
#endif

#if 0
  std::cout<<"----- CashKarp destructor1"<<std::endl;
#endif

}

template <class T_Equation, unsigned int Nvar>
GUVVectorIntegrationStepper* 
VectorCashKarpRKF45<T_Equation,Nvar>::Clone() const
{
   // return new VectorCashKarpRKF45( *this );
   return new VectorCashKarpRKF45<T_Equation,Nvar>( *this );   
}


template <class T_Equation, unsigned int Nvar>
inline void
VectorCashKarpRKF45<T_Equation,Nvar>::
#ifdef NoPointers
  StepWithErrorEstimate(const Double_v  yInput[],    
                        const Double_v  dydx[],
                              Double_v   Step,
                              Double_v  yOut[],
                              Double_v  yErr[])
#else 
  StepWithErrorEstimate(const Double_v*  yInput, // [],    
                        const Double_v*  dydx, // [],
                              Double_v   Step,
                              Double_v*  yOut, // [],
                              Double_v*  yErr  ) // [])
#endif 
{
    // const double a2 = 0.2 , a3 = 0.3 , a4 = 0.6 , a5 = 1.0 , a6 = 0.875;
    unsigned int i;

    const double  b21 = 0.2 ,
          b31 = 3.0/40.0 , b32 = 9.0/40.0 ,
          b41 = 0.3 , b42 = -0.9 , b43 = 1.2 ,

          b51 = -11.0/54.0 , b52 = 2.5 , b53 = -70.0/27.0 ,
          b54 = 35.0/27.0 ,

          b61 = 1631.0/55296.0 , b62 =   175.0/512.0 ,
          b63 =  575.0/13824.0 , b64 = 44275.0/110592.0 ,
          b65 =  253.0/4096.0 ,

          c1 = 37.0/378.0 , c3 = 250.0/621.0 , c4 = 125.0/594.0 ,
          c6 = 512.0/1771.0 ,
          dc5 = -277.0/14336.0 ;

    const double dc1 = c1 - 2825.0/27648.0 ,  
          dc3 = c3 - 18575.0/48384.0 ,
          dc4 = c4 - 13525.0/55296.0 , 
          dc6 = c6 - 0.25 ;

    // Initialise time to t0, needed when it is not updated by the integration.
    //       [ Note: Only for time dependent fields (usually electric) 
    //                 is it neccessary to integrate the time.] 
    //yOut[7] = yTemp[7]   = yIn[7]; 

    //  Saving yInput because yInput and yOut can be aliases for same array
    for(i=0;i<Nvar;i++) 
    {
        yIn[i]=yInput[i];
    }
    // RightHandSideInl(yIn, dydx) ;              // 1st Step

    for(i=0;i<Nvar;i++) 
    {
        yTemp2[i] = yIn[i] + b21*Step*dydx[i] ;
    }
    this->RightHandSideInl(yTemp2, ak2) ;              // 2nd Step

    for(i=0;i<Nvar;i++)
    {
        yTemp3[i] = yIn[i] + Step*(b31*dydx[i] + b32*ak2[i]) ;
    }
    this->RightHandSideInl(yTemp3, ak3) ;              // 3rd Step

    for(i=0;i<Nvar;i++)
    {
        yTemp4[i] = yIn[i] + Step*(b41*dydx[i] + b42*ak2[i] + b43*ak3[i]) ;
    }
    this->RightHandSideInl(yTemp4, ak4) ;              // 4th Step

    for(i=0;i<Nvar;i++)
    {
        yTemp5[i] = yIn[i] + Step*(b51*dydx[i] + b52*ak2[i] + b53*ak3[i] +
                b54*ak4[i]) ;
    }
    this->RightHandSideInl(yTemp5, ak5) ;              // 5th Step

    {
      Double_v yTemp6[sNstore];                
      for(i=0;i<Nvar;i++)
      {
        yTemp6[i] = yIn[i] + Step*(b61*dydx[i] + b62*ak2[i] + b63*ak3[i] +
                b64*ak4[i] + b65*ak5[i]) ;
      }
      this->RightHandSideInl(yTemp6, ak6) ;              // 6th Step
    }
    
    for(i=0;i<Nvar;i++)
    {
        // Accumulate increments with proper weights

        yOut[i] = yIn[i] + Step*(c1*dydx[i] + c3*ak3[i] + c4*ak4[i] + c6*ak6[i]) ;
    }
    for(i=0;i<Nvar;i++)
    {
        // Estimate error as difference between 4th and
        // 5th order methods

        yErr[i] = Step*(dc1*dydx[i] + dc3*ak3[i] + dc4*ak4[i] +
                dc5*ak5[i] + dc6*ak6[i]) ;
#ifdef DEBUGAnanya
        // std::cout<< "----In Stepper, yerrr is: "<<yErr[i]<<std::endl;
#endif 
    }
    for(i=0;i<Nvar;i++)
    {
        // Store Input and Final values, for possible use in calculating chord
        fLastInitialVector[i] = yIn[i] ;
        fLastFinalVector[i]   = yOut[i];
        fLastDyDx[i]          = dydx[i];
    }
    fLastStepLength =Step;

    return ;
}

template <class T_Equation, unsigned int Nvar>
inline 
Double_v
VectorCashKarpRKF45<T_Equation,Nvar>::
  DistChord()  const
{
    Double_v        distLine, distChord; 
    ThreeVectorSimd initialPoint, finalPoint, midPoint;

    // Store last initial and final points (they will be overwritten in self-Stepper call!)
    initialPoint = ThreeVectorSimd( fLastInitialVector[0], 
                                    fLastInitialVector[1], fLastInitialVector[2]); 
    finalPoint   = ThreeVectorSimd( fLastFinalVector[0],  
                                    fLastFinalVector[1],  fLastFinalVector[2]); 

#ifndef RemoveAuxStepper
    // Do half a step using StepNoErr
  #ifdef NoPointers
    fAuxStepper->StepWithErrorEstimate( fLastInitialVector, 
                                        fLastDyDx, 
                                        0.5 * fLastStepLength, 
                                        fMidVector,   
                                        fMidError );
  #else 
    fAuxStepper->VectorCashKarpRKF45::StepWithErrorEstimate( fLastInitialVector, 
                                                                  fLastDyDx, 
                                                                  0.5 * fLastStepLength, 
                                                                  fMidVector,   
                                                                  fMidError );
  #endif 
    midPoint = ThreeVectorSimd( fMidVector[0], fMidVector[1], fMidVector[2]);       

    // Use stored values of Initial and Endpoint + new Midpoint to evaluate
    //  distance of Chord

    distChord  = GUVectorLineSection::Distline( midPoint, initialPoint, finalPoint );
  #endif
    return distChord;
}

#endif /*GUV Vector CashKARP_RKF45 */