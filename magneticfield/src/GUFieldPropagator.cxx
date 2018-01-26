//
//  Simple interface class to ScalarIntegrationDriver (with does Runge Kutta integration)
//   that follows the interface of TGeoHelix
//
#include <iostream>  // for  cout / cerr 

#include "GUFieldPropagator.h"

#include "VScalarEquationOfMotion.h"
#include "ScalarMagFieldEquation.h"
#include "VScalarIntegrationStepper.h"
#include "ScalarIntegrationDriver.h"

#include "GUTCashKarpRKF45.h"  //  ie ScalarCashKarp

#include "MagFieldEquation.h"
#include "CashKarp.h"
#include "FlexIntegrationDriver.h"
#include "SimpleIntegrationDriver.h"

// template <class Equation, unsigned int> using ScalarCashKarp= GUTCashKarpRKF45;

using ThreeVector = vecgeom::Vector3D<double>;

FlexIntegrationDriver*  GUFieldPropagator::fVectorDriver= nullptr;

//------------------------------------------------------------------------------------
GUFieldPropagator::GUFieldPropagator(ScalarIntegrationDriver* driver,
                                     double eps,
                                     FlexIntegrationDriver* flexDriver )
  : fScalarDriver(driver),
    fEpsilon(eps)
{
   const char* methodName="GUFieldPropagator constructor (scalarDriver, eps, *flex - with default = null)";
   std::cout << "------------------------------------------------------------------------------------" << std::endl;
   std::cout << methodName << std::endl
             << "  Arguments:  scalarDrv= " << driver << " eps = " << eps << " flexDriver = " << flexDriver
             << std::endl;
   std::cout << "  Existing vectorDriver (static) = " << fVectorDriver << std::endl;
   
   if ( !fVectorDriver && flexDriver ) {      
      SetFlexIntegrationDriver( flexDriver );
      std::cout << " Installing flex/vectorDriver = " << flexDriver << std::endl;
      std::cout << " Confirmed  address           = " << GetFlexibleIntegrationDriver() << std::endl;
      assert( flexDriver == GetFlexibleIntegrationDriver() ); 
   } else {
      if ( fVectorDriver && flexDriver )
         std::cout << "GUFieldPropagator> Not overwriting Vector/Flexible Driver" << std::endl;
      if ( !fVectorDriver && !flexDriver ) {               
         std::cout << "==============================================================================" << std::endl;
         std::cout << "WARNING: Gufieldpropagator> Not setting Vector/Flexible Driver: none provided." << std::endl;
         std::cout << "==============================================================================" << std::endl;
      }
   }

   std::cout << "GUFieldPropagator constructor> ptr= " << this << std::endl
             << " scalar driver = " << fScalarDriver
             << " flex/vector driver = " << flexDriver << std::endl;
}

//------------------------------------------------------------------------------------
GUFieldPropagator::~GUFieldPropagator()
{
   std::cout << "GUFieldPropagator destructor called for ptr= " << this << std::endl;
}

//------------------------------------------------------------------------------------
void GUFieldPropagator::SetFlexIntegrationDriver( FlexIntegrationDriver * flexDriver)
{
   const std::string methodName = "GUFieldPropagator::SetFlexIntegrationDriver";
   if ( !fVectorDriver && flexDriver ) {
      std::cout << "Replacing Vector/Flexible Driver" << std::endl;
      fVectorDriver = flexDriver;
   }
   else if ( !flexDriver )
   {
      std::cout << methodName // << "GUFieldPropagator::SetFlexIntegrationDriver",
         << "> Not overwriting with Vector/Flexible Driver" << std::endl;
   }
}

// ToDo-s/ideas:
//  - Factory to create the Driver, Stepper and Equation

//____________________________________________________________________________________
template<typename FieldType>  // , typename StepperType>
GUFieldPropagator::GUFieldPropagator(FieldType* magField, double eps, double hminimum)
   : fEpsilon(eps)
{
   constexpr unsigned int Nposmom = 6; // Number of Integration variables - 3 position, 3 momentum
  
// #if 0
   using  ScalarEquationType=  ScalarMagFieldEquation<FieldType, Nposmom>;
   int statVerbose= 1;
   auto *pEquation = new ScalarEquationType(magField, Nposmom);
      // new ScalarFieldEquation<FieldType,Nposmom>(magField, Nposmom);

   // auto stepper = new StepperType<ScalarEquationType,Nposmom>(gvEquation);
   auto scalarStepper =      new // ScalarCashKarp
           GUTCashKarpRKF45<ScalarEquationType,Nposmom>(pEquation);
   auto scalarDriver = new ScalarIntegrationDriver( hminimum,
                                                    scalarStepper,
                                                    Nposmom,
                                                    statVerbose);
   fScalarDriver= scalarDriver;
// #else
//    fScalarDriver= nullptr;
// #endif

   if( ! fVectorDriver ) {    // Share it between all instances
      
      // Create the flexible (vector or scalar) objects 
      using FlexEquationType = MagFieldEquation<FieldType>;
      auto  gvEquation = new FlexEquationType(magField);
      using FlexStepperType = CashKarp<FlexEquationType,Nposmom>;
      auto myFlexStepper = new FlexStepperType(gvEquation);
      int statsVerbose=1;
      auto flexDriver =
         new SimpleIntegrationDriver<FlexStepperType,Nposmom> (hminimum,
                                                               myFlexStepper,
                                                               Nposmom,
                                                               statsVerbose);
      fVectorDriver= flexDriver;
   }
}

// #ifdef FP_CLONE_METHOD
GUFieldPropagator* GUFieldPropagator::Clone() const 
{
    return new GUFieldPropagator( fScalarDriver->Clone(),
                                  fEpsilon );
}
// #endif

// Make a step from current point along the path and compute new point, direction and angle
// VECCORE_ATT_HOST_DEVICE                 
bool
GUFieldPropagator::DoStep( ThreeVector const & startPosition, ThreeVector const & startDirection,
                                   int const & charge,             double const & startMomentumMag,
                                double const & step,
                           ThreeVector       & endPosition,
                           ThreeVector       & endDirection
         )
{
  double notUsed = 1, notUsed2= 2;  // To investigate potential stack corruption ...
  const char *methodName ="GUFieldPropagator::DoStep";
  bool verbose= true;
  if( verbose ) {
     std::cout << methodName << " > this = " << this << std::endl;
     std::cout << methodName << "    junk = " << notUsed + notUsed2 << std::endl;
//     std::cout << methodName << " > fScalarDriver " << fScalarDriver << std::endl;
  }
//  assert( fScalarDriver );

// #if 1  
  // Try using the flexible driver first - for a single value ... ( to be improved )
  double     vals[6] = { startPosition.x(), startPosition.y(), startPosition.z(),
                         startDirection.x(), startDirection.y(), startDirection.z() };
  FieldTrack yTrackInFT( vals ), yTrackOutFT;

  double chargeFlt= charge;
  bool   okFlex= false;
  assert( fVectorDriver && "Vector Driver is not SET" );
  std::cout << methodName << " Dump of suspect arguments: " << std::endl;
  std::cout << "   Step       = " << step << std::endl;
  std::cout << "   yTrackInFT = " << yTrackInFT << std::endl;
  std::cout << "   Epsion     = " << fEpsilon   << std::endl;
  std::cout << "   Epsion     = " << fEpsilon   << std::endl;  
#if 0
// #ifdef EXTEND_SINGLE
  // Using the vector/flexible driver to integrate a single track -- EXPERIMENTAL   
  fVectorDriver->AccurateAdvance( yTrackInFT, step, chargeFlt, fEpsilon, yTrackOutFT, okFlex );
#else
  // Harnessing the vector driver to integrate just a single track -- EXPERIMENTAL   
  fVectorDriver->AccurateAdvance( &yTrackInFT, &step, &chargeFlt, fEpsilon, &yTrackOutFT, 1, &okFlex );
#endif  
  std::cout << " Results:  good = " << okFlex << " track out= " << yTrackOutFT << std::endl;
// #endif

    // Do the work HERE
  ScalarFieldTrack yTrackIn( startPosition, 
                        startDirection * startMomentumMag,
                        charge, 
                        0.0); // s_0  xo
  ScalarFieldTrack yTrackOut( yTrackIn );

  assert( fScalarDriver );
  // Simple call
  bool goodAdvance=
     fScalarDriver->AccurateAdvance( yTrackIn, step, fEpsilon, yTrackOut ); // , hInitial );

  endPosition=  yTrackOut.GetPosition();
  endDirection= yTrackOut.GetMomentumDirection();
  return goodAdvance;
}

VVectorField* GUFieldPropagator::GetField() 
{
   VVectorField* pField = nullptr;
   auto driver= GetScalarIntegrationDriver();
   if( driver ){
     auto equation= driver->GetEquationOfMotion();
     if( equation ) {
       pField = equation->GetFieldObj();
     }
   }
   return pField;
}

// static std::vector<GUFieldPropagator*> fFieldPropagatorVec;
// May change to c-array for CUDA ... but likely CPU only

