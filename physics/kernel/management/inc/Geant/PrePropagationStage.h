
#ifndef PREPROPAGATIONSTAGE_H
#define PREPROPAGATIONSTAGE_H

// from geantV
#include "Geant/SimulationStage.h"
// from geantV
namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {
class Propagator;
class Track;
class TaskData;
class Handler;
} // namespace GEANT_IMPL_NAMESPACE
} // namespace geant

namespace geantphysics {

/**
 * @brief   Special simulation stage for msc to be invoked before the transportation.
 * @class   PrePropagationStage
 * @author  M Novak
 * @date    June 2017
 *
 * Particles that have multiple scattering process will be selected at this stage.
 */

class PrePropagationStage : public geant::SimulationStage {
public:
  /** @brief ctr */
  PrePropagationStage() {}

  /** @brief ctr */
  PrePropagationStage(geant::Propagator *prop);

  /** @brief dtr */
  ~PrePropagationStage();

  VECCORE_ATT_HOST_DEVICE
  PrePropagationStage(const PrePropagationStage &);

  VECCORE_ATT_HOST_DEVICE
  PrePropagationStage &operator=(const PrePropagationStage &);

  /** @brief Clone the stage and copy the existing handlers **/
  VECCORE_ATT_HOST_DEVICE
  virtual geant::SimulationStage *Clone() const;

  /** @brief Get simulation stage name */
  virtual const char *GetName() const { return "PrePropagation"; }

  /** @brief Interface to create all handlers for the simulation stage
   *  @return Number of handlers created */
  virtual int CreateHandlers();

  /** @brief Interface to select the handler matching a track */
  virtual geant::Handler *Select(geant::Track *track, geant::TaskData *td);
};

} // namespace geantphysics

#endif // PREPROPAGATIONSTAGE_H
