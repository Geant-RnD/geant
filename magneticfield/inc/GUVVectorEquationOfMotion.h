//
// class GUVVectorEquationOfMotion
//
// Class description:
//
// Abstract Base Class for the right hand size of the equation of
// motion of a particle in a field.

// History:

//     ( based on GUVEquationOfMotion by J.Apostolakis )
// -------------------------------------------------------------------

#ifndef GUV_VectorEquationOfMotion_H
#define GUV_VectorEquationOfMotion_H

#include <cassert>
#include <iostream>

#include "base/Vector3D.h"
#include <Geant/VectorTypes.h>

// #include "GUVTypes.hh"      // "globals.hh"
#include "GUVField.h"   // required in inline method implementations
#include "GUVVectorField.h"
#include "GUVEquationOfMotion.h"

class GUVVectorEquationOfMotion //: public GUVEquationOfMotion 
{
  
  using Double_v = Geant::Double_v;
  //using Float_v = Geant::Float_v;
   
  template <typename T>
  using Vector3D = vecgeom::Vector3D<T>;

  public:  // with description

     GUVVectorEquationOfMotion( GUVVectorField *Field, unsigned int verbose=0 );
     virtual ~GUVVectorEquationOfMotion();     
       // Constructor and virtual destructor. No operations, just checks

     virtual
     void EvaluateRhsGivenB( const  Double_v yVec[],
                             const  Vector3D<Double_v> B,  // Was double B[3],
                             const  Double_v charge, 
                                    Double_v dydx[]             ) const = 0;
       // Given the value of the  field "B", this function 
       // calculates the value of the derivative dydx.
       // --------------------------------------------------------
       // This is the _only_ function a subclass must define.
       // The other two functions use Rhs_givenB.
     
     inline void RightHandSide( const  Double_v y[],
                                       Double_v charge ,
                                       Double_v dydx[] ) const;
       // This calculates the value of the derivative dydx at y.
       // It is the usual enquiry function.
       // ---------------------------
       // It uses the virtual function EvaluateRhsGivenB

     void EvaluateRhsReturnB( const Double_v y[],
                                    Double_v dydx[],
                                    Double_v charge,
                                    Vector3D<Double_v> &Field ) const;
       // Same as RHS above, but also returns the value of B.
       // Should be made the new default ? after putting dydx & B in a class.

     void GetFieldValue( const Double_v Point[4],
                               Double_v Field[] )  const;
       // Obtain only the field - the stepper assumes it is pure Magnetic.
       // Not protected, because GUVRKG3_Stepper uses it directly.
     inline
     void GetFieldValue( const  Double_v                    Point[4],
                                Vector3D<Double_v>  &FieldValue ) const;

     inline
     void GetFieldValue( const Vector3D<Double_v> &Position,
                               Vector3D<Double_v>  &FieldValue ) const;

     const GUVVectorField* GetFieldObj() const {return fField;}
           GUVVectorField* GetFieldObj()       {return fField;}
     void                  SetFieldObj(GUVVectorField* pField){fField=pField;}

     bool         Initialised() const { return fInitialised; } 
     unsigned int GetId() const       { return fEquationId; }
     static unsigned int GetNumCreated() { return fNumObjectsCreated; }
     static unsigned int GetNumLive() { return fNumObjectsCreated - fNumObjectsDeleted; }


     friend std::ostream&
             operator<<( std::ostream& os, const GUVVectorEquationOfMotion& eq);


  public:
     static const unsigned int idxTime=3;  // Convention for location of time 't' in vector

  private:
     static unsigned int fNumObjectsCreated;
     static unsigned int fNumObjectsDeleted;
     // const int GUVmaximum_number_of_field_components = 24;
     enum { GUVmaximum_number_of_field_components = 24 } ;

     GUVVectorField *     fField;
     unsigned int   fEquationId;  //
     unsigned short fVerbose;
     bool           fInitialised;
};


inline
GUVVectorEquationOfMotion::GUVVectorEquationOfMotion(GUVVectorField* pField, unsigned int verbose)
   : // GUVEquationOfMotion(pField, verbose),
     fField(pField), 
     fEquationId(fNumObjectsCreated++),
     fVerbose(verbose), 
     fInitialised(false)
{
   if(fVerbose)
   {
     std::cout << " Called GUVVectorEquationOfMotion::InformDone() " << std::endl;
     std::cout << *this << std::endl;
   }
}

/*****
inline
void GUVVectorEquationOfMotion::InformReady() // was Initialize()
{
   fInitialised= true;
}

inline
void GUVVectorEquationOfMotion::InformDone()  // was Clear() and before Finished();
{
   if(fVerbose)
   {
     std::cout << " Called GUVVectorEquationOfMotion::InformDone() " << std::endl;
     std::cout << *this << std::endl;
   }
   assert( fInitialised );
   fInitialised= false;
}
****/

// !!!! INTERFACE TO BE CHANGED - INEFFICIENT !!!
inline
void GUVVectorEquationOfMotion::GetFieldValue( const Double_v Point[4],
                                                     Double_v Field[] ) const
{
   vecgeom::Vector3D<Double_v> Position( Point[0], Point[1], Point[2] );   
   vecgeom::Vector3D<Double_v>  FieldVec;
   fField-> GetFieldValueSIMD( Position, FieldVec );
   Field[0] = FieldVec[0];
   Field[1] = FieldVec[1];
   Field[2] = FieldVec[2];
}

inline
void GUVVectorEquationOfMotion::GetFieldValue( const Double_v     Point[4], // Was: const vecgeom::Vector3D<double> &Position,
                                               Vector3D<Double_v> &FieldValue                                               
   ) const   
{
   vecgeom::Vector3D<Double_v> Position( Point[0], Point[1], Point[2] );
   fField-> GetFieldValueSIMD( Position, FieldValue );
}

inline
void GUVVectorEquationOfMotion::GetFieldValue( const Vector3D<Double_v>  &Position,
                                                     Vector3D<Double_v> &FieldValue
   ) const
{
   fField-> GetFieldValue( Position, FieldValue );
}

inline
void
GUVVectorEquationOfMotion::RightHandSide( const Double_v y[],
                                                Double_v charge,
                                                Double_v dydx[] ) const
{
   //using ThreeVectorF = Vector3D<Float_v>;
   using ThreeVectorD = Vector3D<Double_v>;
   // CheckInitialization();

   ThreeVectorD  Field;
   ThreeVectorD  Position( y[0], y[1], y[2] );

   GetFieldValue( Position, Field );
   EvaluateRhsGivenB( y, Field, charge, dydx );
}

#endif /* GUV_VectorEquationOfMotion_DEF */
