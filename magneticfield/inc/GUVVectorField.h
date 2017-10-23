//===----------------------------------------------------------------------===//
/**
 * @file GUVVectorField.h
 * @brief  Abstract field class for Geant-V prototype
 * @author Ananya
 * @author John Apostolakis 
 * 
 * - Created:  Ananya  January 2016
 *     ( based on GUVField by J.Apostolakis )
 */
//===----------------------------------------------------------------------===//

//
// class GUVVectorField
//
// Class description:
//
// Abstract class for any kind of Field.
// It allows a vector field to be defined by implementing the inquiry function
// interface.
//
// The key method is GetFieldValue( vecgeom::Vector3D<double> const &Position,
//                   *************  vecgeom::Vector3D<float>        &FieldValue)
//                                  
// Given an input position 3-vector 'Position',
// this method must return the estimated value of the field in "FieldValue".
//
// A field must also specify whether it changes a track's energy:
//                    DoesFieldChangeEnergy()
//                    *********************
// A field must co-work with a corresponding Equation of Motion, to
// enable the integration of a particle's position, momentum and, 
// optionally, polarisation.
// -------------------------------------------------------------------

#ifndef GUVVECTORFIELD_HH
#define GUVVECTORFIELD_HH

// #include <vector>

#include <base/Global.h>     //  Defines Force_Inline, VectorBackend, .. 
#include <base/Vector3D.h>
#include <Geant/VectorTypes.h>
// #include "base/SOA3D.h"

// #include "GUVField.h"

/*
namespace vecfield {

VECCORE_DEVICE_FORWARD_DECLARE(class GUVVectorField;);
VECCORE_DEVICE_DECLARE_CONV(class, GUVVectorField);

inline namespace VECFIELD_IMPL_NAMESPACE {
*/

class GUVVectorField //  : public GUVField
{
   public:

      using Double_v = Geant::Double_v;
      using Float_v = Geant::Float_v;
   
      template <typename T>
      using Vector3D = vecgeom::Vector3D<T>;
   
      inline
      GUVVectorField( int NumberOfComponents, bool changesEnergy );
      inline
      GUVVectorField( const GUVVectorField &);
      virtual ~GUVVectorField();

      VECCORE_ATT_HOST_DEVICE
      virtual void GetFieldValue( Vector3D<double> const &Position,
                                  Vector3D<float>        &FieldValue ) = 0;

      //Vector interface
      virtual void GetFieldValueSIMD( Vector3D<Double_v> const &Position,
                                      Vector3D<Double_v>       &FieldValue ) = 0;

      // a helper tramponline to dispatch to DistanceToOut if type is not scalar
      template <typename S, typename T>
      VECCORE_FORCE_INLINE
      VECCORE_ATT_HOST_DEVICE
      void GetFieldValue(Vector3D<S> const &position, Vector3D<T> &fieldValue)
      {
         return GetFieldValueSIMD(position, fieldValue);
      }

      VECCORE_ATT_HOST_DEVICE
      bool DoesFieldChangeEnergy() const { return fChangesEnergy; }
      VECCORE_ATT_HOST_DEVICE
      int  GetNumberOfComponents() const { return fNumberOfComponents; } 

      GUVVectorField& operator = (const GUVVectorField &p); // Useful ?
      
      virtual GUVVectorField* Clone() const;

  private:
      const int  fNumberOfComponents;
      bool       fChangesEnergy; 
};

inline GUVVectorField::GUVVectorField( int numberOfComponents, bool changesEnergy )
   : fNumberOfComponents(numberOfComponents),
     fChangesEnergy(changesEnergy)
     //GUVField(numberOfComponents, changesEnergy)
{
   // std::cout<<"-- entered GUVVectorField  constructor--"<<std::endl;
}


inline GUVVectorField::GUVVectorField( const GUVVectorField &field) 
  :  fNumberOfComponents(field.fNumberOfComponents)
    //GUVField(field)
{
  fChangesEnergy= field.fChangesEnergy;
}

/*
} // End inline namespace

} // End global namespace
*/

#endif /* GUVFVECTORIELD_HH */
