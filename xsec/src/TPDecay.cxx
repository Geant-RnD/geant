#include "TPDecay.h"
#include "TFinState.h"

ClassImp(TPDecay)

//___________________________________________________________________
TPDecay::TPDecay():
    fNSamp(0),
    fNPart(0),
    fDecay(0)
{
}

//___________________________________________________________________
TPDecay::TPDecay(Int_t nsample, Int_t npart, TFinState *decay):
    fNSamp(nsample),
    fNPart(npart),
    fDecay(decay)
{
}

//___________________________________________________________________
TPDecay::~TPDecay() {
}

//___________________________________________________________________
Bool_t TPDecay::SampleDecay(Int_t pindex, Int_t &npart,
                            const Int_t *&pid, const Float_t *&mom) const
{
  Float_t kerma;
  Float_t weight;
  Float_t en;
  return fDecay[pindex].SampleReac(npart, weight, kerma, en, pid, mom);
}

//___________________________________________________________________
Bool_t TPDecay::GetDecay(Int_t pindex, Int_t ifs, Int_t &npart,
                         const Int_t *&pid, const Float_t *&mom) const
{
  Float_t kerma;
  Float_t weight;
  Float_t en;
  return fDecay[pindex].GetReac(ifs,npart, weight, kerma, en, pid, mom);
}

