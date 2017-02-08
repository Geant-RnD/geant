#ifndef ROOT_TNudyEndfList
#define ROOT_TNudyEndfList

// @(#)root/meta:$Id: TNuEndf.h 29000 2009-06-15 13:53:52Z rdm $
// Author: F.Carminati 02/05/09

/*
   This is the main class supporting an ENDF material in R-ENDF format

   Units used
   -------------------------------------------------------------------
   Quantity           Units
   energies           electron-volts (eV)
   angles             dimensionless cosines of the angle
   cross sections     barns
   temperatures       Kelvin
   mass               units of the neutron mass
   angular distrib    probability per unit-cosine
   energy distrib     probability per eV
   energy-angle dist  probability per unit-cosine per eV
   half life          seconds
   -------------------------------------------------------------------

*/

#include "TNudyEndfCont.h"

#ifdef USE_ROOT
#include "Rtypes.h"
#endif

class TNudyEndfList : public TNudyEndfCont {
public:
  TNudyEndfList();
  TNudyEndfList(double c1, double c2, int l1, int l2, int n1, int n2);
  virtual ~TNudyEndfList();
  virtual void SetCont(double c1, double c2, int l1, int l2, int n1, int n2);

  int GetNPL() const { return fN1; }
  double GetLIST(int i) const { return fList[i]; }
  void SetLIST(double el, int i) { fList[i] = el; }
  void DumpENDF(int mat, int mf, int mt, int &ns, int flags);
 // int getDebugSize() const { return debugSize; }
private:
  double *fList; //[fN1]
  //int debugSize;

#ifdef USE_ROOT
  ClassDef(TNudyEndfList, 1)
#endif
};

#endif
