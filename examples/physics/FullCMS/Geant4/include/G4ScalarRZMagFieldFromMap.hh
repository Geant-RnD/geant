#ifndef G4SCALARRZMAGFIELDFROMMAP_H_
#define G4SCALARRZMAGFIELDFROMMAP_H_

// #include <vector>
#include "SystemOfUnits.h"

#include "G4MagneticField.h"

// typedef vecgeom::Vector3D<double> Vector3D;
// typedef vecgeom::SOA3D<double> SOA3D;

class G4ScalarRZMagFieldFromMap : public G4MagneticField
{
public:
  G4ScalarRZMagFieldFromMap();
  G4ScalarRZMagFieldFromMap(std::string inputMap);
  G4ScalarRZMagFieldFromMap(const G4ScalarRZMagFieldFromMap &right);

  // New stuff
  // Takes as input x,y,z; Gives output Bx,By,Bz
  void GetFieldValueXYZ(const G4ThreeVector &position, G4ThreeVector &xyzField);

  // Stores rz field as well for cross checking purpose
  void GetFieldValueTest(const G4ThreeVector &position, G4ThreeVector &rzField);

  // @brief   evaluate field value at input position (SOA3D)
  void ObtainFieldValue(const G4ThreeVector &position,
                        G4ThreeVector &field) override final
  { GetFieldValueXYZ(position, field); }

  // @brief   extra method for an arbitrary length SOA3D of positions
  void GetFieldValues(const vecgeom::SOA3D<double> &posVec, vecgeom::SOA3D<double> &fieldVec);
  
  // Reads data from given 2D magnetic field map. Can be easily modified to read a given 2D map, in case the file
  // changes
  void ReadVectorData(std::string inputMap);

  ~G4ScalarRZMagFieldFromMap();

  /** @brief For old interface - when cloning was needed for each thread */
  G4ScalarRZMagFieldFromMap* Copy() const { return new G4ScalarRZMagFieldFromMap(*this); }  
   
  virtual VVectorField* Clone() const override final { return Copy(); }

  G4ScalarRZMagFieldFromMap* CloneOrSafeSelf(bool *isSafe) {
     if( isSafe ) *isSafe = false;
     return Copy();
  }

  
private:
  //  Invariants -- parameters of the field
  const double millimeter = 1.0; // Currently -- to be native GeantV

  const double kRMax     = 9000. * millimeter;  //  Maximum value of R =  9.00 meters
  const double kZMax     = 16000. * millimeter; //  Max value of Z = 16.00 meters
  const int kNoZValues   = 161;
  const int kNoRValues   = 181;
  const int kHalfZValues = 80;

  // Derived values
  // kRDiff and kZDiff take care of mm because they come from kRMax and kZMax which have mm in them
  const double kRDiff = kRMax / (kNoRValues - 1);     //  Radius increment between lattice points
  const double kZDiff = 2 * kZMax / (kNoZValues - 1); //  Z increment

  const double kZ0       = -kZMax;
  const double kRDiffInv = 1.0 / kRDiff;
  const double kZDiffInv = 1.0 / kZDiff;
  const double kAInverse = geant::units::tesla / (kRDiff * kZDiff);  // Values in file assumed in tesla

  // For (R,Z) pairs : gives field in cylindrical coordinates in rzfield
  void GetFieldValueRZ(const double radius, const double z, G4ThreeVector &rzField);

  // Used to convert cartesian coordinates to cylindrical coordinates R-Z-phi
  //  Does not calculate phi
  inline void CartesianToCylindrical(const G4ThreeVector &cart, double cyl[2]);

  // Converts cylindrical magnetic field to field in cartesian coordinates
  inline void CylindricalToCartesian(const G4ThreeVector &B_cyl, const double sinTheta, const double cosTheta,
                                     G4ThreeVector &B_cart);

private:
  std::vector<double> fRadius, fPhi, fZ, fBr, fBz, fBphi;
};

inline void G4ScalarRZMagFieldFromMap::CartesianToCylindrical(const G4ThreeVector &cart, double cyl[2])
{
  // cyl[3] =[r,z,phi]
  cyl[0] = sqrt(cart.x() * cart.x() + cart.y() * cart.y()); // r = sqrt(x^2 + y^2)
  cyl[1] = cart.z();                                        // z = z
}

inline void G4ScalarRZMagFieldFromMap::
   CylindricalToCartesian(const G4ThreeVector & rzField,
                          const double          sinTheta,
                          const double          cosTheta,
                                G4ThreeVector & xyzField )
{
  // B_cyl[] components are:   r, phi and z  ( in order 0, 1, 2  or corresponding to 'x', 'y', 'z' )
  G4double  fldX = rzField.x() * cosTheta - rzField.y() * sinTheta; // Bx= Br cos(theta) - Bphi sin(theta)
  G4double  fldY = rzField.x() * sinTheta + rzField.y() * cosTheta; // By = Br sin(theta) + Bphi cos(theta)
  // xyzField.SetZ ( rzField.z() );                                 // Bz = Bz  
  xyzField = G4ThreeVector( fldX, fldY, rzField.z() );
}

#endif
