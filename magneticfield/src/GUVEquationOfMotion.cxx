//
// $Id: GUVEquationOfMotion.cc 66356 2012-12-18 09:02:32Z gcosmo $
//
// -------------------------------------------------------------------

// #include <iostream>

#include "GUVEquationOfMotion.h"

unsigned int GUVEquationOfMotion::fNumObjectsCreated= 0;
unsigned int GUVEquationOfMotion::fNumObjectsDeleted= 0;

GUVEquationOfMotion::~GUVEquationOfMotion()
{
   CheckDone();
   // std::cout << " Destructing Equation " << this << " info= " << *this << std::endl;   
   fNumObjectsDeleted++;
   // To help ensure that clients call InformDone() - ie. clear
}

void
GUVEquationOfMotion::
EvaluateRhsReturnB( const double           y[],
                          double          dydx[],
                       // double          charge,
                    vecgeom::Vector3D<double> &field
                  ) const
{
   using ThreeVector = vecgeom::Vector3D<double>;
   
   GetFieldValue( ThreeVector(y[0], y[1], y[2]), field) ;
   EvaluateRhsGivenB( y, field, /*charge,*/ dydx );
}

std::ostream&  operator<<( std::ostream& os, const GUVEquationOfMotion& eq)
{
   os << " Equation of Motion # " << eq.GetId()
      << "   field ptr= "  << eq.GetFieldObj() << "  Initialised= " << eq.Initialised()
      << std::endl;
   os << "  Total # of E-of-M = " << GUVEquationOfMotion::GetNumCreated()
      << " live= " << GUVEquationOfMotion::GetNumLive() << std::endl;
   return os;
}
