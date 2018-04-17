//===-- Nudy/TNudyEndfAng.h - Instruction class definition -------*- C++ -*-===//
//
//                     The Project Nudy
//===----------------------------------------------------------------------===//
///
/// \class TNudyEndfAng
/// \author H. Kumawat
/// \date July 2016
//===----------------------------------------------------------------------===//
#ifndef TNudyEndfAng_H
#define TNudyEndfAng_H

#include "TNudyEndfRecoPoint.h"

namespace NudyPhysics {
class TNudyEndfRecoPoint;
}

namespace Nudy {
class TNudyEndfFile;
}

typedef std::vector<double> rowd;
typedef std::vector<int> rowint;
typedef std::vector<rowint> matrixint;
typedef std::vector<rowd> matrixd2;
typedef std::vector<std::vector<rowd>> matrixd3;
typedef std::vector<std::vector<std::vector<rowd>>> matrixd4;
#define PI acos(-1.0)

#ifdef USE_ROOT
#include "Rtypes.h"
class TRandom3;
#endif

namespace NudyPhysics {
class TNudyEndfAng : public NudyPhysics::TNudyEndfRecoPoint {

public:
  TNudyEndfAng();
  /// \brief default constructure 
  TNudyEndfAng(Nudy::TNudyEndfFile *file);
  /// \brief constructure to be used
  virtual double GetCos4(int elemid, int mt, double energyK);
  /// \brief getter for hte cosine  of the neutron from the recopoint class based on element ID (serial no)
  /// MT no. which is based on the type of the raction and energy of the neutron
  virtual int GetCos4Lct(int elemid, int mt);
  /// \brief Getting the cm or lab frame flag for the sampling
  virtual ~TNudyEndfAng();

private:
  double RecursionLinearLeg(int i, double x1, double x2, double pdf1, double pdf2);
  /// \brief linearization of angular distribution if it is given in terms if the legendre polynomial
  double RecursionLinearProb(double x1, double x2, double pdf1, double pdf2);
  /// \brief linearization of angular distribution if it is given in terms if the probability distribution
  void FillPdf1D();
  /// \brief filling 1 dimentional pdf for anglular distribution
  void FillPdf2D();
  /// \brief filling 2 dimentional pdf for anglular distribution
  rowd fEin, fCos4, fCdf, fPdf, fLegendCoef1;
  /// \brief Temp. variables for cosine, cdf, cdf and legendre coefficient for angular distribution  
  matrixd2 fCos2D, fPdf2D, fCdf2D, fLegendCoef, fEin2D;
  /// \brief Temp. variables for cosine, pdf, cdf and legendre coefficient and energy for angular distribution    
  matrixd3 fCos3D, fPdf3D, fCdf3D;
  /// \brief Temp. variables for cosine, pdf, cdf for angular distribution    
  rowd fCosFile4, fCosPdfFile4, fCosCdfFile4;
  /// \brief cosine, pdf and cdf for angular distribution
  rowint fNbt1, fInt1;
  /// \brief standard ENDF interpolation parameter \cite ENDF Manual
  rowint fMtNumbers; 
  /// \brief temp. MT numbers
  rowint fMtLct;     
  /// \brief temp. LCT numbers (flag for angular distribution in cm or lab system)
  int fNr, fNp;       
  /// \brief standard ENDF parameters for no .of regions and points for interpolation
#ifdef USE_ROOT
  TRandom3 *fRnd;
#endif
#ifdef USE_ROOT
  ClassDef(TNudyEndfAng, 1) // class for an ENDF reconstruction
#endif
};

} // namespace 
#endif
