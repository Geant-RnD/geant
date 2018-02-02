/**********************************************************************
 * @file       TARCPhysicsList.cxx
 * @author     Abhijit Bhattacharyya
 * @brief      This file is for setting PhysicsList
 *********************************************************************/

 #include "TARCPhysicsList.h"

namespace tarc {
  TARCPhysicsList::TARCPhysicsList(const std::string &name) : geantphysics::PhysicsList(name) {

  }

  TARCPhysicsList::~TARCPhysicsList() {}

  void TARCPhysicsList::Initialize() {
    std::vector<geantphysics::Particle*> pTable = geantphysics::Particle::GetTheParticleTable();
  }

  void TARCPhysicsList::SetStepMaxValue(double aVal){
    fStepMaxValue = aVal;
  }
}
