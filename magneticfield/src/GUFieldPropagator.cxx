//
//  Simple interface class to GUIntegrationDriver (with does Runge Kutta integration)
//   that follows the interface of TGeoHelix
//
#include <iostream>  // for  cout / cerr 

#include "GUFieldPropagator.h"

// #include "GUVEquationOfMotion.h"
#include "TMagFieldEquation.h"
#include "GUVIntegrationStepper.h"
#include "GUIntegrationDriver.h"

GUFieldPropagator::GUFieldPropagator(GUIntegrationDriver* driver, double eps)
  : fDriver(driver), fEpsilon(eps)
{
   fDriver = driver;
}

#include "TMagFieldEquation.h"
#include "TClassicalRK4.h"

// ToDo-s/ideas:
//  - Factory to create the Driver, Stepper and Equation

template<typename FieldType>  // , typename StepperType>
GUFieldPropagator::GUFieldPropagator(FieldType* magField, double eps, double hminimum)
   : fEpsilon(eps)
{
   constexpr int NumEq= 6;
   using  EquationType=  TMagFieldEquation<FieldType, NumEq>;
   
   int statVerbose= 1;
   auto *pEquation = new EquationType(magField, NumEq);
      // new TMagFieldEquation<FieldType,NumEq>(magField, NumEq);

   // auto stepper = new StepperType<GvEquationType,NumEq>(gvEquation);
   auto stepper =      new TClassicalRK4<EquationType,NumEq>(pEquation);      
   auto integrDriver = new GUIntegrationDriver( hminimum,
                                               stepper,
                                               NumEq,
                                               statVerbose);
   fDriver= integrDriver;
}

// Make a step from current point along the path and compute new point, direction and angle
// GEANT_CUDA_BOTH_CODE
bool
GUFieldPropagator::DoStep( ThreeVector const & startPosition, ThreeVector const & startDirection,
                                   int const & charge,             double const & startMomentumMag,
                                double const & step,
                           ThreeVector       & endPosition,
                           ThreeVector       & endDirection
         )
{
  // Do the work HERE
  GUFieldTrack yTrackIn( startPosition, 
                        startDirection * startMomentumMag,
                        // fCharge, 
                        0.0,  // time
                        0.0); // s_0  xo
  GUFieldTrack yTrackOut( yTrackIn );
  
  // Call the driver HERE
  fDriver->InitializeCharge( charge );
  bool goodAdvance=
     fDriver->AccurateAdvance( yTrackIn, step, fEpsilon, yTrackOut ); // , hInitial );

  // fInitialCurvature; 
  endPosition=  yTrackOut.GetPosition();
  endDirection= yTrackOut.GetMomentumDirection();
  return goodAdvance;
}

// static std::vector<GUFieldPropagator*> fFieldPropagatorVec;
// May change to c-array for CUDA ... but likely CPU only

/// --------------  GUFieldPropagatorPool ------------------------------------
// #include "GUFieldPropagatorPool.h"   // For now, not a separate file

// static
GUFieldPropagatorPool* 
GUFieldPropagatorPool::Instance()
{
   // A lock is REQUIRED for the next line - TODO
   static GUFieldPropagatorPool sInstance;

   return &sInstance;
}

#if 0
//// ---------------------  Postpone handling of multiple 
GUFieldPropagator* 
GUFieldPropagatorPool::CreateOrFind( int noNeeded ) // , void** banks )
{
  static int numberCreated= -1;
  static GUFieldPropagatorPool* pInstance= Instance();

  // A lock is REQUIRED for this section - TODO
  if( numberCreated < noNeeded)
  {
    Extend(noNeeded);
    assert( fFieldPropagatorVec.size() == noNeeded );
    // fNum = fFieldPropagatorVec.size();
    numberCreated= noNeeded;
  }
}

GUFieldPropagator* GUFieldPropagatorPool::GetPropagator(int num)
{
   assert(num>=0);
   assert(num< fFieldPropagatorVec.size());
  
   return fFieldPropagatorVec[num];
}
#endif

#if 0
void GUFieldPropagatorPool::Extend(int noNeeded)
{
    int num= fFieldPropagatorVec.size();
    while ( num < noNeeded )
    {
      //  if( (banks != 0) && (banks[num]!=0) )
      //  fFieldPropagatorVec.push( new(banks(num)) GUFieldPropagator() );
      //  else
      fFieldPropagatorVec.push_back( new GUFieldPropagator() );
    }
}
#endif 
