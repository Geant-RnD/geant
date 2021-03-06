
#ifndef PHYSICS_PROCESS_HANDLER
#define PHYSICS_PROCESS_HANDLER

#include "Geant/PhysicsInterface.h"

// geantV
namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {
class Propagator;
class SimulationStage;
} // namespace GEANT_IMPL_NAMESPACE
} // namespace geant

// realphysics stages
#include "GeantConfig.h"
#include "Geant/FastSimStage.h"
#include "Geant/ComputeIntLStage.h"
#include "Geant/PrePropagationStage.h"
#include "Geant/PostPropagationStage.h"
#include "Geant/AlongStepActionStage.h"
#include "Geant/PostStepActionStage.h"
#include "Geant/AtRestActionStage.h"
#include "Geant/PostStepActionPhysModelStage.h"
#include "Geant/PostPropagationVectorStage.h"

namespace geantphysics {

/**
 * @brief  The top level physics manager: class that handles the physics simulation in one step.
 * @class  PhysicsProcessHandler
 * @author M Novak, A Ribon
 * @date   december 2015
 *
 * This class decides and calls the appropriate methods to simulate the physics of any given particle happening in one
 * step. This final class derives from the PhysicsInterface class of the kernel.
 */
class PhysicsProcessHandler : public PhysicsInterface {
private:
  /** @brief PhysicsProcessHandler copy constructor is deleted */
  PhysicsProcessHandler(const PhysicsProcessHandler &other) = delete;

  /** @brief Operator = is not defined */
  PhysicsProcessHandler &operator=(const PhysicsProcessHandler &other) = delete;

public:
  /** @brief Default constructor */
  PhysicsProcessHandler(const geant::GeantConfig &config)
      : fSimplifiedMSC(config.fUseVectorizedMSC & !config.fUseSDMSC),
        fVectorPostStepAction(config.fUseVectorizedPhysics)
  {
  }

  /** @brief PhysicsProcessHandler destructor */
  virtual ~PhysicsProcessHandler();

  /** @brief Attach the physics data to task data */
  void AttachUserData(TaskData *td);

  /** @brief Initialize the physics
   *
   *  This method is the main physics initialization mathod called directly from the kernel through the interface.
   *  The kernel should pass a pinter to the geometry/materials/regions...number of working threads.
   * - base on the regions and all information stored per region (e.g. production thresholds) together with the set of
   *   materials per regions:
   *   # MategrialCuts obejcts are created and stored in the MaterialCuts table that can be accessed anywhere through
   *     the static const std::vector<MaterialCuts*>& GetTheMaterialCutsTable()
   *   # later on, the only information that we use, regarding the regions is the number of regions, that must be set in
   *     the PhysicsListManager singletone at this point by its SetNumberOfRegions(int) method.
   * NOTE: at the moment we will use our simple region class and we read and create our materials based on material
   *       names! Moreover, we can have only one region at the moment since the kernel is not able to handle regions:
   *     0 we create only 1 region and we set the production cuts.
   *     1 we loop over all TGeoMaterial and we create our own Material-s
   *     2 we add all Material objects to our only one region
   * - calls the PhysicsListManager::BuildPhysicsLists() method:
   *    # creates all particles
   *    # loops over all PhysicsList registered by the user in PhysicsListManager and calls one-by-one the
   *      Initialize() method of the registered PhysicsLists:
   *      0  each call to a PhysicsList Initialize() method will assigne processes to the temporary PhysicsProcess
   * vector
   *         of the static particle object
   *      1  create one PhysicsManagerPerParticle object for each partcile that the current PhysicsList has added at
   *         least one PhysicsProcess
   *       2 add all processes from the current physics list to this PhysicsManagerPerParticle
   *         and push the particle to the particle list of the phsyics process that the physics process is assigned to
   *       3 also set the active region indices both in the PhysicsProcess-es(a) and in the PhysicsManagerPerParticle(b)
   *         objects
   *       4 store the created PhysicsManagerPerParticle object pointer in a table (will be used to delete and initilize
   *         only once them)
   *       5 set pointers to regional PhysicsManagerPerParticle in the static Particle definition to point to this
   *         object where the current physics list is active
   *    # each created PhysicsManagerPerParticle is initilised by calling their Initialize() method:
   *       0 each processes are initialised by calling their Initialize() method:
   *         :: it is checked in the base PhysicsProcess Initialize() method if the process is assigned only to allowed
   *            particles
   *         :: in case of EMPhysicsProcess-es (after calling the base i.e. PhysicsProcess Initialize() method)
   *            the EMModelManager member of the EMPhysicsProcess is initialized that will initialize the EMModel-s
   *            as well togeter with setting reagions where they active. The default active regions for EMModel-s are
   *            determined by the active regions of the EMPhysicsProcess(PhysicsProcess) that they belong to. On the top
   *            of this, user requested inactive regions in case of the individual EMModel-s are considered by the
   *            EMModelManager when it sets the active regions of the individual EMModel-s. At the end of initialisation
   *            of the EMModelManager member of the EMPhysicsProcess: EMModel-s per region ponters are set and the
   *            corresponding EMModel-s are initialised and each EMModel knows its list of active regions.
   *         :: if the base EMModel class InitialiseElementSelectors() method was explicitly called from the derived
   *            emmodel class Initialize() method (at the end i.e. after the model is properly initialised), then
   *            target element selector is built for the given model and tagget elemnt can be sampled at run time by the
   *            int EMModel::SampleTargetElementIndex(const MaterialCuts* matcut, double ekin, double rndm) method.
   *         :: PhysicsProcess-es set to be kEnergyLoss processes are registered in the ELossTableRegister for the given
   *            particle
   *
   *    # the ELossTableManager is initialised i.e. for all partciles that at least one PhysicsProcess::kEnergyLoss
   *      process had been registered in the ELossTableRegister: dedx, range, inverse range tables are built over an
   *      energy grid per material-cuts that belongs to region where the given process is active and run time
   *      interpolation is set up and can be used at run time to get:
   *       - restricted stopping power:
   *            double ELossTableManager::GetRestrictedDEDX(const MaterialCuts *matcut, const Particle *part,
   *                                                        double kinenergy)
   *       - restricted range:
   *            double ELossTableManager::GetRestrictedRange(const MaterialCuts *matcut, const Particle *part,
   *                                                         double kinenergy)
   *       - energy that corresponds to a given restricted range i.e. from the inverse range table:
   *            double ELossTableManager::GetEnergyForRestrictedRange(const MaterialCuts *matcut, const Particle *part,
   *                                                                  xdouble range)
   *
   */
  virtual void Initialize();

  /**
   * @name Interface methods to obtain physics realted symulation stages when V3 is used.
   *
   * These methods are called from the geant::Propagator::CreateSimulationStages
   * methods (when real-physics is used) to obtain the pointers to the physics
   * simulation stages defined in the real-physics library.
   */
  //@{
  /** @brief Obtain/create physics step limit computation stage.
   *
   * @param[in,out] prop  Pointer to the propagator object that requires the simulation stage.
   * @return     Pointer to a created ComputeIntLen real-physics simulation stage object.
   */
  geant::SimulationStage *CreateFastSimStage(geant::Propagator *prop) { return new FastSimStage(prop); }

  geant::SimulationStage *CreateComputeIntLStage(geant::Propagator *prop) { return new ComputeIntLStage(prop); }

  geant::SimulationStage *CreatePrePropagationStage(geant::Propagator *prop) { return new PrePropagationStage(prop); }

  geant::SimulationStage *CreatePostPropagationStage(geant::Propagator *prop)
  {
    if (fSimplifiedMSC) {
      return new PostPropagationVectorStage(prop);
    } else {
      return new PostPropagationStage(prop);
    }
  }

  /** @brief Obtain/create along step action (continuous part) computation stage.
   *
   * @param[in,out] prop  Pointer to the propagator object that requires the simulation stage.
   * @return     Pointer to a created AlongStepAction real-physics simulation stage object.
   */
  geant::SimulationStage *CreateAlongStepActionStage(geant::Propagator *prop) { return new AlongStepActionStage(prop); }

  /** @brief Obtain/create post step action (discrete part) computation stage.
   *
   * @param[in,out] prop  Pointer to the propagator object that requires the simulation stage.
   * @return     Pointer to a created PostStepAction real-physics simulation stage object.
   */
  geant::SimulationStage *CreatePostStepActionStage(geant::Propagator *prop)
  {
    if (fVectorPostStepAction) {
      return new PostStepActionPhysModelStage(prop);
    } else {
      return new PostStepActionStage(prop);
    }
  }

  geant::SimulationStage *CreateAtRestActionStage(geant::Propagator *prop) { return new AtRestActionStage(prop); }
  //@}
private:
  bool fSimplifiedMSC        = true;
  bool fVectorPostStepAction = true;
};

} // end of namespace geantphysics

#endif
