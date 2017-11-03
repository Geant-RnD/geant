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
void CreateParticle0031() {
   vector<int> daughters;
   ParticleOld *part = nullptr;

   // Creating H+
   new ParticleOld("H+", 37, 1, "GaugeBoson", 100, 1, 300, 5.75967, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(37));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(25);
   part->AddDecay(ParticleOld::Decay(0, 0.929792,  daughters));
   daughters.clear();
   daughters.push_back(-5);
   daughters.push_back(6);
   part->AddDecay(ParticleOld::Decay(32, 0.067484,  daughters));
   daughters.clear();
   daughters.push_back(-15);
   daughters.push_back(16);
   part->AddDecay(ParticleOld::Decay(0, 0.002701,  daughters));
   daughters.clear();
   daughters.push_back(-3);
   daughters.push_back(4);
   part->AddDecay(ParticleOld::Decay(32, 1.3e-05,  daughters));
   daughters.clear();
   daughters.push_back(-13);
   daughters.push_back(14);
   part->AddDecay(ParticleOld::Decay(0, 1e-05,  daughters));
   daughters.clear();
   daughters.push_back(-1);
   daughters.push_back(2);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));
   daughters.clear();
   daughters.push_back(-7);
   daughters.push_back(8);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));
   daughters.clear();
   daughters.push_back(-17);
   daughters.push_back(18);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(-11);
   daughters.push_back(12);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000022);
   daughters.push_back(1000024);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000022);
   daughters.push_back(1000037);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000023);
   daughters.push_back(1000024);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000023);
   daughters.push_back(1000037);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000025);
   daughters.push_back(1000024);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000025);
   daughters.push_back(1000037);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000035);
   daughters.push_back(1000024);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000035);
   daughters.push_back(1000037);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000006);
   daughters.push_back(-1000005);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(2000006);
   daughters.push_back(-1000005);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000006);
   daughters.push_back(-2000005);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(2000006);
   daughters.push_back(-2000005);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000001);
   daughters.push_back(1000002);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000003);
   daughters.push_back(1000004);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000011);
   daughters.push_back(1000012);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000013);
   daughters.push_back(1000014);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-1000015);
   daughters.push_back(1000016);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(-2000015);
   daughters.push_back(1000016);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));

   // Creating eta_tech0
   new ParticleOld("eta_tech0", 38, 0, "Unknown", 100, 0, 350, 0.10158, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(38));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(21);
   part->AddDecay(ParticleOld::Decay(32, 0.547101,  daughters));
   daughters.clear();
   daughters.push_back(5);
   daughters.push_back(-5);
   part->AddDecay(ParticleOld::Decay(32, 0.452899,  daughters));
   daughters.clear();
   daughters.push_back(6);
   daughters.push_back(-6);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));

   // Creating LQ_ue
   new ParticleOld("LQ_ue", 39, 1, "Unknown", 100, -0.333333, 200, 0.39162, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(39));
   daughters.clear();
   daughters.push_back(2);
   daughters.push_back(11);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating R0
   new ParticleOld("R0", 40, 1, "Unknown", 100, 0, 5000, 417.465, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(40));
   daughters.clear();
   daughters.push_back(1);
   daughters.push_back(-3);
   part->AddDecay(ParticleOld::Decay(32, 0.215134,  daughters));
   daughters.clear();
   daughters.push_back(2);
   daughters.push_back(-4);
   part->AddDecay(ParticleOld::Decay(32, 0.215134,  daughters));
   daughters.clear();
   daughters.push_back(3);
   daughters.push_back(-5);
   part->AddDecay(ParticleOld::Decay(32, 0.215133,  daughters));
   daughters.clear();
   daughters.push_back(4);
   daughters.push_back(-6);
   part->AddDecay(ParticleOld::Decay(32, 0.214738,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(0, 0.0699301,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(0, 0.0699301,  daughters));
   daughters.clear();
   daughters.push_back(5);
   daughters.push_back(-7);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));
   daughters.clear();
   daughters.push_back(6);
   daughters.push_back(-8);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));
   daughters.clear();
   daughters.push_back(15);
   daughters.push_back(-17);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));

   // Creating pi_tech0
   new ParticleOld("pi_tech0", 51, 0, "Unknown", 100, 0, 110, 0.04104, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(51));
   daughters.clear();
   daughters.push_back(5);
   daughters.push_back(-5);
   part->AddDecay(ParticleOld::Decay(32, 0.596654,  daughters));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(21);
   part->AddDecay(ParticleOld::Decay(32, 0.316112,  daughters));
   daughters.clear();
   daughters.push_back(15);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(0, 0.050055,  daughters));
   daughters.clear();
   daughters.push_back(4);
   daughters.push_back(-4);
   part->AddDecay(ParticleOld::Decay(32, 0.036777,  daughters));
   daughters.clear();
   daughters.push_back(3);
   daughters.push_back(-3);
   part->AddDecay(ParticleOld::Decay(32, 0.000225,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(0, 0.000177,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(6);
   daughters.push_back(-6);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));

   // Creating pi_tech+
   new ParticleOld("pi_tech+", 52, 1, "Unknown", 100, 1, 110, 0.0105, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(52));
   daughters.clear();
   daughters.push_back(4);
   daughters.push_back(-5);
   part->AddDecay(ParticleOld::Decay(32, 0.90916,  daughters));
   daughters.clear();
   daughters.push_back(-15);
   daughters.push_back(16);
   part->AddDecay(ParticleOld::Decay(0, 0.048905,  daughters));
   daughters.clear();
   daughters.push_back(4);
   daughters.push_back(-3);
   part->AddDecay(ParticleOld::Decay(32, 0.041762,  daughters));
   daughters.clear();
   daughters.push_back(-13);
   daughters.push_back(14);
   part->AddDecay(ParticleOld::Decay(0, 0.000173,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(5);
   daughters.push_back(-5);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));
   daughters.clear();
   daughters.push_back(-11);
   daughters.push_back(12);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));

   // Creating pi'_tech0
   new ParticleOld("pi'_tech0", 53, 0, "Unknown", 100, 0, 110, 0.02807, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(53));
   daughters.clear();
   daughters.push_back(5);
   daughters.push_back(-5);
   part->AddDecay(ParticleOld::Decay(32, 0.872445,  daughters));
   daughters.clear();
   daughters.push_back(15);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(0, 0.0731921,  daughters));
   daughters.clear();
   daughters.push_back(4);
   daughters.push_back(-4);
   part->AddDecay(ParticleOld::Decay(32, 0.0537761,  daughters));
   daughters.clear();
   daughters.push_back(3);
   daughters.push_back(-3);
   part->AddDecay(ParticleOld::Decay(32, 0.000328,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(0, 0.000259,  daughters));
   daughters.clear();
   daughters.push_back(6);
   daughters.push_back(-6);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(21);
   part->AddDecay(ParticleOld::Decay(32, 0,  daughters));
}

} // End of inline namespace
} // End of geant namespace
#if defined(__clang__) && !defined(__APPLE__)
#pragma clang optimize on
#endif
