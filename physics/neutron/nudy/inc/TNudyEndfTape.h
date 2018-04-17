#ifndef TNudyEndfTape_H
#define TNudyEndfTape_H

// Author: F.Carminati 02/05/09

/*
   This is the main class to read a file in ENDF format and write a file
   in R-ENDF format

*/
namespace Nudy {
class TNudyEndfMat;
}

#include <TNamed.h>


namespace Nudy {
class TNudyEndfTape : public TNamed {
public:
  TNudyEndfTape();
  TNudyEndfTape(const char *name, unsigned char loglev);
  virtual ~TNudyEndfTape();

  void SetLogLev(unsigned char loglev) { fLogLev = loglev; }
  unsigned char GetLogLev() const { return fLogLev; }
  const TList *GetMats() const { return fMats; }
  void AddMat(Nudy::TNudyEndfMat *mat);
  void DumpENDF(int flags);
  Nudy::TNudyEndfMat *GetMAT(int MAT);
  Nudy::TNudyEndfMat *GetMAT(int Z, int A);
  //  TNudyEndfMat *GetMAT(TString name);  // Original :: Abhijit DEBUG as constructor has char *name
  Nudy::TNudyEndfMat *GetMAT(char *name);

private:
  unsigned char fLogLev; // LogLevel
  TList *fMats;          // List of materials

#ifdef USE_ROOT
  ClassDef(TNudyEndfTape, 1) // class for an ENDF data file
#endif
};

} // namespace

#endif
