/***********************************************************
 * @file TARCPrimaryGenerator.h
 * @author Abhijit Bhattacharyya
 * @brief this is used to defien primary gun generator characteristics
 **************************************************************/
#ifndef TARC_PRIMARYGENERATOR_H
#define TARC_PRIMARYGENERATOR_H

#include <string>

// from GeantV
#include "PrimaryGenerator.h"

// forGEANT_IMPL_NAMESPACE
#include Geant{
 namespace Geant {
   class GeantTrack;
   class GeantEventInfo;
 }
}

// geantphysics classdef
namespace geantphysics {
 class Particle;
}

namespace userapplication{
  class TARCGeometryConstruction;

  class TARCPrimaryGenerator : publilc Geant::PrimaryGenerator {

  }; // class ends


} // namespace ends Here


#endif
