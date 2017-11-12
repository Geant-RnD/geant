#ifndef TNudyEndfNuPh_H
#define TNudyEndfNuPh_H

#include "TNudyEndfRecoPoint.h"

#define PI acos(-1.0)
typedef std::vector<double> rowd;
typedef std::vector<int> rowint;
#ifdef USE_ROOT
#include "Rtypes.h"
#endif

namespace Nudy {
	class TNudyEndfFile;
}

namespace NudyPhysics {
class TNudyEndfNuPh : public TNudyEndfRecoPoint {

public:
  TNudyEndfNuPh();
  TNudyEndfNuPh(Nudy::TNudyEndfFile *file);
  virtual double GetNuTotal(int elemid, double energyK);
  virtual double GetNuPrompt(int elemid, double energyK);
  virtual double GetNuDelayed(int elemid, double energyK);
  virtual double GetFissHeat(int elemid, double energyK);
  virtual double GetLambdaD(int elemid, int time);
  virtual ~TNudyEndfNuPh();

private:
  double recursionLinearNuPh(double x1, double x2, double sig1, double sig2, std::vector<double> x,
                             std::vector<double> sig);
  double A, AWR, ABN, QX; // standard ENDF parameters
  int NR, NP;             // standard ENDF parameters for range and interpolation
  rowd eintFile1, nutFile1, einFile1, nuFile1;
  rowd eindFile1, nudFile1, einphFile1, phFile1;
  rowd einfFile1, heatFile1;
  rowd cnc, nui;
  rowint nbt1, int1;
  double sigDiff;
  ClassDef(TNudyEndfNuPh, 1) // class for an ENDF reconstruction
};

} // namespace
#endif
