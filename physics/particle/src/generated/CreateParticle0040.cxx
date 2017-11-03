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
void CreateParticle0040() {
   vector<int> daughters;
   ParticleOld *part = nullptr;

   // Creating B*_c+
   new ParticleOld("B*_c+", 543, 1, "B-Meson", 100, 1, 6.602, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(543));
   daughters.clear();
   daughters.push_back(541);
   daughters.push_back(22);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating B*_2c+
   new ParticleOld("B*_2c+", 545, 1, "B-Meson", 100, 1, 7.35, 0.02, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(545));
   daughters.clear();
   daughters.push_back(511);
   daughters.push_back(411);
   part->AddDecay(ParticleOld::Decay(0, 0.3,  daughters));
   daughters.clear();
   daughters.push_back(521);
   daughters.push_back(421);
   part->AddDecay(ParticleOld::Decay(0, 0.3,  daughters));
   daughters.clear();
   daughters.push_back(513);
   daughters.push_back(411);
   part->AddDecay(ParticleOld::Decay(0, 0.2,  daughters));
   daughters.clear();
   daughters.push_back(523);
   daughters.push_back(421);
   part->AddDecay(ParticleOld::Decay(0, 0.2,  daughters));

   // Creating eta_b
   new ParticleOld("eta_b", 551, 0, "B-Meson", 100, 0, 9.4, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(551));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(21);
   part->AddDecay(ParticleOld::Decay(32, 1,  daughters));

   // Creating Upsilon
   new ParticleOld("Upsilon", 553, 0, "B-Meson", 100, 0, 9.4603, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(553));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(21);
   daughters.push_back(21);
   part->AddDecay(ParticleOld::Decay(4, 0.7743,  daughters));
   daughters.clear();
   daughters.push_back(4);
   daughters.push_back(-4);
   part->AddDecay(ParticleOld::Decay(32, 0.045,  daughters));
   daughters.clear();
   daughters.push_back(2);
   daughters.push_back(-2);
   part->AddDecay(ParticleOld::Decay(32, 0.045,  daughters));
   daughters.clear();
   daughters.push_back(22);
   daughters.push_back(21);
   daughters.push_back(21);
   part->AddDecay(ParticleOld::Decay(4, 0.029,  daughters));
   daughters.clear();
   daughters.push_back(15);
   daughters.push_back(-15);
   part->AddDecay(ParticleOld::Decay(0, 0.0267,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-11);
   part->AddDecay(ParticleOld::Decay(0, 0.0252,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-13);
   part->AddDecay(ParticleOld::Decay(0, 0.0248,  daughters));
   daughters.clear();
   daughters.push_back(3);
   daughters.push_back(-3);
   part->AddDecay(ParticleOld::Decay(32, 0.015,  daughters));
   daughters.clear();
   daughters.push_back(1);
   daughters.push_back(-1);
   part->AddDecay(ParticleOld::Decay(32, 0.015,  daughters));

   // Creating chi_2b
   new ParticleOld("chi_2b", 555, 0, "B-Meson", 100, 0, 9.9122, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(555));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(21);
   part->AddDecay(ParticleOld::Decay(32, 0.78,  daughters));
   daughters.clear();
   daughters.push_back(553);
   daughters.push_back(22);
   part->AddDecay(ParticleOld::Decay(0, 0.22,  daughters));

   // Creating dd_1
   new ParticleOld("dd_1", 1103, 1, "Unknown", 100, -0.666667, 0.96, 0, -100, -1, -100, -1, -1);

   // Creating delta(1620)-
   new ParticleOld("delta(1620)-", 1112, 1, "Unknown", 100, -1, 1.63, 0.145, -100, 0, -100, -1, -1);

   // Creating Delta-
   new ParticleOld("Delta-", 1114, 1, "Unknown", 100, -1, 1.232, 0.12, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(1114));
   daughters.clear();
   daughters.push_back(2112);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating delta(1905)-
   new ParticleOld("delta(1905)-", 1116, 1, "Unknown", 100, -1, 1.89, 0.33, -100, 0, -100, -1, -1);

   // Creating delta(1950)-
   new ParticleOld("delta(1950)-", 1118, 1, "Unknown", 100, -1, 1.93, 0.28, -100, 0, -100, -1, -1);

   // Creating delta(1620)0
   new ParticleOld("delta(1620)0", 1212, 1, "Unknown", 100, 0, 1.63, 0.145, -100, 0, -100, -1, -1);

   // Creating N(1520)0
   new ParticleOld("N(1520)0", 1214, 1, "Unknown", 100, 0, 1.52, 0.115, -100, 0, -100, -1, -1);

   // Creating delta(1905)0
   new ParticleOld("delta(1905)0", 1216, 1, "Unknown", 100, 0, 1.89, 0.33, -100, 0, -100, -1, -1);

   // Creating N(2190)0
   new ParticleOld("N(2190)0", 1218, 1, "Unknown", 100, 0, 2.19, 0.5, -100, 0, -100, -1, -1);

   // Creating ud_0
   new ParticleOld("ud_0", 2101, 1, "Unknown", 100, 0.333333, 0.0073, 0, -100, -1, -100, -1, -1);

   // Creating ud_1
   new ParticleOld("ud_1", 2103, 1, "Unknown", 100, 0.333333, 0.0072, 0, -100, -1, -100, -1, -1);

   // Creating n_diffr0
   new ParticleOld("n_diffr0", 2110, 1, "Unknown", 100, 0, 0, 0, -100, -1, -100, -1, -1);

   // Creating neutron
   new ParticleOld("neutron", 2112, 1, "Baryon", 100, 0, 0.939565, 0, -100, -1, -100, -1, -1);

   // Creating Delta0
   new ParticleOld("Delta0", 2114, 1, "Baryon", 100, 0, 1.232, 0.12, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(2114));
   daughters.clear();
   daughters.push_back(2112);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(0, 0.663,  daughters));
   daughters.clear();
   daughters.push_back(2212);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.331,  daughters));
   daughters.clear();
   daughters.push_back(2112);
   daughters.push_back(22);
   part->AddDecay(ParticleOld::Decay(0, 0.006,  daughters));

   // Creating N(1675)0
   new ParticleOld("N(1675)0", 2116, 1, "Unknown", 100, 0, 1.675, 0.15, -100, 0, -100, -1, -1);

   // Creating delta(1950)0
   new ParticleOld("delta(1950)0", 2118, 1, "Unknown", 100, 0, 1.93, 0.28, -100, 0, -100, -1, -1);

   // Creating delta(1620)+
   new ParticleOld("delta(1620)+", 2122, 1, "Unknown", 100, 1, 1.63, 0.145, -100, 0, -100, -1, -1);

   // Creating N(1520)+
   new ParticleOld("N(1520)+", 2124, 1, "Unknown", 100, 1, 1.52, 0.115, -100, 0, -100, -1, -1);

   // Creating delta(1905)+
   new ParticleOld("delta(1905)+", 2126, 1, "Unknown", 100, 1, 1.89, 0.33, -100, 0, -100, -1, -1);

   // Creating N(2190)+
   new ParticleOld("N(2190)+", 2128, 1, "Unknown", 100, 1, 2.19, 0.5, -100, 0, -100, -1, -1);

   // Creating uu_1
   new ParticleOld("uu_1", 2203, 1, "Unknown", 100, 1.33333, 0.0048, 0, -100, -1, -100, -1, -1);

   // Creating p_diffr+
   new ParticleOld("p_diffr+", 2210, 1, "Unknown", 100, 1, 0, 0, -100, -1, -100, -1, -1);

   // Creating proton
   new ParticleOld("proton", 2212, 1, "Baryon", 100, 1, 0.938272, 0, -100, -1, -100, -1, -1);

   // Creating Delta+
   new ParticleOld("Delta+", 2214, 1, "Baryon", 100, 1, 1.232, 0.12, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(2214));
   daughters.clear();
   daughters.push_back(2212);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(0, 0.663,  daughters));
   daughters.clear();
   daughters.push_back(2112);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(0, 0.331,  daughters));
   daughters.clear();
   daughters.push_back(2212);
   daughters.push_back(22);
   part->AddDecay(ParticleOld::Decay(0, 0.006,  daughters));

   // Creating N(1675)+
   new ParticleOld("N(1675)+", 2216, 1, "Unknown", 100, 1, 1.675, 0.15, -100, 0, -100, -1, -1);

   // Creating delta(1950)+
   new ParticleOld("delta(1950)+", 2218, 1, "Unknown", 100, 1, 1.93, 0.28, -100, 0, -100, -1, -1);

   // Creating delta(1620)++
   new ParticleOld("delta(1620)++", 2222, 1, "Unknown", 100, 2, 1.63, 0.145, -100, 0, -100, -1, -1);

   // Creating Delta++
   new ParticleOld("Delta++", 2224, 1, "Baryon", 100, 2, 1.232, 0.12, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(2224));
   daughters.clear();
   daughters.push_back(2212);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating delta(1905)++
   new ParticleOld("delta(1905)++", 2226, 1, "Unknown", 100, 2, 1.89, 0.33, -100, 0, -100, -1, -1);

   // Creating delta(1950)++
   new ParticleOld("delta(1950)++", 2228, 1, "Unknown", 100, 2, 1.93, 0.28, -100, 0, -100, -1, -1);

   // Creating sd_0
   new ParticleOld("sd_0", 3101, 1, "Unknown", 100, -0.666667, 0.108, 0, -100, -1, -100, -1, -1);

   // Creating sd_1
   new ParticleOld("sd_1", 3103, 1, "Unknown", 100, -0.666667, 0.1088, 0, -100, -1, -100, -1, -1);

   // Creating Sigma-
   new ParticleOld("Sigma-", 3112, 1, "Baryon", 100, -1, 1.19744, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(3112));
   daughters.clear();
   daughters.push_back(2112);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.999,  daughters));
   daughters.clear();
   daughters.push_back(-12);
   daughters.push_back(11);
   daughters.push_back(2112);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));

   // Creating Sigma*-
   new ParticleOld("Sigma*-", 3114, 1, "Baryon", 100, -1, 1.3872, 0.0394, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(3114));
   daughters.clear();
   daughters.push_back(3122);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.88,  daughters));
   daughters.clear();
   daughters.push_back(3212);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.06,  daughters));
   daughters.clear();
   daughters.push_back(3112);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(0, 0.06,  daughters));

   // Creating sigma(1775)-
   new ParticleOld("sigma(1775)-", 3116, 1, "Unknown", 100, -1, 1.775, 0.12, -100, 0, -100, -1, -1);

   // Creating sigma(2030)-
   new ParticleOld("sigma(2030)-", 3118, 1, "Unknown", 100, -1, 2.03, 0.18, -100, 0, -100, -1, -1);

   // Creating Lambda0
   new ParticleOld("Lambda0", 3122, 1, "Baryon", 100, 0, 1.11568, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(3122));
   daughters.clear();
   daughters.push_back(2212);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.639,  daughters));
   daughters.clear();
   daughters.push_back(2112);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(0, 0.358,  daughters));
   daughters.clear();
   daughters.push_back(2112);
   daughters.push_back(22);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-12);
   daughters.push_back(11);
   daughters.push_back(2212);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));

   // Creating lambda(1520)
   new ParticleOld("lambda(1520)", 3124, 1, "Unknown", 100, 0, 1.5195, 0.0156, -100, 0, -100, -1, -1);

   // Creating lambda(1820)
   new ParticleOld("lambda(1820)", 3126, 1, "Unknown", 100, 0, 1.82, 0.08, -100, 0, -100, -1, -1);

   // Creating lambda(2100)
   new ParticleOld("lambda(2100)", 3128, 1, "Unknown", 100, 0, 2.1, 0.2, -100, 0, -100, -1, -1);

   // Creating su_0
   new ParticleOld("su_0", 3201, 1, "Unknown", 100, 0.333333, 0.1064, 0, -100, -1, -100, -1, -1);

   // Creating su_1
   new ParticleOld("su_1", 3203, 1, "Unknown", 100, 0.333333, 0.1064, 0, -100, -1, -100, -1, -1);

   // Creating Sigma0
   new ParticleOld("Sigma0", 3212, 1, "Baryon", 100, 0, 1.19264, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(3212));
   daughters.clear();
   daughters.push_back(3122);
   daughters.push_back(22);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating Sigma*0
   new ParticleOld("Sigma*0", 3214, 1, "Baryon", 100, 0, 1.3837, 0.036, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(3214));
   daughters.clear();
   daughters.push_back(3122);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(0, 0.88,  daughters));
   daughters.clear();
   daughters.push_back(3222);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.06,  daughters));
   daughters.clear();
   daughters.push_back(3112);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(0, 0.06,  daughters));

   // Creating sigma(1775)0
   new ParticleOld("sigma(1775)0", 3216, 1, "Unknown", 100, 0, 1.775, 0.12, -100, 0, -100, -1, -1);

   // Creating sigma(2030)0
   new ParticleOld("sigma(2030)0", 3218, 1, "Unknown", 100, 0, 2.03, 0.18, -100, 0, -100, -1, -1);

   // Creating Sigma+
   new ParticleOld("Sigma+", 3222, 1, "Baryon", 100, 1, 1.18937, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(3222));
   daughters.clear();
   daughters.push_back(2212);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(0, 0.516,  daughters));
   daughters.clear();
   daughters.push_back(2112);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(0, 0.483,  daughters));
   daughters.clear();
   daughters.push_back(2212);
   daughters.push_back(22);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));

   // Creating Sigma*+
   new ParticleOld("Sigma*+", 3224, 1, "Baryon", 100, 1, 1.3828, 0.0358, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(3224));
   daughters.clear();
   daughters.push_back(3122);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(0, 0.88,  daughters));
   daughters.clear();
   daughters.push_back(3222);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(0, 0.06,  daughters));
   daughters.clear();
   daughters.push_back(3212);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(0, 0.06,  daughters));

   // Creating sigma(1775)+
   new ParticleOld("sigma(1775)+", 3226, 1, "Unknown", 100, 1, 1.775, 0.12, -100, 0, -100, -1, -1);

   // Creating sigma(2030)+
   new ParticleOld("sigma(2030)+", 3228, 1, "Unknown", 100, 1, 2.03, 0.18, -100, 0, -100, -1, -1);

   // Creating ss_1
   new ParticleOld("ss_1", 3303, 1, "Unknown", 100, -0.666667, 2.08, 0, -100, -1, -100, -1, -1);

   // Creating Xi-
   new ParticleOld("Xi-", 3312, 1, "Baryon", 100, -1, 1.32171, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(3312));
   daughters.clear();
   daughters.push_back(3122);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.9988,  daughters));
   daughters.clear();
   daughters.push_back(-12);
   daughters.push_back(11);
   daughters.push_back(3122);
   part->AddDecay(ParticleOld::Decay(0, 0.0006,  daughters));
   daughters.clear();
   daughters.push_back(-14);
   daughters.push_back(13);
   daughters.push_back(3122);
   part->AddDecay(ParticleOld::Decay(0, 0.0004,  daughters));
   daughters.clear();
   daughters.push_back(3112);
   daughters.push_back(22);
   part->AddDecay(ParticleOld::Decay(0, 0.0001,  daughters));
   daughters.clear();
   daughters.push_back(-12);
   daughters.push_back(11);
   daughters.push_back(3212);
   part->AddDecay(ParticleOld::Decay(0, 0.0001,  daughters));
}

} // End of inline namespace
} // End of geant namespace
#if defined(__clang__) && !defined(__APPLE__)
#pragma clang optimize on
#endif
