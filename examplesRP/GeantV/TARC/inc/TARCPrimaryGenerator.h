/***********************************************************
 * @file TARCPrimaryGenerator.h
 * @author Abhijit Bhattacharyya
 * @brief this is used to defien primary gun generator characteristics
 **************************************************************/
#ifndef TARC_PRIMARYGENERATOR_H
#define TARC_PRIMARYGENERATOR_H

#include <string>
#include <cmath>
#include <map>

// from application
#include "TARCGeometryConstruction.h"
#include "TARCapp.h"

// from GeantV
#include "PrimaryGenerator.h"
#include "Particle.h"
#include "GeantTrack.h"
#include "Geant/Typedefs.h"

// forGEANT_IMPL_NAMESPACE
namespace GEANT_IMPL_NAMESPACE {
 namespace Geant {
   class GeantTrack;
   class GeantTaskData;
   class GeantEventInfo;
 }
}

// geantphysics classdef
namespace geantphysics {
 class Particle;
}

namespace tarcapp{
  class TARCGeometryConstruction;

  class TARCPrimaryGenerator : public Geant::PrimaryGenerator {
  public:
    // TARCPrimaryGenerator(const TARCGeometryConstruction *);
    TARCPrimaryGenerator();
    virtual ~TARCPrimaryGenerator();

    // Getters
    static const std::string& GetPrimaryName(const int primarytypeindex) { return gNameParticlesVector[primarytypeindex];} //
    double GetPrimaryParticleEnergy()const { return fPrimaryKE; }
    int GetNumberOfPrimaryPerEvent() const { return fNumPrimaryPerEvt; } //
    static int GetNumberOfPrimaryTypes() { return gNumberCandidateParticles;} //
    static int GetPrimaryTypeIndex(const std::string& pname); //
    static int GetMaxNumberOfPrimariesPerEvent() {return gSupNumPrimaryPerEvt;}


    // Setters
    void SetPrimaryName( const std::string& );
    void SetPrimaryEnergy( const double );
    void SetNumberOfPrimaryPerEvent( int );
    void SetPrimaryDirection(const double pdir[3]);
    void SetGunPosition(const double rpos[3]);

    // interfacing
    virtual void InitPrimaryGenerator(){};
    virtual Geant::GeantEventInfo NextEvent(Geant::GeantTaskData* );
    virtual void GetTrack( int, Geant::GeantTrack &, Geant::GeantTaskData* );

    static void Print();

  private:
    //TARCPrimaryGenerator() = delete;
    TARCPrimaryGenerator( const TARCPrimaryGenerator & ) = delete;
    TARCPrimaryGenerator &operator=( const TARCPrimaryGenerator &) = delete;

  private:
    static const int                          gNumberCandidateParticles;
    static const std::string                  gNameParticlesVector[];
    static const std::map<std::string, int>   gPrimaNameToIndexMap;

    static const int                          gInfNumPrimaryPerEvt;
    static const int                          gSupNumPrimaryPerEvt;

    static const double                       gInfBeamEnergy;
    static const double                       gSupBeamEnergy;

    static int                                gNumPrimaryPerEvt;
    static double                             gPrimaryEnergy;
    static std::string                        gPrimaryType;
    static double                             gPrimaryDir[3];

    bool                                      fIsUserNumPrimaryPerEvt;
    bool                                      fIsUserPrimaryType;
    bool                                      fIsUserPrimaryDir;
    bool                                      fIsUserPrimaryEnergy;

    std::string fPrimaryParticleName; // Name of the primary particle;
    int         fNumPrimaryPerEvt;     // Number of primary particle to be generated per event
    int         fPDG;                 // PDG code of primary particles
    int         fGVPartIndex;         // Internal GV particle index of the primary
    double      fPrimaryKE;           // KE of the primary in internal [energy] unit
    double      fPrimaryParticleEnergy;

    vecgeom::Vector3D<double> fGunPos;
    double      fXPos;                // (x, y, z) position of the primary particles in internal [length] unit
    double      fYPos;
    double      fZPos;

    double      fXDir;                // Direction vector of the primary particles
    double      fYDir;
    double      fZDir;

  }; // class ends


} // namespace ends Here


#endif
