#ifndef GSMSCMODELSIMPLIFIED_H
#define GSMSCMODELSIMPLIFIED_H

#include <string>

#include "Geant/MSCModel.h"
#include "Geant/GSMSCTableSimplified.h"
#include "Geant/GSPWACorrections.h"

// from geantV
#include "Geant/Config.h"
#include "Geant/Track.h"

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {
class TaskData;
}
} // namespace geant

namespace geantphysics {

/**
 * @brief   Model for multiple scattering of e-/e+ based on pre-computed angular distributions computed by means of
 *          Goudsmit-Saunderson theory based on screened Rutherford DCS.
 * @class   GSMSCModelSimplified
 * @author  M Novak
 * @date    November 2017
 */

class Particle;

class GSMSCModelSimplified : public MSCModel {
public:
  GSMSCModelSimplified(bool iselectron = true, const std::string &name = "Goudsmit-Saunderson");

  // implemented base class method
  virtual void Initialize();
  // implemented MSC base class model methods
  virtual void StepLimit(geant::Track *gtrack, geant::TaskData *td);
  virtual bool SampleScattering(geant::Track *gtrack, geant::TaskData *td);
  virtual void SampleScattering(geant::TrackVec_t &gtracks, std::vector<bool> &hasNewDir, geant::TaskData *td);
  virtual void ConvertTrueToGeometricLength(geant::Track *gtrack, geant::TaskData *td);
  virtual void ConvertGeometricToTrueLength(geant::Track *gtrack, geant::TaskData *td);

  // make it public for testing
  void ComputeParameters(const MaterialCuts *matcut, double ekin, double &lambel, double &lambtr1, double &scra,
                         double &g1, double &mccor1, double &mccor2);
  void ComputeParameters(std::array<int, geant::kVecLenD> matIdx, geant::Double_v ekin, geant::Double_v &lambel,
                         geant::Double_v &lambtr1, geant::Double_v &scra, geant::Double_v &g1, geant::Double_v &mccor1,
                         geant::Double_v &mccor2);

private:
  double RandomizeTrueStepLength(geant::TaskData *td, double tlimit);
  void SampleMSC(geant::Track *gtrack, geant::TaskData *td);
  void SampleMSCp1(geant::TrackVec_t &gtracks, geant::TaskData *td);
  void SampleMSCp2(geant::TrackVec_t &gtracks, geant::TaskData *td);
  void SampleMSCp3(geant::TrackVec_t &gtracks, geant::TaskData *td);
  void SampleMSC(geant::TrackVec_t &gtracks, geant::TaskData *td);
  double GetTransportMeanFreePathOnly(const MaterialCuts *matcut, double ekin);

  //  void   ComputeParameters(const MaterialCuts *matcut, double ekin, double &lambel, double &lambtr1,
  //                           double &scra, double &g1);
  // data members
private:
  bool fIsElectron = false; // is the model for e- (e+ otherwise)

  double fCharge = 0.0;

  double fTauSmall      = 1.e-16;
  double fTauLim        = 1.e-6;
  double fTLimitMinfix2 = 1. * geant::units::nm;
  double fDtrl          = 0.05;

  Particle *fParticle = nullptr; // e-/e+
  geant::TrackToken fMSCdata;    // Handle for MSCData

  GSMSCTableSimplified fGSTable;
  GSPWACorrections fPWACorrection;
};

} // namespace geantphysics

#endif // GSMSCMODEL_H
