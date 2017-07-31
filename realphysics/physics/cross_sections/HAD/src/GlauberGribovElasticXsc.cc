#include "GlauberGribovElasticXsc.h"
#include <iostream>
namespace geantphysics {

  GlauberGribovElasticXsc::GlauberGribovElasticXsc() 
  {
    this->SetName("GlauberGribovElasticXsc");

    std::vector< int > projVec;
    projVec.push_back(1);
    projVec.push_back(3);
    
    this->SetProjectileCodeVec(projVec);
    
    GGTotalXsc = new GlauberGribovTotalXsc();
    GGInelasticXsc = new GlauberGribovInelasticXsc();
  }


  GlauberGribovElasticXsc::~GlauberGribovElasticXsc()
  {
    delete GGTotalXsc;
    delete GGInelasticXsc;
  }

  ////////////////////////////////////////////////////////////////////////////////////////
  //
  // Derives elastic as total - inelastic according to
  // Glauber model with Gribov correction calculated in the dipole approximation on
  // light cone. Gaussian density of point-like nucleons helps to calculate rest integrals of the model.
  // [1] B.Z. Kopeliovich, nucl-th/0306044 + simplification above

  double GlauberGribovElasticXsc::GetIsotopeCrossSection(const int particleCode, const double energyKin, const double mass,
							 const int Z, const int N)
  {
    double HadronNucleusElasticXsc =
      GGTotalXsc->GetIsotopeCrossSection(particleCode, energyKin, mass, Z, N) -
      GGInelasticXsc->GetIsotopeCrossSection(particleCode, energyKin, mass, Z, N);
    
    if (HadronNucleusElasticXsc < 0) HadronNucleusElasticXsc = 0;

    return HadronNucleusElasticXsc;
  }

} // namespace geantphysics

