
#ifndef ALIASTABLE_H
#define ALIASTABLE_H

#include <cmath>

namespace geantphysics {
/**
 * @brief   Walker's alias sampling based utility for sampling from continuous distributions.
 * @class   AliasTable
 * @author  M Novak, A Ribon
 * @date    january 2016
 *
 * Available sampling methods are alias table based bin indientification combined with either linear approximation of
 * the p.d.f. or rational interpolation based numerical inversion of the c.d.f..
 *
 */

class AliasTable {
public:
  AliasTable() {}
  ~AliasTable() {}

  /**
    * @brief Public method to prepare sampling table for discretized continuous distribution with combination of alias
    *        sampling and rational interpolation based numerical inversion of the c.d.f. \cite salvat2006penelope.
    *
    * @param[in,out] xdata      Array of discrete samples of the random variable between its minimum and maximum values.
    *                           Size of the array must be provided in  numdata. It stays unchanged at output.
    * @param[in,out] ydata      Array of the (not necessarily normaised) p.d.f. at the discrete sample points of the
    *                           random variable given in xdata. It's used only for the preparation but not used at
    *                           sampling. The p.d.f. will be normaised at output. Size of the array must be provided in
    *                            numdata.
    * @param[in,out] comf       Array to store the cumulative distribution function at the discrete sample points of the
    *                           random variable given in xdata. At input it should be  numdata size array and it
    *                           will contain the cumulative distribution function values at the discrete sample points
    *                           of the random variable.
    * @param[in,out] paradata   Array to store parameters of the rational interpolation based inversion. At input it
    *                           should be  numdata size array and it will contain the parameter values at the
    *                           discrete sample points of the random variable.
    * @param[in,out] parbdata   Array to store parameters of the rational interpolation based inversion. At input it
    *                           should be  numdata size array and it will contain the parameter values at the
    *                           discrete sample points of the random variable.
    * @param[in,out] xx         Array to store 1-alias probabilities. At input it should be  numdata size array and it
    *                           will contain the 1-alias probabilities at output.
    * @param[in,out] binindx    Array to store alias indices. At input it should be  numdata size array and it
    *                           will contain the alias indices at output.
    * @param[in,out] numdata    Number of discrete sample points of the random variable i.e. size of the arrays.
    *
    */
  void PreparRatinTable(double *xdata, double *ydata, double *comf, double *paradata, double *parbdata, double *xx,
                        int *binindx, int numdata, bool isconstraintspline = false, int glnum = 16);

  /**
    * @brief Public method to prepare sampling table for discretized continuous distribution with combination of alias
    *        sampling and linear approximation of the p.d.f..
    *
    *
    * @param[in,out] xdata      Array of discrete samples of the random variable between its minimum and maximum values.
    *                           Size of the array must be provided in  numdata. It stays unchanged at output.
    * @param[in,out] ydata      Array of the (not necessarily normaised) p.d.f. at the discrete sample points of the
    *                           random variable given in xdata. It will be used at sampling as well. Size of the
    *                           array must be provided in  numdata.
    * @param[in,out] xx         Array to store 1-alias probabilities. At input it should be  numdata size array and it
    *                           will contain the 1-alias probabilities at output.
    * @param[in,out] binindx    Array to store alias indices. At input it should be  numdata size array and it
    *                           will contain the alias indices at output.
    * @param[in,out] numdata    Number of discrete sample points of the random variable i.e. size of the arrays.
    *
    */
  void PreparLinearTable(double *xdata, double *ydata, double *xx, int *binindx, int numdata);

  /**
    * @brief Public method to prepare sampling table for discrete distribution with alias sampling.
    *
    *
    * @param[in,out] ydata      Array of the (not necessarily normaised) p.d.f. at the discrete random variable values.
    *                           It stays unchanged at output and not used at sampling.
    * @param[in,out] xx         Array to store 1-alias probabilities. At input it should be  numdata size array and it
    *                           will contain the 1-alias probabilities at output.
    * @param[in,out] binindx    Array to store alias indices. At input it should be  numdata size array and it
    *                           will contain the alias indices at output.
    * @param[in,out] numdata    Number of discrete random variable values i.e. size of the arrays.
    *
    */
  void PreparDiscreteTable(double *ydata, double *xx, int *binindx, int numdata);

  /**
    * @brief Public method to obtain random variable from continuous distribution using discrete samples and the
    *        combination of alias sampling with rational interpolation based numerical inversion of the c.d.f.. All
    *        input data arrays must have been prepared previously by AliasTable::PreparRatinTable method.
    *
    * See more at AliasTable::PreparRatinTable.
    *
    * @param[in]     xdata      Array of discrete samples of the random variable between its minimum and maximum values.
    *                           Size of the array must be provided in  numdata. It stays unchanged at output.
    * @param[in]     comf       Array that stores the cumulative distribution function at the discrete sample points of
    *                           the random variable given in xdata. Size of the array must be provided in numdata.
    * @param[in]     paradata   Array that stores the parameters of the rational interpolation based inversion. Size of
    *                           the array must be provided in  numdata.
    * @param[in]     parbdata   Array that stores the parameters of the rational interpolation based inversion. Size of
    *                           the array must be provided in  numdata.
    * @param[in]     xx         Array that stores the 1-alias probabilities. Size of the array must be provided in
    *                           numdata.
    * @param[in]     binindx    Array that stores the alias indices. Size of the array must be provided in  numdata.
    * @param[in]     numdata    Number of discrete sample points of the random variable i.e. size of the arrays.
    * @param[in]     rndm1      Random number distributed uniformly in [0,1].
    * @param[in]     rndm2      Random number distributed uniformly in [0,1].
    * @return        Random sample from the distribution represented by the input data obtained by the combination of
    *                alias sampling and rational interpolation based numerical inversion of the c.d.f..
    */
  double SampleRatin(double *xdata, double *comf, double *paradata, double *parbdata, double *xx, int *binindx,
                     int numdata, double rndm1, double rndm2, int above = -1);
  double SampleRatin(const double *xdata, const double *comf, const double *paradata, const double *parbdata,
                     const double *xx, const int *binindx, const int numdata, const double rndm1, const double rndm2,
                     const int above);
  /**
    * @brief Public method to obtain random variable from continuous distribution using discrete samples and the
    *        combination of alias sampling with linear approximation of the p.d.f.. All
    *        input data arrays must have been prepared previously by AliasTable::PreparLinearTable method.
    *
    * @param[in]     xdata      Array of discrete samples of the random variable between its minimum and maximum values.
    *                           Size of the array must be provided in  numdata. It stays unchanged at output.
    * @param[in]     ydata      Array that stores the (not necessarily normaised) p.d.f. at the discrete sample points
    *                           of the random variable given in xdata. Size of the array must be provided in
    *                           numdata.
    * @param[in]     xx         Array that stores the 1-alias probabilities. Size of the array must be provided in
    *                           numdata.
    * @param[in]     binindx    Array that stores the alias indices. Size of the array must be provided in  numdata.
    * @param[in]     numdata    Number of discrete sample points of the random variable i.e. size of the arrays.
    * @param[in]     rndm1      Random number distributed uniformly in [0,1].
    * @param[in]     rndm2      Random number distributed uniformly in [0,1].
    * @return        Random sample from the distribution represented by the input data obtained by the combination of
    *                alias sampling and linear approximation of the p.d.f..
    */
  double SampleLinear(double *xdata, double *ydata, double *xx, int *binindx, int numdata, double rndm1, double rndm2);

  int SampleLinearGetIndex(double rndm1, int numdata, double *xx, int *binindx)
  {
    const double rest           = rndm1 * (numdata - 1);
    int indxl                   = (int)(rest);
    const double dum0           = rest - indxl;
    if (xx[indxl] < dum0) indxl = binindx[indxl];
    return indxl;
  }

  double SampleLinearCached(double xval, double xdelta, bool ydata, double ydataDiff, double rndm2, double xdeltaDivDum)
  {
    // get the lower index of the bin by using the alias part
    //  const double rest           = rndm1 * (numdata - 1);
    //  int indxl                   = (int)(rest);
    //  const double dum0           = rest - indxl;
    //  if (xx[indxl] < dum0) indxl = binindx[indxl];

    // sample value within the selected bin by using linear approximation
    if (ydata > 0) {
      const double dum = ydataDiff;
      if (std::abs(dum) > 0.1)
        return xval - xdeltaDivDum * (1.0 - std::sqrt(1.0 + rndm2 * dum * (dum + 2.0)));
      else // use second order Taylor around dum = 0.0
        return xval + rndm2 * xdelta * (1.0 - 0.5 * dum * (rndm2 - 1.0) * (1.0 + dum * rndm2));
    }
    return xval + xdelta * std::sqrt(rndm2);
  }

  double SampleLinear(const double *xdata, const double *ydata, const double *xx, const int *binindx, const int numdata,
                      const double rndm1, const double rndm2);

  /**
    * @brief Public method to obtain random variable from discrete distribution using alias sampling . All
    *        input data arrays must have been prepared previously by AliasTable::PreparDiscreteTable method.
    *
    * @param[in]     xx         Array that stores the 1-alias probabilities. Size of the array must be provided in
    *                           numdata.
    * @param[in]     binindx    Array that stores the alias indices. Size of the array must be provided in  numdata.
    * @param[in]     numdata    Number of discrete random variable values i.e. size of the arrays.
    * @param[in]     rndm1      Random number distributed uniformly in [0,1].
    * @return        Index of the sampled discrete random variable.
    */
  int SampleDiscrete(double *xx, int *binindx, int numdata, double rndm1);

  /**
    * @brief Public method to prepare rational interpolation based numerical inversion of c.d.f. obtained for a
    *        discretized continuous distribution provided at input. Data, prepared with this method, can be used in
    *        AliasTable::GetRatinForPDF and AliasTable::GetRatinForPDF1 to compute the error of the approximation.
    *
    * @param[in,out] xdata      Array of discrete samples of the random variable between its minimum and maximum values.
    *                           Size of the array must be provided in  numdata. It stays unchanged at output.
    * @param[in,out] ydata      Array of the (not necessarily normaised) p.d.f. at the discrete sample points of the
    *                           random variable given in xdata. It's used only for the preparation but not used at
    *                           computation of the approximated p.d.f.. The p.d.f. will be normaised at output. Size of
    *                           the array must be provided in  numdata.
    * @param[in,out] comf       Array to store the cumulative distribution function at the discrete sample points of the
    *                           random variable given in xdata. At input it should be  numdata size array and it
    *                           will contain the cumulative distribution function values at the discrete sample points
    *                           of the random variable.
    * @param[in,out] paradata   Array to store parameters of the rational interpolation based inversion. At input it
    *                           should be  numdata size array and it will contain the parameter values at the
    *                           discrete sample points of the random variable.
    * @param[in,out] parbdata   Array to store parameters of the rational interpolation based inversion. At input it
    *                           should be  numdata size array and it will contain the parameter values at the
    *                           discrete sample points of the random variable.
    * @param[in,out] numdata    Number of discrete sample points of the random variable i.e. size of the arrays.
    * @return        The normalisation factor used to ensure normality of the input p.d.f. .
    *
    */
  double PreparRatinForPDF(double *xdata, double *ydata, double *comf, double *paradata, double *parbdata, int numdata,
                           bool isconstraintspline = false, int glnum = 16);

  /**
    * @brief Public method to obtain approximated p.d.f. value by using rational interpolation based numerical inversion
    *        of the c.d.f. obtained for the discretized continuous distribution. All input data arrays must have been
    *        prepared previously by AliasTable::PreparRatinForPDF method. This method can be used to compute the
    *        approximation error.
    *
    *  See more at AliasTable::PreparRatinForPDF.
    *
    * @param[in]     x          Value of the random variable at which the approximated p.d.f. value is required.
    * @param[in]     xdata      Array of discrete samples of the random variable between its minimum and maximum values.
    *                           Size of the array must be provided in numdata.
    * @param[in]     comf       Array that stores the cumulative distribution function at the discrete sample points of
    *                           the random variable given in xdata. Size of the array must be provided in numdata.
    * @param[in]     paradata   Array that stores the parameters of the rational interpolation based inversion. Size of
    *                           the array must be provided in  numdata.
    * @param[in]     parbdata   Array that stores the parameters of the rational interpolation based inversion. Size of
    *                           the array must be provided in  numdata.
    * @param[in]     numdata    Number of discrete sample points of the random variable i.e. size of the arrays.
    * @return        The approximated p.d.f. value at the given random variable value obtained by rational interpolation
    *                based numerical inversion of the c.d.f..
    */
  double GetRatinForPDF(double x, double *xdata, double *comf, double *paradata, double *parbdata, int numdata);

  /** @brief Same as AliasTable::GetRatinForPDF but the binary search part is skipped since the indx of lower bin of
   * xdata
    *        where x is located is provided as the last input parameter.
    *
    *  See more at AliasTable::PreparRatinForPDF.
    */
  double GetRatinForPDF1(double x, double *xdata, double *comf, double *paradata, double *parbdata, int lindx);

  /**
    * @brief Method to find lower index value \f$i\f$ in a grid of discrete values \f$\{x_i\}_{i=0}^{N-1}\f$ such that
    *        \f$ x_i \leq x < x_{i+1} \f$ by performing binary search.
    *
    * @param[in] val     The \f$ x \f$ value.
    * @param[in] xdata   The grid i.e. \f$\{x_i\}_{i=0}^{N-1}\f$.
    * @param[in] npoints Number of discrete data points on the grid i.e. \f$ N\f$.
    * @return    Index value \f$i\f$ such that \f$ x_i \leq x < x_{i+1} \f$ or \f$ 0\f$ if \f$ x\leq x_0\f$ and
    *            \f$ N-2 \f$ if \f$ x \geq x_{N-1}\f$
    */
  double BSearch(double val, double *xdata, int npoints);
};

} // namespace geantphysics

#endif // ALIASTABLE_H
