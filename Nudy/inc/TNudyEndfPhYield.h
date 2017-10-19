#ifndef TNudyEndfPhYield_H
#define TNudyEndfPhYield_H

#include "TNudyEndfRecoPoint.h"

namespace NudyPhysics {
	class TNudyEndfRecoPoint;
}

namespace Nudy {
	class TNudyEndfFile;
}

typedef std::vector<double> rowd;
typedef std::vector<int> rowi;
typedef std::vector<rowd> matrixd2;
#ifdef USE_ROOT
#include "Rtypes.h"
class TRandom3;
#endif


namespace NudyPhysics {
class TNudyEndfPhYield : public TNudyEndfRecoPoint {

public:
  TNudyEndfPhYield();
  TNudyEndfPhYield(Nudy::TNudyEndfFile *file);
  virtual ~TNudyEndfPhYield();

private:
  int NR, NP, LP, LF ;
  double ES, EG, Eint, Yk ;
  rowi nbt1, int1, lp, lpk1, lfk1, nrk1, npk1;                  // tab1 parameter
  rowd eint1, y1, es, esk1, egk1, eintk1, yk1 ;        			// incident energy, yield and tab1 parameters
  rowd esb, tpb, gpb ;        			// incident energy, yield and tab1 parameters
  double A, AWR, ABN, QX;                                      	// standard ENDF parameters
  rowd ein, einc;                                              	// incident energy
#ifdef USE_ROOT
  TRandom3 *fRnd;
#endif
  ClassDef(TNudyEndfPhYield, 1) // class for an ENDF fission yield reconstruction
};

} //namespace 
#endif