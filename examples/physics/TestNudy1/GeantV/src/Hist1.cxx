
#include "Hist1.h"

namespace userapplication {

Hist1::Hist1(double min, double max, int numbin) : fMin(min), fMax(max), fNumBins(numbin)
{
  fDelta = (fMax - fMin) / (numbin);
  fx     = new double[fNumBins];
  fy     = new double[fNumBins];
  for (int i = 0; i < fNumBins; ++i) {
    fx[i] = fMin + i * fDelta;
    fy[i] = 0.0;
  }
}

Hist1::Hist1(double min, double max, double delta) : fMin(min), fMax(max), fDelta(delta)
{
  fNumBins = (int)((fMax - fMin) / (delta)) + 1.0;
  fx       = new double[fNumBins];
  fy       = new double[fNumBins];
  for (int i = 0; i < fNumBins; ++i) {
    fx[i] = fMin + i * fDelta;
    fy[i] = 0.0;
  }
}

void Hist1::Fill(double x)
{
  int indx = (int)((x - fMin) / fDelta);
  if (indx < 0) {
    std::cerr << "\n ***** ERROR in Hist1::FILL  =>  x = " << x << " < fMin = " << fMin << std::endl;
    exit(1);
  }
  fy[indx] += 1.0;
}

void Hist1::Fill(double x, double w)
{
  int indx = (int)((x - fMin) / fDelta);
  if (indx < 0) {
    std::cerr << "\n ***** ERROR in Hist1::FILL  =>  x = " << x << " < fMin = " << fMin << std::endl;
    exit(1);
  }
  fy[indx] += 1.0 * w;
}

Hist1 &Hist1::operator+=(const Hist1 &other)
{
  for (int i = 0; i < fNumBins; ++i)
    fy[i] += other.fy[i];
  return *this;
}

} // namespace userapplication
