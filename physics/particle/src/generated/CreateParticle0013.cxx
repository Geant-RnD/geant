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
void CreateParticle0013() {
   vector<int> daughters;
   ParticleOld *part = nullptr;

   // Creating Xi_cc--
   new ParticleOld("Xi_cc--", -4422, 0, "CharmedBaryon", 100, -2, 3.59798, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4422));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.76,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(3);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.08,  daughters));

   // Creating Xi*_cc-
   new ParticleOld("Xi*_cc-", -4414, 0, "CharmedBaryon", 100, -1, 3.65648, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4414));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.76,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(3);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.08,  daughters));

   // Creating Xi_cc-
   new ParticleOld("Xi_cc-", -4412, 0, "CharmedBaryon", 100, -1, 3.59798, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4412));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.76,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(3);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.08,  daughters));

   // Creating cc_1_bar
   new ParticleOld("cc_1_bar", -4403, 0, "Unknown", 100, -1.33333, 3.27531, 0, 100, 100, 1, 100, 1);

   // Creating Omega*_c0_bar
   new ParticleOld("Omega*_c0_bar", -4334, 0, "CharmedBaryon", 100, 0, 2.7683, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4334));
   daughters.clear();
   daughters.push_back(-4332);
   daughters.push_back(-22);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating Omega_c0_bar
   new ParticleOld("Omega_c0_bar", -4332, 0, "CharmedBaryon", 100, 0, 2.6975, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4332));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.76,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(3);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.08,  daughters));

   // Creating Xi*_c-
   new ParticleOld("Xi*_c-", -4324, 0, "CharmedBaryon", 100, -1, 2.6466, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4324));
   daughters.clear();
   daughters.push_back(-4232);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(0, 0.5,  daughters));
   daughters.clear();
   daughters.push_back(-4232);
   daughters.push_back(-22);
   part->AddDecay(ParticleOld::Decay(0, 0.5,  daughters));

   // Creating Xi'_c-
   new ParticleOld("Xi'_c-", -4322, 0, "CharmedBaryon", 100, -1, 2.5757, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4322));
   daughters.clear();
   daughters.push_back(-4232);
   daughters.push_back(-22);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating Xi*_c0_bar
   new ParticleOld("Xi*_c0_bar", -4314, 0, "CharmedBaryon", 100, 0, 2.6461, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4314));
   daughters.clear();
   daughters.push_back(-4132);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(0, 0.5,  daughters));
   daughters.clear();
   daughters.push_back(-4132);
   daughters.push_back(-22);
   part->AddDecay(ParticleOld::Decay(0, 0.5,  daughters));

   // Creating Xi'_c0_bar
   new ParticleOld("Xi'_c0_bar", -4312, 0, "CharmedBaryon", 100, 0, 2.578, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4312));
   daughters.clear();
   daughters.push_back(-4132);
   daughters.push_back(-22);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating cs_1_bar
   new ParticleOld("cs_1_bar", -4303, 0, "Unknown", 100, -0.333333, 2.17967, 0, 100, 100, 1, 100, 1);

   // Creating cs_0_bar
   new ParticleOld("cs_0_bar", -4301, 0, "CharmedBaryon", 100, -0.333333, 2.15432, 0, 100, 100, 1, 100, 1);

   // Creating Xi_c-
   new ParticleOld("Xi_c-", -4232, 0, "CharmedBaryon", 100, -1, 2.4679, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4232));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.76,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(3);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.08,  daughters));

   // Creating Sigma*_c--
   new ParticleOld("Sigma*_c--", -4224, 0, "CharmedBaryon", 100, -2, 2.5184, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4224));
   daughters.clear();
   daughters.push_back(-4122);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating Sigma_c--
   new ParticleOld("Sigma_c--", -4222, 0, "CharmedBaryon", 100, -2, 2.45402, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4222));
   daughters.clear();
   daughters.push_back(-4122);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating Sigma*_c-
   new ParticleOld("Sigma*_c-", -4214, 0, "CharmedBaryon", 100, -1, 2.5175, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4214));
   daughters.clear();
   daughters.push_back(-4122);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating Sigma_c-
   new ParticleOld("Sigma_c-", -4212, 0, "CharmedBaryon", 100, -1, 2.4529, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4212));
   daughters.clear();
   daughters.push_back(-4122);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(0, 1,  daughters));

   // Creating cu_1_bar
   new ParticleOld("cu_1_bar", -4203, 0, "Unknown", 100, -1.33333, 2.00808, 0, 100, 100, 1, 100, 1);

   // Creating cu_0_bar
   new ParticleOld("cu_0_bar", -4201, 0, "Unknown", 100, -1.33333, 1.96908, 0, 100, 100, 1, 100, 1);

   // Creating Xi_c0_bar
   new ParticleOld("Xi_c0_bar", -4132, 0, "CharmedBaryon", 100, 0, 2.471, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4132));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.76,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(42, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(3);
   daughters.push_back(-3);
   daughters.push_back(-81);
   part->AddDecay(ParticleOld::Decay(11, 0.08,  daughters));

   // Creating Lambda_c-
   new ParticleOld("Lambda_c-", -4122, 0, "CharmedBaryon", 100, -1, 2.28646, 0, 100, 100, 1, 100, 1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(-4122));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-3);
   daughters.push_back(-2101);
   part->AddDecay(ParticleOld::Decay(13, 0.2432,  daughters));
   daughters.clear();
   daughters.push_back(-3);
   daughters.push_back(-2203);
   part->AddDecay(ParticleOld::Decay(13, 0.15,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(-3201);
   part->AddDecay(ParticleOld::Decay(13, 0.075,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(-3203);
   part->AddDecay(ParticleOld::Decay(13, 0.075,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-3);
   daughters.push_back(-2103);
   part->AddDecay(ParticleOld::Decay(13, 0.057,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(1);
   daughters.push_back(-1);
   daughters.push_back(-2101);
   part->AddDecay(ParticleOld::Decay(13, 0.035,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(3);
   daughters.push_back(-3);
   daughters.push_back(-2101);
   part->AddDecay(ParticleOld::Decay(13, 0.035,  daughters));
   daughters.clear();
   daughters.push_back(-1);
   daughters.push_back(-2203);
   part->AddDecay(ParticleOld::Decay(13, 0.03,  daughters));
   daughters.clear();
   daughters.push_back(-2224);
   daughters.push_back(323);
   part->AddDecay(ParticleOld::Decay(0, 0.025,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3122);
   part->AddDecay(ParticleOld::Decay(42, 0.018,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3122);
   part->AddDecay(ParticleOld::Decay(42, 0.018,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(311);
   part->AddDecay(ParticleOld::Decay(0, 0.016,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(-2101);
   part->AddDecay(ParticleOld::Decay(13, 0.015,  daughters));
   daughters.clear();
   daughters.push_back(-2);
   daughters.push_back(-2103);
   part->AddDecay(ParticleOld::Decay(13, 0.015,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(313);
   part->AddDecay(ParticleOld::Decay(0, 0.0088,  daughters));
   daughters.clear();
   daughters.push_back(-2224);
   daughters.push_back(321);
   part->AddDecay(ParticleOld::Decay(0, 0.0066,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-2212);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(42, 0.006,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-2112);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(42, 0.006,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-2112);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(42, 0.006,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-2212);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(42, 0.006,  daughters));
   daughters.clear();
   daughters.push_back(-3122);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.0058,  daughters));
   daughters.clear();
   daughters.push_back(-3212);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.0055,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3212);
   part->AddDecay(ParticleOld::Decay(42, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(-2214);
   daughters.push_back(311);
   part->AddDecay(ParticleOld::Decay(0, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(-2214);
   daughters.push_back(313);
   part->AddDecay(ParticleOld::Decay(0, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3212);
   part->AddDecay(ParticleOld::Decay(42, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(-3122);
   daughters.push_back(-213);
   part->AddDecay(ParticleOld::Decay(0, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-3214);
   part->AddDecay(ParticleOld::Decay(42, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(-3122);
   daughters.push_back(-321);
   part->AddDecay(ParticleOld::Decay(0, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(-3122);
   daughters.push_back(-323);
   part->AddDecay(ParticleOld::Decay(0, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-3214);
   part->AddDecay(ParticleOld::Decay(42, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(-3212);
   daughters.push_back(-213);
   part->AddDecay(ParticleOld::Decay(0, 0.004,  daughters));
   daughters.clear();
   daughters.push_back(-3214);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.004,  daughters));
   daughters.clear();
   daughters.push_back(-3214);
   daughters.push_back(-213);
   part->AddDecay(ParticleOld::Decay(0, 0.004,  daughters));
   daughters.clear();
   daughters.push_back(-3222);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(0, 0.004,  daughters));
   daughters.clear();
   daughters.push_back(-3222);
   daughters.push_back(-113);
   part->AddDecay(ParticleOld::Decay(0, 0.004,  daughters));
   daughters.clear();
   daughters.push_back(-3222);
   daughters.push_back(-223);
   part->AddDecay(ParticleOld::Decay(0, 0.004,  daughters));
   daughters.clear();
   daughters.push_back(-3224);
   daughters.push_back(-113);
   part->AddDecay(ParticleOld::Decay(0, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(-3224);
   daughters.push_back(-223);
   part->AddDecay(ParticleOld::Decay(0, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(-2112);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(-2112);
   daughters.push_back(-213);
   part->AddDecay(ParticleOld::Decay(0, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(-2114);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(-2114);
   daughters.push_back(-213);
   part->AddDecay(ParticleOld::Decay(0, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-2112);
   part->AddDecay(ParticleOld::Decay(42, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-2112);
   part->AddDecay(ParticleOld::Decay(42, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(-3224);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(0, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(-3322);
   daughters.push_back(-321);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3212);
   daughters.push_back(-321);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3212);
   daughters.push_back(-323);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3222);
   daughters.push_back(-311);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3222);
   daughters.push_back(-313);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3322);
   daughters.push_back(-323);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3324);
   daughters.push_back(-321);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(-113);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(-223);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(11);
   daughters.push_back(-12);
   daughters.push_back(-2114);
   part->AddDecay(ParticleOld::Decay(42, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3222);
   daughters.push_back(-221);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3224);
   daughters.push_back(-221);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-3222);
   daughters.push_back(-331);
   part->AddDecay(ParticleOld::Decay(0, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(13);
   daughters.push_back(-14);
   daughters.push_back(-2114);
   part->AddDecay(ParticleOld::Decay(42, 0.002,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(-10221);
   part->AddDecay(ParticleOld::Decay(0, 0.0018,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(-333);
   part->AddDecay(ParticleOld::Decay(0, 0.0013,  daughters));
   daughters.clear();
   daughters.push_back(-2224);
   daughters.push_back(213);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-3224);
   daughters.push_back(-311);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-3224);
   daughters.push_back(-313);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-2224);
   daughters.push_back(211);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(-221);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-2212);
   daughters.push_back(-331);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-2214);
   daughters.push_back(-111);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-2214);
   daughters.push_back(-221);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-2214);
   daughters.push_back(-331);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-2214);
   daughters.push_back(-113);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-3214);
   daughters.push_back(-321);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-3214);
   daughters.push_back(-323);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
   daughters.clear();
   daughters.push_back(-2214);
   daughters.push_back(-223);
   part->AddDecay(ParticleOld::Decay(0, 0.001,  daughters));
}

} // End of inline namespace
} // End of geant namespace
#if defined(__clang__) && !defined(__APPLE__)
#pragma clang optimize on
#endif
