#ifndef TNudyEndfPhAng_H
#define TNudyEndfPhAng_H

#include "TNudyEndfRecoPoint.h"

namespace Nudy {
class TNudyEndfFile;
}

namespace NudyPhysics {
class TNudyEndfRecoPoint;
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

class TNudyEndfPhAng : public NudyPhysics::TNudyEndfRecoPoint {

public:
  TNudyEndfPhAng();
  TNudyEndfPhAng(Nudy::TNudyEndfFile *file);
  virtual double GetCos4(int elemid, int mt, double energyK);
  virtual int GetCos4Lct(int elemid, int mt);
  virtual ~TNudyEndfPhAng();

private:
  double recursionLinearLeg(int i, double x1, double x2, double pdf1, double pdf2);
  double recursionLinearProb(double x1, double x2, double pdf1, double pdf2);
  void fillPdf1d();
  void fillPdf2d();
  double A, AWR, ABN, QX; // standard ENDF parameters
  rowd ein, cos4, cdf, pdf, lCoef1;
  matrixd2 cos2d, pdf2d, cdf2d, lCoef, ein2d;
  matrixd3 cos3d, pdf3d, cdf3d;
  rowd cosFile4;
  rowd cosPdfFile4;
  rowd cosCdfFile4;
  rowint nbt1, int1;
  rowint MtNumbers; // MT numbers
  rowint MtLct;     // LCT numbers
  int nr, np;       // standard ENDF parameters
#ifdef USE_ROOT
  TRandom3 *fRnd;
#endif
#ifdef USE_ROOT
  ClassDef(TNudyEndfPhAng, 1) // class for an ENDF reconstruction
#endif
};

} // namespace
#endif
