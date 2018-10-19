#ifndef GEANT_CMS_Detector_Construction
#define GEANT_CMS_Detector_Construction

#include "Geant/UserFieldConstruction.h"

#include <string>
#include "Geant/Error.h"
#include "Geant/UserFieldConstruction.h"
#include "Geant/FieldConfig.h"
#include "Geant/CMSmagField.h"

class CMSmagField;
class GUVMagneticField;

class CMSFieldConstruction : public geant::cxx::UserFieldConstruction
{
public:
  /** @brief Destructor */
  CMSFieldConstruction() : fFieldFilename(std::string("")), fCMSfield(nullptr) {}
  // CMSFieldConstruction(const char* fieldFilename);
  // CMSFieldConstruction(std::string fieldFilename);
  inline ~CMSFieldConstruction();

  /** @brief Destructor */
  void SetFileForField(const char *filename) { fFieldFilename = filename; }
  void SetFileForField(std::string filename) { fFieldFilename = filename; }

  /** @brief Method to register a B-field, and create integrator for it. */
  bool CreateFieldAndSolver(bool useRungeKutta = true, VVectorField **fieldPP = nullptr) override final;

private:
  std::string fFieldFilename;
  CMSmagField *fCMSfield;
  // ScalarUniformMagField*  fUniformField; // Alternative - for debugging only
  /** Field is created and owned by this class */
public:
  CMSFieldConstruction(const char *fieldFilename) : fFieldFilename(fieldFilename), fCMSfield(nullptr) {}

  // CMSFieldConstruction::
  CMSFieldConstruction(std::string fieldFilename) : fFieldFilename(fieldFilename), fCMSfield(nullptr) {}

  // ClassImp(CMSFieldConstruction);
};

CMSFieldConstruction::~CMSFieldConstruction()
{
  delete fCMSfield;
}

bool CMSFieldConstruction::CreateFieldAndSolver(bool useRungeKutta, VVectorField **fieldPP)
{
  using FieldType = CMSmagField;

  geant::Print("CMSFieldConstruction::CreateFieldAndSolver", " Called with Arg: useRungeKutta=");
  if (useRungeKutta) {
    printf("on");
  } else {
    printf("Off");
  }

  if (fieldPP) {
    *fieldPP = nullptr;
  }

  std::cout << "    Calling CMSmagField constructor with filename= " << fFieldFilename << std::endl;
  fCMSfield = new CMSmagField(fFieldFilename);
  // fUniformField= nullptr;
  useRungeKutta = true; // Must initialize it always --
  printf("CMSFieldConstruction::CratedFieldAndSolver> useRungeKutta - forced ON, until 'general helix' is available ");

  auto fieldConfig = new geant::FieldConfig(fCMSfield, false);
  geant::FieldLookup::SetFieldConfig(fieldConfig);

  auto fieldPtr = fCMSfield;

  if (fieldPP && fieldPtr) *fieldPP = fieldPtr;

  fpField = fieldPtr; // UserFieldConstruction::SetField( fieldPtr );

  geant::Print("CMSFieldConstruction::CreateFieldAndSolver", "CMSmagfield created.");

  if (useRungeKutta) {
    CreateSolverForField<FieldType>(fieldPtr);
    printf("%s", "CMSFieldConstruction - Configured field propagation for Runge Kutta.");
  } else {
    printf("%s", "CMSFieldConstruction - NOT configuring field propagation with Runge Kutta.");
  }

  return true;
}

#endif
