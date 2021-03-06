#include "Geant/PropagationStage.h"

#include "Geant/RunManager.h"
#include "Geant/FieldLookup.h"
#include "Geant/LinearPropagationHandler.h"
#include "Geant/FieldPropagationHandler.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
PropagationStage::PropagationStage(Propagator *prop) : SimulationStage(kPropagationStage, prop)
{
  auto fldConfig = FieldLookup::GetFieldConfig();
  assert(fldConfig != nullptr);
  fHasField = fldConfig->FieldExists();
}

VECCORE_ATT_HOST_DEVICE
PropagationStage::PropagationStage(const PropagationStage &other) : SimulationStage(other)
{
  fHasField = other.fHasField;
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
PropagationStage &PropagationStage::operator=(const PropagationStage &other)
{
  SimulationStage::operator=(other);
  fHasField                = other.fHasField;
  return *this;
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
SimulationStage *PropagationStage::Clone() const
{
  PropagationStage *stage = new PropagationStage(*this);
  return stage;
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
int PropagationStage::CreateHandlers()
{
  // Create all volume handlers.
  int threshold                     = fPropagator->fConfig->fNperBasket;
  LinearPropagationHandler *hlinear = new LinearPropagationHandler(threshold, fPropagator);
  FieldPropagationHandler *hfield   = new FieldPropagationHandler(threshold, fPropagator);
  hlinear->SetMayBasketize(false);
  hlinear->SetLocal(bool(fLocalHandlers & kLinearPropagator));
  if (fPropagator->fConfig->fUseSDField) Printf("=== Field propagation using scalar dispatch for baskets\n");
  hfield->SetScalarDispatch(fPropagator->fConfig->fUseSDField);
  hfield->SetMayBasketize(true);
  hfield->SetLocal(bool(fLocalHandlers & kFieldPropagator));

  AddHandler(hlinear);
  AddHandler(hfield);
  return 2;
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
Handler *PropagationStage::Select(Track *track, TaskData *)
{
  // Retrieve the appropriate handler depending on the track charge
  if (!fHasField || track->Charge() == 0) return fHandlers[0];
  return fHandlers[1];
}

} // namespace GEANT_IMPL_NAMESPACE
} // namespace geant
