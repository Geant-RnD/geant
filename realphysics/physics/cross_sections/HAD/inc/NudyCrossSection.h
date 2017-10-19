//==================================================================
// @file NudyCrossSection.h
// @author Abhijit Bhattacharyya
// @brief Interface of NUDY to GV for getting CS to use in HAD
//
// Like Hadronic CS we are initially showing elastic and inelastic
// Later it should cover thermal etc.
//======================================================================

#ifndef NUDY_CROSS_SECTION_H
#define NUDY_CROSS_SECTION_H

#include <vector>
#include <string>

#ifdef USE_ROOT
#include "NudyInterface.h"
#endif


// Forward declaration
namespace geantphysics {
  class LightTrack;
  class LightTrack_v;
  class HadronicCrossSection;
  class HadronicProcess;
  enum class HadronicProcessType;

  inline namespace GEANT_IMPL_NAMESPACE {
    class Isotope;
    class Material;
    class Element;
  }
}

namespace NudyPhysics {
  class NudyInterface;
}

namespace geantphysics {

  class NudyCrossSection {
  public:
    // @brief Use NUDY Crossection if HadronicCrossSection method
    // HadronicCrossSection::IsApplilcable() returns false;
    // This could be done for elements having resonance and KE of "n" < 20 MeV.

    NudyCrossSection();
    NudyCrossSection( const std::string isoName, const int Aval, const int Zval,
      const double projectileKE);
    NudyCrossSection( const NudyCrossSection &other );
    NudyCrossSection& operator=( const NudyCrossSection &other );
    virtual ~NudyCrossSection();    // destructor

    double GetIsotopeCrossSection(int projCode, double projKE, double temp, std::string isoName,
      int tZ, int tA, HadronicProcessType pType);

    //--------- Getters --------------
    std::string GetisoName() const { return fIsoName; }           // returns name of the isotope
    int GetAval() const { return fTargetA; }                      // returns neucleon number of isotope
    int GetZval() const { return fTargetZ; }                      // returns atomic number of isotope
    double GetProjKE() const { return fProjEnergy; }              // returns KE of projectile

    // Method that returns true is cross section is applicable
    virtual bool IsApplicable(const int projCode, const double projKE ); // this checks for neutron and KE < 20 MeV

    //---------- Setters ----------------
    void SetisoName ( const std::string &isoname ) { fIsoName = isoname; }
    void SetAval ( const int Aval ) { fTargetA = Aval; }
    void SetZval ( const int Zval ) { fTargetZ = Zval; }
    void SetProjKE ( const double projke ) { fProjEnergy = projke; }


  private:
    std::string fIsoName;                     // Isotope Name for which CS required
    int fTargetA;                          // Nucleon number for the Isotope
    int fTargetZ;                         // Atomic number for the isotope
    std::vector<int> fProjectileCodeVec;   // vector of GV code for allowed projectiles
    int fProjCode;                        // code for projectile as par code vector from HAD CS e.g. neutron = 3
    double fProjEnergy;                    // LE for the projectile
  };

}  // namespace

#endif
