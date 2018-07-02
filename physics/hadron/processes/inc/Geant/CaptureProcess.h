
#ifndef CAPTUREPROCESS_H
#define CAPTUREPROCESS_H

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
 * @brief   Hadron Capture process.
 * @class   CaptureProcess
 * @author  H Kumawat
 * @date    June 2018
 */

class CaptureProcess : public HadronicProcess {
public:
  // CTR
  CaptureProcess(const std::string &name = "kCapture");
};

} // namespace geantphysics

#endif
