/***********************************************************
 * @file TroitskPrimaryGenerator.h
 * @author Abhijit Bhattacharyya
 * @brief this is used to define primary gun generator characteristics
 **************************************************************/
#ifndef Troitsk_PRIMARYGENERATOR_H
#define Troitsk_PRIMARYGENERATOR_H

#include <string>
#include <cmath>

// from application
#include "TroitskGeometryConstruction.h"

// from GeantV
#include "PrimaryGenerator.h"
#include "Particle.h"
#include "GeantTrack.h"
#include "Geant/Typedefs.h"

// forGEANT_IMPL_NAMESPACE
namespace GEANT_IMPL_NAMESPACE {
 namespace Geant {
   class GeantTrack;
   class GeantEventInfo;
 }
}

// geantphysics classdef
namespace geantphysics {
 class Particle;
}

namespace userapplication{
  class TroitskGeometryConstruction;

  class TroitskPrimaryGenerator : public Geant::PrimaryGenerator {
  public:
    TroitskPrimaryGenerator(const TroitskGeometryConstruction *geomTARC);
    ~TroitskPrimaryGenerator();

    // Getters
    const std::string &GetPrimaryParticleName() const { return fPrimaryParticleName; }
    double GetPrimaryParticleEnergy()const { return fPrimaryKE; }
    int GetNumberOfPrimaryParticlePerEvent() const { return fPrimaryPerEvent; }
    const geantphysics::Particle* GetPrimaryParticle() const { return fParticle; }

    // Setters
    void SetPrimaryParticleName( std::string particleName ) { fPrimaryParticleName = particleName; }
    void SetPrimaryParticleEnergy( double ekin )  { fPrimaryKE = ekin; }
    void SetNumberOfPrimaryParticlePerEvent( int val ) { fPrimaryPerEvent = val; }

    // interfacing
    virtual void InitPrimaryGenerator();
    virtual Geant::GeantEventInfo NextEvent();
    virtual void GetTrack( int n, Geant::GeantTrack &gTrack );

  private:
    TroitskPrimaryGenerator() = delete;
    TroitskPrimaryGenerator( const TroitskPrimaryGenerator & ) = delete;
    TroitskPrimaryGenerator &operator=( const TroitskPrimaryGenerator & ) = delete;

  private:
    std::string fPrimaryParticleName; // Name of the primary particle;
    int         fPrimaryPerEvent;     // Number of primary particle to be generated per event
    int         fPDG;                 // PDG code of primary particles
    int         fGVPartIndex;         // Internal GV particle index of the primary
    double      fPrimaryKE;           // KE of the primary in internal [energy] unit

    double      fXPos;                // (x, y, z) position of the primary particles in internal [length] unit
    double      fYPos;
    double      fZPos;

    double      fXDir;                // Direction vector of the primary particles
    double      fYDir;
    double      fZDir;

    double      fMass;                // Rest Mass of the primary in internal [energy] unit
    double      fCharge;              // charge of the primary in internal [energy] unit
    double      fEnergyTotal;         // Total Energy of primary in internal [energy] unit
    double      fMomentumTotal;       // Total momentum of the primary in internal [energy] unit

    const geantphysics::Particle   *fParticle;  // the primary Particle
    const TroitskGeometryConstruction *fGeometry;  // The geometry

  }; // class ends


} // namespace ends Here


#endif
