#ifndef GEANTV_KLEINNISHINATESTCOMMON_H
#define GEANTV_KLEINNISHINATESTCOMMON_H

#include "Common.h"
#include "Geant/KleinNishinaComptonModel.h"
#include "Geant/VecKleinNishinaComptonModel.h"

using geantphysics::KleinNishinaComptonModel;
using geantphysics::VecKleinNishinaComptonModel;

const double kKNminEn = 100 * geant::units::eV;
const double kKNmaxEn = 100.0 * geant::units::TeV;

#endif // GEANTV_KLEINNISHINATESTCOMMON_H
