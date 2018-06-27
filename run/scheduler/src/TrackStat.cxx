#include "TrackStat.h"

#include "Geant/Typedefs.h"
#include "Geant/TaskData.h"
#include "StackLikeBuffer.h"
#include "Geant/SimulationStage.h"
#include "Geant/Handler.h"
#include "Geant/Basketizer.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
int TrackStat::CountBalance()
{
  // Makes the difference between the total number of tracks injected by the
  // thread and the sum of tracks in different containers.
  fNstacked             = fTd->fStackBuffer->GetNtracks() + fTd->fStackBuffer->GetNprioritized();
  constexpr int nstages = int(ESimulationStage::kSteppingActionsStage) + 1;
  fNstagebuff           = 0;
  fNbasketized          = 0;
  SimulationStage *stage;
  for (int istage = 0; istage < nstages; ++istage) {
    // Count tracks in the input buffer
    fNstagebuff += fTd->fStageBuffers[istage]->size();
    stage         = fTd->fPropagator->fStages[istage];
    int nhandlers = stage->GetNhandlers();
    // Count tracks in the baskets
    for (int i = 0; i < nhandlers; ++i) {
      if (stage->GetHandler(i)->IsActive()) {
        fNbasketized += stage->GetHandler(i)->GetNbasketized();
      }
    }
  }
  int balance = fNtotal - fNstacked - fNstagebuff - fNbasketized;
  return balance;
}

} // GEANT_IMPL_NAMESPACE
} // Geant
