
#ifndef EMMODEL_H
#define EMMODEL_H

#include <string>
#include <vector>

#include "GeantTaskData.h"

namespace geantphysics {

// forward declarations
class Material;
class MaterialCuts;
class MaterialProperties;
class Element;
class Particle;
class PhysicsParameters;
class EMElementSelector;
class LightTrack;
/**
 * @brief   Base class for electromagnetic physics models.
 * @class   EMModel
 * @author  M Novak, A Ribon
 * @date    august 2016
 */

class EMModel {
public:
  /**
   * @brief CTR
   */
  EMModel(const std::string& name);
  /**
   * @brief DTR
   */
  virtual ~EMModel();

//
// The following 5 virtual methods might be implemented by the derived electromagnetic models.
//
  // will set the physics parameter object that belongs to the regions(s) where this model is active
  /**
   * @brief Model initialization method. Needs to be implemented by the derived class. This base class implementation
   *        needs to be called from the derived class Initialize() method as well as the first line because it will set
   *        the PhysicsParameters member variable to that belongs to the regions(s) where this model is active. This
   *        method is called when the EMPhysicsProcess, that the EMModel belongs to, is initialized by the
   *        EMModelManager member of the EMPhysicsProcess.
   */
  virtual void  Initialize();


  /**
   * @brief Method to compute stopping power for a given MaterialCuts, Partcile, kinetic energy.
   *
   * ALL ENERGY LOSS MODELS (i.e. those that has enegy loss along the step) NEEDS TO IMPLEMENT this method. It is called
   * at initialization to build the energy loss related tables through the corresponding PhysicsProcess by an ELossTable
   * object.
   *
   * @param[in] matcut      Pointer to the MaterialCuts object in which the dEdx must be computed.
   * @param[in] kinenergy   Kinetic energy of the Partcile at which the dEdx must be computed.
   * @param[in] particle    Pointer to the Partcile object for which the dEdx must be computed.
   * @param[in] istotal     Flag to indicate if full dEdx must be computed. False by default i.e. restricted dEdx is
   *                        required.
   * @return    Restricted or full stopping power computed by the given electromagnetic model in internal [energy/length]
   *            units for the given Particle, MaterialCuts/Material and particle kinetic energy combination.
   */
  virtual double ComputeDEDX(const MaterialCuts * /*matcut*/, double /*kinenergy*/, const Particle * /*particle*/,
                             bool istotal=false);


  /**
   * @brief Method to compute macroscopic cross section for a given MaterialCuts, Partcile, kinetic energy.
   *
   * ALL DISCRETE MODELS (i.e. those that describes interaction happening at the post-step point) NEEDS TO IMPLEMENT
   * this method. This method is called at initialization to build the lambda tables through the corresponding
   * PhysicsProcess by the PhysicsManagerPerParticle object.
   *
   * @param[in] matcut      Pointer to the MaterialCuts object in which the macroscopic cross section must be computed.
   * @param[in] kinenergy   Kinetic energy of the Partcile at which the macroscopic cross section must be computed.
   * @param[in] particle    Pointer to the Partcile object for which the macroscopic cross section must be computed.
   * @return    Macroscopic cross section computed by the given electromagnetic model in internal [1/length] units for
   *            the given Particle, MaterialCuts/Material and particle kinetic energy combination.
   */
  virtual double ComputeMacroscopicXSection(const MaterialCuts * /*matcut*/, double /*kinenergy*/,
                                            const Particle * /*particle*/) = 0; // { return 0.; }


  /**
   * @brief Method to compute atomic cross section for a given Element, MaterialCuts, Partcile, kinetic energy.
   *
   * ALL DISCRETE MODELS (i.e. those that describes interaction happening at the post-step point) NEEDS TO IMPLEMENT
   * this method. This method is called at initialization to build the lambda tables through the corresponding
   * PhysicsProcess by the PhysicsManagerPerParticle object and by the EMElementSelector object if it was requested in
   * the derived class Initialize() method.
   * Note, that the MatrialCut obejct, that corresponds to the material-region to which the current element belongs to
   * is also provided to include any material, or cut dependences. However, this infomation does not necessary to use by
   * each EMModel-s (e.g. those models that describes discrete interaction that doesn't depend on production threshold
   * and material dependent properties won't use this extra information).
   *
   * @param[in] elem        Pointer to the Element object for which the atomic cross section must be computed.
   * @param[in] matcut      Pointer to the MaterialCuts object in which the atomic cross section must be computed.
   * @param[in] kinenergy   Kinetic energy of the Partcile at which the atomic cross section must be computed.
   * @param[in] particle    Pointer to the Partcile object for which the atomic cross section must be computed.
   * @return    Atomic cross section computed by the given electromagnetic model in internal [length^2] units for
   *            the given ELement, Particle, MaterialCuts/Material and particle kinetic energy combination.
   */
  virtual double ComputeXSectionPerAtom(const Element * /*elem*/, const MaterialCuts * /*matcut*/, double /*kinenergy*/,
                                        const Particle * /*particle*/) = 0; // { return 0.; }


  virtual int    SampleSecondaries(LightTrack & /*track*/, std::vector<LightTrack> & /*sectracks*/,
                                   Geant::GeantTaskData * /*td*/) = 0; // { return 0; }

  /**
   * @brief Method to obtain minim primary particle kinetic energy at which the discrete part (if any) of the interaction
   *        can happen i.e. kinematic minimum for the discrete part of the interaction.
   *
   * ALL DISCRETE MODELS (i.e. those that describes interaction happening at the post-step point) THAT HAS PRODUCTION
   * CUT DEPENDENCE NEEDS TO IMPLEMENT this method. It is used e.g. to build the energy grid of the target element
   * selector (if it was requested by the derived model) for providing run-time functionality to select target element
   * for the discrete interaction.
   *
   * @param[in] matcut      Pointer to the MaterialCuts object in which the discrete kinematic minimum is requested.
   * @param[in] particle    Pointer to the Partcile object for which the discrete kinematic minimum is requested.
   * @return    Minimum primary particle kinetic energy at which the discrete interaction can happen in interanl
   *            [energy] units for the given Particle, MaterialCuts/Material combination.
   */
  virtual double MinimumPrimaryEnergy(const MaterialCuts * /*matcut*/, const Particle * /*part*/) const = 0; // { return 0.0; }
//
//
//

  const std::string& GetName() const { return fName; }

  void    SetIndex(int indx) { fIndex = indx; }
  int     GetIndex() const   { return fIndex; }

  void    SetLowEnergyUsageLimit(double val)  { fLowEnergyUsageLimit  = val; }
  double  GetLowEnergyUsageLimit() const      { return fLowEnergyUsageLimit; }
  void    SetHighEnergyUsageLimit(double val) { fHighEnergyUsageLimit = val; }
  double  GetHighEnergyUsageLimit() const     { return fHighEnergyUsageLimit;}

  const PhysicsParameters* GetPhysicsParameters() const { return fPhysicsParameters; }


  void  AddToUserRequestedInActiveRegions(int regionindx) { fListOfUserRequestedInActiveRegions.push_back(regionindx); }
  const std::vector<int>&  GetListOfUserRequestedInActiveRegions() { return fListOfUserRequestedInActiveRegions; }

  std::vector<bool>& GetListActiveRegions()       { return fListActiveRegions;             }
  bool IsActiveRegion(const int regionindx) const { return fListActiveRegions[regionindx]; }


// protected
  void RotateToLabFrame(double &u, double &v, double &w, double x, double y, double z);

//
// Target element selector related
//
  // will be protected;  selects target element; currently returns with the index of the selected Element, latter it
  // will return with const Element*
  int SampleTargetElementIndex(const MaterialCuts* matcut, double ekin, double rndm);

protected:
  // initilise the element selectors: must be called from the derived emmodel class explicitly at the end of its
  // Initialise() method i.e. after the model has been initialised properly.
  void  InitialiseElementSelectors(EMModel *emmodel, const Particle *part, bool ispermaterial);

private:
  // delete all EMElementSelector-s
  void ClearElementSelectros();
//
//
//

private:
  std::string              fName;                 // name of the model
  int                      fIndex;                // will be set by the model manager and it will be the index of the model in the model manager table
  double                   fLowEnergyUsageLimit;     // set by the user to be used
  double                   fHighEnergyUsageLimit;    // set by the user to be used
  const PhysicsParameters *fPhysicsParameters;       // physics parameters object that belongs to the region(s) where this model is active
                                                     // the class do not own the object
  // flag to indicate if element selectors are per material or per material-cuts
  bool        fIsElementSelectorsPerMaterial;
  std::vector<EMElementSelector*> fElementSelectors; // EMElementSelector pointers per Material/MaterialCuts for those that are
                                                   // in regions where this model is active and has more than one elements;
                                                   // each ELSelectorData is owned by the object and will be cleand by
                                                   // ClearELSelectros(); all of these EMElementSelector are owned by the
                                                   // class and cleand up by ClearElementSelectros();
                                                   // size is non-zero only if InitialiseElementSelectors() was called
                                                   // explicitly from the derived EM model at the end of its Initialise()
                                                   // method

  std::vector<int>  fListOfUserRequestedInActiveRegions; // will be checked only if the process is not active in some of them
  std::vector<bool> fListActiveRegions;   // by default a model will be active in regions in which the process is active
                                          // unless the user inactivated the model in some reagions i.e. possible to have
                                          // different set of models for a given process per region
                                          // so this vector will be set by the process through the model manager automatically
                                          // then the user requested inactivations will be checked and active region list will
                                          // be updated
};

} // namespace geantphysics

#endif // EMMODEL_H
