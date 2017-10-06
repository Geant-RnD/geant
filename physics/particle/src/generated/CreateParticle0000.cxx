// This files was autogenerated by geant::ParticleOld::ReadFile

#if defined(__clang__) && !defined(__APPLE__)
#pragma clang optimize off
#endif
#include "ParticleOld.h"
#ifdef VECCORE_CUDA
#include "base/Vector.h"
template <typename T>
using vector = vecgeom::Vector<T>;
#else
using std::vector;
#endif

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {


//________________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void CreateParticle0000() {
   vector<int> daughters;
   ParticleOld *part = nullptr;

   // Creating Alpha_bar
   new ParticleOld("Alpha_bar", -1000020040, 0, "ion", 100, -2, 3.7284, 1.6916e-33, 100, 100, 0, 100, 1);

   // Creating HE3_bar
   new ParticleOld("HE3_bar", -1000020030, 0, "ion", 100, -2, 2.80941, 0, 100, 100, 0, 100, 1);

   // Creating Triton_bar
   new ParticleOld("Triton_bar", -1000010030, 0, "ion", 100, -1, 2.80941, 1.6916e-33, 100, 100, 0, 100, 1);

   // Creating Deuteron_bar
   new ParticleOld("Deuteron_bar", -1000010020, 0, "ion", 100, -1, 1.87106, 0, 100, 100, 0, 100, 1);

   // Creating N(2250)+_bar
   new ParticleOld("N(2250)+_bar", -100012210, 0, "Unknown", 100, -1, 2.275, 0.5, 100, 100, 0, 100, 1);

   // Creating N(2250)0_bar
   new ParticleOld("N(2250)0_bar", -100012110, 0, "Unknown", 100, 0, 2.275, 0.5, 100, 100, 0, 100, 1);

   // Creating N(2220)+_bar
   new ParticleOld("N(2220)+_bar", -100002210, 0, "Unknown", 100, -1, 2.25, 0.4, 100, 100, 0, 100, 1);

   // Creating N(2220)0_bar
   new ParticleOld("N(2220)0_bar", -100002110, 0, "Unknown", 100, 0, 2.25, 0.4, 100, 100, 0, 100, 1);

   // Creating GenericIon_bar
   new ParticleOld("GenericIon_bar", -50000060, 0, "Unknown", 100, -0.333333, 0.938272, 0, 100, 100, 0, 100, 1);

   // Creating Cherenkov_bar
   new ParticleOld("Cherenkov_bar", -50000050, 0, "Unknown", 100, 0, 0, 0, 100, 100, 0, 100, 1);

   // Creating f2(2010)_bar
   new ParticleOld("f2(2010)_bar", -9060225, 0, "Unknown", 100, 0, 2.01, 0.2, 100, 100, 0, 100, 1);

   // Creating f2(1810)_bar
   new ParticleOld("f2(1810)_bar", -9030225, 0, "Unknown", 100, 0, 1.815, 0.197, 100, 100, 0, 100, 1);

   // Creating f0(1500)_bar
   new ParticleOld("f0(1500)_bar", -9030221, 0, "Unknown", 100, 0, 1.505, 0.109, 100, 100, 0, 100, 1);

   // Creating eta(1405)_bar
   new ParticleOld("eta(1405)_bar", -9020221, 0, "Unknown", 100, 0, 1.4098, 0.0511, 100, 100, 0, 100, 1);

   // Creating f0(980)_bar
   new ParticleOld("f0(980)_bar", -9010221, 0, "Unknown", 100, 0, 0.98, 0.07, 100, 100, 0, 100, 1);

   // Creating f0(600)_bar
   new ParticleOld("f0(600)_bar", -9000221, 0, "Unknown", 100, 0, 0.8, 0.8, 100, 100, 0, 100, 1);

   // Creating a0(980)-_bar
   new ParticleOld("a0(980)-_bar", -9000211, 0, "Unknown", 100, -1, 0.98, 0.06, 100, 100, 0, 100, 1);

   // Creating a0(980)0_bar
   new ParticleOld("a0(980)0_bar", -9000111, 0, "Unknown", 100, 0, 0.98, 0.075, 100, 100, 0, 100, 1);

   // Creating nu*_e0_bar
   new ParticleOld("nu*_e0_bar", -4000012, 0, "Excited", 100, 0, 400, 0.41917, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4000012));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(0, 0.610139,  daughters));
   daughters.clear();
   daughters.push_back(-23);
   daughters.push_back(-12);
   part->AddDecay(ParticleOld::Decay(0, 0.389861,  daughters));

   // Creating e*+
   new ParticleOld("e*+", -4000011, 0, "Excited", 100, 1, 400, 0.42901, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4000011));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(-12);
   part->AddDecay(ParticleOld::Decay(0, 0.596149,  daughters));
   daughters.clear();
   daughters.push_back(-22);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(0, 0.294414,  daughters));
   daughters.clear();
   daughters.push_back(-23);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(0, 0.109437,  daughters));

   // Creating u*_bar
   new ParticleOld("u*_bar", -4000002, 0, "Excited", 100, -0.666667, 400, 2.65499, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4000002));
   daughters.clear();
   daughters.push_back(-21);
   daughters.push_back(-2);
   part->AddDecay(ParticleOld::Decay(0, 0.853166,  daughters));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(-1);
   part->AddDecay(ParticleOld::Decay(0, 0.0963291,  daughters));
   daughters.clear();
   daughters.push_back(-23);
   daughters.push_back(-2);
   part->AddDecay(ParticleOld::Decay(0, 0.029361,  daughters));
   daughters.clear();
   daughters.push_back(-22);
   daughters.push_back(-2);
   part->AddDecay(ParticleOld::Decay(0, 0.021144,  daughters));

   // Creating d*_bar
   new ParticleOld("d*_bar", -4000001, 0, "Excited", 100, 0.333333, 400, 2.65171, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4000001));
   daughters.clear();
   daughters.push_back(-21);
   daughters.push_back(-1);
   part->AddDecay(ParticleOld::Decay(53, 0.85422,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(-2);
   part->AddDecay(ParticleOld::Decay(0, 0.096449,  daughters));
   daughters.clear();
   daughters.push_back(-23);
   daughters.push_back(-1);
   part->AddDecay(ParticleOld::Decay(0, 0.044039,  daughters));
   daughters.clear();
   daughters.push_back(-22);
   daughters.push_back(-1);
   part->AddDecay(ParticleOld::Decay(0, 0.005292,  daughters));

   // Creating ~nu_tauR_bar
   new ParticleOld("~nu_tauR_bar", -2000016, 0, "Sparticle", 100, 0, 500, 0, 100, 100, 1, 100, 1);

   // Creating ~tau_2+
   new ParticleOld("~tau_2+", -2000015, 0, "Sparticle", 100, 1, 500, 1, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-2000015));
   daughters.clear();
   daughters.push_back(-1000039);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000024);
   daughters.push_back(-16);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000037);
   daughters.push_back(-16);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000022);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000023);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000025);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000035);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000015);
   daughters.push_back(-23);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000015);
   daughters.push_back(-25);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000015);
   daughters.push_back(-35);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000015);
   daughters.push_back(-36);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000016);
   daughters.push_back(24);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-2000016);
   daughters.push_back(24);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000016);
   daughters.push_back(37);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-2000016);
   daughters.push_back(37);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));

   // Creating ~nu_muR_bar
   new ParticleOld("~nu_muR_bar", -2000014, 0, "Sparticle", 100, 0, 500, 0, 100, 100, 1, 100, 1);

   // Creating ~mu_R+
   new ParticleOld("~mu_R+", -2000013, 0, "Sparticle", 100, 1, 500, 1, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-2000013));
   daughters.clear();
   daughters.push_back(-1000039);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000024);
   daughters.push_back(-14);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000037);
   daughters.push_back(-14);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000022);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000023);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000025);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000035);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000013);
   daughters.push_back(-23);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000013);
   daughters.push_back(-25);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000013);
   daughters.push_back(-35);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000013);
   daughters.push_back(-36);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000014);
   daughters.push_back(24);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-2000014);
   daughters.push_back(24);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000014);
   daughters.push_back(37);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-2000014);
   daughters.push_back(37);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));

   // Creating ~nu_eR_bar
   new ParticleOld("~nu_eR_bar", -2000012, 0, "Sparticle", 100, 0, 500, 0, 100, 100, 1, 100, 1);

   // Creating ~e_R+
   new ParticleOld("~e_R+", -2000011, 0, "Sparticle", 100, 1, 500, 1, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-2000011));
   daughters.clear();
   daughters.push_back(-1000039);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000024);
   daughters.push_back(-12);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000037);
   daughters.push_back(-12);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000022);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000023);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000025);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000035);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000011);
   daughters.push_back(-23);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000011);
   daughters.push_back(-25);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000011);
   daughters.push_back(-35);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000011);
   daughters.push_back(-36);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000012);
   daughters.push_back(24);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-2000012);
   daughters.push_back(24);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000012);
   daughters.push_back(37);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-2000012);
   daughters.push_back(37);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
}

} // End of inline namespace
} // End of geant namespace
#if defined(__clang__) && !defined(__APPLE__)
#pragma clang optimize on
#endif
