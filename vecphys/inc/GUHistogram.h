#ifndef GUHistogram_H
#define GUHistogram_H 1

#ifdef VECPHYS_ROOT
#include "TFile.h"
#include "TH1F.h"
#endif

#include "GUPhysicsModelName.h"
#include "GUPhysicsProcessName.h"

namespace vecphys {

class GUHistogram {
public:
  GUHistogram(std::string fileName, double maxEnergy);
  ~GUHistogram();

  void RecordTime(int imodel, double elapsedTime);
  void RecordHistos(int imodel, double energyIn, double energyOut1, double AngleOut1, double energyOut2,
                    double AngleOut2);

  void RecordHistosProc(int iprocess, double energy, double nint, double step, double lambda);

#ifdef VECPHYS_ROOT
private:
  void BookHistograms(double maxEnergy);
  TFile *fHistFile;

  TH1F *fTime[kNumberPhysicsModel];
  TH1F *fEnergyIn[kNumberPhysicsModel];
  TH1F *fEnergyOut1[kNumberPhysicsModel];
  TH1F *fEnergyOut2[kNumberPhysicsModel];
  TH1F *fAngleOut1[kNumberPhysicsModel];
  TH1F *fAngleOut2[kNumberPhysicsModel];

  TH1F *fProcEnergy[kNumberPhysicsProcess];
  TH1F *fNint[kNumberPhysicsProcess];
  TH1F *fStep[kNumberPhysicsProcess];
  TH1F *fLambda[kNumberPhysicsProcess];

#endif
};

} // end namespace vecphys

#endif
