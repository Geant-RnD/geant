/*****************************************************************
 * @file NudyCrossSectionStore.h
 * @author Abhijit Bhattacharyya
 * @brief class for all GV process cross section like Hadronic processes.
 ********************************************************************/
#ifndef NUDY_CROSS_SECTION_STORE
#define NUDY_CROSS_SECTION_STORE

#include <string>
#include <vector>
#include "Geant/Config.h"

namespace geantphysics {
  class HadronicCrossSection;
  class HadronicProcess;
  enum class HadronicProcessType;
  inline namespace GEANT_IMPL_NAMESPACE {
    class Isotope;
    class Material;
    class Element;
  }
  class NudyInterface;
  class NudyCrossSection;

  class NudyCrossSectionStore {
  public:
    // constructor
    NudyCrossSectionStore();
    NudyCrossSectionStore( const std::string name );
    NudyCrossSectionStore( const NudyCrossSectionStore &other );
    NudyCrossSectionStore& operator=( const NudyCrossSectionStore &other );

    // destructor
    ~NudyCrossSectionStore();

    // @brief registers a cross sectio  to the store
    void RegisterNudyCrossSection( NudyCrossSection* ptrnudyxsec ); // following LIFO


    //@brief few points to note:
    //@param projectileCode = GV particle code
    //@param projectileEnergy = Energy expressed in GeV.

    //@brief return index of the applicable cross section that was registered last
    int GetIndexFirstApplicableXsec( const int projectileCode,
      const double projectileEnergy );

    // @brief get microscopic cross section for target isotope
    double GetIsotopeCrossSection( const int projectileCode,
      const double projectileEnergy, const Isotope* targetIsotope,
       HadronicProcessType pType
    );

    // @brief get cross section for element
    double GetElementCrossSection( const int projectileCode,
      const double projectileEnergy, const Element* targetElement,
      HadronicProcessType pType
    );

    // @brief get macroscopic cross section for target
    double GetMacroscopicCrossSection( const int projectileCode,
      const double projectileEnergy, const Material* targetMaterial, HadronicProcessType pType
    );

    // @brief Method that returns a target neucleus as a pair of Z and A
    std::pair< int, int > SampleTarget( const int projectileCode,
      const double projectileEnergy,const Material* targetMaterial, HadronicProcessType pType
    );

    // @brief Method returns vector of NudyCrossSection
    std::vector< NudyCrossSection* >& GetNudyCrossSectionVec() {
      return fNudyXsecVec;
    }

    // @brief method returns name of NudyCrossSection
    std::string GetName() const { return fChannelName; }

    //@brief Method that sets the name of Nudy Cross section
    void SetName( const std::string &name ) { fChannelName = name; }

  private:
    std::vector< NudyCrossSection* > fNudyXsecVec; // vector of Nudy Cross section
    std::string fChannelName;
  };   // end of class
}


#endif
