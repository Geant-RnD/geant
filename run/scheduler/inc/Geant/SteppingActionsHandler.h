//===--- SteppingActionsHandler.h - Geant-V -----------------------------*- C++ -*-===//
//
//                     Geant-V Prototype
//
//===----------------------------------------------------------------------===//
/**
 * @file SteppingActionsHandler.h
 * @brief Implementation of the stepping actions handler.
 *
 * @author Andrei Gheata
 */
//===----------------------------------------------------------------------===//

#ifndef GEANT_STEPPING_ACTIONS_HANDLER
#define GEANT_STEPPING_ACTIONS_HANDLER

#include "Geant/Handler.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

/**
 * @brief Handler invoked once per step per particle. Calling the user actions.
 */

class SteppingActionsHandler : public Handler {

protected:
private:
  SteppingActionsHandler(const SteppingActionsHandler &) = delete;
  SteppingActionsHandler &operator=(const SteppingActionsHandler &) = delete;

public:
  /** @brief Default constructor */
  VECCORE_ATT_HOST_DEVICE
  SteppingActionsHandler() : Handler() {}

  /**
   * @brief Default constructor
   * @param propagator Propagator working with this handler
   */
  VECCORE_ATT_HOST_DEVICE
  SteppingActionsHandler(int threshold, Propagator *propagator);

  /** @brief Geometry filter destructor */
  VECCORE_ATT_HOST_DEVICE
  virtual ~SteppingActionsHandler();

  /** @brief Scalar DoIt interface */
  VECCORE_ATT_HOST_DEVICE
  virtual void DoIt(Track *track, Basket &output, TaskData *td);

  /** @brief Vector DoIt interface. Base class implements it as a loop. */
  VECCORE_ATT_HOST_DEVICE
  virtual void DoIt(Basket &input, Basket &output, TaskData *td);

  /** @brief Enable verbosity */
  void SetVerbosity(bool val);

private:
  bool fVerboseTracking = false;
};

} // GEANT_IMPL_NAMESPACE
} // Geant

#endif
