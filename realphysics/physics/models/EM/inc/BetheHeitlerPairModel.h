
#ifndef BETHEHEITLERPAIRMODEL_H
#define BETHEHEITLERPAIRMODEL_H

#include "EMModel.h"

#include <string>

namespace geantphysics {
/**
 * @brief   Model for conversion of gamma particles into e-/e+ pair in the the field of nucleus and atomic electrons.
 * @class   BetheHeitlerPairModel
 * @author  F Hariri, M Novak
 * @date    May 2017
 *
 * detailed description will be added later
 */

class Material;
class MaterialCuts;
class Element;
class AliasTable;
class Particle;
class LightTrack;


class BetheHeitlerPairModel : public EMModel {
public:
/**
* @name Constructor, destructor:
*/
//@{
  /**
  * @brief Constructor.
  *
  * @param[in] modelname   Name of the model.
  */
  BetheHeitlerPairModel(const std::string &modelname="PairBetheHeitler");
  /** @brief Destructor. */
  ~BetheHeitlerPairModel();
//@}

/**
* @name Implemented EMModel base class methods:
*/
//@{
  /** @brief Interface method to initilize the model. */
  virtual void   Initialize();
  /** @brief Interface method to obtain macroscopic cross sections. */
  virtual double ComputeMacroscopicXSection(const MaterialCuts *matcut, double kinenergy, const Particle *particle);
  /** @brief Interface method to obtain atomic cross sections. */
  virtual double ComputeXSectionPerAtom(const Element *elem, const MaterialCuts *matcut, double kinenergy, const Particle *particle);
  /** @brief Interface method to generate final state of the interaction. */
  virtual int    SampleSecondaries(LightTrack &track, Geant::GeantTaskData *td);

  /**
   * @brief Method to obtain the minimum primary gamma energy at which the interaction can happen.
   *
   * It's used by the target element selector to build partial (per element) lambda tables that are used at run-time to
   * select target atom for the interaction:
   * \f$ E_{\gamma}^{\text{min}} = \text{max}\{2m_ec^2,\text{min-energy-usage} \} \f$
   *
   * @return \f$ E_{\gamma}^{\text{min}} \f$
   */
  virtual double MinimumPrimaryEnergy(const MaterialCuts * /*matcut*/, const Particle * /*part*/) const {
    return fMinimumPrimaryEnergy;
  }
//
//@}


/**
* @name Model specific public methods for customize sampling tables (if they are used):
*/
//@{
  /** @brief Public method to set the number of primary photon energy grid points per decade for sampling tables.
    *        Will be used only if sampling tables were required and must be set before initialisation.
    */
  int   GetNumberOfPhotonEnergiesPerDecade()   const { return fNumSamplingPrimEnergiesPerDecade; }
  /** @brief Public method to get the number of primary photon energy grid points per decade for sampling tables.*/
  void  SetNumberOfPhotonEnergiesPerDecade(int val)  { fNumSamplingPrimEnergiesPerDecade = val;  }
  /** @brief Public method to set the number of discrete samples used by the sampling tables.
    *        Will be used only if sampling tables were required and must be set before initialisation.
    */
  int   GetNumberOfDiscretePDFSamples()        const { return fNumSamplingEnergies;              }
  /** @brief Public method to get the number of discrete samples used by the sampling tables.*/
  void  SetNumberOfDiscretePDFSamples(int val)       { fNumSamplingEnergies = val;               }
//@}


private:
/**
* @name Model specific private methods.
*/
//@{
  /** @brief Internal method to initilise the model.*/
  void   InitialiseModel();

  /**
   * @brief Internal method to compute parametrized atomic cross section of gammas conversion into e-/e+ pair
   *        for a given target atom, gamma particle kinetic energy..
   *
   * @param[in] z        Atomic number of the target atom.
   * @param[in] egamma   Kinetic energy of the gamma particle in internal [energy] units.
   * @return    The computed atomic cross section in internal [lenght^2] units.
   */
  double ComputeAtomicCrossSection(double z, double egamma);

  /** @brief Internal method to build data collection of some frequently used target atom dependent variables. */
  void InitialiseElementData();

  /**
   * @brief NEED TO BE DOCUMENTED
   *
   * reate gamma energy grid; create sampling tables for each elements that are in materials that
   *          are in regions where this model is active
   */
   void InitSamplingTables();

   double SampleTotalEnergyTransfer(double primekin, double epsmin, int zindx, double r1, double r2, double r3);

   // elemindx the element index in the global Element table
   void BuildSamplingTablesForElement(const Element *elem, int elemindx);
   void BuildOneRatinAlias(double egamma, const Element *elem, double *thePdfdata, int elemindx, int egammaindx);
   /** @brief Internal method to compute the total energy, transferd to one of the e-/e+ pair, related transformed
     *        variable dependent part of the DCS.
     *
     *  The method is used to prepare sampling tables at initialization for the fast sampling of the reduced total
     *  energy, transfered to one of the e-/e+ pair, at run-time.
     *
     *  @param[in] epsmin      Minimum value of the reduced total energy transfer \f$ \epsilon_{\text{min}} =
     *                         \text{max}[\epsilon_0,\epsilon']\f$
     *  @param[in] eps0        Kinematical minimum of the reduced total energy transfer
     *                         \f$ \epsilon_0 \equiv m_ec^2/E_{\gamma} \f$
     *  @param[in] deltafactor Target atomic number dependent constant \f$ 136Z^{-1/3} \f$
     *  @param[in] fz          The target atomic number dependent Coulomb correction function
     *                          \f[
     *                             F(Z) =
     *                               \begin{cases}
     *                                 \frac{8}{3} \ln(Z) & \quad \text{if } E_{\gamma} < 50 \text{[MeV]} \\
     *                                 \frac{8}{3} \ln(Z) + 8 f_c(Z) & \quad \text{if } E_{\gamma} \geq 50 \text{[MeV]}
     *                               \end{cases}
     *                           \f]
     *  @param[in] xi           Total energy transfer related transformed variable \f$ \xi \in [0,1] \f$ when
     *                          \f$ \epsilon \in [\epsilon_{\text{min}},0.5] \f$  and  \f$ \xi(\epsilon) =
     *                               \ln[\epsilon/\epsilon_{\text{min}}]/\ln[0.5/\epsilon_{\text{min}}]\f$
     *  @return                 The \f$\xi\f$ depends part of the transformed DCS \f$ \left( \mathrm{d}\sigma(Z,
     *                          \epsilon(\xi))/\mathrm{d}\xi \right)^* \f$ at the given input parameters.
     */
   double ComputeDXSection(double epsmin, double eps0, double deltafactor, double fz, double xi) ;
//@}


// data members
private:
  /** @brief Size of some containers that store data per elements (\f$ Z_{\text{max}} = gMaxZet-1)\f$. */
  static const int gMaxZet = 120; // max Z+1

  /** @brief  Internal code of the secondary e-. */
  int fElectronInternalCode;
  /** @brief  Internal code of the secondary e+. */
  int fPositronInternalCode;

  /** @brief Minimum photon energy at which the discrete interaction can happen:
             \f$ E_{\gamma}^{\text{min}} = \text{max}\{2m_ec^2,\text{min-energy-usage} \} \f$
    */
  double fMinimumPrimaryEnergy;

  /** simplified sampling below this energy limit: 2 MeV */
  double fGammaEneregyLimit;


  /** Data collection that stores some frequently used target atom specific constants for one atom. */
  struct ElementData {
    /** @brief \f$ 136*Z^{-1/3} \f$ */
    double  fDeltaFactor;
    /** @brief Coulomb correction \f$ f_c \f$ as in \cite davies1954theory [Eqs(36-38)] */
    double  fCoulombCor;
    /** @brief \f$ 8\ln(Z)/3 \f$ */
    double  fFzLow;
    /** @brief \f$ 8\ln(Z)/3 + f_c \f$ */
    double  fFzHigh;
    /** @brief \f$ \exp[(42.24-8\ln(Z)/3)/8.368]-0.952 \f$ */
    double  fDeltaMaxLow;
    /** @brief \f$ \exp[(42.24-(8\ln(Z)/3+f_c))/8.368]-0.952 \f$ */
    double  fDeltaMaxHigh;
  };

  /** @brief Container to store target atom specific data collections (ElementData ) for all target atoms which
    *        the model needs to respond.
    *
    * The size of the container will be equal to #gMaxZet. After initialisation (i.e. calling the InitialiseElementData()
    * method) an element with index \f$Z\f$ will contain data collection for target atom with atomic number \f$ Z \f$.
    * Only those elements will be non-nullptr that corresponds to an atom with atomic number \f$ Z \f$ that the model
    * needs to provide response(final state): i.e. that appears in materials that belongs to regions inwhich the model
    * is active.
    */
  ElementData  **fElementData;


/**
 * @name Members to describe the common discrete photon energy grid for sampling tables:
 *
 * These variables describe and define the primary gamma kinetic energy grid above we build sampling tables in the
 * InitSamplingTables() method for run-time samling of the total energy transferd to one particle. The min of the table
 * is set to #fMinimumPrimaryEnergy and the max is the maximum energy usage limit of the model. The number of discrete
 * gamma energy points will be determined by the value of #fNumSamplingPrimEnergiesPerDecade variable. The default value
 * is 10 and it can be changed by the user through the SetNumberOfPhotonEnergiesPerDecade() public method (must be set
 * before the the initialisation of the model!).
 */
//@{
  /** @brief Number of primary gamma kinetic energy grid points in [#fMinPrimEnergy,#fMaxPrimEnergy].*/
  int fNumSamplingPrimEnergies;
  /** @brief Number of primary gamma kinetic energy grid points per decade. */
  int fNumSamplingPrimEnergiesPerDecade;
  /** @brief Number of "total energy transfered to one of the secondaries" related discrete transformed variable PDF
    *        points in [0,1].
    */
  int fNumSamplingEnergies;
  /** @brief Minimum of the primary gamma kinetic energy grid. */
  double  fMinPrimEnergy;
  /** @brief Maximum of the primary gamma kinetic energy grid. */
  double  fMaxPrimEnergy;
  /** @brief Logarithm of #fMinPrimEnergy ie ln(#fMinPrimEnergy) . */
  double  fPrimEnLMin;
  /** @brief Inverse of the primary gamma kinetic energy grid delta ie
    *        ln[#fMaxPrimEnergy/#fMinPrimEnergy]/(#fNumSamplingPrimEnergies-1)
    */
  double  fPrimEnILDelta;
  /** @brief The logarithmically spaced primary gamma kinetic energy grid.
    *
    * Size of the array is #fNumSamplingPrimEnergies points in the [#fMinPrimEnergy, #fMaxPrimEnergy] interval.
    */
  double *fSamplingPrimEnergies;
  /** @brief The logarithm of #fSamplingPrimEnergies grid.
    *
    *  Size of the array is #fNumSamplingPrimEnergies points in the [ln(#fMinPrimEnergy), ln(#fMaxPrimEnergy)] interval.
    */
  double *fLSamplingPrimEnergies;
//@}

/** @brief Internal data structure to store data that are required to sample the energy (transfered to one of the e-/e+
  *        pair) related transformed variable by using the combination of Walker's alias method and rational
  *        interpolation based numerical inversion of the cumulative function (by using AliasTable::SampleRatin() method).
  */
struct RatinAliasData {
  /** @brief Number of dicrete data points ie size of the arrays = #fNumSamplingEnergies. */
  int     fNumdata;
  double  fXiDelta1;
  /** @brief Total energy (transfered to one of the secondaries) related transformed variable values. */
  double *fXdata;
  /** @brief The cumulative distribution function values over the energy transfer related transformed variable values.*/
  double *fCumulative;
  /** @brief Interpolation parameters over the energy transfer related transformed variable values.*/
  double *fParaA;
  /** @brief Interpolation parameters over the energy transfer related transformed variable values.*/
  double *fParaB;
  /** @brief The alias probabilities over the energy transfer related transformed variable values.*/
  double *fAliasW;
  /** @brief The alias indices over the energy transfer related transformed variable values. */
  int    *fAliasIndx;
};

/** @brief Internal data structure to store RatinAliasData for a given target element. */
struct RatinAliasDataPerElement {
  /** @brief Container that stores #fNumSamplingEnergies pointers to RatinAliasData structure over the primary gamma
    *         energy gird #fSamplingPrimEnergies for a specific target atom built at initialisation if sampling tables
    *         were requested.
    *  The indices of the RatinAliasData pointers correspond to the primary gamma energies in #fSamplingPrimEnergies.
    */
  RatinAliasData **fRatinAliasDataForOneElement;
};

/** @brief Container to store pointers to RatinAliasDataPerElement data structures for all target elements that the
  *        model needs to respond.
  *  Size of the container is equal to #gMaxZet and indexed by the atomic number of the target elements. The
  *  corresponding RatinAliasDataPerElement data structures are built at initialization if sampling tables were required.
  *  Non-nullptr only at indices that corresponds to target atomic number that appears in materials that belongs to
  *  regions in which the model is active.
  */
RatinAliasDataPerElement **fRatinAliasDataForAllElements;

/** @brief Pointer to an AliasTable uitility object that is used both to prepare(at initialization) and to provide
  *        samples(at run-time) from the sampling tables. (Used only if sampling tables were required).
  */
AliasTable *fAliasSampler;


};

}      // namespace geantphysics

#endif // BETHEHEITLERPAIRMODEL_H
