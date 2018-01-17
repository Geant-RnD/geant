/***********************************************************
 * @file TARCPrimaryGenerator.h
 * @author Abhijit Bhattacharyya
 * @brief this is used to defien primary gun generator characteristics
 **************************************************************/
#ifndef TARC_PRIMARYGENERATOR_H
#define TARC_PRIMARYGENERATOR_H

#include <string>
#include <cmath>

// from application
#include "TARCGeometryConstruction.h"
#include "TARC.h"

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
  class TARCGeometryConstruction;

  class TARCPrimaryGenerator : public Geant::PrimaryGenerator {
  public:
    TARCPrimaryGenerator(const TARCGeometryConstruction *geomTARC);
    ~TARCPrimaryGenerator();

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
    TARCPrimaryGenerator() = delete;
    TARCPrimaryGenerator( const TARCPrimaryGenerator & ) = delete;
    TARCPrimaryGenerator &operator=( const TARCPrimaryGenerator & ) = delete;

  private:
    std::string fPrimaryParticleName; // Name of the primary particle;
    int         fPrimaryPerEvent;     // Number of primary particle to be generated per event
    int         fPDG;                 // PDG code of primary particles
    int         fGVPartIndex;         // Internal GV particle index of the primary
    double      fPrimaryKE;           // KE of the primary in internal [energy] unit

    vecgeom::Vector3D<double> fGunPos;
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
    const TARCGeometryConstruction *fGeomSetup;  // The geometry

  }; // class ends


} // namespace ends Here


#endif
