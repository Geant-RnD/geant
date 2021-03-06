/*
   This is the main class to read a file in ENDF format and write a file
   in R-ENDF format

   fca 2-mai-2010
*/

#include <iostream>
#include "TList.h"
#include "Geant/TNudyEndfTape.h"
#include "Geant/TNudyEndfMat.h"

using namespace Nudy;

#ifdef USE_ROOT
ClassImp(TNudyEndfTape)
#endif

//_______________________________________________________________________________
TNudyEndfTape::TNudyEndfTape()
  : fLogLev(0), fMats(0)
{
  //
  // Standard constructor
  //
}

//_______________________________________________________________________________
TNudyEndfTape::TNudyEndfTape(const char *name, unsigned char loglev) : TNamed("", name), fLogLev(loglev)
{
  // Key names cannot contain blanks
  TString sname(name);
  sname = sname.Strip(TString::kLeading);
  sname = sname(0, 10);
  sname = sname.Strip();
  sname.ReplaceAll(" ", "_");
  sname.ReplaceAll("/", "_");
  SetName(sname.Data());
  // std::cout << "Creating ENDF Tape:" << std::endl << name << std::endl;
  fMats = new TList();
};

//______________________________________________________________________________
TNudyEndfTape::~TNudyEndfTape()
{
  //  printf("Destroying TAPE %s\n",GetName());
  if (fMats) fMats->Delete();
  SafeDelete(fMats);
}

//_______________________________________________________________________________
void TNudyEndfTape::DumpENDF(int flags = 1)
{
  // Name of the tape
  printf("%80s\n", GetName());
  // Materials
  for (int i = 0; i <= fMats->LastIndex(); i++) {
    TNudyEndfMat *mat = (TNudyEndfMat *)fMats->At(i);
    mat->DumpENDF(flags);
  }
  // TEND
  printf("%66s", " 0.000000+0 0.000000+0          0          0          0          0");
  printf("%4d%2d%3d%5d", -1, 0, 0, 0);
  if (flags)
    printf("  ---TEND\n");
  else
    printf("\n");
}

//_______________________________________________________________________________
void TNudyEndfTape::AddMat(TNudyEndfMat *mat)
{
  fMats->Add(mat);
}

//_______________________________________________________________________________
TNudyEndfMat *TNudyEndfTape::GetMAT(int MAT)
{
  for (int i = 0; i <= this->GetMats()->LastIndex(); i++) {
    TNudyEndfMat *thisMat = (TNudyEndfMat *)this->GetMats()->At(i);
    if (thisMat->GetMAT() == MAT) return thisMat;
  }
  Error("TNudyEndfMat::GetMAT(int)", "Could not find material %d on tape", MAT);
  return NULL;
}

//_______________________________________________________________________________
TNudyEndfMat *TNudyEndfTape::GetMAT(int Z, int A)
{
  int ZA = 1000 * Z + A;
  for (int i = 0; i <= this->GetMats()->LastIndex(); i++) {
    TNudyEndfMat *thisMat = (TNudyEndfMat *)this->GetMats()->At(i);
    if (thisMat->GetZA() == ZA) return thisMat;
  }
  return NULL;
}

//------------------------------------------------------------------------------
// TNudyEndfMat *TNudyEndfTape::GetMAT(TString name) {    // Abhijit as name was supplied as char *name
TNudyEndfMat *TNudyEndfTape::GetMAT(char *name)
{
  TString tstStr; // DEBUG
  for (int i = 0; i <= this->GetMats()->LastIndex(); i++) {
    TNudyEndfMat *thisMat = (TNudyEndfMat *)this->GetMats()->At(i);
    tstStr                = thisMat->GetName(); // DEBUG
    if (tstStr.CompareTo(name) == 0) return thisMat;
  }
  return NULL;
}
