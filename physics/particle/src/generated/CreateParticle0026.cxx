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
void CreateParticle0026() {
   vector<int> daughters;
   ParticleOld *part = nullptr;

   // Creating t
   new ParticleOld("t", 6, 1, "Quark", 100, 0.666667, 171.2, 1.39883, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(6));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(5);
   part->AddDecay(ParticleOld::Decay(0, 0.998205,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(3);
   part->AddDecay(ParticleOld::Decay(0, 0.001765,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(1);
   part->AddDecay(ParticleOld::Decay(0, 3e-05,  daughters));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(6);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(22);
   daughters.push_back(6);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(23);
   daughters.push_back(6);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(7);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(25);
   daughters.push_back(6);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(37);
   daughters.push_back(5);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000022);
   daughters.push_back(1000006);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000023);
   daughters.push_back(1000006);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000025);
   daughters.push_back(1000006);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000035);
   daughters.push_back(1000006);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000021);
   daughters.push_back(1000006);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));
   daughters.clear();
   daughters.push_back(1000039);
   daughters.push_back(1000006);
   part->AddDecay(ParticleOld::Decay(53, 0,  daughters));

   // Creating b'
   new ParticleOld("b'", 7, 1, "Quark", 100, -0.333333, 468, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(7));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(7);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(22);
   daughters.push_back(7);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(23);
   daughters.push_back(7);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(2);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(4);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(6);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(8);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(25);
   daughters.push_back(7);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(-37);
   daughters.push_back(4);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(-37);
   daughters.push_back(6);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));

   // Creating t'
   new ParticleOld("t'", 8, 1, "Quark", 100, 0.666667, 171.2, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(8));
   daughters.clear();
   daughters.push_back(21);
   daughters.push_back(8);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(22);
   daughters.push_back(8);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(23);
   daughters.push_back(8);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(1);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(3);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(5);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(7);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(25);
   daughters.push_back(8);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(37);
   daughters.push_back(5);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));
   daughters.clear();
   daughters.push_back(37);
   daughters.push_back(7);
   part->AddDecay(ParticleOld::Decay(0, 0,  daughters));

   // Creating e-
   new ParticleOld("e-", 11, 1, "Lepton", 100, -1, 0.000510999, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(11));
   daughters.clear();
   daughters.push_back(22);
   daughters.push_back(11);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(23);
   daughters.push_back(11);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(12);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(25);
   daughters.push_back(11);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));

   // Creating nu_e
   new ParticleOld("nu_e", 12, 1, "Lepton", 100, 0, 0, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(12));
   daughters.clear();
   daughters.push_back(23);
   daughters.push_back(12);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(11);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));

   // Creating mu-
   new ParticleOld("mu-", 13, 1, "Lepton", 100, -1, 0.105658, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(13));
   daughters.clear();
   daughters.push_back(-12);
   daughters.push_back(11);
   daughters.push_back(14);
   part->AddDecay(ParticleOld::Decay(42, 1,  daughters));
   daughters.clear();
   daughters.push_back(22);
   daughters.push_back(13);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(23);
   daughters.push_back(13);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(14);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(25);
   daughters.push_back(13);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));

   // Creating nu_mu
   new ParticleOld("nu_mu", 14, 1, "Lepton", 100, 0, 0, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(14));
   daughters.clear();
   daughters.push_back(23);
   daughters.push_back(14);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(24);
   daughters.push_back(13);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));

   // Creating tau-
   new ParticleOld("tau-", 15, 1, "Lepton", 100, -1, 1.77684, 0, -100, -1, -100, -1, -1);
   part = const_cast<ParticleOld*>(&ParticleOld::Particles().at(15));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   part->AddDecay(ParticleOld::Decay(0, 0.2494,  daughters));
   daughters.clear();
   daughters.push_back(-12);
   daughters.push_back(11);
   daughters.push_back(16);
   part->AddDecay(ParticleOld::Decay(42, 0.1783,  daughters));
   daughters.clear();
   daughters.push_back(-14);
   daughters.push_back(13);
   daughters.push_back(16);
   part->AddDecay(ParticleOld::Decay(42, 0.1735,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(0, 0.1131,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.09,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(113);
   part->AddDecay(ParticleOld::Decay(41, 0.08,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(223);
   part->AddDecay(ParticleOld::Decay(41, 0.0191,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(113);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0133,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-323);
   part->AddDecay(ParticleOld::Decay(0, 0.012,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(211);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(41, 0.011,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.01,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-321);
   part->AddDecay(ParticleOld::Decay(0, 0.0071,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(211);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(41, 0.0067,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(113);
   part->AddDecay(ParticleOld::Decay(41, 0.005,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(223);
   part->AddDecay(ParticleOld::Decay(41, 0.0035,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-321);
   daughters.push_back(321);
   daughters.push_back(-211);
   part->AddDecay(ParticleOld::Decay(41, 0.0034,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.003,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0027,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(310);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.00205,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(130);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.00205,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(221);
   part->AddDecay(ParticleOld::Decay(41, 0.0015,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(111);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0014,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(111);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0012,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(113);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0011,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-321);
   daughters.push_back(310);
   part->AddDecay(ParticleOld::Decay(41, 0.00078,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-321);
   daughters.push_back(130);
   part->AddDecay(ParticleOld::Decay(41, 0.00078,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(113);
   daughters.push_back(113);
   part->AddDecay(ParticleOld::Decay(41, 0.00075,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-323);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.00075,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-321);
   daughters.push_back(310);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.00069,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-321);
   daughters.push_back(130);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.00069,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(223);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0006,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(310);
   daughters.push_back(130);
   part->AddDecay(ParticleOld::Decay(41, 0.00051,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(211);
   daughters.push_back(-211);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0005,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-323);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0004,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-321);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0004,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(130);
   part->AddDecay(ParticleOld::Decay(41, 0.00025,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(310);
   daughters.push_back(310);
   part->AddDecay(ParticleOld::Decay(41, 0.00025,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(310);
   part->AddDecay(ParticleOld::Decay(41, 0.00025,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(130);
   daughters.push_back(130);
   part->AddDecay(ParticleOld::Decay(41, 0.00025,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(113);
   daughters.push_back(113);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.00022,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(221);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.00021,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(-213);
   daughters.push_back(211);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0002,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(113);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0002,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-213);
   daughters.push_back(113);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0002,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(213);
   daughters.push_back(-213);
   part->AddDecay(ParticleOld::Decay(41, 0.0002,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(213);
   daughters.push_back(-211);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0002,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-321);
   daughters.push_back(111);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 0.0001,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(221);
   daughters.push_back(221);
   part->AddDecay(ParticleOld::Decay(41, 0.0001,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-323);
   daughters.push_back(111);
   daughters.push_back(111);
   part->AddDecay(ParticleOld::Decay(41, 6e-05,  daughters));
   daughters.clear();
   daughters.push_back(16);
   daughters.push_back(-211);
   daughters.push_back(221);
   part->AddDecay(ParticleOld::Decay(41, 6e-05,  daughters));
   daughters.clear();
   daughters.push_back(22);
   daughters.push_back(15);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(23);
   daughters.push_back(15);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(-24);
   daughters.push_back(16);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
   daughters.clear();
   daughters.push_back(25);
   daughters.push_back(15);
   part->AddDecay(ParticleOld::Decay(102, 0,  daughters));
}

} // End of inline namespace
} // End of geant namespace
#if defined(__clang__) && !defined(__APPLE__)
#pragma clang optimize on
#endif
