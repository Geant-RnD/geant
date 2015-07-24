// Author: Federico Carminati   27/05/13

/*************************************************************************
 * Copyright (C) 1995-2000, fca                                          *
 * All rights reserved.                                                  *
 *                                                                       *
 *************************************************************************/

#ifndef TFinState_H
#define TFinState_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TFinState                                                            //
//                                                                      //
// Final states for a reaction                                          //
//                                                                      //
// This class contains the final states for a given particle, energy    //
// and reaction                                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TPartIndex.h"

class TFinState {
public:
  TFinState();
  TFinState(int nfstates, const int npart[], const float weight[], const float kerma[], const float en[],
            const char surv[], const int pid[], const float mom[]);
  ~TFinState();
  TFinState &operator=(const TFinState &right);

  bool SetFinState(int nfstates, const int npart[], const float weight[], const float kerma[], const float en[],
                   const char surv[], const int pid[], const float mom[]);
  bool SetFinState(const TFinState &right);
  void NormFinSateWeights();
  int GetNsecs() const { return fNsecs; }

  bool Prune() { return kTRUE; }
  bool SampleReac(int &npart, float &weight, float &kerma, float &en, const int *&pid, const float *&mom) const;
  bool SampleReac(int &npart, float &weight, float &kerma, float &en, const int *&pid, const float *&mom,
                  double randn) const;

  bool GetReac(int finstat, int &npart, float &weight, float &kerma, float &en, const int *&pid,
               const float *&mom) const;
  void Dump() const {}
  void Print(const char * /*opt*/ = "") const {
    printf("fNFstates %d, fNsecs %d, fNMom %d, fPID %p, fSurv %p, fNpart %p, fWeight %p, fKerma %p, fMom %p\n",
           fNFstates, fNsecs, fNMom, (void *)fPID, (void *)fSurv, (void *)fNpart, (void *)fWeight, (void *)fKerma,
           (void *)fMom);
  }

  static void SetVerbose(int verbose) { fVerbose = verbose; }
  static int GetVerbose() { return fVerbose; }

private:
  TFinState(const TFinState &); // Not implemented

  static int fVerbose; // Controls verbosity level

  int fNFstates;  // Number of final states
  int fNsecs;     // Total number of secondaries
  int fNMom;      // 3*fNsecs, just because ROOT cannot use formulas in dimensions
  int *fNpart;    // [fNFstates] number of particles in each final state
  float *fWeight; // [fNFstates] Weight of the final states
  float *fKerma;  // [fNFstates] Released energy
  float *fEn;     // [fNFstates] Energy of final states in GeV
  char *fSurv;    // [fNFstates] whether the orignal particle has survived or not
  int *fPID;      // [fNsecs] GeantV particle code
  float *fMom;    // [fNMom] Particle momentum (GeV)

  ClassDefNV(TFinState, 1) // Particle Final States
};

#endif
