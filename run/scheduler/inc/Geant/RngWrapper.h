#ifndef GEANTV_RNGWRAPPER_H
#define GEANTV_RNGWRAPPER_H

#include <VecCore/VecCore>
#include "Geant/VectorTypes.h"
#include <random>

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

class RngWrapper {
public:

  double uniform() { return fUniform(fRngEng); }

  double uniform(double a, double b) { return a + (b - a) * uniform(); }

  void uniform_array(size_t n, double *array, const double min = 0., const double max = 1.)
  {
    for (size_t i = 0; i < n; ++i) {
      array[i] = uniform(min, max);
    }
  }

  inline Double_v uniformV() { 
    Double_v tmp;
    for(int l = 0; l < kVecLenD; ++l){
      vecCore::Set(tmp,l,fUniform(fRngEngV[l]));
    }
    return tmp; 
  }

  double Gauss(double mean, double sigma) { return fGauss(fRngEng)*sigma + mean; }

private:
  std::minstd_rand fRngEng;
  std::minstd_rand fRngEngV[kVecLenD];
  std::uniform_real_distribution<double> fUniform;
  std::normal_distribution<double> fGauss;
};
}
}

#endif // GEANTV_VECRNGWRAPPER_H
