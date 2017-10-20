//
//
#include <iomanip>

#include "base/Vector3D.h"
#include <Geant/VectorTypes.h>

#include "GUVVectorEquationOfMotion.h"
#include "GUVVectorField.h"
#include "TVectorMagFieldEquation.h"
#include "GUVMagneticField.h"
#include "GUVVectorMagneticField.h"
#include "TVectorUniformMagField.h"

#include "GUVField.h"
#include "TMagFieldEquation.h"
#include "FieldEquationFactory.h"

// #include "TUniformMagField.h"

// #define CMS_FIELD 1

#ifdef CMS_FIELD
// #include "VecMagFieldRoutine/CMSmagField.h"
#include "CMSmagField.h"
#endif


using ThreeVector_f = vecgeom::Vector3D<float>;  // vecgeom::Vector3D<float>;
using ThreeVector_d = vecgeom::Vector3D<double>; // vecgeom::Vector3D<double>;

using Double_v = Geant::Double_v;
using Float_v  = Geant::Float_v;

// using ThreeVec_DblV = vecgeom::Vector3D<Double_v>;
// using ThreeVec_FltV = vecgeom::Vector3D<Float_v>; 

using std::cout;
using std::cerr;
using std::endl;
using std::setw;

GUVVectorEquationOfMotion* CreateUniformFieldAndEquation(ThreeVector_f constFieldValue);
GUVVectorEquationOfMotion* CreateFieldAndEquation(const char* filename);

bool  TestEquation(GUVVectorEquationOfMotion* );

constexpr unsigned int gNposmom= 6; // Position 3-vec + Momentum 3-vec

const char *defaultFieldFileName= "cmsmagfield2015.txt";

int
main( int, char** )
{
  ThreeVector_f  FieldValue(0.0, 0.0, 1.0);
   
  GUVVectorEquationOfMotion* eq = CreateUniformFieldAndEquation( FieldValue );
  bool okUniform = TestEquation(eq);
  bool good = okUniform;
  
#ifdef CMS_FIELD
  GUVVectorEquationOfMotion* eq2 = CreateFieldAndEquation( defaultFieldFileName ); // ("cmsMagneticField2015.txt");
  bool okCMSfield = TestEquation(eq2);

  good = good && okCMSfield;
#endif
  
  return good;
}

GUVVectorEquationOfMotion* CreateUniformFieldAndEquation(ThreeVector_f FieldValue)
{
  // using Field_t = TUniformMagField;

  auto *pConstBfield = new TVectorUniformMagField( FieldValue );

  // 1. Original way of creating an equation
  using EquationType = TVectorMagFieldEquation<TVectorUniformMagField, gNposmom>;
  auto   magEquation = new EquationType(pConstBfield);
  return magEquation;

  //  2. Different method of creating equation:  Factory
  // auto vecEquation = FieldEquationFactory::CreateMagEquation<TUniformMagField>(pConstBfield);  
  // return vecEquation;
}

#ifdef CMS_FIELD
GUVVectorEquationOfMotion* CreateFieldAndEquation(const char* filename)
{
  // const char *defaultFieldFileName= "cmsMagneticField2015.txt";
   
  //  3. Equation for CMS field
  auto cmsField = new CMSmagField( filename ? filename : defaultFieldFileName ); 
  auto equationCMS = FieldEquationFactory::CreateMagEquation<CMSmagField>(cmsField);

  return equationCMS;
}
#endif

int gVerbose = 0;

bool TestEquation(GUVVectorEquationOfMotion *equation)
{
   
  constexpr double perMillion = 1e-6;
  bool   hasError = false;  // Return value
  
  ThreeVector_d Position( 1., 2.,  3.);  // initial
  ThreeVector_d Momentum( 0., 0.1, 1.);
  ThreeVector_f FieldVecVal( 0., 0., 1.);  // Magnetic field value (constant)

  // double PositionTime[4] = { Position.x(), Position.y(), Position.z(), 0.0};
  Double_v PositionMomentum[gNposmom];
  Double_v dydxVec[gNposmom];
  Double_v chargeVec= Double_v(0.0);  // { -1.0, 1.0, 2.0, -2.0 } ; 

  // Revise the values, so that they are no longer equal
  vecCore::Set( chargeVec, 0, -1.0 );
  vecCore::Set( chargeVec, 1,  1.0 );
  vecCore::Set( chargeVec, 2, -2.0 );
  vecCore::Set( chargeVec, 3,  2.0 );
/***
  // Try to make the code portable - how to find width ?  ( number of lanes ) 
  if( Double_v::Width() >= 8 ) { 
    vecCore::Set( chargeVec, 4, -3.0);
    vecCore::Set( chargeVec, 5, -3.0);
    vecCore::Set( chargeVec, 6, -3.0);
    vecCore::Set( chargeVec, 7, -3.0);    
  }
  ***/ 

  // Input check
  //
  const bool printInput= false;
  if( printInput ) { 
     for ( int i = 0; i < 3; i ++ ) {
        PositionMomentum[i]   = Double_v( Position[i] );
        PositionMomentum[3+i] = Double_v( Momentum[i] );
     }
     for ( int i= 0; i<3; i++ )  
        cout << " pos["<<i<<"] = " << setw(6) << Position[i] << " PositionMomentum[] = " << PositionMomentum[i] << endl;
     for ( int i= 0; i<3; i++ )
        cout << " mom["<<i<<"] = " << setw(6) << Momentum[i] << " PositionMomentum[] = " << PositionMomentum[3+i] << endl;  
  
     cout << "Charge Vec = " << chargeVec << "  expected  -1, 1, -2, 2. "  << endl;
  }
  
  vecgeom::Vector3D<Float_v> FieldVec = { Float_v( FieldVecVal[0] ),
                                          Float_v( FieldVecVal[1] ),
                                          Float_v( FieldVecVal[2] ) };
  
  equation->EvaluateRhsGivenB( PositionMomentum, FieldVec, chargeVec, dydxVec );

  /****
  //   Simple printing for visual cross check of output
  cout << endl;
  cout << " ============================================ " << endl;
  cout << " Output - dy/dx Vec " << endl;
  cout << " dy/dx Vec [3] : "  << dydxVec[3] << endl;
  cout << " dy/dx Vec [4] : "  << dydxVec[4] << endl;
  cout << " dy/dx Vec [5] : "  << dydxVec[5] << endl;
  cout << " ============================================ " << endl;
  ****/
  
  // 1. Test each output individually first
  // ======================================
  const unsigned int laneWidth= 4;
  bool printContents= false;
  
  for( unsigned int lane= 0; lane < laneWidth; lane++ )
  // int lane= 0; 
  {
     double   dydxArr[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };   // To ensure zeroes at each iteration
     double      yArr[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
     
     for( int i= 0; i< 6; i++ ) { 
        dydxArr[i] = vecCore::Get ( dydxVec[i],          lane );
        yArr[i]    = vecCore::Get ( PositionMomentum[i], lane );
     }
     ThreeVector_d  ForceVec( dydxArr[3], dydxArr[4], dydxArr[5]);

     // PositionMomentum[3+i] = Double_v( Momentum[i] );

     if( printContents ) { 
        cout << " Vectors:      Y   dy/dx   ( lane = " << lane << " ) " << endl;
        for ( int i = 0; i < 6; i ++ ) {
           cout << "  [" << i << "] "
                << " " << setw(10) << yArr[i]  // PositionMomentum[i]
                << " " << setw(10) << dydxArr[i] << endl;
        }
     }

     // if( lane >= 0 ) continue;
     
     double charge= vecCore::Get( chargeVec, lane );

     // Check result
     double MdotF = Momentum.Dot(ForceVec);
     double BdotF = FieldVecVal.Dot(ForceVec);
     
     double momentumMag = Momentum.Mag();
     double fieldMag =    FieldVecVal.Mag();
     double sineAngle =   FieldVecVal.Cross( Momentum ).Mag() / ( momentumMag  * fieldMag );
     
     double ForceMag =   ForceVec.Mag();
     const double c = Constants::c_light;
     
     // Tolerance of difference in values (used below)
     double tolerance = perMillion;
  
     if ( gVerbose ) { cout << "Test output:  "  << endl; }
     if( std::fabs(ForceMag - c * std::fabs(charge) * fieldMag * sineAngle) >  tolerance * ForceMag ) {
        cerr << "ERROR: Force magnitude is not equal to   c * |charge| * |field| * sin( p, B )."  << endl;     
        cerr << "       Force magnitude = " << ForceMag << endl;
        cerr << "         other side =    " <<  c * std::fabs(charge) * fieldMag * sineAngle ; 
        cerr << " charge = " << charge 
             << " field-Mag= " << fieldMag  << endl;     
        cerr << "       Force = " << ForceVec[0] << " " << ForceVec[1] << " " << ForceVec[2] << " "  << endl;
     }
     
     assert( ForceMag != momentumMag * fieldMag );  // Must add coefficient !!
     
     if( std::fabs(MdotF) > tolerance * Momentum.Mag() * ForceVec.Mag() )
     { 
        cerr << "ERROR: Force due to magnetic field is not perpendicular to momentum!!"  << endl;
        hasError= true;
     }
     else if ( gVerbose )
     {
        cout << " Success:  Good (near zero) dot product momentum . force " << endl;
     }
     if( std::fabs(BdotF) > tolerance * FieldVecVal.Mag() * ForceVec.Mag() )
     { 
        cerr << "ERROR: Force due to magnetic field is not perpendicular to B field!"
             << std::endl; 
        cerr << " Vectors:  BField   Momentum   Force " << std::endl;
        for ( int i = 0; i < 3; i ++ )
           cerr << "  [" << i << "] "
                << " " << setw(10) << FieldVecVal[i]
                << " " << setw(10) << Momentum[i]              
                << " " << setw(10) << ForceVec[i] << std::endl;
        
        hasError = true;
     }
     else if ( gVerbose )
     {
        cout << " Success:  Good (near zero) dot product magnetic-field . force " << std::endl;
     }
  }
  
  return hasError;
}
