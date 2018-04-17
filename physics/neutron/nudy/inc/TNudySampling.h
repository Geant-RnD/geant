#ifndef TNudySampling_H
#define TNudySampling_H

#include "TNudyParticleTest.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TGraph.h"
#define PI acos(-1.0)
#ifdef USE_ROOT
#include "Rtypes.h"
class TRandom3;

namespace NudyPhysics {
class TNudyEndfRecoPoint;
}
#endif


namespace NudyPhysics {
class TNudySampling {

public:
  TNudySampling();
  TNudySampling(TParticle *, NudyPhysics::TNudyEndfRecoPoint *recoPoint);
  virtual ~TNudySampling();

private:
  void GetSecParameter(TParticle *, NudyPhysics::TNudyEndfRecoPoint *recoPoint);
  void FillHisto(double icosLab, double isecEnergyLab);
  double kinematicNonRel(TParticle *particle, NudyPhysics::TNudyEndfRecoPoint *recoPoint);
  double kinematicRel(TParticle *particle, NudyPhysics::TNudyEndfRecoPoint *recoPoint);
  double kinematicGama(TParticle *particle, NudyPhysics::TNudyEndfRecoPoint *recoPoint);
  std::vector<double> crs;
  double kineticE;
  double cosCM = 0, cosLab = 0, secEnergyCM = 0, secEnergyLab = 0;
  double x[1000000], y[1000000];
  double ene[1000000], nu1[1000000], nu2[1000000], nu3[1000000];
  double residueA, residueZ;
  int elemId   = 0;
  int isel     = 0;
  int counter  = 0;
  int ecounter = 0;
  int LCT, MF, MT, MF4, MF5, MF6;
  int events;
  TH2D *h;
  TH2D *hist[21];
  TH1D *h1;
  TH1D *h2;
  TH1D *fissZ1[21];
  TH1D *fissA1[21];
  TH1D *fissA;
  TGraph *gr1;
  TGraph *gr[5];
#ifdef USE_ROOT
  TRandom3 *fRnd;
#endif

#ifdef USE_ROOT
  ClassDef(TNudySampling, 1) // class for sampling
#endif
};

} // namespace 
#endif
