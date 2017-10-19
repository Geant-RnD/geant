/***************************************************
 ***************************************************/
#include "NudyProcess.h"

using namespace geantphysics;
using namespace Nudy;
using namespace NudyPhysics;

NudyProcess::NudyProcess() : geantphysics::PhysicsProcess(""),
fType( HadronicProcessType::kNotDefined ), fXsecStore( nullptr ),
fModelStore( nullptr )
{}

NudyProcess::NudyProcess( const std::string &name ) : geantphysics::PhysicsProcess( name )
{
  fModelStore = new HadronicFinalStateModelStore();
  fXsecStore = new NudyCrossSectionStore();
}

NudyProcess::NudyProcess( const std::string &name,
  const std::vector< int > &particlecodevec, const HadronicProcessType pType,
  const bool isatrest, NudyCrossSectionStore* xsecstore,
  HadronicFinalStateModelStore* modelstore ) : PhysicsProcess( true, false,
    isatrest, ForcedCondition::kNotForced, ProcessType::kHadronic, name ),
  fType( pType ), fXsecStore( xsecstore ), fModelStore( modelstore )
{
  SetParticleCodeVec( particlecodevec );
}

NudyProcess::~NudyProcess() {}

double NudyProcess::ComputeMacroscopicXSection(
    const MaterialCuts *matCuts, double kEnergy,  const Particle *particle,
    HadronicProcessType pType
  ) const {
    double xsec = 0.0;
    const Material* mat = matCuts->GetMaterial();
    const Vector_t< Element* > theElements = mat->GetElementVector();
    const double* theAtomicNumDensityVector = mat->GetMaterialProperties()->GetNumOfAtomsPerVolumeVect();
    int numElems = theElements.size();

    for (int iel = 0; iel < numElems; ++iel) {
      xsec += theAtomicNumDensityVector[iel] * GetAtomicCrossSection(
        particle->GetInternalCode(), kEnergy,theElements[iel], pType
      );
    }
    return xsec;
  }

  double NudyProcess::GetAtomicCrossSection( const int particlecode,
    const double particlekineticenergy, const Element* targetelement,
    HadronicProcessType pType ) const {
    double xsec = -1.0;
    if ( fXsecStore ) {
      xsec = fXsecStore->GetElementCrossSection( particlecode,
        particlekineticenergy, targetelement, pType
      );
    }
    return xsec;
  }

  Isotope* NudyProcess::SampleTarget( LightTrack &track, HadronicProcessType pType ) const {
    Isotope* targetIsotope = nullptr;
    int particleCode = track.GetGVcode();
    double eKin = track.GetKinE();
    int indexMaterialCutCouple = track.GetMaterialCutCoupleIndex();

    const Material* material = MaterialCuts::GetTheMaterialCutsTable()[indexMaterialCutCouple]->GetMaterial();

    if ( fXsecStore ) {
      std::pair< int, int > pairZandN = fXsecStore->SampleTarget( particleCode,
        eKin, material, pType );
      track.SetTargetZ( pairZandN.first );
      track.SetTargetN( pairZandN.second );
      targetIsotope = Isotope::GetIsotope( pairZandN.first, pairZandN.second );
    }
    return targetIsotope;
  }

  void NudyProcess::AddModel(HadronicFinalStateModel *model) {
    if (!model) {
      std::string partNames = "\n";
      for (unsigned long p=0; p<GetListParticlesAssignedTo().size(); ++p)
        partNames += GetListParticlesAssignedTo()[p]->GetName() + "  ";
      partNames += "\n";
      std::cerr<<" *** WARNING: HadronicProcess::AddModel() \n"
               <<"  Attempt to add HadronicFinalStateModel = nullptr to process with Name = " << GetName() << " that is assigned to particles:"
               << partNames << " is ignored."
               << std::endl;
    } else {
      fModelStore->RegisterHadronicFinalStateModel(model);
    }
    return;
  }

  void NudyProcess::AddCrossSection(NudyCrossSection *xsection) {
    if (!xsection) {
      std::string partNames = "\n";
      for (unsigned long p=0; p<GetListParticlesAssignedTo().size(); ++p)
        partNames += GetListParticlesAssignedTo()[p]->GetName() + "  ";
      partNames += "\n";
      std::cerr<<" *** WARNING: HadronicProcess::AddCrossSection() \n"
               <<"  Attempt to add HadronicCrossSection = nullptr to process with Name = " << GetName() << " that is assigned to particles:"
               << partNames << " is ignored."
               << std::endl;
    } else {
      fXsecStore->RegisterNudyCrossSection(xsection);
    }
    return;
  }