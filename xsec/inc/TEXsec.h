// Author: Federico Carminati   27/05/13

/*************************************************************************
 * Copyright (C) 1995-2000, fca                                          *
 * All rights reserved.                                                  *
 *                                                                       *
 *************************************************************************/

#ifndef TEXsec_H
#define TEXsec_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TEXSec                                                               //
//                                                                      //
// X-section for GV per material                                        //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TPartIndex.h"
#ifdef USE_ROOT
#include "Rtypes.h"
class TGHorizontalFrame;
class TGListBox;
class TGMainFrame;
class TGraph;
class TRootEmbeddedCanvas;
#endif
class TFile;
class TPXsec;

class TEXsec {
public:
  enum { kCutGamma, kCutElectron, kCutPositron, kCutProton };

  TEXsec();
  TEXsec(int z, int a, float dens, int np);
  virtual ~TEXsec();
  static const char *ClassName() { return "TEXsec"; }
  bool AddPart(int kpart, int pdg, int nxsec);
  bool AddPartXS(int kpart, const float xsec[], const int dict[]);
  bool AddPartIon(int kpart, const float dedx[]);
  bool AddPartMS(int kpart, const float angle[], const float ansig[], const float length[], const float lensig[]);

  int Ele() const { return fEle; }
  int Index() const { return fIndex; }
  void SetIndex(int index) { fIndex = index; }
  double Emin() const { return fEmin; }
  double Emax() const { return fEmax; }
  int NEbins() const { return fNEbins; }
  double EilDelta() const { return fEilDelta; }
  float XS(int pindex, int rindex, float en) const;
  float DEdx(int pindex, float en) const;
  bool MS(int index, float en, float &ang, float &asig, float &len, float &lsig) const;
#ifdef USE_ROOT
  TGraph *XSGraph(const char *part, const char *reac, float emin, float emax, int nbin) const;
  TGraph *DEdxGraph(const char *part, float emin, float emax, int nbin) const;
  TGraph *MSGraph(const char *part, const char *what, float emin, float emax, int nbin) const;
#endif

  float Lambda(int pindex, double en) const;
  bool Lambda_v(int npart, const int pindex[], const double en[], double lam[]) const;
  bool Lambda_v(int npart, int pindex, const double en[], double lam[]) const;
  int SampleReac(int pindex, double en) const;
  int SampleReac(int pindex, double en, double randn) const;

  static bool FloatDiff(double a, double b, double prec) { return fabs(a - b) > 0.5 * fabs(a + b) * prec; }

  const float *Cuts() const { return fCuts; }
  bool SetCuts(const double cuts[4]) {
    for (int jc = 0; jc < 4; ++jc)
      fCuts[jc] = cuts[jc];
    return true;
  }

  void DumpPointers() const;
  void Draw(const char *option);
  void Viewer(); // *MENU*
  void UpdateReactions();
  void SelectAll();
  void DeselectAll();
  void PreDraw();
  void ResetFrame();

  bool Resample();

  bool Prune();

  static int NLdElems() { return fNLdElems; }
  static TEXsec *Element(int i) {
    if (i < 0 || i >= fNLdElems)
      return 0;
    return fElements[i];
  }

  const char *GetName() const { return fName; }
  const char *GetTitle() const { return fTitle; }

  static TEXsec *GetElement(int z, int a = 0, TFile *f = 0);
  static TEXsec **GetElements() { return fElements; }

private:
  TEXsec(const TEXsec &);            // Not implemented
  TEXsec &operator=(const TEXsec &); // Not implemented

  char fName[32];   // Name
  char fTitle[128]; // Title

  int fEle;             // Element code Z*10000+A*10+metastable level
  int fIndex;           // Index of this in TTabPhysMgr::fElemXsec
  double fAtcm3;        // Atoms per cubic cm unit density
  double fEmin;         // Minimum of the energy Grid
  double fEmax;         // Maximum of the energy Grid
  int fNEbins;          // Number of log steps in energy
  double fEilDelta;     // Inverse log energy step
  const double *fEGrid; //! Common energy grid
  int fNRpart;          // Number of particles with reaction
  TPXsec *fPXsec;       // [fNRpart] Cross section table per particle
  float fCuts[4];       // Production cuts "a la G4"

  static int fNLdElems;            //! number of loaded elements
  static TEXsec *fElements[NELEM]; //! databases of elements

#ifdef USE_ROOT
  static TGMainFrame *fMain;           //! Main window
  static TGHorizontalFrame *fSecond;   //! Window for the graph and the bar on left
  static TRootEmbeddedCanvas *fCanvas; //! For the graphs
  static TGListBox *fReactionBox;      //! Reaction list
  static TGListBox *fParticleBox;      //! Particle list

  ClassDefNV(TEXsec, 3) // Element X-secs
#endif
};

#endif
