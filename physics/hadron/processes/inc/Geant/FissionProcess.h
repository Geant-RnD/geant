
#ifndef FISSIONPROCESS_H
#define FISSIONPROCESS_H

#include "Geant/HadronicProcess.h"

#include "Geant/Proton.h"
#include "Geant/Neutron.h"
#include "Geant/PionPlus.h"
#include "Geant/PionMinus.h"
#include "Geant/PionZero.h"
#include "Geant/KaonPlus.h"
#include "Geant/KaonMinus.h"
#include "Geant/KaonZero.h"
#include "Geant/KaonShort.h"
#include "Geant/KaonLong.h"

#include <string>

namespace geantphysics {

/**
 * @brief   Hadron Fission process.
 * @class   FissionProcess
 * @author  H Kumawat
 * @date    June 2018
 */

class FissionProcess : public HadronicProcess {
public:
  // CTR
  FissionProcess(const std::string &name = "kFission");
};

} // namespace geantphysics

#endif
