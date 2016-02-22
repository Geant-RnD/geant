// Author: Federico Carminati   27/05/13

/*************************************************************************
 * Copyright (C) 1995-2000, fca                                          *
 * All rights reserved.                                                  *
 *                                                                       *
 *************************************************************************/

#ifndef TPXsec_H
#define TPXsec_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPXSec                                                               //
//                                                                      //
// X-section for GV per particle                                        //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TPartIndex.h"
#ifndef GEANT_NVCC
#ifdef USE_ROOT
#include "Rtypes.h"
#endif
#endif
#include <iostream>
 
class TPXsec {
public:
  GEANT_CUDA_BOTH_CODE
  TPXsec();
  GEANT_CUDA_BOTH_CODE
  TPXsec(int pdg, int nxsec);
  GEANT_CUDA_BOTH_CODE
  TPXsec(const TPXsec &other);
  GEANT_CUDA_BOTH_CODE
  virtual ~TPXsec();
  void Print(const char *opt = "") const;
 #ifndef GEANT_NVCC
  const char *Name() const { return TPartIndex::I()->PartName(fPDG); }
 #else
  GEANT_CUDA_DEVICE_CODE
  const char *Name() const { return TPartIndex::I()->PartName(fPDG); }
 #endif
  bool SetPart(int pdg, int nxsec);
  bool SetPartXS(const float xsec[], const int dict[]);
  bool SetPartIon(const float dedx[]);
  bool SetPartMS(const float angle[], const float ansig[], const float length[], const float lensig[]);
  int PDG() const { return fPDG; }
  GEANT_CUDA_BOTH_CODE
  float XS(int rindex, double en, bool verbose=false) const;
  bool XS_v(int npart, int rindex, const double en[], double lam[]) const;
  float DEdx(double en) const;
  bool MS(double en, float &ang, float &asig, float &len, float &lsig) const;
  GEANT_CUDA_BOTH_CODE
  bool Resample();
  bool Prune();
  int SampleReac(double en) const;
  int SampleReac(double en, double randn) const;

  void Dump() const;
  GEANT_CUDA_BOTH_CODE
  void Interp(double egrid[], float value[], int nbins, double eildelta, int stride, double en, float result[]);

  static void SetVerbose(int verbose) { fVerbose = verbose; }
  static int GetVerbose() { return fVerbose; }
  GEANT_CUDA_BOTH_CODE
  int SizeOf() const;
  GEANT_CUDA_BOTH_CODE
  void Compact();
  GEANT_CUDA_BOTH_CODE
  void RebuildClass();
#ifdef MAGIC_DEBUG
  GEANT_CUDA_BOTH_CODE
  int GetMagic() const {return fMagic;}
#endif

  GEANT_CUDA_BOTH_CODE
bool CheckAlign() {
  bool isaligned=true;
  if(((unsigned long) &fPDG) % sizeof(fPDG) != 0) {printf("TPXsec::fPDG misaligned\n");isaligned=false;}
  if(((unsigned long) &fNEbins) % sizeof(fNEbins) != 0) {printf("TPXsec::fNEbins misaligned\n");isaligned=false;}
  if(((unsigned long) &fNCbins) % sizeof(fNCbins) != 0) {printf("TPXsec::fNCbins misaligned\n");isaligned=false;}
  if(((unsigned long) &fNXsec) % sizeof(fNXsec) != 0) {printf("TPXsec::fNXsec misaligned\n");isaligned=false;}
  if(((unsigned long) &fNTotXs) % sizeof(fNTotXs) != 0) {printf("TPXsec::fNTotXs misaligned\n");isaligned=false;}
  if(((unsigned long) &fNXSecs) % sizeof(fNXSecs) != 0) {printf("TPXsec::fNXSecs misaligned\n");isaligned=false;}
  if(((unsigned long) fEGrid) % sizeof(fEGrid[0]) != 0) {printf("TPXsec::fEGrid misaligned\n");isaligned=false;}
  if(((unsigned long) fMSangle) % sizeof(fMSangle[0]) != 0) {printf("TPXsec::fMSangle misaligned\n");isaligned=false;}
  if(((unsigned long) fMSansig) % sizeof(fMSansig[0]) != 0) {printf("TPXsec::fMSansig misaligned\n");isaligned=false;}
  if(((unsigned long) fMSlength) % sizeof(fMSlength[0]) != 0) {printf("TPXsec::fMSlength misaligned\n");isaligned=false;}
  if(((unsigned long) fMSlensig) % sizeof(fMSlensig[0]) != 0) {printf("TPXsec::fMSlensig misaligned\n");isaligned=false;}
  if(((unsigned long) fdEdx) % sizeof(fdEdx[0]) != 0) {printf("TPXsec::fdEdx misaligned\n");isaligned=false;}
  if(((unsigned long) fTotXs) % sizeof(fTotXs[0]) != 0) {printf("TPXsec::fTotXs misaligned\n");isaligned=false;}
  if(((unsigned long) fXSecs) % sizeof(fXSecs[0]) != 0) {printf("TPXsec::fXSecs misaligned\n");isaligned=false;}
  if(int delta = ((unsigned long) &fEmin) % sizeof(fEmin) != 0) {printf("TPXsec::fEmin misaligned %d \n",delta);isaligned=false;}
  if(int delta = ((unsigned long) &fEmax) % sizeof(fEmax) != 0) {printf("TPXsec::fEmax misaligned %d \n",delta);isaligned=false;}
  if(int delta = ((unsigned long) &fEilDelta) % sizeof(fEilDelta) != 0) {printf("TPXsec::fEilDelta misaligned %d \n",delta);isaligned=false;}
  if(((unsigned long) &fRdict) % sizeof(int) != 0) {printf("TPXsec::fRdict misaligned\n");isaligned=false;}
  if(((unsigned long) &fRmap) % sizeof(int) != 0) {printf("TPXsec::fRmap misaligned\n");isaligned=false;}
#ifdef MAGIC_DEBUG
  if(((unsigned long) &fMagic) % sizeof(fMagic) != 0) {printf("TPXsec::fMagic misaligned\n");isaligned=false;}
#endif
  if(((unsigned long) &fStore) % sizeof(double) != 0) {printf("TPXsec::fStore misaligned\n");isaligned=false;}
  return isaligned;
}
#ifdef GEANT_NVCC
GEANT_CUDA_BOTH_CODE
char *strncpy(char *dest, const char *src, size_t n)
{
    char *ret = dest;
    do {
        if (!n--)
            return ret;
    } while (*dest++ = *src++);
    while (n--)
        *dest++ = 0;
    return ret;
};
#endif

private:
  TPXsec &operator=(const TPXsec &); // Not implemented

  static int fVerbose; // Controls verbosity level

  int fPDG;             // particle pdg code
  int fNEbins;          // number of energy bins
  int fNCbins;          // number of energy bins for dEdx and MS
  int fNXsec;           // number of reactions
  int fNTotXs;          // tot size of fTotXs
  int fNXSecs;          // tot size of fXSecs
  const double *fEGrid; //![fNEbins] energy grid
  float *fMSangle;      // [fNCbins] table of MS average angle
  float *fMSansig;      // [fNCbins] table of MS sigma angle
  float *fMSlength;     // [fNCbins] table of MS average lenght correction
  float *fMSlensig;     // [fNCbins] table of MS sigma lenght correction
  float *fdEdx;         // [fNCbins] table of dE/dx
  float *fTotXs;        // [fNTotXs] table of total x-sec
  float *fXSecs;        // [fNXSecs] table of partial x-sec
  double fEmin;         // Min energy of the energy grid
  double fEmax;         // Max energy of the energy grid
  double fEilDelta;     // logarithmic energy delta
  int fRdict[FNPROC];   // reaction dictionary from reaction number to position
                        // in the X-sec array
  int fRmap[FNPROC];    // reaction map, from reaction position in the X-sec
                        // array to the raction number
#ifdef MAGIC_DEBUG
  const int fMagic = -777777;
#endif

#ifndef GEANT_NVCC
#ifdef USE_ROOT
  ClassDefNV(TPXsec, 4) // Particle X-secs
#endif
#endif

private:
  alignas(sizeof(double)) char fStore[1];        //! Pointer to the compact data of the class
};

#endif
