
#ifndef SAUTERGAVRILAPHOTOELECTRICMODEL_H
#define SAUTERGAVRILAPHOTOELECTRICMODEL_H

#include "EMModel.h"

// from geantV
#include "Geant/Config.h"
namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {
class GeantTaskData;
}
} // namespace Geant

namespace geantphysics {
inline namespace GEANT_IMPL_NAMESPACE {
class Material;
class Element;
} // namespace GEANT_IMPL_NAMESPACE
} // namespace geantphysics

// from geantV
#include "Geant/Config.h"
namespace Geant {
    inline namespace GEANT_IMPL_NAMESPACE {
        class GeantTaskData;
    }
}

namespace geantphysics {
    inline namespace GEANT_IMPL_NAMESPACE {
        class Material;
        class Element;
    }
}


#include <string>

namespace geantphysics {
    
    class MaterialCuts;
    class AliasTable;
    class Particle;
    class LightTrack;
    
    /**
     * @brief   Photoelectric model for gamma.
     * @class   SauterGavrilaPhotoElectricModel
     * @author  M Bandieramonte
     * @date    June 2017
     *
     * PhotoElectric model for gamma based on SauterGavrila differential cross section
     * \cite
     */
    double *fAliasW;

    /** @brief The alias indices over the photon energy variable values. */
    int *fAliasIndx; // Alias indices
  };

  //---------------------------------------------
  /** @brief Linear array to store pointers to LinAlias data structures.
   *
   * The size is
   * SauterGavrilaPhotoElectricModel::fNumSamplingPrimEnergies.
   */
  LinAlias **fAliasData; // alias data structure

  //---------------------------------------------
  /** @brief An alias sampler used at run-time to sample the emitted e- (photoelectron) direction. */
  AliasTable *fAliasSampler;
};

} // namespace geantphysics

#endif // SAUTERGAVRILAPHOTOELECTRICMODEL_H
