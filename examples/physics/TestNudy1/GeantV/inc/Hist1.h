
#ifndef HIST_H
#define HIST_H

#include <iostream>

namespace userapplication {

/**
 * @brief Simple histogram object.
 *
 * @class   Hist1
 * @author  M Novak
 * @date    june 2016
 */

class Hist1 {
public:
  Hist1(double min, double max, int numbin);
  Hist1(double min, double max, double delta);

  void Fill(double x);
  void Fill(double x, double w);

  int GetNumBins() const { return fNumBins; }
  double GetDelta() const { return fDelta; }
  double *GetX() const { return fx; }
  double *GetY() const { return fy; }

  Hist1 &operator+=(const Hist1 &other);

private:
  double *fx;
  double *fy;
  double fMin;
  double fMax;
  double fDelta;
  int fNumBins;
};

} // namespace userapplication

#endif // HIST_H
