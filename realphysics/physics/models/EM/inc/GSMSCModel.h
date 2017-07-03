
#ifndef GSMSCMODEL_H
#define GSMSCMODEL_H

#include <string>

#include "MSCModel.h"

// from geantV
#include "Geant/Config.h"
namespace Geant {
  inline namespace GEANT_IMPL_NAMESPACE {
  class GeantTrack;
  class TrackToken;
  class GeantTaskData;
}
}


namespace geantphysics {

/**
 * @brief   Model for multiple scattering of e-/e+ based on pre-computed angular distributions computed by means of
 *          Goudsmit-Saunderson theory based on screened Rutherford DCS.
 * @class   GSMSCModel
 * @author  M Novak
 * @date    June 2017
 */


class GSMSCTable;
class PWATotalXsecTable;
class Particle;

class GSMSCModel : public MSCModel {
public:
  GSMSCModel(bool iselectron = true, const std::string &name = "Goudsmit-Saunderson");
 ~GSMSCModel();

 // implemented base class method
  virtual void  Initialize();
 // implemented MSC base class model methods
  virtual void  StepLimit(Geant::GeantTrack *gtrack, Geant::GeantTaskData *td);
  virtual bool  SampleScattering(Geant::GeantTrack *gtrack, Geant::GeantTaskData *td);
  virtual void  ConvertTrueToGeometricLength(Geant::GeantTrack *gtrack, Geant::GeantTaskData *td);
  virtual void  ConvertGeometricToTrueLength(Geant::GeantTrack *gtrack, Geant::GeantTaskData *td);

 // model specifc method
  void SetOptionPWAScreening(bool val) { fIsUsePWATotalXsecData = val; }


// just for testing
GSMSCTable* GetGSTable() const { return gGSTable; }
// make it public for testing
void   ComputeParameters(const MaterialCuts *matcut, double ekin, double &lambel, double &lambtr1,
                         double &scra, double &g1);

private:
  double RandomizeTrueStepLength(Geant::GeantTaskData *td, double tlimit);
  void   SingleScattering(double scra, Geant::GeantTaskData *td, double &cost, double &sint);
  void   SampleMSC(Geant::GeantTrack *gtrack, Geant::GeantTaskData *td);
  double GetTransportMeanFreePathOnly(const MaterialCuts *matcut, double ekin);

//  void   ComputeParameters(const MaterialCuts *matcut, double ekin, double &lambel, double &lambtr1,
//                           double &scra, double &g1);
// data members
private:
  bool   fIsElectron;            // is the model for e- (e+ otherwise)
  bool   fIsUsePWATotalXsecData; // use screening that gives back pwa first transport mean free path
  bool   fIsUseAccurate;         // use accurate step limits
  bool   fIsOptimizationOn;      // use optimisation in the step limit: check current range and pre-safety

  double fCharge;

  double fTauSmall;
  double fTauLim;
  double fTLimitMinfix2;
  double fDtrl;

  Particle* fParticle;    //e-/e+
  Geant::TrackToken *fMSCdata = nullptr;   // Handle for MSCData

  static GSMSCTable         *gGSTable;
  static PWATotalXsecTable  *gPWAXsecTable;

};

}      // namespace geantphysics

#endif // GSMSCMODEL_H
