#ifndef GUVVectorMagneticField_H
#define GUVVectorMagneticField_H

#include "GUVVectorField.h"
#include "GUVField.h"
#include "GUVMagneticField.h"

/*
namespace vecfield {

VECGEOM_DEVICE_FORWARD_DECLARE(class GUVVectorField;);
VECGEOM_DEVICE_DECLARE_CONV(class, GUVVectorField);

inline namespace VECFIELD_IMPL_NAMESPACE {
*/

class GUVVectorMagneticField :  public GUVVectorField 
{
   using Double_v = Geant::Double_v;
      
   template <typename T>
   using Vector3D = vecgeom::Vector3D<T>;
  
  public:
    static constexpr int   sNumFieldComponents= 3;
    static constexpr bool  sFieldChangesEnergy= false;
  
    GUVVectorMagneticField():
     GUVVectorField( sNumFieldComponents, sFieldChangesEnergy) 
    { /* std::cout<<"--- GUVVectorMagneticField def c-tor called ---"<<std::endl; */ }

    virtual ~GUVVectorMagneticField(){}; 

    /***
    void  GetFieldValue( const Double_v  Point[4],     // The old interface
                               Double_v* Field );
    ****/
    VECCORE_ATT_HOST_DEVICE
    virtual void GetFieldValue( Vector3D<double> const &Position,
                                Vector3D<float>        &FieldValue ) = 0;

    virtual void GetFieldValueSIMD( Vector3D<Double_v>  const &Position, 
                                    Vector3D<Double_v>         &FieldValue ) = 0;

    GUVVectorMagneticField& operator = (const GUVVectorMagneticField &p);
    //  Copy 'standard' components ...
};

/*******
void
GUVVectorMagneticField::GetFieldValue( const Double_v  Point[4],     // The old interface
                                             Double_v* FieldArr )
{
   
   Vector3D<Double_v> PositionV3D( Point[0], Point[1], Point[2]);
   Vector3D<Float_v>  Field_v3f;
   this->GetFieldValue( PositionV3D, Field_v3f );
   FieldArr[0]= (Double_v) Field_v3f.x();
   FieldArr[1]= (Double_v) Field_v3f.y();
   FieldArr[2]= (Double_v) Field_v3f.z();
}
 ******/

/*
} // End inline namespace

} // End global namespace
*/

#endif
