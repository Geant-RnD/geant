
#ifndef INELASTICPROCESS_H
#define INELASTICPROCESS_H

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
 * @brief   Hadron Inelastic process.
 * @class   InelasticProcess
 * @author  H Kumawat
 * @date    June 2018
 */

class InelasticProcess : public HadronicProcess {
public:
  // CTR
  InelasticProcess(const std::string &name = "kInelastic");
};

} // namespace geantphysics

#endif
