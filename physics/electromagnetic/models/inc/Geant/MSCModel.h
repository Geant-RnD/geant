

#ifndef MSCMODEL_H
#define MSCMODEL_H

#include <string>

#include "Geant/EMModel.h"
#include "Geant/MSCdata.h"

// from geantV
#include "Geant/Config.h"
namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {
class TaskData;
class Track;
}
}

namespace geantphysics {

/**
 * @brief   Base class for multiple Coulomb scattering models.
 * @class   MSCModel
 * @author  M Novak
 * @date    June 2017
 */

enum class MSCSteppingAlgorithm { kUseSaftey, kUseDistanceToBoundary, kErrorFree };

class MSCModel : public EMModel {
public:
  MSCModel(const std::string &name);
  virtual ~MSCModel();

  // implemented base class method
  virtual void Initialize();

  // special MSC model interface methods
  virtual void StepLimit(geant::Track * /*gtrack*/, geant::TaskData * /*td*/) {}
  virtual void ConvertTrueToGeometricLength(geant::Track * /*gtrack*/, geant::TaskData * /*td*/) {}
  virtual void ConvertGeometricToTrueLength(geant::Track * /*gtrack*/, geant::TaskData * /*td*/) {}
  virtual bool SampleScattering(geant::Track * /*gtrack*/, geant::TaskData * /*td*/) { return false; }

  //
  void SetMSCSteppingAlgorithm(MSCSteppingAlgorithm steppingalg) { fMSCSteppingAlgorithm = steppingalg; }
  MSCSteppingAlgorithm GetMSCSteppingAlgorithm() const { return fMSCSteppingAlgorithm; }

  void SetRangeFactor(double rf) { fRangeFactor = rf; }
  double GetRangeFactor() const { return fRangeFactor; }

  void SetSafetyFactor(double sf) { fSafetyFactor = sf; }
  double GetSafetyFactor() const { return fSafetyFactor; }

  void SetGeomFactor(double gf) { fGeomFactor = gf; }
  double GetGeomFactor() const { return fGeomFactor; }

  void SetSkin(double skin) { fSkin = skin; }
  double GetSkin() const { return fSkin; }

private:
  // some generaly used parameters or not ?
  double fRangeFactor;
  double fSafetyFactor;
  double fGeomFactor;
  double fSkin;

  MSCSteppingAlgorithm fMSCSteppingAlgorithm;
};

} // geantphysics

#endif // MSCMODEL_H
