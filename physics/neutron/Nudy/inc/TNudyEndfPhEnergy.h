#ifndef TNudyEndfPhEnergy_H
#define TNudyEndfPhEnergy_H

#include "TNudyEndfRecoPoint.h"

namespace Nudy {
	class TNudyEndfFile;
}

namespace NudyPhysics {
	class TNudyEndfRecoPoint;
}

#define PI acos(-1.0)
typedef std::vector<double> rowd;
typedef std::vector<int> rowint;
typedef std::vector<rowd> matrixd2;
typedef std::vector<std::vector<rowd>> matrixd3;

#ifdef USE_ROOT
#include "Rtypes.h"
class TRandom3;
#endif


namespace NudyPhysics {
class TNudyEndfPhEnergy : public NudyPhysics::TNudyEndfRecoPoint {

public:
  TNudyEndfPhEnergy();
  TNudyEndfPhEnergy(Nudy::TNudyEndfFile *file);
  virtual double GetEnergy5(int elemid, int mt, double energyK);
  virtual double GetDelayedFraction(int ielemId, int mt, double energyK);
  virtual ~TNudyEndfPhEnergy();

private:
  double recursionLinearFile5Prob(double x1, double x2, double pdf1, double pdf2);
  double recursionLinearFile5GenEva(double x1, double x2, double pdf1, double pdf2, double energy);
  double recursionLinearFile5Maxwell(double x1, double x2, double pdf1, double pdf2, double energy);
  double recursionLinearFile5Watt(double x1, double x2, double pdf1, double pdf2, double energy);
  void fillPdf1d();

  double A, AWR, ABN, QX;     // standard ENDF parameters
  int NR, NP;                 // standard ENDF parameters for range and interpolation
  double fQValue[999];         // ENDF parameter and Q values from file 3
  double sigDiff;             // precision for cross-section reconstruction
  std::vector<int> MtNumbers; // MT numbers temporary
  rowd fE1, fP1, fE2, fP2, fE3, fP3, INorm;
  std::vector<int> nbt1, int1;
  int nr1, np1; // standard ENDF parameters
  std::vector<int> nbt2, int2;
  int nr2, np2; // standard ENDF parameters
  std::vector<int> nbt3, int3;
  int nr3, np3; // standard ENDF parameters
  rowd energyFile5;
  rowd energyPdfFile5;
  rowd energyCdfFile5;
  rowd ein, eneE, cdf, pdf;
  matrixd2 ene2d, frac2d, cdf2d, pdf2d, ein2d;
  matrixd3 ene3d, cdf3d, pdf3d;
#ifdef USE_ROOT
  TRandom3 *fRnd;
#endif
  ClassDef(TNudyEndfPhEnergy, 1) // class for an ENDF energy reconstruction
};

} // namespace 
#endif
