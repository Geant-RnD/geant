
#ifndef CALOPRIMARYGENERATOR_H
#define CALOPRIMARYGENERATOR_H

// from GeantV
#include "PrimaryGenerator.h"
// for GEANT_IMPL_NAMESPACE
#include "Geant/Typedefs.h"

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

#include <string>

namespace userapplication {

/**
 * @brief User primary generator for CaloApp.
 *
 * The primary generator is a simple particle gun with configurable primary particle type and kinetic energy.
 *
 * @class   CaloPrimaryGenerator
 * @author  M Novak, minor edits by R Schmitz
 * @date    July 2017
 */

class CaloDetectorConstruction;

class CaloPrimaryGenerator : public Geant::PrimaryGenerator {
public:
  // CTR DTR
  CaloPrimaryGenerator(CaloDetectorConstruction *det);
 ~CaloPrimaryGenerator();

  // public setters/getters
  void SetPrimaryParticleName(std::string pname) { fPrimaryParticleName = pname; }
  const std::string & GetPrimaryParticleName() const { return fPrimaryParticleName; }

  void   SetPrimaryParticleEnergy(double ekin) { fPrimaryEkin = ekin; }
  double GetPrimaryParticleEnergy() const { return fPrimaryEkin; }

  void SetNumberOfPrimaryParticlePerEvent(int val) { fPrimaryPerEvent = val; }
  int  GetNumberOfPrimaryParticlePerEvent() const { return fPrimaryPerEvent; }

  const geantphysics::Particle* GetPrimaryParticle() const { return fParticle; }

  // interface methods
  virtual void InitPrimaryGenerator();
  virtual Geant::GeantEventInfo NextEvent();
  virtual void GetTrack(int n, Geant::GeantTrack &gtrack);

private:
 CaloPrimaryGenerator() = delete;
 CaloPrimaryGenerator(const CaloPrimaryGenerator &) = delete;
 CaloPrimaryGenerator &operator=(const CaloPrimaryGenerator &) = delete;

private:
  std::string  fPrimaryParticleName; // name of the primary particle
  int          fPrimaryPerEvent;           // number of primary particle to be generated per event
  int    fPDG;          // PDG code of parimary particles
  int    fGVPartIndex;  // internal GV particle index of the primary
  double fPrimaryEkin;  // kinetic energy of the primary in internal [energy] unit
  double fXPos;         // (x,y,z) position of the primary particles in internal [length] unit
  double fYPos;
  double fZPos;
  double fXDir;          // direction vector of the primary particles
  double fYDir;
  double fZDir;
  //
  double fMass;         // rest mass of the primary in internal [energy] unit
  double fCharge;       // charge of the primary in internal [charge] unit
  double fETotal;       // total energy of the primary in internal [energy] unit
  double fPTotal;       // total momentum of the primary in internal [energy] unit
  //
  const geantphysics::Particle    *fParticle; // the primary particle
  CaloDetectorConstruction  *fDetector; // the detector
};

}       // namespace userapplication

#endif  // CALOPRIMARYGENERATOR_H
