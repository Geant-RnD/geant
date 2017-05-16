#ifndef GEANT_TYPEDEFS_H
#define GEANT_TYPEDEFS_H

#include "Geant/Config.h"

#ifdef VECCORE_CUDA
#include "base/Vector.h"
  template <class T>
  using vector_t = vecgeom::Vector<T>;
#else
#include <vector>
#ifdef GEANT_USE_NUMA
#include <GeantNuma.h>
  template <class T>
  using vector_t = std::vector<T, Geant::NumaAllocator<T>>;
#else
  template <class T>
  using vector_t = std::vector<T>;
#endif
#endif

#ifdef USE_VECGEOM_NAVIGATOR
#include "navigation/NavigationState.h"
typedef VECGEOM_NAMESPACE::NavigationState VolumePath_t;
#include "materials/Material.h"
typedef VECGEOM_NAMESPACE::Material Material_t;
#include "materials/Medium.h"
typedef VECGEOM_NAMESPACE::Medium Medium_t;
#include "volumes/LogicalVolume.h"
typedef VECGEOM_NAMESPACE::LogicalVolume Volume_t;
#include "volumes/PlacedVolume.h"
typedef VECGEOM_NAMESPACE::VPlacedVolume Node_t;
#include "Particle.h"
#ifdef Particle_H
typedef geant::Particle Particle_t;
#else
typedef geantphysics::Particle Particle_t;
#endif
#else
#include "TGeoBranchArray.h"
typedef TGeoBranchArray VolumePath_t;
#include "TGeoMaterial.h"
typedef TGeoMaterial Material_t;
#include "TGeoMedium.h"
typedef TGeoMedium Medium_t;
#include "TGeoVolume.h"
typedef TGeoVolume Volume_t;
#include "TGeoNode.h"
typedef TGeoNode Node_t;
#include "TDatabasePDG.h"
typedef TParticlePDG Particle_t;
#endif
#endif
