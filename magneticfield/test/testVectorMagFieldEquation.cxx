//
//
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


using ThreeVector_f = vecgeom::Vector3D<float>;
using ThreeVector_d = vecgeom::Vector3D<double>;

using std::cout;
using std::cerr;
using std::endl;

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

int gVerbose = 1;
using Double_v = Geant::Double_v;
using Float_v  = Geant::Float_v;

// using Set = vecCore::Set;
// using Get = vecCore::Get;

bool TestEquation(GUVVectorEquationOfMotion *equation)
{
   
  constexpr double perMillion = 1e-6;
  bool   hasError = false;  // Return value
  
  ThreeVector_d PositionVec( 1., 2.,  3.);  // initial
  ThreeVector_d MomentumVec( 0., 0.1, 1.);
  ThreeVector_f FieldVec( 0., 0., 1.);  // Magnetic field value (constant)

  // double PositionTime[4] = { PositionVec.x(), PositionVec.y(), PositionVec.z(), 0.0};
  Double_v PositionMomentum[gNposmom];
  Double_v dydxVec[gNposmom];
  Double_v chargeVec= Double_v(0.0);  // { -1.0, 1.0, 2.0, -2.0 } ; 

  for ( int i = 0; i < 3; i ++ ) {
     PositionMomentum[i]   = Double_v( PositionVec[i] );
     PositionMomentum[3+i] = Double_v( MomentumVec[i] );
  }

  // Revise the values, so that they are no longer equal
  // Set( &chargeVec, 0, -1.0 );
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

  equation->EvaluateRhsGivenB( PositionMomentum, FieldVec, chargeVec, dydxVec );

  // Test one output first
  //
  int      lane= 0;
  double   dydxArr[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  
  for( int i= 0; i< 6; i++ ) { 
     dydxArr[i] = vecCore::Get ( dydxVec[lane], i );
  }
  ThreeVector_d  ForceVec( dydxArr[3], dydxArr[4], dydxArr[5]);

  double charge= vecCore::Get( chargeVec, lane );
  
  // Check result
  double MdotF = MomentumVec.Dot(ForceVec);
  double BdotF = FieldVec.Dot(ForceVec);

  double momentumMag = MomentumVec.Mag();
  double fieldMag =    FieldVec.Mag();
  double sineAngle =   FieldVec.Cross( MomentumVec ).Mag() / ( momentumMag  * fieldMag );

  double ForceMag =   ForceVec.Mag();
  const double c = Constants::c_light;

  // Tolerance of difference in values (used below)
  double tolerance = perMillion;
  
  if ( gVerbose ) { std::cout << "Test output:  "  << std::endl; }
  if( std::fabs(ForceMag - c * std::fabs(charge) * fieldMag * sineAngle) >  tolerance * ForceMag ) {
     cerr << "ERROR: Force magnitude is not equal to   c * |charge| * |field| * sin( p, B )."  << endl;     
     cerr << "       Force magnitude = " << ForceMag << endl;
     cerr << "         other side =    " <<  c * std::fabs(charge) * fieldMag * sineAngle ; 
     cerr << " charge = " << charge 
               << " field-Mag= " << fieldMag  << std::endl;     
     cerr << "       Force = " << ForceVec[0] << " " << ForceVec[1] << " " << ForceVec[2] << " "  << endl;
  }
     
  assert( ForceMag != momentumMag * fieldMag );  // Must add coefficient !!

  if( std::fabs(MdotF) > tolerance * MomentumVec.Mag() * ForceVec.Mag() )
  { 
     cerr << "ERROR: Force due to magnetic field is not perpendicular to momentum!!"  << endl;
     hasError= true;
  }
  else if ( gVerbose )
  {
     cout << " Success:  Good (near zero) dot product momentum . force " << endl;
  }
  if( std::fabs(BdotF) > tolerance * FieldVec.Mag() * ForceVec.Mag() )
  { 
    cerr << "ERROR: Force due to magnetic field is not perpendicular to B field!"
              << std::endl; 
    cerr << " Vectors:  BField   Force " << std::endl;
    for ( int i = 0; i < 3; i ++ )
       cerr << "   [" << i << "] " << FieldVec[i] << " " << ForceVec[i] << std::endl;

    hasError = true;
  }
  else if ( gVerbose )
  {
    cout << " Success:  Good (near zero) dot product magnetic-field . force " << std::endl;
  }

  return hasError;
}
