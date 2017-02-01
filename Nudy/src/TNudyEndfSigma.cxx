// This class is reconstructing ENDF cross-section data and rewrite to ROOT file
// Author: Dr. Harphool Kumawat
// Email: harphool@barc.gov.in; harphool.kumawat@cern.ch
// date of creation: July 25, 2016

#include "TList.h"
#include "TFile.h"
#include "TKey.h"
#include "TNudyENDF.h"
#include "TNudyEndfFile.h"
#include "TNudyEndfTape.h"
#include "TNudyEndfList.h"
#include "TNudyEndfTab1.h"
#include "TNudyEndfTab2.h"
#include "TNudyEndfMat.h"
#include "TNudyEndfDoppler.h"
#include "TNudyCore.h"
#include "TNudyEndfSigma.h"
#include "Math/SpecFuncMathMore.h"
#include "TNudyEndfEnergy.h"
#include "TNudyEndfEnergyAng.h"
#include "TNudyEndfFissionYield.h"
#include "TNudyEndfPhYield.h"
#include "TNudyEndfPhProd.h"
#include "TNudyEndfPhAng.h"
#include "TNudyEndfPhEnergy.h"

#ifdef USE_ROOT
ClassImp(TNudyEndfSigma)
#endif

#ifdef USE_ROOT
#include "TRandom3.h"
#endif

#include <algorithm>

TNudyEndfSigma::TNudyEndfSigma()
    : rENDF(), sigDiff(0)
{
}
TNudyEndfSigma::TNudyEndfSigma(const char *irENDF, double isigDiff) : rENDF(irENDF), sigDiff(isigDiff)
{
  GetData(irENDF, isigDiff);
}

//____________________________________________________________________________________________________________________
double TNudyEndfSigma::recursionLinearNuPh(double x1, double x2, double sig1, double sig2, std::vector<double> x,
                                           std::vector<double> sig)
{
  double siga;
  double mid = 0.5 * (x1 + x2);
  if ((sig1 == 0.0 && sig2 == 0.0) || x1 == x2 || x1 < 1E-5 || x2 < 1E-5) return 0;
  siga           = TNudyCore::Instance()->Interpolate(nbt1, int1, NR, x, sig, NP, mid);
  double sigmid1 = sig1 + (sig2 - sig1) * (mid - x1) / (x2 - x1);
  if (fabs((siga - sigmid1) / sigmid1) <= sigDiff) {
    return 0;
  }
  x.push_back(mid);
  sig.push_back(siga);
  recursionLinearNuPh(x1, mid, sig1, siga, x, sig);
  recursionLinearNuPh(mid, x2, siga, sig2, x, sig);
  return 0;
}
//______________________________________________________________________________
void TNudyEndfSigma::ReadFile1(TNudyEndfFile *file)
{
  TIter secIter(file->GetSections());
  TNudyEndfSec *sec;
  while ((sec = (TNudyEndfSec *)secIter.Next())) {
    TIter recIter(sec->GetRecords());
    //	double ZA   = sec->GetC1();
    //	double AWR  = sec->GetC2();
    int MT = sec->GetMT();
    if (MT == 452) { // Total fission neutron multiplicity polynomial expansion
      int LNU = sec->GetL2();
      if (LNU == 1) {
        TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
        for (int j = 0; j < list->GetN1(); j++) {
          cnc.push_back(list->GetLIST(j));
        }
        double ein = 1E-5;
        do {
          double nun = 0;
          for (int i = 0; i < list->GetN1(); i++) {
            nun += cnc[i] * pow(ein, i);
          }
          eintFile1.push_back(ein);
          nutFile1.push_back(nun);
          ein *= 2;
        } while (ein < 21E8);
        cnc.clear();
      } else { // Total fission neutron multiplicity tabulated representation
        TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter.Next();
        NR                  = tab1->GetN1();
        NP                  = tab1->GetN2();
        for (int cr = 0; cr < NR; cr++) {
          nbt1.push_back(tab1->GetNBT(cr));
          int1.push_back(tab1->GetINT(cr));
        }
        for (int crs = 0; crs < NP; crs++) {
          eintFile1.push_back(tab1->GetX(crs));
          nutFile1.push_back(tab1->GetY(crs));
        }
        for (int cr = 0; cr < NP - 1; cr++) {
          recursionLinearNuPh(eintFile1[cr], eintFile1[cr + 1], nutFile1[cr], nutFile1[cr + 1], eintFile1, nutFile1);
        }
        TNudyCore::Instance()->Sort(eintFile1, nutFile1);
      }
      eint.push_back(eintFile1);
      nut.push_back(nutFile1);
      eintFile1.clear();
      nutFile1.clear();
      nbt1.clear();
      int1.clear();
    } else if (MT == 455) { // delayed neutron multiplicity
      int LDG = sec->GetL1();
      int LNU = sec->GetL2();
      if (LNU == 1 && LDG == 0) {

      } else if (LNU == 1 && LDG == 1) {

      } else if (LNU == 2 && LDG == 0) {
        TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
        int NNF             = list->GetNPL();
        for (int i = 0; i < NNF; i++) {
          nui.push_back(list->GetLIST(i));
        }
        TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter.Next();
        NR                  = tab1->GetN1();
        NP                  = tab1->GetN2();
        for (int cr = 0; cr < NR; cr++) {
          nbt1.push_back(tab1->GetNBT(cr));
          int1.push_back(tab1->GetINT(cr));
        }
        for (int crs = 0; crs < NP; crs++) {
          eindFile1.push_back(tab1->GetX(crs));
          nudFile1.push_back(tab1->GetY(crs));
        }
        for (int cr = 0; cr < NP - 1; cr++) {
          recursionLinearNuPh(eindFile1[cr], eindFile1[cr + 1], nudFile1[cr], nudFile1[cr + 1], eindFile1, nudFile1);
        }
        TNudyCore::Instance()->Sort(eindFile1, nudFile1);
        eind.push_back(eindFile1);
        nud.push_back(nudFile1);
        lambdaD.push_back(nui);
        eindFile1.clear();
        nudFile1.clear();
        nui.clear();
        nbt1.clear();
        int1.clear();
      } else if (LNU == 2 && LDG == 1) {
      }
    } else if (MT == 456) { // prompt neutron multiplicity
      int LNU = sec->GetL2();
      if (LNU == 1) {
        //      std::cout<<"prompt nu = "<< ZA <<" LNU "<< LNU << std::endl;
        //	TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
        // double nup = list->GetLIST(0);
      } else {
        TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter.Next();
        NR                  = tab1->GetN1();
        NP                  = tab1->GetN2();
        for (int cr = 0; cr < NR; cr++) {
          nbt1.push_back(tab1->GetNBT(cr));
          int1.push_back(tab1->GetINT(cr));
        }
        for (int crs = 0; crs < NP; crs++) {
          einFile1.push_back(tab1->GetX(crs));
          nuFile1.push_back(tab1->GetY(crs));
        }
        for (int cr = 0; cr < NP - 1; cr++) {
          recursionLinearNuPh(einFile1[cr], einFile1[cr + 1], nuFile1[cr], nuFile1[cr + 1], einFile1, nuFile1);
        }
        TNudyCore::Instance()->Sort(einFile1, nuFile1);
        nbt1.clear();
        int1.clear();
      }
      einp.push_back(einFile1);
      nup.push_back(nuFile1);
      einFile1.clear();
      nuFile1.clear();
    } else if (MT == 458) {
      TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
      int NPLY            = list->GetL2();
      if (NPLY == 0) {
        double EFR = list->GetLIST(0);
        double ENP = list->GetLIST(2);
        double END = list->GetLIST(4);
        double EGP = list->GetLIST(6);
        double EGD = list->GetLIST(8);
        double EB  = list->GetLIST(10);
        double ENU = list->GetLIST(12);
        //	double ER  = list->GetLIST(14);
        // double ET  = list->GetLIST(16);
        double ein = 1E-5;
        do {
          double EFis = 0;
          EFis        = EFR + ENP + END + EGP + EGD + EB + ENU;
          EFis -= 0.100 * ein; // nuetrino energy dependence
          EFis -= 0.075 * ein; // delayed gamma energy dependence
          EFis -= 0.075 * ein; // delayed beta energy dependence
          //	      for(unsigned long i = 0; i < eintFile1.size(); i++)
          //		std::cout<< eintFile1[i] <<"  "<<nutFile1[i] << std::endl;
          int n0  = 0;
          int max = eintFile1.size() - 1;
          int mid = 0;
          if (ein <= eintFile1[n0]) {
            n0 = 0;
          } else if (ein >= eintFile1[max]) {
            n0 = max - 1;
          } else {
            while (max - n0 > 1) {
              mid = (n0 + max) / 2;
              if (ein < eintFile1[mid])
                max = mid;
              else
                n0 = mid;
              //      std::cout<<"n0 "<< n0 <<" max "<< max << std::endl;
            }
          }
          double nue = TNudyCore::Instance()->LinearInterpolation(eintFile1[n0], nutFile1[n0], eintFile1[n0 + 1],
                                                                  nutFile1[n0 + 1], ein);
          double nu0 = nutFile1[0];
          EFis -= -1.307 * ein + 8.07 * 1E6 * (nue - nu0); // prompt neutron energy dependence
          einfFile1.push_back(ein);
          heatFile1.push_back(EFis);
          ein *= 2;
        } while (ein < 21E8);
      } else {
        double c0[9 * (NPLY + 1)], c1[9 * (NPLY + 1)];
        for (int i = 0; i < 9 * (NPLY + 1); i++) {
          c0[i] = list->GetLIST(i);
        }
        for (int i = 9 * (NPLY + 1); i < 18 * (NPLY + 1); i++) {
          c1[i - 9 * (NPLY + 1)] = list->GetLIST(i);
        }
        double ein = 1E-5;
        do {
          double EFis = 0;
          for (int i = 0; i < 9 * NPLY / 2 - 2; i++) {
            EFis += c0[i * 2] + c1[i * 2] * ein;
          }
          einfFile1.push_back(ein);
          heatFile1.push_back(EFis);
          ein *= 2;
        } while (ein < 21E8);
      }
      einFissHeat.push_back(einfFile1);
      fissHeat.push_back(heatFile1);
      einfFile1.clear();
      heatFile1.clear();
    } else if (MT == 460) {
      int LO = sec->GetL1();
      int NG = sec->GetN1();
      if (LO == 1) {
        for (int ng = 0; ng < NG; ng++) {
          TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter.Next();
          NR                  = tab1->GetN1();
          NP                  = tab1->GetN2();
          for (int cr = 0; cr < NR; cr++) {
            nbt1.push_back(tab1->GetNBT(cr));
            int1.push_back(tab1->GetINT(cr));
          }
          for (int crs = 0; crs < NP; crs++) {
            einphFile1.push_back(tab1->GetX(crs));
            phFile1.push_back(tab1->GetY(crs));
          }
          // linearzation is stopped due to discrete photons which are to be matched with file 12 later
          // for(int cr=0; cr < NP - 1 ; cr ++){
          //  recursionLinearNuPh(einphFile1[cr], einphFile1[cr+1], phFile1[cr], phFile1[cr+1],einphFile1, phFile1);
          //}
          // TNudyCore::Instance()->Sort(einphFile1, phFile1);
          einphFile1.clear();
          phFile1.clear();
          nbt1.clear();
          int1.clear();
        }
      } else {
        TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
        int NNF             = list->GetN1();
        // double lambda[NNF];
        for (int i = 0; i < NNF; i++) {
          // lambda[i] = list->GetLIST(i);
          //	     std::cout <<"lambda  "<< lambda[i] << std::endl;
        }
      }
    }
  }
}
//______________________________________________________________________________
void TNudyEndfSigma::ReadFile2(TNudyEndfFile *file)
{
  TIter secIter(file->GetSections());
  TNudyEndfSec *sec;
  NLS           = 0;
  double gjdeno = 1;
  LSSF          = 0;
  int nrsl0     = 0;
  while ((sec = (TNudyEndfSec *)secIter.Next())) {
    for (int k = 0; k < sec->GetN1(); k++) {
      TIter recIter(sec->GetRecords());
      ZAI                  = sec->GetC1();
      AWRI                 = sec->GetC2();
      NIS                  = sec->GetN1();
      TNudyEndfCont *cont1 = (TNudyEndfCont *)recIter.Next();
      ABN                  = cont1->GetC2();
      LFW                  = cont1->GetL2();
      NER                  = cont1->GetN1();
      for (int j = 0; j < NER; j++) {
        TNudyEndfCont *cont2 = (TNudyEndfCont *)recIter.Next();
        eLo                  = cont2->GetC1();
        eHi                  = cont2->GetC2();
        LRU                  = cont2->GetL1();
        LRF                  = cont2->GetL2();
        NRO                  = cont2->GetN1();
        NAPS                 = cont2->GetN2();
        if (LRU == 2 && LRF == 1 && LFW == 1) {
        } else {
          TNudyEndfCont *cont3 = (TNudyEndfCont *)recIter.Next();
          SPI                  = cont3->GetC1();
          AP                   = cont3->GetC2();
          if (LRU == 2) LSSF   = cont3->GetL1();
          // if(LRF==3)int LAD = cont3->GetL1();
          if (LRU == 1) NLS = cont3->GetN1();
          NLS2              = cont3->GetN1();
          NRS.resize(NLS, 0);
          gjdeno = 4.0 * SPI + 2.0;
        }
        switch (LRU) {
        case 0: {
        } break;
        case 1: { // resolved resonance region
          switch (LRF) {
          case 1: // single level resonance region
          case 2: // multi level resonance region
          case 3: // RM resonance region
          case 4: // Adler - Adler resonance region
          {
            flagResolve      = 1;
            if (j == 0) eLo1 = eLo;
            eHi1             = eHi;
            for (int lseries = 0; lseries < NLS; lseries++) {
              TNudyEndfList *list                    = (TNudyEndfList *)recIter.Next();
              AWRI                                   = list->GetC1();
              QX                                     = list->GetC2();
              if (QX == 0.0) APL[lseries]            = AP;
              if (LRF == 3 && QX > 0.0) APL[lseries] = list->GetC2();
              l.push_back(list->GetL1());
              LRX               = list->GetL2();
              NRS[lseries]      = list->GetN2();
              A                 = Mn * AWRI;
              Z                 = (int)(ZA - A) / 1000.0;
              rad_a             = 0.08 + 0.123 * pow(1.00866491578 * AWRI, (1. / 3.));
              if (AP == 0.0) AP = rad_a;
              factor_k          = kconst * (AWRI / (AWRI + 1.0));
              JMIN              = (std::fabs(SPI - l[lseries]) - 0.5);
              JMAX              = (SPI + l[lseries] + 0.5);
              nrsl0             = (!lseries) ? 0 : nrsl0 + NRS[lseries - 1]; // last NRS value
              double jtemp[NRS[lseries]];
              if (LRF == 1 || LRF == 2) {
                for (int ii = 0; ii < NRS[lseries]; ii++) {
                  cueMat = nrsl0 + ii;
                  Er.push_back(list->GetLIST(ii * 6 + 0));
                  J.push_back(list->GetLIST(ii * 6 + 1));                    // J value
                  Gamma_r.push_back(list->GetLIST(ii * 6 + 2));              // total width
                  Gamma_n.push_back(list->GetLIST(ii * 6 + 3));              // neutron width
                  Gamma_g.push_back(list->GetLIST(ii * 6 + 4));              // gamma width
                  Gamma_f.push_back(list->GetLIST(ii * 6 + 5));              // fission width
                  GJ.push_back((2.0 * std::fabs(J[cueMat]) + 1.0) / gjdeno); //(2J+1)/2(2I+1)
                  PhiEr.push_back(calcPene(std::fabs(list->GetLIST(ii * 6 + 0)), lseries));
                  ShiftEr.push_back(calcShift(std::fabs(list->GetLIST(ii * 6 + 0)), lseries));
                  Gamma_n[nrsl0 + ii] = Gamma_n[nrsl0 + ii] / PhiEr[nrsl0 + ii];
                  jtemp[ii]           = list->GetLIST(ii * 6 + 1);
                }
                double jmis = -99, temp, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9;
                for (int isort = 0; isort < NRS[lseries]; isort++) {
                  for (int isort1 = 1; isort1 < NRS[lseries]; isort1++) {
                    if (jtemp[isort1] < jtemp[isort1 - 1]) {
                      temp              = jtemp[isort1];
                      jtemp[isort1]     = jtemp[isort1 - 1];
                      jtemp[isort1 - 1] = temp;

                      temp1                  = Er[nrsl0 + isort1];
                      Er[nrsl0 + isort1]     = Er[nrsl0 + isort1 - 1];
                      Er[nrsl0 + isort1 - 1] = temp1;

                      temp2                 = J[nrsl0 + isort1];
                      J[nrsl0 + isort1]     = J[nrsl0 + isort1 - 1];
                      J[nrsl0 + isort1 - 1] = temp2;

                      temp3                       = Gamma_r[nrsl0 + isort1];
                      Gamma_r[nrsl0 + isort1]     = Gamma_r[nrsl0 + isort1 - 1];
                      Gamma_r[nrsl0 + isort1 - 1] = temp3;

                      temp4                       = Gamma_n[nrsl0 + isort1];
                      Gamma_n[nrsl0 + isort1]     = Gamma_n[nrsl0 + isort1 - 1];
                      Gamma_n[nrsl0 + isort1 - 1] = temp4;

                      temp5                       = Gamma_g[nrsl0 + isort1];
                      Gamma_g[nrsl0 + isort1]     = Gamma_g[nrsl0 + isort1 - 1];
                      Gamma_g[nrsl0 + isort1 - 1] = temp5;

                      temp6                       = Gamma_f[nrsl0 + isort1];
                      Gamma_f[nrsl0 + isort1]     = Gamma_f[nrsl0 + isort1 - 1];
                      Gamma_f[nrsl0 + isort1 - 1] = temp6;

                      temp7                  = GJ[nrsl0 + isort1];
                      GJ[nrsl0 + isort1]     = GJ[nrsl0 + isort1 - 1];
                      GJ[nrsl0 + isort1 - 1] = temp7;

                      temp8                     = PhiEr[nrsl0 + isort1];
                      PhiEr[nrsl0 + isort1]     = PhiEr[nrsl0 + isort1 - 1];
                      PhiEr[nrsl0 + isort1 - 1] = temp8;

                      temp9                       = ShiftEr[nrsl0 + isort1];
                      ShiftEr[nrsl0 + isort1]     = ShiftEr[nrsl0 + isort1 - 1];
                      ShiftEr[nrsl0 + isort1 - 1] = temp9;
                    }
                  }
                }
                int ju              = 0;
                NJValue[l[lseries]] = 0;
                MisGj[l[lseries]]   = 0;
                for (int j1 = 0; j1 < NRS[lseries]; j1++) {
                  if (jtemp[j1] != jmis) {
                    MissingJ[l[lseries]][ju] = jtemp[j1];
                    NJValue[l[lseries]] += 1;
                    jmis = jtemp[j1];
                    ju += 1;
                  }
                }
              } else if (LRF == 3) {
                for (int ii = 0; ii < NRS[lseries]; ii++) {
                  cueMat = nrsl0 + ii;
                  Er.push_back(list->GetLIST(ii * 6 + 0));
                  J.push_back(list->GetLIST(ii * 6 + 1));                                 // J value
                  Gamma_n.push_back(list->GetLIST(ii * 6 + 2));                           // total width
                  Gamma_g.push_back(list->GetLIST(ii * 6 + 3));                           // neutron width
                  Gamma_fa.push_back(list->GetLIST(ii * 6 + 4));                          // gamma width
                  Gamma_fb.push_back(list->GetLIST(ii * 6 + 5));                          // fission width
                  Gamma_fasq.push_back(sqrt(0.5 * std::fabs(list->GetLIST(ii * 6 + 4)))); // gamma width
                  Gamma_fbsq.push_back(sqrt(0.5 * std::fabs(list->GetLIST(ii * 6 + 5)))); // fission width
                  if (Gamma_fa[cueMat] < 0.0) Gamma_fasq[cueMat] = -Gamma_fasq[cueMat];
                  if (Gamma_fb[cueMat] < 0.0) Gamma_fbsq[cueMat] = -Gamma_fbsq[cueMat];
                  GJ.push_back((2.0 * std::fabs(J[cueMat]) + 1.0) / gjdeno); //(2J+1)/2(2I+1)
                  jtemp[ii] = list->GetLIST(ii * 6 + 1);
                  PhiEr.push_back(calcPene(std::fabs(list->GetLIST(ii * 6 + 0)), lseries));
                  ShiftEr.push_back(calcShift(std::fabs(list->GetLIST(ii * 6 + 0)), lseries));
                  Gamma_n[nrsl0 + ii] = Gamma_n[nrsl0 + ii] / PhiEr[nrsl0 + ii];
                }
                double gjfound = 0.0, jmis = -99, temp, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9,
                       temp10, temp11;
                for (int isort = 0; isort < NRS[lseries]; isort++) {
                  for (int isort1 = 1; isort1 < NRS[lseries]; isort1++) {
                    if (jtemp[isort1] < jtemp[isort1 - 1]) {
                      temp              = jtemp[isort1];
                      jtemp[isort1]     = jtemp[isort1 - 1];
                      jtemp[isort1 - 1] = temp;

                      temp1                  = Er[nrsl0 + isort1];
                      Er[nrsl0 + isort1]     = Er[nrsl0 + isort1 - 1];
                      Er[nrsl0 + isort1 - 1] = temp1;

                      temp2                 = J[nrsl0 + isort1];
                      J[nrsl0 + isort1]     = J[nrsl0 + isort1 - 1];
                      J[nrsl0 + isort1 - 1] = temp2;

                      temp3                        = Gamma_fa[nrsl0 + isort1];
                      Gamma_fa[nrsl0 + isort1]     = Gamma_fa[nrsl0 + isort1 - 1];
                      Gamma_fa[nrsl0 + isort1 - 1] = temp3;

                      temp4                       = Gamma_n[nrsl0 + isort1];
                      Gamma_n[nrsl0 + isort1]     = Gamma_n[nrsl0 + isort1 - 1];
                      Gamma_n[nrsl0 + isort1 - 1] = temp4;

                      temp5                       = Gamma_g[nrsl0 + isort1];
                      Gamma_g[nrsl0 + isort1]     = Gamma_g[nrsl0 + isort1 - 1];
                      Gamma_g[nrsl0 + isort1 - 1] = temp5;

                      temp6                        = Gamma_fb[nrsl0 + isort1];
                      Gamma_fb[nrsl0 + isort1]     = Gamma_fb[nrsl0 + isort1 - 1];
                      Gamma_fb[nrsl0 + isort1 - 1] = temp6;

                      temp7                  = GJ[nrsl0 + isort1];
                      GJ[nrsl0 + isort1]     = GJ[nrsl0 + isort1 - 1];
                      GJ[nrsl0 + isort1 - 1] = temp7;

                      temp8                     = PhiEr[nrsl0 + isort1];
                      PhiEr[nrsl0 + isort1]     = PhiEr[nrsl0 + isort1 - 1];
                      PhiEr[nrsl0 + isort1 - 1] = temp8;

                      temp9                       = ShiftEr[nrsl0 + isort1];
                      ShiftEr[nrsl0 + isort1]     = ShiftEr[nrsl0 + isort1 - 1];
                      ShiftEr[nrsl0 + isort1 - 1] = temp9;

                      temp10                         = Gamma_fasq[nrsl0 + isort1];
                      Gamma_fasq[nrsl0 + isort1]     = Gamma_fasq[nrsl0 + isort1 - 1];
                      Gamma_fasq[nrsl0 + isort1 - 1] = temp10;

                      temp11                         = Gamma_fbsq[nrsl0 + isort1];
                      Gamma_fbsq[nrsl0 + isort1]     = Gamma_fbsq[nrsl0 + isort1 - 1];
                      Gamma_fbsq[nrsl0 + isort1 - 1] = temp11;
                    }
                  }
                }
                int ju              = 0;
                NJValue[l[lseries]] = 0;
                MisGj[l[lseries]]   = 0;
                for (int j1 = 0; j1 < NRS[lseries]; j1++) {
                  if (jtemp[j1] != jmis) {
                    MissingJ[l[lseries]][ju] = jtemp[j1];
                    NJValue[l[lseries]] += 1;
                    jmis = jtemp[j1];
                    gjfound += (2 * std::fabs(jtemp[j1]) + 1) / gjdeno;
                    ju += 1;
                  }
                }
                MisGj[l[lseries]] = 2 * l[lseries] + 1 - gjfound;
              } else if (LRF == 4) {                        // Adler -Adler resonance region
                for (int ii = 0; ii < NRS[lseries]; ii++) { // line 6 onwards data
                  cueMat = nrsl0 + ii;
                  at1.push_back(list->GetLIST(ii * 6 + 0));
                  at2.push_back(list->GetLIST(ii * 6 + 1));
                  at3.push_back(list->GetLIST(ii * 6 + 2));
                  at4.push_back(list->GetLIST(ii * 6 + 3));
                  bt1.push_back(list->GetLIST(ii * 6 + 4));
                  bt2.push_back(list->GetLIST(ii * 6 + 5));
                }
                TNudyEndfList *list1 = (TNudyEndfList *)recIter.Next();
                for (int ii = 0; ii < list1->GetN2(); ii++) {
                  det1.push_back(list1->GetLIST(ii * 6 + 0));
                  dwt1.push_back(list1->GetLIST(ii * 6 + 1));
                  grt1.push_back(list1->GetLIST(ii * 6 + 2));
                  git1.push_back(list1->GetLIST(ii * 6 + 3));
                  def1.push_back(list1->GetLIST(ii * 6 + 4));
                  dwf1.push_back(list1->GetLIST(ii * 6 + 5));
                  grf1.push_back(list1->GetLIST(ii * 6 + 6 + 0));
                  gif1.push_back(list1->GetLIST(ii * 6 + 6 + 1));
                  dec1.push_back(list1->GetLIST(ii * 6 + 6 + 2));
                  dwc1.push_back(list1->GetLIST(ii * 6 + 6 + 3));
                  grc1.push_back(list1->GetLIST(ii * 6 + 6 + 4));
                  gic1.push_back(list1->GetLIST(ii * 6 + 6 + 5));
                }
              }
            } // loop for L values
          } break;
          case 7: { // R-Matrix
          } break;
          } // break;
          Linearize(j);
          // clearing vectors for different energy regions
          NRS.clear();
          l.clear();
          Er.clear();
          J.clear();
          Gamma_r.clear();
          Gamma_n.clear();
          Gamma_g.clear();
          Gamma_f.clear();
          GJ.clear();
          PhiEr.clear();
          ShiftEr.clear();
          Gamma_fa.clear();
          Gamma_fasq.clear();
          Gamma_fb.clear();
          Gamma_fbsq.clear();
          at1.clear();
          at2.clear();
          at3.clear();
          at4.clear();
          bt1.clear();
          bt2.clear();
          det1.clear();
          dwt1.clear();
          grt1.clear();
          git1.clear();
          def1.clear();
          dwf1.clear();
          grf1.clear();
          gif1.clear();
          dec1.clear();
          dwc1.clear();
          grc1.clear();
          gic1.clear();

        } break;
        case 2: { // unresolved resonance region
          flagUnResolve = 1;
          switch (LRF) {
          case 2: {
            for (int lseries = 0; lseries < NLS2; lseries++) {
              TNudyEndfList *list2 = (TNudyEndfList *)recIter.Next();
              AWRI                 = list2->GetC1();
              l.push_back(list2->GetL1());
              NJS = list2->GetN1();
              NRJ.push_back(NJS);
              APL[lseries] = AP;
              for (int jseries = 0; jseries < NJS; jseries++) {
                TNudyEndfList *list3 = (TNudyEndfList *)recIter.Next();
                JSM.push_back(list3->GetN2());
                rad_a             = 0.08 + 0.123 * pow(1.00866491578 * AWRI, (1. / 3.));
                if (AP == 0.0) AP = rad_a;
                factor_k          = kconst * (AWRI / (AWRI + 1.0));
                JMIN              = list3->GetC1();
                INT               = list3->GetL1();
                amux.push_back(list3->GetLIST(2));            // total width
                amun.push_back(list3->GetLIST(3));            // neutron width
                amug.push_back(list3->GetLIST(4));            // gamma width
                amuf.push_back(list3->GetLIST(5));            // fission width
                GJ.push_back((2.0 * JMIN + 1.0) / gjdeno);    //(2J+1)/2(2I+1)
                for (int ii = 0; ii < list3->GetN2(); ii++) { // line 6 onwards data
                  Es.push_back(list3->GetLIST((ii + 1) * 6 + 0));
                  D.push_back(list3->GetLIST((ii + 1) * 6 + 1));   // J value
                  GX.push_back(list3->GetLIST((ii + 1) * 6 + 2));  // total width
                  GNO.push_back(list3->GetLIST((ii + 1) * 6 + 3)); // neutron width
                  GG.push_back(list3->GetLIST((ii + 1) * 6 + 4));  // gamma width
                  GF.push_back(list3->GetLIST((ii + 1) * 6 + 5));  // fission width
                }
              } // complete NJS reading
            }   // complete NLS2 reading
            eLo2 = eLo;
            eHi2 = eHi;
            // std::cout << " eLo2 "<< eLo2 <<" eHi2 "<< eHi2 << std::endl;
            if (LSSF == 0) Linearize(j);
            l.clear();
            NRS.clear();
            NRJ.clear();
            JSM.clear();
            amux.clear();
            amun.clear();
            amug.clear();
            amuf.clear();
            GJ.clear();
            Es.clear();
            D.clear();
            GX.clear();
            GNO.clear();
            GG.clear();
            GF.clear();
          } break;
          case 1: {
            switch (LFW) {
            case 0: {
              for (int lseries = 0; lseries < NLS2; lseries++) {
                TNudyEndfList *list2 = (TNudyEndfList *)recIter.Next();
                AWRI                 = list2->GetC1();
                l.push_back(list2->GetL1());
                NJS = list2->GetN2();
                NRJ.push_back(NJS);
                APL[lseries] = AP;
                for (int ii = 0; ii < NJS; ii++) { // line 6 onwards data
                  JSM.push_back(1);
                  rad_a             = 0.08 + 0.123 * pow(1.00866491578 * AWRI, (1. / 3.));
                  if (AP == 0.0) AP = rad_a;
                  factor_k          = kconst * (AWRI / (AWRI + 1.0));
                  INT               = 2;
                  Es.push_back(eLo);
                  D.push_back(list2->GetLIST((ii)*6 + 0));    // Average level spacing for resonances with spin J
                  amun.push_back(list2->GetLIST((ii)*6 + 2)); // degree of freedom neutron width
                  GNO.push_back(list2->GetLIST((ii)*6 + 3));  // neutron width
                  GG.push_back(list2->GetLIST((ii)*6 + 4));   // gamma width
                  GF.push_back(0.0);                          // fission width
                  GX.push_back(0.0);                          // fission width
                  amug.push_back(0.0);                        // degree of freedom gamma width
                  amuf.push_back(0.0);                        // degree of freedom fission width
                  amux.push_back(0.0);                        // degree of freedom competitive width
                  GJ.push_back((2.0 * list2->GetLIST((ii)*6 + 1) + 1.0) / gjdeno); //(2J+1)/2(2I+1)
                }
              }
              eLo2 = eLo;
              eHi2 = eHi;
              if (LSSF == 0) Linearize(j);
              l.clear();
              NRS.clear();
              NRJ.clear();
              JSM.clear();
              amux.clear();
              amun.clear();
              amug.clear();
              amuf.clear();
              GJ.clear();
              Es.clear();
              D.clear();
              GX.clear();
              GNO.clear();
              GG.clear();
              GF.clear();
            } break;
            case 1: {
              TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
              SPI                 = list->GetC1();
              AP                  = list->GetC2();
              LSSF                = list->GetL1();
              NLS2                = list->GetN2();
              NE                  = list->GetN1();
              NRS.resize(NLS, 0);
              gjdeno = 4.0 * SPI + 2.0;
              for (int lseries = 0; lseries < NLS2; lseries++) {
                TNudyEndfCont *list2 = (TNudyEndfCont *)recIter.Next();
                AWRI                 = list2->GetC1();
                l.push_back(list2->GetL1());
                NJS = list2->GetN1();
                NRJ.push_back(NJS);
                APL[lseries] = AP;
                for (int jseries = 0; jseries < NJS; jseries++) {
                  TNudyEndfList *list3 = (TNudyEndfList *)recIter.Next();
                  JSM.push_back(NE);
                  rad_a             = 0.08 + 0.123 * pow(1.00866491578 * AWRI, (1. / 3.));
                  if (AP == 0.0) AP = rad_a;
                  factor_k          = kconst * (AWRI / (AWRI + 1.0));
                  INT               = 2;
                  amux.push_back(0.0);                                      // total width
                  amun.push_back(list3->GetLIST(2));                        // neutron width
                  amug.push_back(0.0);                                      // gamma width
                  amuf.push_back(list3->GetL2());                           // fission width
                  GJ.push_back((2.0 * list3->GetLIST((1)) + 1.0) / gjdeno); //(2J+1)/2(2I+1)
                  for (int ii = 0; ii < NE; ii++) {                         // line 6 onwards data
                    Es.push_back(list->GetLIST(ii));
                    GF.push_back(list3->GetLIST(6 + ii)); // fission width
                    GG.push_back(list3->GetLIST(4));      // gamma width
                    GX.push_back(0.0);                    // gamma width
                    D.push_back(list3->GetLIST(0));       // J value
                    GNO.push_back(list3->GetLIST(3));     // neutron width
                  }
                } // complete NJS reading
              }
              eLo2 = eLo;
              eHi2 = eHi;
              if (LSSF == 0) Linearize(j);
              l.clear();
              NRS.clear();
              NRJ.clear();
              JSM.clear();
              amux.clear();
              amun.clear();
              amug.clear();
              amuf.clear();
              GJ.clear();
              Es.clear();
              D.clear();
              GX.clear();
              GNO.clear();
              GG.clear();
              GF.clear();
            } break;
            }
          } break;
          }
        } break;
        }
      }
    }
  }
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::recursionLinearFile3(double x1, double x2, double sig1, double sig2, std::vector<double> ene,
                                            std::vector<double> sig)
{
  double siga;
  double mid = 0.5 * (x1 + x2);
  if (sig1 == 0.0) return 0;
  if (sig1 == 0.0 && sig2 == 0.0) return 0;
  if (x1 == x2 || x1 < 1E-5 || x2 < 1E-5) {
    return 0;
  }
  siga           = TNudyCore::Instance()->Interpolate(nbt1, int1, NR, ene, sig, NP, mid);
  double sigmid1 = sig1 + (sig2 - sig1) * (mid - x1) / (x2 - x1);
  if (siga == 0.0 || x1 == x2) return 0;
  //std::cout <<"x \t"<< x1 <<"  \t"<< x2 <<"  \t"<< sig1 <<"  \t"<< sig2 <<"  \t"<< sigmid1 <<"   \t"<< siga << std::endl;
  if (std::fabs((siga - sigmid1) / siga) <= sigDiff) {
    return 0;
  } else {
  //std::cout <<"mid \t"<< x1 <<"  \t"<< x2 <<"  \t"<< sig1 <<"  \t"<< sig2 <<"  \t"<< sigmid1 <<"   \t"<< siga <<"   \t"<< std::fabs((siga - sigmid1) / siga )<< std::endl;
    eLinearFile3.push_back(mid);
    xLinearFile3.push_back(siga);
  }
  recursionLinearFile3(x1, mid, sig1, siga, ene, sig);
  recursionLinearFile3(mid, x2, siga, sig2, ene, sig);
  return 0;
}

//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::addFile3Resonance(double &x1, double &x2, std::vector<double> &x3, std::vector<double> &x4)
{
  int linsize = x3.size();
  int size1   = eLinearFile3.size();
  for (int cr1 = 0; cr1 < linsize; cr1++) {
    if (x3[cr1] >= x1 && x3[cr1] <= x2) {
      double crs = TNudyCore::Instance()->Interpolate(nbt1, int1, NR, eLinearFile3, xLinearFile3, size1, x3[cr1]);
      x4[cr1] += crs;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::insertFile3(std::vector<double> &x1, std::vector<double> &x2)
{
  int size1 = eLinearFile3.size();
  for (int cr1 = 0; cr1 < size1; cr1++) {
    if ((eLinearFile3[cr1] > eLo2 && eLinearFile3[cr1] <= eHi2)) {
      x1.push_back(eLinearFile3[cr1]);
      x2.push_back(xLinearFile3[cr1]);
    }
  }
  return 0;
}

//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::insertFile3High(std::vector<double> &x1, std::vector<double> &x2)
{
  if (flagResolve != 0) {
    for (unsigned long cr = 0; cr < eLinearFile3.size(); cr++) {
      if (xLinearFile3[cr] > 0.0 && eLinearFile3[cr] <= eLo1) {
        x1.push_back(eLinearFile3[cr]);
        x2.push_back(xLinearFile3[cr]);
      }
    }
  } else if (flagResolve == 0 && flagUnResolve != 0) {
    for (unsigned long cr = 0; cr < eLinearFile3.size(); cr++) {
      if (xLinearFile3[cr] > 0.0 && eLinearFile3[cr] <= eLo2) {
        x1.push_back(eLinearFile3[cr]);
        x2.push_back(xLinearFile3[cr]);
      }
    }
  }
  if (flagUnResolve != 0) {
    for (unsigned long cr = 0; cr < eLinearFile3.size(); cr++) {
      if (xLinearFile3[cr] > 0.0 && eLinearFile3[cr] > eHi2) {
        x1.push_back(eLinearFile3[cr]);
        x2.push_back(xLinearFile3[cr]);
      }
    }
  } else if (flagUnResolve == 0) {
    for (unsigned long cr = 0; cr < eLinearFile3.size(); cr++) {
      if (xLinearFile3[cr] > 0.0 && eLinearFile3[cr] > eHi1) {
        x1.push_back(eLinearFile3[cr]);
        x2.push_back(xLinearFile3[cr]);
      }
    }
  }
  return 0;
}

//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::ReadFile3(TNudyEndfFile *file)
{
  TIter secIter(file->GetSections());
  TNudyEndfSec *sec;
  std::vector<double> eneExtra;
  if (LRF == 0) eHi = 0.0;
  double eneLow     = 1E-5;
  do {
    eneExtra.push_back(eneLow);
    eneLow *= 1.15;
  } while (eneLow < 1E3);

  while ((sec = (TNudyEndfSec *)secIter.Next())) {
    int MT = sec->GetMT();
//     if (MT != 1 && MT != 3 && MT != 4 && MT != 5 && MT != 27 && MT != 19 && MT != 20 && MT != 21 && MT != 38 && MT != 101 &&
//         MT < 250) {
//       MtNumbers.push_back(MT);
      TIter recIter(sec->GetRecords());
      TNudyEndfCont *header = (TNudyEndfCont *)recIter.Next();
      ZA    		= sec->GetC1();
      AWR	  	= sec->GetC2();
      MAT		= sec->GetMAT();
      // double AWR  = sec->GetC2();
      // double QM   = header->GetC1();
//       double QI            = header->GetC2();
//       QValue[sec->GetMT()] = QI;
//       qvaluetemp.push_back(QI);
//       qvaluetemp.push_back(MT);
      // int LR = header->GetL2();
      NR = header->GetN1();
      NP = header->GetN2();
//       std::cout<<sec->GetMT() <<" MT "<< NR <<"  "<< NP << std::endl;
      TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)(sec->GetRecords()->At(0));
      for (int cr = 0; cr < NR; cr++) {
        nbt1.push_back(tab1->GetNBT(cr));
        int1.push_back(tab1->GetINT(cr));
        //      TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)(sec->GetRecords()->At(0));
        //   std::cout << tab1->GetNBT(cr) <<" NBT "<< tab1->GetINT(cr)<< std::endl;
        //   std::cout << tab1->GetINT(cr)<< std::endl;
      }
      int npp = 0;
      for (int crs = 0; crs < NP; crs++) {
        eneTemp.push_back(tab1->GetX(crs));
        sigTemp.push_back(tab1->GetY(crs));
	if(prepro == 0){
	  if (npp > 0 && eneTemp[eneTemp.size() - 1] == eneTemp[eneTemp.size() - 2]){
	    eneTemp[eneTemp.size() - 1] += 1E-9; // adding small difference for boundary points
	  }
	  if (npp > 0 && eneTemp[eneTemp.size() - 1] == eneTemp[eneTemp.size() - 3]){
	    eneTemp[eneTemp.size() - 1] += 2E-9; // adding small difference for boundary points
	  }
	  if (npp > 0 && eneTemp[eneTemp.size() - 1] == eneTemp[eneTemp.size() - 4]){
	    eneTemp[eneTemp.size() - 1] += 3E-9; // adding small difference for boundary points
	  }
	}
        npp += 1;
      }
      if(prepro == 0){
	TNudyCore::Instance()->Sort(eneTemp, sigTemp);
	TNudyCore::Instance()->ThinningDuplicate(eneTemp, sigTemp);
      }
      NP = eneTemp.size();
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(eneTemp[crs]);
        xLinearFile3.push_back(sigTemp[crs]);
      }
//         for (unsigned int j= 0 ; j < eLinearFile3.size() ; j++) {
//  	std::cout << eLinearFile3 [j] <<" before linear \t"<< xLinearFile3 [j] << std::endl;
//         }
      // Linearization of file 3 data
      if(prepro == 0){
	for (int cr = 0; cr < NP - 1; cr++) {
	  recursionLinearFile3(eLinearFile3[cr], eLinearFile3[cr + 1], xLinearFile3[cr], xLinearFile3[cr + 1],
			      eLinearFile3, xLinearFile3);
	}
	double enehigh     = 1E+3 ;
	       eneLow      = 1E+3 ;
	int multifac       = 2 ;
	do {
	  eneLow += multifac * enehigh ;
	  eneExtra.push_back(eneLow);
	  multifac += 1;
	} while (eneLow < eneTemp[NP - 1]);

	for (unsigned long ie = 0; ie < eneExtra.size(); ie++) {
	  double sigExtra =
	      TNudyCore::Instance()->Interpolate(nbt1, int1, NR, eLinearFile3, xLinearFile3, NP, eneExtra[ie]);
	  if (sigExtra > 0.0) {
	    eLinearFile3.push_back(eneExtra[ie]);
	    xLinearFile3.push_back(sigExtra);
	  }
	}
//        for (unsigned int j= 0 ; j < eLinearFile3.size() ; j++) {
// 	 std::cout << eLinearFile3 [j] <<" after linear \t"<< xLinearFile3 [j] << std::endl;
//        }
	TNudyCore::Instance()->Sort(eLinearFile3, xLinearFile3);
	TNudyCore::Instance()->ThinningDuplicate(eLinearFile3, xLinearFile3);
      } 
//        for (unsigned int j= 0 ; j < eLinearFile3.size() ; j++) {
// 	std::cout << eLinearFile3 [j] <<"  \t"<< xLinearFile3 [j] << std::endl;
//        }
//	Filling of array to interpolate and add with file 2 data if it is given
	nbt1.clear();
	int1.clear();

	nbt1.push_back(eLinearFile3.size());
	int1.push_back(2);
	NR = 1;
	/////////////////////////////////////////////////////////////////////
//	resolved range is always added in file 3
      if(prepro == 0){	
	if (LRU != 0) {
	  if (flagResolve != 0) {
	    switch (MT) {
	    case 2: {
	      addFile3Resonance(eLo1, eHi1, eLinElastic, xLinElastic);
	    } break;
	    case 18: {
	      addFile3Resonance(eLo1, eHi1, eLinFission, xLinFission);
	    } break;
	    case 102: {
	      addFile3Resonance(eLo1, eHi1, eLinCapture, xLinCapture);
	    } break;
	    }
	  }
	  // unresolved resonance region is added if LSSF = 0
	  if (flagUnResolve != 0) {
	    switch (LSSF) {
	    case 0: {
	      switch (MT) {
	      case 2: {
		addFile3Resonance(eLo2, eHi2, eLinElastic, xLinElastic);
	      } break;
	      case 18: {
		addFile3Resonance(eLo2, eHi2, eLinFission, xLinFission);
	      } break;
	      case 102: {
		addFile3Resonance(eLo2, eHi2, eLinCapture, xLinCapture);
	      } break;
	      }
	    } break;
	    case 1: {
	      switch (MT) {
	      case 2: {
		insertFile3(eLinElastic, xLinElastic);
	      } break;
	      case 18: {
		insertFile3(eLinFission, xLinFission);
	      } break;
	      case 102: {
		insertFile3(eLinCapture, xLinCapture);
	      } break;
	      }
	    } break;
	    }
	  }
	  // This adds additional points to the elastic, fission and capture below resolved range and above uresolved
	  // range
	  switch (MT) {
	  case 2: {
	    insertFile3High(eLinElastic, xLinElastic);
	  } break;
	  case 18: {
	    insertFile3High(eLinFission, xLinFission);
	  } break;
	  case 102: {
	    insertFile3High(eLinCapture, xLinCapture);
	  } break;
	  }
	} else { // no resonance parameters are given
	  switch (MT) {
	  case 2: {
	    for (unsigned long cr = 0; cr < eLinearFile3.size(); cr++) {
	      eLinElastic.push_back(eLinearFile3[cr]);
	      xLinElastic.push_back(xLinearFile3[cr]);
	    }
	  } break;
	  case 18: {
	    for (unsigned long cr = 0; cr < eLinearFile3.size(); cr++) {
	      eLinFission.push_back(eLinearFile3[cr]);
	      xLinFission.push_back(xLinearFile3[cr]);
	    }
	  } break;
	  case 102: {
	    for (unsigned long cr = 0; cr < eLinearFile3.size(); cr++) {
	      eLinCapture.push_back(eLinearFile3[cr]);
	      xLinCapture.push_back(xLinearFile3[cr]);
	    }
	  } break;
	  }
	}
	int NR                = 1;
	if (MT == 2) {
	  header->SetCont(header->GetC1(), header->GetC2(), header->GetL1(), header->GetL2(), NR,
			  (int)eLinElastic.size());
	  tab1->SetNBT((int)eLinElastic.size(), 0);
	  tab1->SetINT(2, 0);
	  for (unsigned int crs = 0; crs < eLinElastic.size(); crs++) {
	    tab1->SetX(eLinElastic[crs], crs);
	    tab1->SetY(xLinElastic[crs], crs);
	  }	  
	} else if (MT == 18) {
	  header->SetCont(header->GetC1(), header->GetC2(), header->GetL1(), header->GetL2(), NR,
			  (int)eLinFission.size());
	  tab1->SetNBT((int)eLinFission.size(), 0);
	  tab1->SetINT(2, 0);
	  for (unsigned int crs = 0; crs < eLinFission.size(); crs++) {
	    tab1->SetX(eLinFission[crs], crs);
	    tab1->SetY(xLinFission[crs], crs);
	  }	  
	} else if (MT == 102) {
	  header->SetCont(header->GetC1(), header->GetC2(), header->GetL1(), header->GetL2(), NR,
			  (int)eLinCapture.size());
	  tab1->SetNBT((int)eLinCapture.size(), 0);
	  tab1->SetINT(2, 0);
	  for (unsigned int crs = 0; crs < eLinCapture.size(); crs++) {
	    tab1->SetX(eLinCapture[crs], crs);
	    tab1->SetY(xLinCapture[crs], crs);
	  }	  
	} else if (MT != 2 && MT != 18 && MT != 102) {
	  header->SetCont(header->GetC1(), header->GetC2(), header->GetL1(), header->GetL2(), NR,
			  (int)eLinearFile3.size());
	  tab1->SetNBT((int)eLinearFile3.size(), 0);
	  tab1->SetINT(2, 0);
	  for (unsigned int crs = 0; crs < eLinearFile3.size(); crs++) {
	    tab1->SetX(eLinearFile3[crs], crs);
	    tab1->SetY(xLinearFile3[crs], crs);
//	    std::cout <<" MT "<< sec->GetMT()<<" energy "<< eLinearFile3 [crs] <<" sigma "<< xLinearFile3 [crs] << std::endl;
	  }
	}
	
// 	std::cout <<" proton flag \t" <<  MTChargeFlag [0] <<" proton flag \t" <<  MTChargeFlag [5] <<" MT \t" <<  MT << std::endl;
	// if data are given only in MT = 600-850
	if(MTChargeFlag [0] != -1 && (MT >= 600 && MT < 650)){
       //  summing for n,p reaction
// 	  std::cout<<" first case0 != -1 MT= 600-649 "<< std::endl; 
	  eneUniP.insert (std::end(eneUniP), std::begin(eLinearFile3), std::end(eLinearFile3));
	  eneUniPAll.insert (std::end(eneUniPAll), std::begin(eLinearFile3), std::end(eLinearFile3));	 
	} else if (MTChargeFlag [1] != -1 && MT >= 650 && MT < 700){
       //  summing for n,d reaction
// 	  std::cout<<" first case0 != -1 MT= 650-700 "<< std::endl; 
	  eneUniD.insert (std::end(eneUniD), std::begin(eLinearFile3), std::end(eLinearFile3)); 
	  eneUniDAll.insert (std::end(eneUniDAll), std::begin(eLinearFile3), std::end(eLinearFile3)); 
	} else if (MTChargeFlag [2] != -1 && MT >= 700 && MT < 750){
       //  summing for n,t reaction
// 	  std::cout<<" first case0 != -1 MT= 700-749 "<< std::endl; 
	  eneUniT.insert (std::end(eneUniT), std::begin(eLinearFile3), std::end(eLinearFile3)); 
	  eneUniTAll.insert (std::end(eneUniTAll), std::begin(eLinearFile3), std::end(eLinearFile3)); 
	} else if (MTChargeFlag [3] != -1 && MT >= 750 && MT < 800){
       //  summing for n,He3 reaction
// 	  std::cout<<" first case0 != -1 MT= 749-800 "<< std::endl; 
	  eneUniHe3.insert (std::end(eneUniHe3), std::begin(eLinearFile3), std::end(eLinearFile3)); 
	  eneUniHe3All.insert (std::end(eneUniHe3All), std::begin(eLinearFile3), std::end(eLinearFile3)); 
	} else if (MTChargeFlag [4] != -1 && MT >= 800 && MT < 850){
       //  summing for n,He4 reaction
// 	  std::cout<<" first case0 != -1 MT= 800-849 "<< std::endl; 
	  eneUniHe4.insert (std::end(eneUniHe4), std::begin(eLinearFile3), std::end(eLinearFile3)); 
	  eneUniHe4All.insert (std::end(eneUniHe4All), std::begin(eLinearFile3), std::end(eLinearFile3)); 
	} else if(MT == 28 || (MT >= 41 && MT < 46) || MT == 111 || MT == 112 || MT == 115 || MT == 116 || MT == 103){
       //  summing for n,px reaction
	  eneUniPAll.insert (std::end(eneUniPAll), std::begin(eLinearFile3), std::end(eLinearFile3));	 
	} else if(MT == 11 ||MT == 32 || MT == 35 || MT == 114 || MT == 115 || MT == 117 || MT == 104){
       //  summing for n,dx reaction
	  eneUniDAll.insert (std::end(eneUniDAll), std::begin(eLinearFile3), std::end(eLinearFile3));	 
	} else if(MT == 33 || MT == 113 || MT == 116 || MT == 105){
       //  summing for n,tx reaction
	  eneUniTAll.insert (std::end(eneUniTAll), std::begin(eLinearFile3), std::end(eLinearFile3));	 
	} else if(MT == 34 || MT == 113 || MT == 116 || MT == 106){
       //  summing for n,He3x reaction
	  eneUniHe3All.insert (std::end(eneUniHe3All), std::begin(eLinearFile3), std::end(eLinearFile3));	 
	} else if((MT >= 22 && MT < 26 ) || MT == 29 || MT == 30 || MT == 36 || MT == 36 || MT == 45 
	           || MT == 108 || MT == 109 || (MT >= 112 && MT < 115 ) || MT == 117 || MT == 107){
       //  summing for n,He4x reaction
	  eneUniHe4All.insert (std::end(eneUniHe4All), std::begin(eLinearFile3), std::end(eLinearFile3));	 
	}
//	check the sum of cross sections with total cross-section given in the MT=1 + resonance
//	    int size = eLinearFile3.size();
	if (MT == 2) {
	  sigmaMts.insert(std::end(sigmaMts), std::begin(eLinElastic), std::end(eLinElastic));
	  sigmaMts.insert(std::end(sigmaMts), std::begin(xLinElastic), std::end(xLinElastic));
// 	  energyUni.insert(std::end(energyUni), std::begin(eLinElastic), std::end(eLinElastic));
	} else if (MT == 18) {
	  sigmaMts.insert(std::end(sigmaMts), std::begin(eLinFission), std::end(eLinFission));
	  sigmaMts.insert(std::end(sigmaMts), std::begin(xLinFission), std::end(xLinFission));
// 	  energyUni.insert(std::end(energyUni), std::begin(eLinFission), std::end(eLinFission));
	} else if (MT == 102) {
	  sigmaMts.insert(std::end(sigmaMts), std::begin(eLinCapture), std::end(eLinCapture));
	  sigmaMts.insert(std::end(sigmaMts), std::begin(xLinCapture), std::end(xLinCapture));
// 	  energyUni.insert(std::end(energyUni), std::begin(eLinCapture), std::end(eLinCapture));
	} else if (MT != 2 && MT != 18 && MT != 102) {
	  sigmaMts.insert(std::end(sigmaMts), std::begin(eLinearFile3), std::end(eLinearFile3));
	  sigmaMts.insert(std::end(sigmaMts), std::begin(xLinearFile3), std::end(xLinearFile3));
// 	  energyUni.insert(std::end(energyUni), std::begin(eLinearFile3), std::end(eLinearFile3));
	}
      }
      if(prepro == 1){
	if (MT == 2) {
	  eLinElastic.insert(std::end(eLinElastic), std::begin(eLinearFile3), std::end(eLinearFile3));
	  xLinElastic.insert(std::end(xLinElastic), std::begin(xLinearFile3), std::end(xLinearFile3));
	} else if (MT == 18) {
	  eLinFission.insert(std::end(eLinFission), std::begin(eLinearFile3), std::end(eLinearFile3));
	  xLinFission.insert(std::end(xLinFission), std::begin(xLinearFile3), std::end(xLinearFile3));
	} else if (MT == 102) {
	  eLinCapture.insert(std::end(eLinCapture), std::begin(eLinearFile3), std::end(eLinearFile3));
	  xLinCapture.insert(std::end(xLinCapture), std::begin(xLinearFile3), std::end(xLinearFile3));
        }
        sigmaMts.insert(std::end(sigmaMts), std::begin(eLinearFile3), std::end(eLinearFile3));
        sigmaMts.insert(std::end(sigmaMts), std::begin(xLinearFile3), std::end(xLinearFile3));
//         energyUni.insert(std::end(energyUni), std::begin(eLinearFile3), std::end(eLinearFile3));
      }
      eLinearFile3.clear();
      xLinearFile3.clear();
      eneTemp.clear();
      sigTemp.clear();
      nbt1.clear();
      int1.clear();
      sigmaOfMts.push_back(sigmaMts);
      sigmaMts.clear();
//     }
  }
  // sorting and removing duplicate points
  std::sort(eneUniP.begin(), eneUniP.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniP);
  
  std::sort(eneUniD.begin(), eneUniD.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniD);
  
  std::sort(eneUniT.begin(), eneUniT.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniT);
  
  std::sort(eneUniHe3.begin(), eneUniHe3.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniHe3);
  
  std::sort(eneUniHe4.begin(), eneUniHe4.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniHe4);
  
  std::sort(eneUniPAll.begin(), eneUniPAll.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniPAll);
  
  std::sort(eneUniDAll.begin(), eneUniDAll.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniDAll);
  
  std::sort(eneUniTAll.begin(), eneUniTAll.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniTAll);
  
  std::sort(eneUniHe3All.begin(), eneUniHe3All.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniHe3All);
  
  std::sort(eneUniHe4All.begin(), eneUniHe4All.end());
  TNudyCore::Instance()->ThinningDuplicate(eneUniHe4All);
  
  // summing cross-section for making total from ground and excited states for n,p
  TIter secIter1(file->GetSections());
  TNudyEndfSec *sec1;
  while ((sec1 = (TNudyEndfSec *)secIter1.Next())) {
    int MT = sec1->GetMT();
//     std::cout<<" MT "<< MT << std::endl;
    TIter recIter1(sec1->GetRecords());
    TNudyEndfCont *header = (TNudyEndfCont *)recIter1.Next();
    NR = header->GetN1();
    NP = header->GetN2();
    TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)(sec1->GetRecords()->At(0));
    if((MTChargeFlag [0] != -1 && MT >= 600 && MT < 650)/* || (MTChargeFlag [5] != -1 && MT == 103)
        || (MTChargeFlag [0] == -1 && MTChargeFlag [5] == -1 && MT == 103)*/ ) {
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab1->GetX(crs));
        xLinearFile3.push_back(tab1->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniP.size(); k++) {
	int min = BinarySearch(eneUniP[k], eLinearFile3);
	if (eneUniP[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniP[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniP[k] - eLinearFile3[min]) /
			    (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	  if (sigmaAdd > 1E-20) {
	    eneTemp.push_back(eneUniP[k]);
	    sigTemp.push_back(sigmaAdd);
	  }
	}
	if (eneTemp.size() == 1) {
	  eneLocP.push_back(k);
	}
      }
      sigUniOfP.push_back(sigTemp);
    } else if((MTChargeFlag [1] != -1 && MT >= 650 && MT < 700)/* || (MTChargeFlag [6] != -1 && MT == 104)
        || (MTChargeFlag [1] == -1 && MTChargeFlag [6] == -1 && MT == 104)*/) {
     for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab1->GetX(crs));
        xLinearFile3.push_back(tab1->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniD.size(); k++) {
	int min = BinarySearch(eneUniD[k], eLinearFile3);
	if (eneUniD[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniD[k]);
	  sigTemp.push_back(xLinearFile3[min]);
 	} else {
	  double sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniD[k] - eLinearFile3[min]) /
			    (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    eneTemp.push_back(eneUniD[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocD.push_back(k);
	}
      }
      sigUniOfD.push_back(sigTemp);
    } else if((MTChargeFlag [2] != -1 && MT >= 700 && MT < 750)/* || (MTChargeFlag [7] != -1 && MT == 105)
        || (MTChargeFlag [2] == -1 && MTChargeFlag [7] == -1 && MT == 105)*/ ) {
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab1->GetX(crs));
        xLinearFile3.push_back(tab1->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniT.size(); k++) {
	
	int min = BinarySearch(eneUniT[k], eLinearFile3);
	if (eneUniT[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniT[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniT[k] - eLinearFile3[min]) /
			    (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    eneTemp.push_back(eneUniT[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocT.push_back(k);
	}
      }
      sigUniOfT.push_back(sigTemp);
     } else if((MTChargeFlag [3] != -1 && MT >= 750 && MT < 800)/* || (MTChargeFlag [8] != -1 && MT == 106)
        || (MTChargeFlag [3] == -1 && MTChargeFlag [8] == -1 && MT == 106)*/ ) {
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab1->GetX(crs));
        xLinearFile3.push_back(tab1->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniHe3.size(); k++) {
	
	int min = BinarySearch(eneUniHe3[k], eLinearFile3);
	if (eneUniHe3[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniHe3[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniHe3[k] - eLinearFile3[min]) /
			    (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    eneTemp.push_back(eneUniHe3[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocHe3.push_back(k);
	}
      }
      sigUniOfHe3.push_back(sigTemp);
    } else if((MTChargeFlag [4] != -1 && MT >= 800 && MT < 850)/* || (MTChargeFlag [9] != -1 && MT == 107)
        || (MTChargeFlag [4] == -1 && MTChargeFlag [9] == -1 && MT == 107)*/ ) {
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab1->GetX(crs));
        xLinearFile3.push_back(tab1->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniHe4.size(); k++) {
	
	int min = BinarySearch(eneUniHe4[k], eLinearFile3);
	if (eneUniHe4[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniHe4[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniHe4[k] - eLinearFile3[min]) /
			    (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    eneTemp.push_back(eneUniHe4[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocHe4.push_back(k);
	}
      }
      sigUniOfHe4.push_back(sigTemp);
    }
    eneTemp.clear();
    sigTemp.clear();
    eLinearFile3.clear();
    xLinearFile3.clear();    
  }
//	Adding charge particle reaction cross-sections  
  sigUniP.resize(eneUniP.size());
  for (unsigned long i = 0; i < sigUniOfP.size(); i++) {
    int size = sigUniOfP[i].size();
    for (int j = 0; j < size; j++) {
      sigUniP[eneLocP[i] + j] += sigUniOfP[i][j];
    }
  }
  sigUniD.resize(eneUniD.size());
  for (unsigned long i = 0; i < sigUniOfD.size(); i++) {
    int size = sigUniOfD[i].size();
    for (int j = 0; j < size; j++) {
      sigUniD[eneLocD[i] + j] += sigUniOfD[i][j];
    }
  }
  sigUniT.resize(eneUniT.size());
  for (unsigned long i = 0; i < sigUniOfT.size(); i++) {
    int size = sigUniOfT[i].size();
    for (int j = 0; j < size; j++) {
      sigUniT[eneLocT[i] + j] += sigUniOfT[i][j];
    }
  }
  sigUniHe3.resize(eneUniHe3.size());
  for (unsigned long i = 0; i < sigUniOfHe3.size(); i++) {
    int size = sigUniOfHe3[i].size();
    for (int j = 0; j < size; j++) {
      sigUniHe3[eneLocHe3[i] + j] += sigUniOfHe3[i][j];
    }
  }
  sigUniHe4.resize(eneUniHe4.size());
  for (unsigned long i = 0; i < sigUniOfHe4.size(); i++) {
    int size = sigUniOfHe4[i].size();
    for (int j = 0; j < size; j++) {
      sigUniHe4[eneLocHe4[i] + j] += sigUniOfHe4[i][j];
    }
  }
  TIter secIter2(file->GetSections());
  TNudyEndfSec *sec2;
  double QM[5] = {0,0,0,0,0};
  double QI[5] = {0,0,0,0,0};
  while ((sec2 = (TNudyEndfSec *)secIter2.Next())) {
    TIter recIter2(sec2->GetRecords());
    TNudyEndfCont *header2 = (TNudyEndfCont *)recIter2.Next();
    int MT 		= sec2->GetMT();
    if (MT == 600) {QM[0]  = header2->GetC1(); QI[0]   = header2->GetC2();}
    if (MT == 650) {QM[1]  = header2->GetC1(); QI[1]   = header2->GetC2();}
    if (MT == 700) {QM[2]  = header2->GetC1(); QI[2]   = header2->GetC2();}
    if (MT == 750) {QM[3]  = header2->GetC1(); QI[3]   = header2->GetC2();}
    if (MT == 800) {QM[4]  = header2->GetC1(); QI[4]   = header2->GetC2();}
  }
  if (MTChargeFlag[0] !=-1 && MTChargeFlag[5] ==-1) {
    AddSecFile3 (file, QM[0], QI[0], 103, eneUniP, sigUniP);  
  }
  if (MTChargeFlag[1] !=-1 && MTChargeFlag[6] ==-1) {
    AddSecFile3 (file, QM[1], QI[1], 104, eneUniD, sigUniD);  
  }
  if (MTChargeFlag[2] !=-1 && MTChargeFlag[7] ==-1) {
    AddSecFile3 (file, QM[2], QI[2], 105, eneUniT, sigUniT);  
  }
  if (MTChargeFlag[3] !=-1 && MTChargeFlag[8] ==-1) {
    AddSecFile3 (file, QM[3], QI[3], 106, eneUniHe3, sigUniHe3);  
  }
  if (MTChargeFlag[4] !=-1 && MTChargeFlag[9] ==-1) {
    AddSecFile3 (file, QM[4], QI[4], 107, eneUniHe4, sigUniHe4);  
  }

// Total charge production for the gas production cross-section
  TIter secIter0(file->GetSections());
  TNudyEndfSec *sec0;
  while ((sec0 = (TNudyEndfSec *)secIter0.Next())) {
    int MT = sec0->GetMT();
    TIter recIter0(sec0->GetRecords());
    TNudyEndfCont *header0 = (TNudyEndfCont *)recIter0.Next();
    NR = header0->GetN1();
    NP = header0->GetN2();
    TNudyEndfTab1 *tab0 = (TNudyEndfTab1 *)(sec0->GetRecords()->At(0));
//         for (int crs = 0; crs < NP; crs++) {
//          std::cout<<"MT "<< MT <<"  "<< tab0->GetX(crs) <<"  "<< tab0->GetY(crs) << std::endl;
//        }      
   if(MT == 103 || MT == 28 || (MT >= 41 && MT < 46) || MT == 111 || MT == 112 || MT == 115 || MT == 116  ) {
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab0->GetX(crs));
        xLinearFile3.push_back(tab0->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniPAll.size(); k++) {
	int min = BinarySearch(eneUniPAll[k], eLinearFile3);
	if (eneUniPAll[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniPAll[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = 0;
	    if(eneUniPAll[k] > eLinearFile3[min]){
	      sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniPAll[k] - eLinearFile3[min]) /
			      (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    }
	    eneTemp.push_back(eneUniPAll[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocPAll.push_back(k);
	}
      }
      sigUniOfPAll.push_back(sigTemp);
    } else if(MT == 11 || MT == 32 || MT == 35 || MT == 114 || MT == 115 || MT == 117 || MT == 104) {      
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab0->GetX(crs));
        xLinearFile3.push_back(tab0->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniDAll.size(); k++) {
	
	int min = BinarySearch(eneUniDAll[k], eLinearFile3);
	if (eneUniDAll[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniDAll[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = 0;
	    if(eneUniDAll[k] > eLinearFile3[min]){
	      sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniDAll[k] - eLinearFile3[min]) /
			      (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    }
	    eneTemp.push_back(eneUniDAll[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocDAll.push_back(k);
	}
      }
      sigUniOfDAll.push_back(sigTemp);
    } else if(MT == 33 || MT == 113 || MT == 116 || MT == 105) {
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab0->GetX(crs));
        xLinearFile3.push_back(tab0->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniTAll.size(); k++) {
	
	int min = BinarySearch(eneUniTAll[k], eLinearFile3);
	if (eneUniTAll[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniTAll[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = 0;
	    if(eneUniTAll[k] > eLinearFile3[min]){
	    sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniTAll[k] - eLinearFile3[min]) /
			      (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    }
	    eneTemp.push_back(eneUniTAll[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocTAll.push_back(k);
	}
      }
      sigUniOfTAll.push_back(sigTemp);
     } else if(MT == 34 || MT == 113 || MT == 116 || MT == 106) {
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab0->GetX(crs));
        xLinearFile3.push_back(tab0->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniHe3All.size(); k++) {
	
	int min = BinarySearch(eneUniHe3All[k], eLinearFile3);
	if (eneUniHe3All[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniHe3All[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = 0;
	    if(eneUniHe3All[k] > eLinearFile3[min]){
	    sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniHe3All[k] - eLinearFile3[min]) /
			      (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    }
	    eneTemp.push_back(eneUniHe3All[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocHe3All.push_back(k);
	}
      }
      sigUniOfHe3All.push_back(sigTemp);
    } else if((MT >= 22 && MT < 26 ) || MT == 29 || MT == 30 || MT == 36 || MT == 36 || MT == 45 
	           || MT == 108 || MT == 109 || (MT >= 112 && MT < 115 ) || MT == 117  || MT == 107) {
      for (int crs = 0; crs < NP; crs++) {
        eLinearFile3.push_back(tab0->GetX(crs));
        xLinearFile3.push_back(tab0->GetY(crs));
      }      
      for (unsigned long k = 0; k < eneUniHe4All.size(); k++) {
	
	int min = BinarySearch(eneUniHe4All[k], eLinearFile3);
	if (eneUniHe4All[k] == eLinearFile3[min]) {
	  eneTemp.push_back(eneUniHe4All[k]);
	  sigTemp.push_back(xLinearFile3[min]);
	} else {
	  double sigmaAdd = 0;
	    if(eneUniHe4All[k] > eLinearFile3[min]){
	    sigmaAdd = xLinearFile3[min] + (xLinearFile3[min + 1] - xLinearFile3[min]) * (eneUniHe4All[k] - eLinearFile3[min]) /
			      (eLinearFile3[min + 1] - eLinearFile3[min]); // linear interpolation
	    }
	    eneTemp.push_back(eneUniHe4All[k]);
	    sigTemp.push_back(sigmaAdd);
	}
	if (eneTemp.size() == 1) {
	  eneLocHe4All.push_back(k);
	}
      }
      sigUniOfHe4All.push_back(sigTemp);
    }
    eneTemp.clear();
    sigTemp.clear();
    eLinearFile3.clear();
    xLinearFile3.clear();
  }
//	Adding all charge particle reaction cross-sections  
  sigUniPAll.resize(eneUniPAll.size());
  for (unsigned long i = 0; i < sigUniOfPAll.size(); i++) {
    int size = sigUniOfPAll[i].size();
    for (int j = 0; j < size; j++) {
      sigUniPAll[eneLocPAll[i] + j] += sigUniOfPAll[i][j];
    }
  }
  sigUniDAll.resize(eneUniDAll.size());
  for (unsigned long i = 0; i < sigUniOfDAll.size(); i++) {
    int size = sigUniOfDAll[i].size();
    for (int j = 0; j < size; j++) {
      sigUniDAll[eneLocDAll[i] + j] += sigUniOfDAll[i][j];
    }
  }
  sigUniTAll.resize(eneUniTAll.size());
  for (unsigned long i = 0; i < sigUniOfTAll.size(); i++) {
    int size = sigUniOfTAll[i].size();
    for (int j = 0; j < size; j++) {
      sigUniTAll[eneLocTAll[i] + j] += sigUniOfTAll[i][j];
    }
  }
  sigUniHe3All.resize(eneUniHe3All.size());
  for (unsigned long i = 0; i < sigUniOfHe3All.size(); i++) {
    int size = sigUniOfHe3All[i].size();
    for (int j = 0; j < size; j++) {
      sigUniHe3All[eneLocHe3All[i] + j] += sigUniOfHe3All[i][j];
    }
  }
  sigUniHe4All.resize(eneUniHe4All.size());
  for (unsigned long i = 0; i < sigUniOfHe4All.size(); i++) {
    int size = sigUniOfHe4All[i].size();
    for (int j = 0; j < size; j++) {
      sigUniHe4All[eneLocHe4All[i] + j] += sigUniOfHe4All[i][j];
    }
  }
// Adding new MF for charge particles (p, d, t, He3, He4)
  
  if ((MTChargeFlag[0] ==-1 || MTChargeFlag[5] ==-1)) {
    AddSecFile3 (file, 0, 0, 203, eneUniPAll, sigUniPAll);  
  }
  if ((MTChargeFlag[1] ==-1 || MTChargeFlag[6] ==-1)) {
    AddSecFile3 (file, 0, 0, 204, eneUniDAll, sigUniDAll);  
  }
  if ((MTChargeFlag[2] ==-1 || MTChargeFlag[7] ==-1)) {
    AddSecFile3 (file, 0, 0, 205, eneUniTAll, sigUniTAll);  
  }
  if ((MTChargeFlag[3] ==-1 || MTChargeFlag[8] ==-1)) {
    AddSecFile3 (file, 0, 0, 206, eneUniHe3All, sigUniHe3All);  
  }
  if ((MTChargeFlag[4] ==-1 || MTChargeFlag[9] ==-1)) {
    AddSecFile3 (file, 0, 0, 207, eneUniHe4All, sigUniHe4All);  
  }

  TIter secIter20(file->GetSections());
  TNudyEndfSec *sec20;
  while ((sec20 = (TNudyEndfSec *)secIter20.Next())) {
    MtNumbers.push_back(sec20->GetMT());
//       std::cout<<"MT push "<< sec20->GetMT() << std::endl;
    TIter recIter21(sec20->GetRecords());
    TNudyEndfCont *header21 = (TNudyEndfCont *)recIter21.Next();
    qvaluetemp.push_back(header21->GetC2());
    qvaluetemp.push_back(sec20->GetMT());
  }

  /*  
  TIter secIterx(file->GetSections());
  TNudyEndfSec *secx;
  while ((secx = (TNudyEndfSec *)secIterx.Next())) {
    int MT = secx->GetMT();
    TIter recItery(secx->GetRecords());
    TNudyEndfCont *headerx = (TNudyEndfCont *)recItery.Next();
    NR = headerx->GetN1();
    NP = headerx->GetN2();
    std::cout<<"MT "<< MT <<" NP "<< NP <<"  "<< headerx->GetC1()<<"  "<< headerx->GetC2()<<"  "<< headerx->GetL1()<<"  "<< headerx->GetL2()
    <<"  "<< headerx->GetN1()<<"  "<< headerx->GetN2()<< std::endl;
    TNudyEndfTab1 *tabx = (TNudyEndfTab1 *)(secx->GetRecords()->At(0));
       for (int crs = 0; crs < NP; crs++) {
        std::cout<<"MTX "<< MT<<" NP "<< NP <<"  "<< tabx->GetX(crs) <<"  "<< tabx->GetY(crs) << std::endl;
      }      
  }  */
// fill sigmaMts
  TIter secIter22(file->GetSections());
  TNudyEndfSec *sec22;
  while ((sec22 = (TNudyEndfSec *)secIter22.Next())) {
      TIter recIter23(sec22->GetRecords());
      TNudyEndfCont *header23 = (TNudyEndfCont *)recIter23.Next();
      if((sec22->GetMT() >= 103 && sec22->GetMT() < 108) || (sec22->GetMT() >= 203 && sec22->GetMT() < 208)){
	if(MTChargeFlag[0] == -1 && sec22->GetMT() == 103)continue;  
	if(MTChargeFlag[1] == -1 && sec22->GetMT() == 104)continue;  
	if(MTChargeFlag[2] == -1 && sec22->GetMT() == 105)continue;  
	if(MTChargeFlag[3] == -1 && sec22->GetMT() == 106)continue;  
	if(MTChargeFlag[4] == -1 && sec22->GetMT() == 107)continue;  
	NP = header23->GetN2();
	TNudyEndfTab1 *tab23 = (TNudyEndfTab1 *)(sec22->GetRecords()->At(0));
	for (int crs = 0; crs < NP; crs++) {
	  eLinearFile3.push_back(tab23->GetX(crs));
	  xLinearFile3.push_back(tab23->GetY(crs));
//  	  std::cout <<" MT "<< sec22->GetMT()<<" energy "<< eLinearFile3 [crs] <<" sigma "<< xLinearFile3 [crs] << std::endl;
	}
	sigmaMts.insert(std::end(sigmaMts), std::begin(eLinearFile3), std::end(eLinearFile3));
	sigmaMts.insert(std::end(sigmaMts), std::begin(xLinearFile3), std::end(xLinearFile3));
	eLinearFile3.clear();
	xLinearFile3.clear();
	sigmaOfMts.push_back(sigmaMts);
	sigmaMts.clear();
      }
  } 
  qvalue.push_back(qvaluetemp);
  qvaluetemp.clear();
// empty vectors
  sigUniOfP.clear();
  eneLocP.clear();
  sigUniOfD.clear();
  eneLocD.clear();
  sigUniOfT.clear();
  eneLocT.clear();
  sigUniOfHe3.clear();
  eneLocHe3.clear();
  sigUniOfHe4.clear();
  eneLocHe4.clear();
  sigUniOfPAll.clear();
  eneLocPAll.clear();
  sigUniOfDAll.clear();
  eneLocDAll.clear();
  sigUniOfTAll.clear();
  eneLocTAll.clear();
  sigUniOfHe3All.clear();
  eneLocHe3All.clear();
  sigUniOfHe4All.clear();
  eneLocHe4All.clear();
}
int TNudyEndfSigma::BinarySearch(double x1, rowd &x2){
    int min = 0;
    int size = x2.size();
    min = 0;
    int max = size - 1;
    int mid = 0;
    if (x1 <= x2[min])
      min = 0;
    else if (x1 >= x2[max])
      min = max - 1;
    else {
      while (max - min > 1) {
	mid = (min + max) / 2;
	if (x1 < x2[mid])
	  max = mid;
	else
	  min = mid;
      }
    }
    return min;
}
// Adding new MF for charge particles
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::AddSecFile3(TNudyEndfFile *file1,
				 double a, 
				 double b, 
				 int MF2Add, 
				 rowd &x1, 
				 rowd &x2){
  
  TNudyEndfSec *sec3 = new TNudyEndfSec(MAT, 3, MF2Add, ZA, AWRI,0,0,0,0);
//  TNudyEndfCont *secCont = new TNudyEndfCont (a,b,0,0,1,(int)x1.size());
//  sec3->Add(secCont);
  TNudyEndfTab1 *sec3Tab1 = new TNudyEndfTab1();
  sec3Tab1->SetCont(a,b,0,0,1,(int)x1.size());
  sec3Tab1->SetNBT((int)x1.size(), 0);
  sec3Tab1->SetINT(2, 0);
  for (unsigned int j = 0; j < x1.size(); ++j) {
    sec3Tab1->SetX(x1[j], j);
    sec3Tab1->SetY(x2[j], j);
//     std::cout << "x2 " << x2[j] << std::endl;
  }
  sec3->Add(sec3Tab1);
//   TNudyEndfCont *secCont1 = new TNudyEndfCont (0,0,0,0,0,0);
//   secCont1->SetContMF(MAT,MF2Add,0);
//   sec3->Add(secCont1);
  file1->Add(sec3);
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::K_wnum(double x)
{
  double k;
  k = factor_k * sqrt(std::fabs(x));
  return k;
}

//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::GetRho(double x1, int lVal)
{
  if (!NAPS && !NRO)
    return factor_k * sqrt(std::fabs(x1)) * rad_a; // Use rad_a in the penetrabilities Pl and shift factors Sl , and AP
                                                   // in the hard-sphere phase shifts φl
  if (!NAPS && NRO)
    return factor_k * sqrt(std::fabs(x1)) *
           rad_a; // Use rad_a in the penetrabilities Pl and shift factors Sl,AP(E) in the hard-sphere phase shifts φl
  if (NAPS && !NRO)
    return factor_k * sqrt(std::fabs(x1)) *
           APL[lVal]; // use AP in the penetrabilities and shift factor as well as in the phase shifts
  if (NAPS && NRO)
    return factor_k * sqrt(std::fabs(x1)) * APL[lVal]; // read AP(E) and use it in all three places, Pl , Sl , φl
  return 0;
}

//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::GetRhoC(double x, int isDiff, int lVal)
{
  if (!isDiff)
    return factor_k * sqrt(std::fabs(x)) * APL[lVal]; // read AP(E) and use it in all three places, Pl , Sl , φl
  if (isDiff)
    return factor_k * sqrt(std::fabs(x)) * APL[lVal]; // read AP(E) and use it in all three places, Pl , Sl , φl
  return 0;
}

//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::calcPhi(double x1, int l)
{
  double x = GetRhoC(x1, 0, l);
  switch (l) {
  case 0:
    return x;
  case 1:
    return (x - atan(x));
  case 2:
    return (x - atan(3.0 * x / (3.0 - x2(x))));
  case 3:
    return (x - atan(x * (15.0 - x * x) / (15.0 - 6.0 * x2(x))));
  case 4:
    return (x - atan(x * (105.0 - 10.0 * x2(x)) / (105.0 - 45.0 * x2(x) + x4(x))));
  }
  return 0;
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::calcShift(double x1, int l)
{
  double x = GetRho(x1, l);
  switch (l) {
  case 0:
    return 0.0;
  case 1:
    return (-1.0 / (1.0 + x * x));
  case 2:
    return (-(18.0 + 3.0 * x2(x)) / Fac2(x));
  case 3:
    return (-(675.0 + 90.0 * x2(x) + 6.0 * x4(x)) / (x6(x) + 6.0 * x4(x) + 45.0 * x2(x) + 225.0));
  case 4:
    return (-(44100.0 + 4725.0 * x2(x) + 270.0 * x4(x) + 10.0 * x6(x)) /
            (x8(x) + 10.0 * x6(x) + 135.0 * x4(x) + 1575.0 * x2(x) + 11025.0));
  }
  return 0;
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::calcPene(double x1, int l)
{
  double x = GetRho(x1, l);
  switch (l) {
  case 0:
    return x;
  case 1:
    return (x * x * x) / (1.0 + x * x);
  case 2:
    return (x5(x) / Fac2(x));
  case 3:
    return ((x * x6(x)) / (225.0 + 45.0 * x2(x) + 6.0 * x4(x) + x6(x)));
  case 4:
    return ((x * x8(x)) / (x8(x) + 10.0 * x6(x) + 135.0 * x4(x) + 1575.0 * x2(x) + 11025.0));
  }
  return 0;
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::GetERP(double x, int r, int lVal)
{
  double er = Er[r];
  if (lVal == 0) return er;
  return er + (ShiftEr[r] - calcShift(x, lVal)) / (2.0 * PhiEr[r]) * Gamma_nrE(std::fabs(er), r, lVal);
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::Gamma_nrE(double x, int ii, int lval)
{
  return calcPene(x, lval) * Gamma_n[ii];
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::Gamma_xrE(int ii, int lrx)
{
  if (!lrx) {
    return 0;
  } else {
    return Gamma_r[ii] - (Gamma_n[ii] + Gamma_g[ii] + Gamma_f[ii]);
  }
}
//------------------------------------------------------------------------------------------------------

double TNudyEndfSigma::Gamma_rE(double x, int ii, int lval, int lrx)
{
  return Gamma_nrE(x, ii, lval) + Gamma_xrE(ii, lrx) + Gamma_g[ii] + Gamma_f[ii];
}
// MC2 formalism is inspired from M. Kalki
//------------------------------------------------------------------------------------------------------
int TNudyEndfSigma::widthFluctuation(double GNR, double gx, double gg, double gf, int jval)
{
  double XX[10][5] = {{3.0013465E-03, 1.3219203E-02, 1.0004488E-03, 1.3219203E-02, 1.0E+0},
                      {7.8592886E-02, 7.2349624E-02, 2.6197629E-02, 7.2349624E-02, 0.0E+0},
                      {4.3282415E-01, 1.9089473E-01, 1.4427472E-01, 1.9089473E-01, 0.0E+0},
                      {1.3345267E+00, 3.9528842E-01, 4.4484223E-01, 3.9528842E-01, 0.0E+0},
                      {3.0481846E+00, 7.4083443E-01, 1.0160615E+00, 7.4083443E-01, 0.0E+0},
                      {5.8263198E+00, 1.3498293E+00, 1.9421066E+00, 1.3498293E+00, 0.0E+0},
                      {9.9452656E+00, 2.5297983E+00, 3.3150885E+00, 2.5297983E+00, 0.0E+0},
                      {1.5782128E+01, 5.2384894E+00, 5.2607092E+00, 5.2384894E+00, 0.0E+0},
                      {2.3996824E+01, 1.3821772E+01, 7.9989414E+00, 1.3821772E+01, 0.0E+0},
                      {3.6216208E+01, 7.5647525E+01, 1.2072069E+01, 7.5647525E+01, 0.0E+0}};

  double WW[10][5] = {{1.1120413E-01, 3.3773418E-02, 3.3376214E-04, 1.7623788E-03, 1.0E+0},
                      {2.3546798E-01, 7.9932171E-02, 1.8506108E-02, 2.1517749E-02, 0.0E+0},
                      {2.8440987E-01, 1.2835937E-01, 1.2309946E-01, 8.0979849E-02, 0.0E+0},
                      {2.2419127E-01, 1.7652616E-01, 2.9918923E-01, 1.8797998E-01, 0.0E+0},
                      {1.0967668E-01, 2.1347043E-01, 3.3431475E-01, 3.0156335E-01, 0.0E+0},
                      {3.0493789E-02, 2.1154965E-01, 1.7766657E-01, 2.9616091E-01, 0.0E+0},
                      {4.2930874E-03, 1.3365186E-01, 4.2695894E-02, 1.0775649E-01, 0.0E+0},
                      {2.5827047E-04, 2.2630659E-02, 4.0760575E-03, 2.5171914E-03, 0.0E+0},
                      {4.9031965E-06, 1.6313638E-05, 1.1766115E-04, 8.9630388E-10, 0.0E+0},
                      {1.4079206E-08, 0.0000000E+00, 5.0989546E-07, 0.0000000E+00, 0.0E+0}};

  RN      = 0.0;
  RG      = 0.0;
  RF      = 0.0;
  int mux = (int)amux[jval];
  int mun = (int)amun[jval];
  int muf = (int)amuf[jval];  
  if (GNR <= 0.0 || gg <= 0.0) {
    return 0;
  }
  if (mun < 1 || mun > 4 ) mun = 5 ;
  if (muf < 1 || muf > 4 ) muf = 5 ;
  if (mux < 1 || mux > 4 ) mux = 5 ;
  if (gf < 0.0) {
    return 0;
  }
  if (gf == 0.0 && gx == 0.0) {
    for (int j = 0; j < 10; j++) {
      double XJ  = XX[j][mun - 1];
      double fak = XJ * WW[j][mun - 1] / (GNR * XJ + gg);
      RN += fak * XJ;
      RG += fak;
    } // for loop
    return 0;
  }   // if
  if (gf == 0.0 && gx > 0.0) {
    for (int j = 0; j < 10; j++) {
      double XJ   = XX[j][mun - 1];
      double WJXJ = WW[j][mun - 1] * XJ;
      double EFFJ = GNR * XJ + gg;
      for (int k = 0; k < 10; k++) {
        double XK = XX[k][mux - 1];
        double fk = WW[k][mux - 1] * WJXJ / (EFFJ + gx * XK);
        RN += XJ * fk;
        RG += fk;
        RX += XK * fk;
      } // 2 for loop
    }   // 1 for loop
  }     // if
  if (gf > 0.0 && gx == 0.0) {
    for (int j = 0; j < 10; j++) {
      double XJ   = XX[j][mun - 1];
      double WJXJ = WW[j][mun - 1] * XJ;
      double EFFJ = GNR * XJ + gg;
      for (int k = 0; k < 10; k++) {
        double fk = WW[k][muf - 1] * WJXJ / (EFFJ + gf * XX[k][muf - 1]);
        RN += XJ * fk;
        RG += fk;
        RF += XX[k][muf - 1] * fk;
      } // 2 for loop
    }   // 1 for loop
  }     // if
  if (gf > 0.0 && gx > 0.0) {
    for (int j = 0; j < 10; j++) {
      double XJ   = XX[j][mun - 1];
      double WJXJ = WW[j][mun - 1] * XJ;
      double EFFJ = GNR * XJ + gg;
      for (int k = 0; k < 10; k++) {
        double XK     = XX[k][muf - 1];
        double WKWJXJ = WW[k][muf - 1] * WJXJ;
        double EFFJK  = gf * XK + EFFJ;
        for (int l = 0; l < 10; l++) {
          double XL = XX[l][mux - 1];
          double fk = WW[l][mux - 1] * WKWJXJ / (EFFJK + gx * XL);
          RN += XJ * fk;
          RG += fk;
          RX += fk * XL;
          RF += XK * fk;
        } // 3 for loop
      }   // 2 for loop
    }     // 1 for loop
  }       // if
  return 0;
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::InverseMatrix()
{
  double A[3][3], C[3][3], C2[3][3], C3[3][3], AI[3][3], AIB[3][3];
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      A[i][j] = R[i][j];
    }
    A[i][i] = A[i][i] + 1.0;
  }
  double det1 = A[1][1] * A[2][2] - A[1][2] * A[2][1];
  double det2 = A[1][2] * A[2][0] - A[1][0] * A[2][2];
  double det3 = A[1][0] * A[2][1] - A[1][1] * A[2][0];
  double det  = A[0][0] * det1 + A[0][1] * det2 + A[0][2] * det3;
  AI[0][0]    = det1 / det;
  AI[1][0]    = det2 / det;
  AI[2][0]    = det3 / det;
  AI[0][1]    = AI[1][0];
  AI[1][1]    = (A[0][0] * A[2][2] - A[2][0] * A[0][2]) / det;
  AI[2][1]    = (A[0][1] * A[2][0] - A[0][0] * A[2][1]) / det;
  AI[0][2]    = AI[2][0];
  AI[1][2]    = AI[2][1];
  AI[2][2]    = (A[0][0] * A[1][1] - A[0][1] * A[1][0]) / det;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      double sum1 = 0.0;
      for (int k = 0; k < 3; k++) {
        sum1 += AI[i][k] * S[k][j];
      }
      AIB[i][j] = sum1;
    }
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      double sum1 = 0.0;
      for (int k = 0; k < 3; k++) {
        sum1 += S[i][k] * AIB[k][j];
      }
      C[i][j]  = A[i][j] + sum1;
      C2[i][j] = R[i][j] + sum1;
    }
  }
  det1     = C[1][1] * C[2][2] - C[1][2] * C[2][1];
  det2     = C[1][2] * C[2][0] - C[1][0] * C[2][2];
  det3     = C[1][0] * C[2][1] - C[1][1] * C[2][0];
  det      = C[0][0] * det1 + C[0][1] * det2 + C[0][2] * det3;
  C3[0][0] = det1 / det;
  C3[1][0] = det2 / det;
  C3[2][0] = det3 / det;
  C3[0][1] = C3[1][0];
  C3[1][1] = (C[0][0] * C[2][2] - C[2][0] * C[0][2]) / det;
  C3[2][1] = (C[0][1] * C[2][0] - C[0][0] * C[2][1]) / det;
  C3[0][2] = C3[2][0];
  C3[1][2] = C3[2][1];
  C3[2][2] = (C[0][0] * C[1][1] - C[0][1] * C[1][0]) / det;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      double sum1 = 0.0;
      for (int k = 0; k < 3; k++) {
        sum1 += C3[i][k] * C2[k][j];
      }
      C[i][j]  = -sum1;
      RI[i][j] = -sum1;
    }
    C[i][i] = C[i][i] + 1.0;
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      double sum1 = 0.0;
      for (int k = 0; k < 3; k++) {
        sum1 += AIB[i][k] * C[k][j];
      }
      SI[i][j] = -sum1;
    }
  }
}
// The Reich Moore formalism is inspired from D.E. Cullen  
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::GetSigmaRMP(double x, double &siga, double &sigb, double &sigc)
{
  std::vector<double> row;
  int nrsl, nrs = 0, jVal, nrstemp;
  double fac2;
  double phil, sfil, cfil, sfil2, s2fil;
  double deno = 0.0;
  double ERP, GNR;
  double sumCrsElastic = 0.0, sumCrsCapture = 0.0, sumCrsFission = 0.0;
  double GR;
  double reduced_n, reduced_g, reduced_fa, reduced_fb;
  x             = std::fabs(x);
  double pibyk2 = PI / x2(K_wnum(x));
  double rt11 = 0.0, st11 = 0.0;
  double sigrm1 = 0.0, sigab1 = 0.0, sigf1 = 0.0, sigel = 0.0;
  double gj;
  for (int lcnt = 0; lcnt < NLS; lcnt++) {
    fac2     = 0.0;
    int lval = l[lcnt];
    nrsl     = NRS[lval];
    phil     = calcPhi(x, l[lcnt]);
    sfil     = sin(phil);
    cfil     = cos(phil);
    sfil2    = sfil * sfil;
    s2fil    = 2. * sfil * cfil;
    fac2     = 4.0 * MisGj[l[lcnt]] * sfil2 * pibyk2;
    sigrm1   = 0.0;
    sigel = 0.0, sigab1 = 0.0, sigf1 = 0.0;
    jVal      = 0;
    nrstemp   = nrs;
    int nrend = nrs + nrsl;
    do {
      rt11 = 0.0;
      st11 = 0.0;
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          R[i][j]  = 0.0;
          S[i][j]  = 0.0;
          RI[i][j] = 0.0;
          SI[i][j] = 0.0;
        }
      }
      for (int r = nrs; r < nrend; r++) {
        if (MissingJ[l[lcnt]][jVal] != (J[r])) {
          nrs = r;
          break;
        }
        // Gamma_n,r term
        reduced_n =
            0.5 *
            Gamma_nrE(x, r, l[lcnt]); 
        reduced_g     = 0.5 * std::fabs(Gamma_g[r]); //
        ERP           = Er[r];
        GR            = reduced_g;
        GNR           = reduced_n;
        double DE     = ERP - x;
        deno          = DE * DE + GR * GR;
        double de2    = DE / deno;
        double gamma2 = GR / deno;
        if (LFW == 0) {
          rt11 += gamma2 * GNR;
          st11 += de2 * GNR;
        } else {
          reduced_fa  = Gamma_fasq[r]; //
          reduced_fb  = Gamma_fbsq[r]; //
          double GR2  = sqrt(GNR);
          double GFA  = reduced_fa * reduced_fa;
          double GFB  = reduced_fb * reduced_fb;
          double GFAN = reduced_fa * GR2;
          double GFBN = reduced_fb * GR2;
          double GFAB = reduced_fa * reduced_fb;
          R[0][0] += gamma2 * GNR;
          R[0][1] += gamma2 * GFAN;
          R[0][2] += gamma2 * GFBN;
          R[1][1] += gamma2 * GFA;
          R[1][2] += gamma2 * GFAB;
          R[2][2] += gamma2 * GFB;
          S[0][0] += de2 * GNR;
          S[0][1] += de2 * GFAN;
          S[0][2] += de2 * GFBN;
          S[1][1] += de2 * GFA;
          S[1][2] += de2 * GFAB;
          S[2][2] += de2 * GFB;
          R[1][0] = R[0][1];
          S[1][0] = S[0][1];
          R[2][0] = R[0][2];
          S[2][0] = S[0][2];
          R[2][1] = R[1][2];
          S[2][1] = S[1][2];
        }
      }
      if (LFW == 0) {
        gj         = (2. * std::fabs(MissingJ[l[lcnt]][jVal]) + 1.) / (4. * SPI + 2.0);
        double det = (rt11 + 1.0) * (rt11 + 1.0) + st11 * st11;
        //	    std::cout<<st11<<"  "<<rt11<<std::endl;
        double SI11 = -st11 / det;
        double RI11 = -(rt11 * (rt11 + 1.0) + st11 * st11) / det;
        sigrm1 += -4. * gj * (RI11 + (RI11 * RI11 + SI11 * SI11));
        sigel +=
            gj * ((2.0 * sfil2 + 2. * RI11) * (2.0 * sfil2 + 2. * RI11) + (s2fil + 2.0 * SI11) * (s2fil + 2.0 * SI11));
        sigf1 = 0.0;
      } else {
        InverseMatrix();
        gj          = (2. * std::fabs(MissingJ[l[lcnt]][jVal]) + 1.) / (4. * SPI + 2.0);
        double SI11 = SI[0][0];
        double RI11 = RI[0][0];
        double SI12 = SI[0][1];
        double RI12 = RI[0][1];
        double SI13 = SI[0][2];
        double RI13 = RI[0][2];
        sigab1 += -4.0 * gj * (RI11 + (RI11 * RI11 + SI11 * SI11));
        sigel +=
            gj * ((2.0 * sfil2 + 2. * RI11) * (2.0 * sfil2 + 2. * RI11) + (s2fil + 2.0 * SI11) * (s2fil + 2.0 * SI11));
        sigf1 += 4.0 * gj * (RI12 * RI12 + RI13 * RI13 + SI12 * SI12 + SI13 * SI13);
        sigrm1 = sigab1 - sigf1;
      }
      jVal += 1;
    } while (jVal < NJValue[l[lcnt]]);
    nrs = nrstemp;
    nrs += nrsl;
    sumCrsElastic += pibyk2 * (sigel) + fac2;
    sumCrsCapture += sigrm1 * pibyk2;
    sumCrsFission += sigf1 * pibyk2;
  }
  siga = sumCrsElastic;
  sigb = sumCrsCapture;
  sigc = sumCrsFission;
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::GetSigma(int lrfa, double x, double &siga, double &sigb, double &sigc)
{
  std::vector<double> row;
  int nrsl, nrs = 0, lVal = 0, jVal, nrstemp;
  double fac1 = 0.0;
  double phil, sfil, cfil, sfil2, s2fil;
  double deno = 0.0;
  double ERP, ERPP1, GNR;
  double fachNumElastic = 0.0, fachNumCapture = 0.0, fachNumFission = 0.0;
  double facElastic = 0.0, facCapture = 0.0, facFission = 0.0;
  double sumElastic = 0.0, sumCapture = 0.0, sumFission = 0.0;
  double sumCrsElastic = 0.0, sumCrsCapture = 0.0, sumCrsFission = 0.0;
  double GNRS, GR, GS;
  double fachNumElasticG, fachNumElasticH, fachNumElasticM = 0.0;
  double facElasticM = 0.0, sumElasticM = 0.0;
  x             = std::fabs(x);
  double pibyk2 = PI / x2(K_wnum(x));
  switch (LRU) 
  {
  case 1:
    switch (lrfa) 
    {
  // SLBW--------
    case 1: 
    {
      for (int lcnt = 0; lcnt < NLS; lcnt++) {
        sumElastic  = 0.0;
        sumCapture  = 0.0;
        sumFission  = 0.0;
        sumElasticM = 0.0;
        lVal        = l[lcnt];
        nrsl        = NRS[lVal];
        phil        = calcPhi(x, lVal);
        sfil        = sin(phil);
        cfil        = cos(phil);
        sfil2       = sfil * sfil;
        // cfil2 = cfil*cfil;
        s2fil = 2. * sfil * cfil;
        // c2fil = 1. - 2.*sfil2;
        fac1      = 4.0 * (2.0 * lVal + 1.0) * sfil2;
        jVal      = 0;
        nrstemp   = nrs;
        int nrend = nrs + nrsl;
        do {
          facElastic = 0.0;
          facCapture = 0.0;
          facFission = 0.0;
          for (int r = nrs; r < nrend; r++) {
            if (MissingJ[l[lcnt]][jVal] != (J[r])) {
              nrs = r;
              break;
            }
            GNR            = Gamma_nrE(x, r, lVal); // Gamma_nr(E)
            GR             = Gamma_rE(x, r, lVal, LRX);
            ERP            = GetERP(x, r, lVal);
            deno           = (x - ERP) * (x - ERP) + 0.25 * GR * GR;
            fachNumElastic = GNR * ((GNR - 2.0 * GR * sfil2 + 2.0 * (x - ERP) * s2fil) / deno);
            fachNumCapture = (GNR * Gamma_g[r] / deno);
            fachNumFission = (GNR * Gamma_f[r] / deno);
            facElastic += GJ[r] * fachNumElastic;
            facCapture += GJ[r] * fachNumCapture;
            facFission += GJ[r] * fachNumFission;
          }
          sumElastic += facElastic;
          sumCapture += facCapture;
          sumFission += facFission;
          jVal += 1;
        } while (jVal < NJValue[l[lcnt]]);
        nrs = nrstemp;
        nrs += nrsl;
        sumCrsElastic += pibyk2 * (fac1 + sumElastic) ;
        sumCrsCapture += pibyk2 * (sumCapture);
        sumCrsFission += pibyk2 * (sumFission);
      }
      siga = sumCrsElastic;
      sigb = sumCrsCapture;
      sigc = sumCrsFission;
    } break;
    case 2: 
    {
      for (int lcnt = 0; lcnt < NLS; lcnt++) {
        sumElastic  = 0.0;
        sumCapture  = 0.0;
        sumFission  = 0.0;
        sumElasticM = 0.0;
        lVal        = l[lcnt];
        nrsl        = NRS[lVal];
        phil        = calcPhi(x, lVal);
        sfil        = sin(phil);
        cfil        = cos(phil);
        sfil2       = sfil * sfil;
        // cfil2 = cfil * cfil;
        s2fil = 2. * sfil * cfil;
        // c2fil = 1. - 2.*sfil2;
        fac1      = 4.0 * (2.0 * lVal + 1.0) * sfil2;
        jVal      = 0;
        nrstemp   = nrs;
        int nrend = nrs + nrsl;
        do {
          facElastic  = 0.0;
          facCapture  = 0.0;
          facFission  = 0.0;
          facElasticM = 0.0;
          for (int r = nrs; r < nrend; r++) {
            fachNumElastic  = 0.0;
            fachNumCapture  = 0.0;
            fachNumFission  = 0.0;
            fachNumElasticM = 0.0;
            if (MissingJ[l[lcnt]][jVal] != (J[r])) {
              nrs = r;
              break;
            }
            GNR            = Gamma_nrE(x, r, lVal); // Gamma_nr(E)
            GR             = Gamma_rE(x, r, lVal, LRX);
            ERP            = (GetERP(x, r, lVal));
            deno           = (x - ERP) * (x - ERP) + 0.25 * GR * GR;
            fachNumElastic = GNR * ((GNR - 2.0 * GR * sfil2 + 2.0 * (x - ERP) * s2fil) / deno);
            fachNumCapture = (GNR * Gamma_g[r] / deno);
            fachNumFission = (GNR * Gamma_f[r] / deno);
            facElastic += GJ[r] * fachNumElastic;
            facCapture += GJ[r] * fachNumCapture;
            facFission += GJ[r] * fachNumFission;
            fachNumElasticG = 0.0;
            fachNumElasticH = 0.0;
            for (int rs = nrs; rs < nrend; rs++) {
              if (MissingJ[l[lcnt]][jVal] != J[rs]) continue;
              GNRS  = Gamma_nrE(x, rs, lVal);
              GS    = Gamma_rE(x, rs, lVal, LRX);
              ERPP1 = (GetERP(x, rs, lVal));
              if (r != rs) {
                fachNumElasticG +=
                    0.5 * GNRS * GNR * (GR + GS) / ((ERP - ERPP1) * (ERP - ERPP1) + 0.25 * (GR + GS) * (GR + GS));
                fachNumElasticH +=
                    GNRS * GNR * (ERP - ERPP1) / ((ERP - ERPP1) * (ERP - ERPP1) + 0.25 * (GR + GS) * (GR + GS));
              }
            }
            fachNumElasticM =
                (fachNumElasticG * GR + 2. * fachNumElasticH * (x - ERP)) / ((x - ERP) * (x - ERP) + 0.25 * GR * GR);
            facElasticM += GJ[r] * fachNumElasticM;
          }
          sumElastic += facElastic;
          sumCapture += facCapture;
          sumFission += facFission;
          sumElasticM += facElasticM;
          jVal += 1;
        } while (jVal < NJValue[l[lcnt]]);
        nrs = nrstemp;
        nrs += nrsl;
        sumCrsElastic += pibyk2 * (fac1 + sumElastic + sumElasticM);
        sumCrsCapture += pibyk2 * (sumCapture);
        sumCrsFission += pibyk2 * (sumFission);
      }
      siga = sumCrsElastic;
      sigb = sumCrsCapture;
      sigc = sumCrsFission;
    } break;
    case 3: 
    {
      GetSigmaRMP(x, siga, sigb, sigc);
    } break;
    // Adler-Adler
    case 4: 
    {
    } break;
    // R-Matrix
    case 7: 
    {
    } break;
    }break;
    case 2: 
    {
      int nrsj = 0;
      nrs      = 0;
      for (int lcnt = 0; lcnt < NLS2; lcnt++) {
        sumElastic  = 0.0;
        sumCapture  = 0.0;
        sumFission  = 0.0;
        sumElasticM = 0.0;
        lVal        = l[lcnt];
        phil        = calcPhi(x, lVal);
        sfil        = sin(phil);
        cfil        = cos(phil);
        sfil2       = sfil * sfil;
        s2fil       = 2. * sfil * cfil;
        fac1        = 4.0 * (2.0 * lVal + 1.0) * sfil2;
        NJS         = NRJ[lcnt];
        for (int jVal = 0; jVal < NJS; jVal++) {
          nrsl        = JSM[nrsj];
          facElastic  = 0.0;
          facCapture  = 0.0;
          facFission  = 0.0;
          facElasticM = 0.0;
          int min     = 0;
          int max     = nrsl - 1;
          int mid     = 0;
          if (x <= Es[min])
            min = 0;
          else if (x >= Es[max])
            min = max - 1;
          else {
            while (max - min > 1) {
              mid = (min + max) / 2;
              if (x < Es[mid])
                max = mid;
              else
                min = mid;
            }
          }
	  double gnr=0, gx=0, gg=0, gf=0, dr=0;
	  double gnop[2] = {GNO[nrs + min], GNO[nrs + min + 1]};
	  double gxp[2] = {GX[nrs + min], GX[nrs + min + 1]};
	  double ggp[2] = {GG[nrs + min], GG[nrs + min + 1]};
	  double gfp[2] = {GF[nrs + min], GF[nrs + min + 1]};
	  double drp[2] = {D[nrs + min], D[nrs + min + 1]};
	  double esp[2] = {Es[nrs + min], Es[nrs + min + 1]};
	  gnr = TNudyCore::Instance()->InterpolateScale(esp, gnop, INT, x);
	  gx = TNudyCore::Instance()->InterpolateScale(esp, gxp, INT, x);
	  gg = TNudyCore::Instance()->InterpolateScale(esp, ggp, INT, x);
	  gf = TNudyCore::Instance()->InterpolateScale(esp, gfp, INT, x);
	  dr = TNudyCore::Instance()->InterpolateScale(esp, drp, INT, x);
	  if (gnr < 1E-19) gnr = 0.0;
	  if (gg < 1E-19) gg = 0.0;
	  if (gf < 1E-19) gf = 0.0;
	  if (gx < 1E-19) gx = 0.0;
	  if(GNO[nrs + min] == 0.0 && INT >3){
	    gnr = gnop[0] + (gnop[1] - gnop[0])*(x - esp[0])/ (esp[1] - esp[0]);
	  }
	  if(GG[nrs + min] == 0.0 && INT >3){
	    gg = ggp[0] + (ggp[1] - ggp[0])*(x - esp[0])/ (esp[1] - esp[0]);
	  }
	  if(GX[nrs + min] == 0.0 && INT >3){
	    gx = gxp[0] + (gxp[1] - gxp[0])*(x - esp[0])/ (esp[1] - esp[0]);
	  }
	  if(GF[nrs + min] == 0.0 && INT >3){
	    gf = gfp[0] + (gfp[1] - gfp[0])*(x - esp[0])/ (esp[1] - esp[0]);
	  }
	  if(D[nrs + min] == 0.0 && INT >3){
	    dr = drp[0] + (drp[1] - drp[0])*(x - esp[0])/ (esp[1] - esp[0]);
	  }
          GNR = amun[nrsj] * gnr * calcPene(x, lVal) * sqrt(x) / GetRho(x, lVal);
          widthFluctuation(GNR, gx, gg, gf, nrsj);
          double temp1 = 2. * PI * GNR * GJ[nrsj] / dr;
          facElastic   = (GNR * RN - 2. * sfil2) * temp1;
          facCapture   = gg * temp1 * RG;
          facFission   = gf * temp1 * RF;

          sumElastic += facElastic;
          sumCapture += facCapture;
          sumFission += facFission;
          nrs += nrsl;
          nrsj += 1;
        }
        sumCrsElastic += pibyk2 * (fac1 + sumElastic);
        sumCrsCapture += pibyk2 * (sumCapture);
        sumCrsFission += pibyk2 * (sumFission);
      }
      siga = sumCrsElastic;
      sigb = sumCrsCapture;
      sigc = sumCrsFission;
    } break;
  }
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::backCrsAdler(double x, int nx)
{
  double crs1 = 0.0, crs2 = 0.0, backCrs = 0.0, pibyk2;
  double phil, sfil, sfil2, s2fil, c2fil, deno;
  phil    = calcPhi(x, 0);
  sfil    = sin(phil);
  sfil2   = x2(sfil);
  s2fil   = sin(2.0 * phil);
  c2fil   = cos(2.0 * phil);
  pibyk2  = PI / x2(K_wnum(x));
  backCrs = sqrt(x) * pibyk2 /
            (at1[nx] + at2[nx] / x + at3[nx] / x2(x) + at4[nx] / (x * x * x) + bt1[nx] * x + bt2[nx] * x2(x));
  crs1 = 4.0 * pibyk2 * sfil2;
  switch (nx) {
  case 0: {
    for (int nrs = 0; nrs < totalAdler; nrs++) {
      deno = (det1[nrs] - x) * (det1[nrs] - x) + dwt1[nrs] * dwt1[nrs];
      crs2 = sqrt(x) * (dwt1[nrs] * (grt1[nrs] * c2fil + git1[nrs] * s2fil) +
                        (det1[nrs] - x) * (git1[nrs] * c2fil - grt1[nrs] * s2fil)) /
             deno;
    }
    crsAdler[nx] = crs1 + crs2 + backCrs;
  } break;
  case 1: {
    for (int nrs = 0; nrs < totalAdler; nrs++) {
      deno = (def1[nrs] - x) * (def1[nrs] - x) + dwf1[nrs] * dwf1[nrs];
      crs2 = sqrt(x) * (dwf1[nrs] * grf1[nrs] + (def1[nrs] - x) * gif1[nrs]) / deno;
    }
    crsAdler[nx] = crs2 + backCrs;
  } break;
  case 2: {
    for (int nrs = 0; nrs < totalAdler; nrs++) {
      deno = (dec1[nrs] - x) * (dec1[nrs] - x) + dwc1[nrs] * dwc1[nrs];
      crs2 = sqrt(x) * (dwc1[nrs] * grc1[nrs] + (dec1[nrs] - x) * gic1[nrs]) / deno;
    }
    crsAdler[nx] = crs2 + backCrs;
  } break;
  }
  return 0;
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::recursionLinear(double x1, double x2, double sig1, double sig2, double sig3, double sig4,
                                       double sig5, double sig6)
{
  double siga, sigb, sigc, elRatio = sigDiff - 0.1 * sigDiff, capRatio = sigDiff - 0.1 * sigDiff,
                           fisRatio = sigDiff - 0.1 * sigDiff;

  double x10 = x1 + 0.001 * ( x2 - x1 ) ;
  double siga0, sigb0, sigc0 ;
  GetSigma(LRF, x10, siga0, sigb0, sigc0);
  double slope10 = ( siga0 - sig1 ) / ( x10 - x1 ) ;
  double slope11 = ( sigb0 - sig3 ) / ( x10 - x1 ) ;
  double slope12 = ( sigc0 - sig5 ) / ( x10 - x1 ) ;
  
  double x20 = x2 - 0.001 * ( x2 - x1 ) ;
  double siga1, sigb1, sigc1 ;
  GetSigma(LRF, x20, siga1, sigb1, sigc1);
  double slope20 = ( sig2 - siga1 ) / ( x2 - x20 ) ;
  double slope21 = ( sig4 - sigb1 ) / ( x2 - x20 ) ;
  double slope22 = ( sig6 - sigc1 ) / ( x2 - x20 ) ;

  double mid               = 0.5 * ( x1 + x2 ) ;
  if ((sig1 == 0.0 && sig2 == 0.0) || x1 == x2 || (x1 < 1E-5 || x2 < 1E-5)) return 0;
  GetSigma(LRF, mid, siga, sigb, sigc);
  double sigmid1         = sig1 + (sig2 - sig1) * (mid - x1) / (x2 - x1);
  double sigmid2         = sig3 + (sig4 - sig3) * (mid - x1) / (x2 - x1);
  double sigmid3         = sig5 + (sig6 - sig5) * (mid - x1) / (x2 - x1);
  if (siga > 0) elRatio  = std::fabs((siga - sigmid1) / siga);
  if (sigb > 0) capRatio = std::fabs((sigb - sigmid2) / sigb);
  if (sigc > 0) fisRatio = std::fabs((sigc - sigmid3) / sigc);
  
  int slope = -1 ;
    
  if ( slope10 / slope20 < 0.0 ){
    slope = 1 ;
  }
  if (slope11 / slope21 < 0.0 ){
    slope = 1 ;
  }
  if ( slope12 / slope22 < 0.0 ){
    slope = 1 ;
  }
//  if (elRatio <= sigDiff && capRatio <= sigDiff && fisRatio <= sigDiff ) {
//   if (elRatio <= sigDiff && capRatio <= sigDiff && fisRatio <= sigDiff && slope == -1) {
    if (elRatio <= sigDiff && capRatio <= sigDiff && fisRatio <= sigDiff && fabs (sig2/siga -1) < 0.01 
        && fabs (sig4/sigb -1) < 0.01 && slope == -1) {
    return 0;
  } else {
    eLinElastic.push_back(mid);
    xLinElastic.push_back(siga);
    eLinCapture.push_back(mid);
    xLinCapture.push_back(sigb);
    eLinFission.push_back(mid);
    xLinFission.push_back(sigc);
  }
//  std::cout << std::endl;  
  recursionLinear(x1, mid, sig1, siga, sig3, sigb, sig5, sigc);
  recursionLinear(mid, x2, siga, sig2, sigb, sig4, sigc, sig6);
  return 0;
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::recursionLinear(double x1, double x2, double sig1, double sig2)
{
  double siga, sigb, sigc, elRatio; 
  double mid = 0.5 * (x1 + x2);
  if ((sig1 == 0.0 && sig2 == 0.0) || x1 == x2 || (x1 < 1E-5 || x2 < 1E-5)) return 0;
  GetSigma(LRF, mid, siga, sigb, sigc);
  double sigmid1 = sig1 + (sig2 - sig1) * (mid - x1) / (x2 - x1);
  elRatio        = std::fabs((siga - sigmid1) / siga);
  if (elRatio <= sigDiff) {
    return 0;
  } else {
    eLinElastic.push_back(mid);
    xLinElastic.push_back(siga);
    eLinCapture.push_back(mid);
    xLinCapture.push_back(sigb);
    eLinFission.push_back(mid);
    xLinFission.push_back(sigc);
  }
  recursionLinear(x1, mid, sig1, siga);
  recursionLinear(mid, x2, siga, sig2);
  return 0;
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::additionalSigma(int LRF, double x)
{
  double siga, sigb, sigc;
  GetSigma(LRF, x, siga, sigb, sigc);
  eLinElastic.push_back(x);
  eLinCapture.push_back(x);
  eLinFission.push_back(x);
  xLinElastic.push_back(siga);
  xLinCapture.push_back(sigb);
  xLinFission.push_back(sigc);
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::recoPlusBroad(int flagNer)
{
  int nrs = 0, nrsl;
  if (LRU == 1) {
    for (int j = 0; j < NLS; j++) {
      nrsl = NRS[l[j]];
      for (int i = nrs; i < nrs + NRS[l[j]]; i++) {
        if (Er[i] < eLo || Er[i] > eHi1) {
          continue;
        }
        additionalSigma(LRF, Er[i]);
      }
      nrs += nrsl;
    }
    additionalSigma(LRF, eHi1);    
    if (flagNer == 0) {
      double eneLow = 1.0E-5;
      do {
        if (eneLow >= eLo) additionalSigma(LRF, eneLow);
        eneLow *= 1.03;
      } while (eneLow < 1.0E3 && eneLow < eHi1);
          eneLow = 1.0E3 ;
      do {
	for (int incrE = 0 ; incrE < 10 ; incrE++){
	  if (eneLow >= 1.0E3 && eneLow + (incrE * eneLow) < eHi1) {
	    additionalSigma(LRF, eneLow + (incrE * eneLow));
	  }
	}
	eneLow *= 10;
      } while (eneLow < 1.0E3 && eneLow < eHi1);
    }
    TNudyCore::Instance()->Sort(eLinElastic, xLinElastic);
    TNudyCore::Instance()->Sort(eLinCapture, xLinCapture);
    TNudyCore::Instance()->Sort(eLinFission, xLinFission);
    TNudyCore::Instance()->ThinningDuplicate(eLinElastic, xLinElastic);
    TNudyCore::Instance()->ThinningDuplicate(eLinCapture, xLinCapture);
    TNudyCore::Instance()->ThinningDuplicate(eLinFission, xLinFission);
    int nvectorend = eLinElastic.size();
    for (int ju = intLinLru1; ju < nvectorend - 1; ju++) {
//     std::cout << eLinElastic[ju] <<"  "<< xLinCapture[ju] << std::endl ;
      recursionLinear(eLinElastic[ju], eLinElastic[ju + 1], xLinElastic[ju], xLinElastic[ju + 1], xLinCapture[ju],
                      xLinCapture[ju + 1], xLinFission[ju], xLinFission[ju + 1]);
    }
    intLinLru1 = eLinElastic.size();
  } else {
    intLinLru1 = eLinElastic.size();
    for (int l = 0; l < JSM[0]; l++) {
      double ei = Es[l];
      if (ei == Es[0]) {
        ei += 0.01;
        additionalSigma(LRF, ei);
      } else {
	double gape =( Es[l] - Es[l - 1] ) / 100 ;
	ei = Es[l - 1] + gape ;
	do
	{ 
	  if ( ei >= Es[l] ){
	    ei = Es[l];
	  }
	  additionalSigma( LRF, ei );
	  ei += gape ;
	} while( ei <= Es[l] );
      }
    }
    additionalSigma( LRF, eHi2);
  }
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::Linearize(int flagNer)
{
  double a = 0.0;
  switch (LRF) {
  case 1:
  case 2:
  case 3: {
    recoPlusBroad(flagNer);
  } break;
  case 4: {
    for (int i = 1; i < (int)eHi; i++) {
      a                     = (double)i;
      if (eHi > 100000.0) a = (double)i * 10.;
      if (eHi < 10000.0) a  = (double)i / 10.;
      if (eHi < 5000.0) a   = (double)i / 100.;
      a += eLo;
      crsAdler[0] = backCrsAdler(a, 0);
      crsAdler[1] = backCrsAdler(a, 1);
      crsAdler[2] = backCrsAdler(a, 2);
      crsAdler[3] = crsAdler[0] - crsAdler[1] - crsAdler[2];
    }
  } break;
  }
}
//******************** Get Resonant PArameter and cross-section data from ENDF file **********
void TNudyEndfSigma::GetData(const char *rENDF, double isigDiff)
{
  sigDiff = isigDiff;
  for (int i = 0; i < 10; i++){ MTChargeFlag[i] = 0;}
  TFile *rEND = TFile::Open(rENDF, "UPDATE");
  if (!rEND || rEND->IsZombie()) printf("Error: TFile :: Cannot open file %s\n", rENDF);
  TKey *rkey              = (TKey *)rEND->GetListOfKeys()->First();
  TNudyEndfTape *rENDFVol = (TNudyEndfTape *)rkey->ReadObj();
  TNudyEndfMat *tMat      = 0;
  TList *mats             = (TList *)rENDFVol->GetMats();
  int nmats               = mats->GetEntries();
  for (int iMat = 0; iMat < nmats; iMat++) {
    tMat = (TNudyEndfMat *)mats->At(iMat);
    TIter iter(tMat->GetFiles());
    TNudyEndfFile *file, *file2;
    std::vector<int>().swap(MtNumAng4Photon);
    while ((file = (TNudyEndfFile *)iter.Next())) {
      if (file->GetMF() > 13 && (file->GetMF() <= 15 || file->GetMF() <= 33)){
	std::cout<< "add file 2 called "<< file->GetMF() << std::endl;
	tMat->Add(file2);
      }
      switch (file->GetMF()) {
      case 1:
        if (flagRead != 1) {
	  for (int nxc = 0; nxc < tMat->GetNXC(); nxc++){
	    int mfn 	= tMat->GetMFn(nxc) ;
	    int mtn	= tMat->GetMTn(nxc) ;
	    if(mfn == 3 ){
	      if(mtn == 103){
		MTChargeFlag[0] = -1;
	      } else if (mtn == 104){
		MTChargeFlag[1] = -1;
	      } else if (mtn == 105){
		MTChargeFlag[2] = -1;
	      } else if (mtn == 106){
		MTChargeFlag[3] = -1;
	      } else if (mtn == 107){
		MTChargeFlag[4] = -1;
	      } else if (mtn >= 600 && mtn < 650) {
		MTChargeFlag[5] = -1;
	      } else if (mtn >= 650 && mtn < 700) {
		MTChargeFlag[6] = -1;
	      } else if (mtn >= 700 && mtn < 750) {
		MTChargeFlag[7] = -1;
	      } else if (mtn >= 750 && mtn < 800) {
		MTChargeFlag[8] = -1;
	      } else if (mtn >= 800 && mtn < 850) {
		MTChargeFlag[9] = -1;
	      }
	    } else if (mfn == 12 || mfn == 13) {
	      MtNumSig4Photon.push_back (mtn) ;
	      std::cout <<" MF = \t"<< mfn <<" MT = \t"<< mtn << std::endl;
	    } else if (mfn == 14) {
	      MtNumAng4Photon.push_back (mtn) ;
	      std::cout <<" MF = \t"<< mfn <<" MT = \t"<< mtn << std::endl;
	    } else if (mfn == 15) {
	      MtNumEng4Photon.push_back (mtn) ;
	      std::cout <<" MF = \t"<< mfn <<" MT = \t"<< mtn << std::endl;
	    }
	  }
	  if (MtNumAng4Photon.size() == 0 ) {
	    file2 = new TNudyEndfFile(tMat->GetMAT(), 14);
	  } else {
	    file2 = file;
	  }
          ReadFile1(file);
          flagRead = 1;
//          std::cout << "file 1 OK: Should be printed only one time " << std::endl;
          // ReWriteFile1(file);
        }
        break;
      case 2:
	if(prepro == 0){
	  ReadFile2(file);
	  if (LRU != 0) {
	    TNudyCore::Instance()->Sort(eLinElastic, xLinElastic);
	    TNudyCore::Instance()->Sort(eLinCapture, xLinCapture);
	    TNudyCore::Instance()->Sort(eLinFission, xLinFission);
	    TNudyCore::Instance()->ThinningDuplicate(eLinElastic, xLinElastic);
	    TNudyCore::Instance()->ThinningDuplicate(eLinCapture, xLinCapture);
	    TNudyCore::Instance()->ThinningDuplicate(eLinFission, xLinFission);
	    Thinning(eLinElastic, xLinElastic);
	    Thinning(eLinCapture, xLinCapture);
	    Thinning(eLinFission, xLinFission);
	  }
	}
//         double siga, sigb, sigc; 
//	for (unsigned int i = 0; i < eLinCapture.size() ; i++) {
// 	    GetSigma(3, eLinCapture[i], siga, sigb, sigc);
// 	    std::cout <<std::setprecision(12)<< eLinCapture[i] <<"   "<< xLinCapture[i] <<"   "<< sigb <<"  "<< xLinCapture[i] - sigb <<  std::endl;
// 	}
        	std::cout<<"file 2 OK "<< std::endl;
        break;
      case 3: {
//           for (unsigned long j = 0 ; j < eLinCapture.size() ; j++)
//             std::cout << std::setprecision(12) << eLinCapture [ j ] << "  \t" << xLinCapture [ j ] << std::endl;
        ReadFile3(file);
        sigma.clear();
	
            std::cout << "file 3 OK \t" << sigmaOfMts.size() << std::endl;
	    TNudyCore::Instance()->ThinningDuplicate(eLinElastic, xLinElastic);
	    TNudyCore::Instance()->ThinningDuplicate(eLinCapture, xLinCapture);
	    TNudyCore::Instance()->ThinningDuplicate(eLinFission, xLinFission);
	
//           std::cout << "before elstic Doppler begins " << eLinElastic.size() <<"  \t"<< xLinElastic.size() << std::endl;
//           for (unsigned long j = 0 ; j < eLinElastic.size() ; j++)
//             std::cout << std::setprecision(12) << eLinElastic [ j ] << "  \t" << xLinElastic [ j ] << std::endl;
        broadSigma(eLinElastic, xLinElastic, xBroadElastic);
	TNudyCore::Instance()->Sort(eLinElastic, xBroadElastic);
//           std::cout << "after elstic Doppler " << eLinElastic.size() <<"  \t"<< xBroadElastic.size() << std::endl;
//           for (unsigned long j = 0 ; j < eLinFission.size() ; j++)
//             std::cout << std::setprecision(12) << eLinFission [ j ] << "  " << xLinFission [ j ] << std::endl;
	if(prepro==0)Thinning(eLinElastic, xBroadElastic);
        // std::cout << eLinElastic.size() << std::endl;
//          std::cout << "before capture Doppler begins " << eLinCapture.size() <<"  "<< xLinCapture.size() << std::endl;
//            for (unsigned long j = 0 ; j < eLinCapture.size() ; j++)
//              std::cout << std::setprecision(12) << eLinCapture [ j ] << "  " << xLinCapture [ j ] << std::endl;
        broadSigma(eLinCapture, xLinCapture, xBroadCapture);
	TNudyCore::Instance()->Sort(eLinCapture, xBroadCapture);
//           std::cout << "after capture Doppler begins " << eLinCapture.size() <<"  "<< xBroadCapture.size() << std::endl;
	if(prepro==0)Thinning(eLinCapture, xBroadCapture);
        // std::cout << eLinCapture.size() << std::endl;
//          std::cout << "before fission Doppler begins "<< eLinFission.size() <<"  "<< xLinFission.size() << std::endl;
        broadSigma(eLinFission, xLinFission, xBroadFission);
	TNudyCore::Instance()->Sort(eLinFission, xBroadFission);
	if(prepro==0)Thinning(eLinFission, xBroadFission);
//            std::cout << "after Fission Doppler begins " << eLinFission.size() <<"  "<< xBroadFission.size() << std::endl;
       // std::cout << eLinFission.size() << std::endl;
//        	std::cout<<"Doppler done "<<outstring << std::endl;
        dopplerBroad = 0;

        out << eLinElastic.size() << std::endl;
        for (unsigned long j = 0; j < eLinElastic.size(); j++)
          out << std::setprecision(12) << eLinElastic[j] << "  " << xBroadElastic[j] << std::endl;
        eLinElastic.clear();
        xLinElastic.clear();
        xBroadElastic.clear();
        out << eLinCapture.size() << std::endl;
        for (unsigned long j = 0; j < eLinCapture.size(); j++)
          out << std::setprecision(12) << eLinCapture[j] << "  " << xBroadCapture[j] << std::endl;
        eLinCapture.clear();
        xLinCapture.clear();
        xBroadCapture.clear();
        out << eLinFission.size() << std::endl;
        for (unsigned long j = 0; j < eLinFission.size(); j++)
          out << std::setprecision(12) << eLinFission[j] << "  " << xBroadFission[j] << std::endl;
        eLinFission.clear();
        xLinFission.clear();
        xBroadFission.clear();
	
        std::cout << "dopplerall started \t" << std::endl;
        dopplerAll();
        std::cout << "dopplerall OK \t" << std::endl;
        fixupTotal(file, energyUni, sigmaUniTotal);
        std::cout << "fixup total OK \t" <<energyUni.size() << std::endl;

        ReWriteFile3(file);
        MtNumbers.clear();
      } break;

// //       case 4:
// // //        ReadFile4(file);
// //         std::cout << "file 4 OK " << std::endl;
// //         //ReWriteFile4(file);
// //         MtNumbers.clear();
// //         break;
// // 	case 5:
// // //	recoEnergy = new TNudyEndfEnergy(file);
// // 	std::cout<<"file 5 OK "<<std::endl;
// // 	break;
	case 6:
	{
	  TIter iter1(tMat->GetFiles());
	  TNudyEndfFile *file1;
	  while ((file1 = (TNudyEndfFile *)iter1.Next())) {
	    if (file1->GetMF() == 6) {
	      std::cout << "file 6 found " << std::endl ; 
	    TIter secIter1(file1->GetSections());
	    TNudyEndfSec *sec1;
	    while ((sec1 = (TNudyEndfSec *)secIter1.Next())) {
	      int NK = sec1->GetN1();
	      TIter recIter1(sec1->GetRecords());
	      for (int k = 0; k < NK; k++) {
		// std::cout<<k<<" NK "<<NK<< std::endl;
		TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter1.Next();
		div_t divr;
		AWRI   = sec1->GetC2();
		int MT = sec1->GetMT();
		//      int LCT = sec->GetL2();
		divr       = div(sec1->GetC1(), 1000);
		double ZA  = divr.quot;
		double AA  = divr.rem;
		double NA1 = AA - ZA;
		int ZAP    = tab1->GetC1();
		double AWP = tab1->GetC2();
		std::cout<<" ZAP = \t"<< ZAP <<" MT "<< MT <<  std::endl;
		// int LIP =tab1->GetL1();
		int LAW = tab1->GetL2();
		if (LAW == 3 || LAW == 4 || LAW == 0) continue;
		divr          = div(ZAP, 1000);
		int particleA = divr.rem;
		int particleZ = divr.quot;
		std::cout<<"LAW = "<< LAW <<" MT "<<MT <<" ZAP = "<< ZAP <<" AWP "<<AWP <<" parZ "<< particleZ
		<<" parA "<< particleA << std::endl;
		if (LAW == 2 && particleZ == 0 && particleA == 0) {
		  int LANG, NL;
		  TNudyEndfTab2 *tab2 = (TNudyEndfTab2 *)recIter1.Next();
		  int ne2               = tab2->GetN2();
		  TNudyEndfList *header = (TNudyEndfList *)recIter1.Next();
		  LANG = header->GetL1();
		  TNudyEndfSec *sec3;
		  if (LANG == 0) {
		    sec3 = new TNudyEndfSec(sec1->GetMAT(), 14, MT, sec1->GetC1(), AWRI,0,1,1,0);
		  } else {
		    sec3 = new TNudyEndfSec(sec1->GetMAT(), 14, MT, sec1->GetC1(), AWRI,0,2,1,0);
		  }
		  TNudyEndfTab2 *sec3Tab2 = new TNudyEndfTab2();
		  sec3Tab2->SetCont(tab2->GetC1(),tab2->GetC2(),0,0,tab2->GetN1(),ne2);
		  sec3Tab2->SetNBT(ne2, 0);
		  sec3Tab2->SetINT(tab2->GetINT(0), 0);
		  sec3->Add(sec3Tab2);
		  std::cout<<"tab2->GetINT(0) "<< sec3Tab2->GetINT(0) <<" NR "<< sec3Tab2->GetN1() <<" NE "<< sec3Tab2->GetN2() << std::endl;
		  
		  TNudyEndfList *sec3List = new TNudyEndfList();
		  sec3List->SetCont(header->GetC1(),header->GetC2(),0,0,header->GetN2(),0);
		  std::cout<<"C1 "<< header->GetC1() <<" C2 "<< header->GetC2() <<" N2 "<< header->GetN2() << std::endl;
		  for (int j = 0; j < header->GetN2(); ++j) {
		    sec3List->SetLIST(header->GetLIST(j), j);
		  }
		  sec3->Add(sec3List);
		  std::cout<<"sec3List C1 "<< sec3List->GetC1()<<" sec3List C2 "<< sec3List->GetC2() <<" nl "<< sec3List->GetN1() << std::endl;
		  for (int j = 0; j < sec3List->GetN1(); ++j) {
		    std::cout <<"list "<< sec3List->GetLIST(j) << std::endl;
		  }
		  for (int lis = 1; lis < ne2; lis++) {
		    TNudyEndfList *header = (TNudyEndfList *)recIter1.Next();
		    TNudyEndfList *sec3List = new TNudyEndfList();
		    sec3List->SetCont(header->GetC1(),header->GetC2(),0,0,header->GetN2(),0);
		    NL = header->GetN2();
		    if (LANG == 0) {
		      for (int j = 0; j < NL; j++) {
			sec3List->SetLIST(header->GetLIST(j),j);
		      }
		      std::cout<<"sec3List C1 "<< sec3List->GetC1()<<" sec3List C2 "<< sec3List->GetC2() <<" nl "<< sec3List->GetN1() << std::endl;
		      for (int j = 0; j < sec3List->GetN1(); ++j) {
			std::cout <<"list "<< sec3List->GetLIST(j) << std::endl;
		      }
		    } else if (LANG > 0) {
		      for (int i = 0; i < NL; i++) {
			sec3List->SetLIST(header->GetLIST(2 * i + 0),2 * i + 0);
			sec3List->SetLIST(header->GetLIST(2 * i + 1),2 * i + 1);
		      }
		    }
		    sec3->Add(sec3List);
		  }
		  file2->Add(sec3);
		} else if (LAW == 1 || LAW == 5) {
		    TNudyEndfTab2 *tab2 = (TNudyEndfTab2 *)recIter1.Next();
		    for (unsigned int lis = 0; lis < tab2->GetN2(); lis++) {
		      TNudyEndfList *header = (TNudyEndfList *)recIter1.Next();
		    }
		  } else if (LAW == 6){
		    TNudyEndfCont *header = (TNudyEndfCont *)recIter1.Next();
		  } else if (LAW == 7){
		    TNudyEndfTab2 *tab2 = (TNudyEndfTab2 *)recIter1.Next();
		    for (unsigned int cr1 = 0; cr1 < tab2->GetN2(); cr1++) {
		      TNudyEndfTab2 *tab3 = (TNudyEndfTab2 *)recIter1.Next();
		      for (unsigned int cr2 = 0; cr2 < tab3->GetN2(); cr2++) {
			TNudyEndfTab1 *tab12 = (TNudyEndfTab1 *)recIter1.Next();
		      }
		    }
		  }
		}
	      }
	    }
	  }
//	recoEnergyAng = new TNudyEndfEnergyAng(file,QValue);
// 	std::cout<<"file 6 OK "<<std::endl;
	}break;
// // 	case 8:
// // //	recoFissY = new TNudyEndfFissionYield(file);
// // 	std::cout<<"file 8 OK "<<std::endl;
// // 	break;
	case 12:
	recoPhYield = new TNudyEndfPhYield(file);
// 	std::cout<<"file 12 OK "<<std::endl;
	break;
	case 13:
	recoPhProd = new TNudyEndfPhProd(file);
// 	std::cout<<"file 13 OK "<<std::endl;
	break;
	case 14:
	recoPhAng = new TNudyEndfPhAng(file);
 	std::cout<<"file 14 OK "<<std::endl;
	break;
	case 15:
	recoPhEnergy = new TNudyEndfPhEnergy(file);
// 	std::cout<<"file 15 OK "<<std::endl;
	break;
      }
    }
  }
  for (unsigned int i = 0; i < MtNumSig4Photon.size () ; i++){
    std::cout <<"Photon Sig MT = \t"<< MtNumSig4Photon[i] << std::endl;
  }
  for (unsigned int i = 0; i < MtNumAng4Photon.size () ; i++){
    std::cout <<"Photon Ang MT = \t"<< MtNumAng4Photon[i] << std::endl;
  }
  for (unsigned int i = 0; i < MtNumEng4Photon.size () ; i++){
    std::cout <<"Photon Ene MT = \t"<< MtNumEng4Photon[i] << std::endl;
  }
  rENDFVol->Write();
  rEND->Close();
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::broadSigma(std::vector<double> &x1, std::vector<double> &x2, std::vector<double> &x3)
{
//     std::cout<<"before sort \t"<< x1.size() << std::endl;
  TNudyCore::Instance()->Sort(x1, x2);
//   for (unsigned long j = 0 ; j < x1.size() ; j++)
//     std::cout << std::setprecision(12) << j << "  \t"<< x1 [ j ] << "  \t" << x2 [ j ] << std::endl;
//     std::cout<<"after sort \t"<< x1.size() << std::endl;
  if (x1.size() > 0) {
    doppler      = new TNudyEndfDoppler(sigDiff, AWRI, doppTemp1, doppTemp2, x1, x2);
//      std::cout<<"after doppler \t"<< x1.size() << std::endl;
    dopplerBroad = 1;
    for (unsigned long j = 0; j < x1.size(); j++) {
//      std::cout<<"size2 "<< x1[j] <<"  "<< x2[j] <<"  "<< doppler->sigma[j]<< std::endl;
      x3.push_back(doppler->sigma[j]);
    }
  }
  doppler->sigma.clear();
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::fixupTotal(TNudyEndfFile *file1, std::vector<double> &x1, std::vector<double> &x2)
{
  std::sort(x1.begin(), x1.end()); // Unionization of energy grid for cross-section
  TNudyCore::Instance()->ThinningDuplicate(x1);
  TIter secIter(file1->GetSections());
  TNudyEndfSec *sec;
  for (unsigned long i = 0; i < sigmaOfMtsDop.size(); i++) {
    while ((sec = (TNudyEndfSec *)secIter.Next())) {
//       std::cout << sec->GetMT() <<" fixuptotal \t"<< MtNumbers[i] << std::endl;
      int MT = sec->GetMT();
      if (MT == MtNumbers[i]) {
// 	if (MT != 1 && MT != 3 && MT != 4 && MT != 27 && MT != 19 && MT != 20 && MT != 21 && MT != 38 && MT != 101 && MT < 120) {
	int size = sigmaOfMtsDop[i].size() / 2;
	for (unsigned long k = 0; k < x1.size(); k++) {
	  int min = 0;
	  int max = size - 1;
	  int mid = 0;
	  if (x1[k] <= sigmaOfMtsDop[i][min])
	    min = 0;
	  else if (x1[k] >= sigmaOfMtsDop[i][max])
	    min = max - 1;
	  else {
	    while (max - min > 1) {
	      mid = (min + max) / 2;
	      if (x1[k] < sigmaOfMtsDop[i][mid])
		max = mid;
	      else
		min = mid;
	    }
	  }
	  if (x1[k] == sigmaOfMtsDop[i][min] && sigmaOfMtsDop[i][min] >0) {
	    eneTemp.push_back(x1[k]);
	    sigTemp.push_back(sigmaOfMtsDop[i][size + min]);
	  } else {
	    double sigmaAdd = 0;
	      if(x1[k] > sigmaOfMtsDop[i][min]){
		sigmaAdd = sigmaOfMtsDop[i][size + min] +
			      (sigmaOfMtsDop[i][size + min + 1] - sigmaOfMtsDop[i][size + min]) * (x1[k] - sigmaOfMtsDop[i][min]) /
				  (sigmaOfMtsDop[i][min + 1] - sigmaOfMtsDop[i][min]); // linear interpolation
	      }
            if (sigmaAdd > 0) {				  
	      eneTemp.push_back(x1[k]);
	      sigTemp.push_back(sigmaAdd);
	    }
	  }
	  if (eneTemp.size() == 1) {
	    energyLocationMts.push_back(k);
	  }
	}
	sigmaUniOfMts.push_back(sigTemp);
	eneTemp.clear();
	sigTemp.clear();
// 	}
      }break;
    }
  }
  x2.resize(x1.size());
  sigmaOfMtsDop.clear();
  for (unsigned long i = 0; i < sigmaUniOfMts.size(); i++) {
    int size = sigmaUniOfMts[i].size();
    int MT = MtNumbers[i] ;
    if (MT != 1 && MT != 3 && MT != 4 && MT != 27 && MT != 19 && MT != 20 && MT != 21 && MT != 38 && MT != 101 && MT < 120) {
    // std::cout<<"size "<<  energyLocationMts.size  << std::endl;
      for (int j = 0; j < size; j++) {
	x2[energyLocationMts[i] + j] += sigmaUniOfMts[i][j];
      }
    }
  }
  outtotal << x1.size() << std::endl;
  for (unsigned long j = 0; j < x1.size(); j++) {
    outtotal << x1[j] << "  " << x2[j] << std::endl;
  }
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::dopplerAll()
{
  for (unsigned long i = 0; i < sigmaOfMts.size(); i++) {
//       std::cout<<"MT number \t"<< MtNumbers[i] <<"  \t"<< sigmaOfMts.size() << std::endl;
    int size = sigmaOfMts[i].size() / 2;
    for (int k = 0; k < size ; k++) {
      if (sigmaOfMts[i][size + k] > 1E-20) {
        eneTemp.push_back ( sigmaOfMts [ i ][ k ] );
        sigTemp.push_back ( sigmaOfMts [ i ][ size + k ] );
//	std::cout<<std::setprecision(12)<<  sigmaOfMts [ i ][ k ]<<"  \t"<< sigmaOfMts [ i ][ size + k ] <<std::endl;
      } 
    }
//      std::cout<<"before broad \t"<< std::endl;
    broadSigma(eneTemp, sigTemp, sigma);
//     std::cout<<"after broad \t"<< sigma.size() << std::endl;
    if(prepro==0){
      TNudyCore::Instance()->Sort(eneTemp, sigma);
      Thinning(eneTemp, sigma);
    }
//    std::cout<<"after thinning \t"<< sigma.size() << std::endl;
    sigmaMts.insert(std::end(sigmaMts), std::begin(eneTemp), std::end(eneTemp));
    sigmaMts.insert(std::end(sigmaMts), std::begin(sigma), std::end(sigma));
    sigmaOfMtsDop.push_back(sigmaMts);
    energyUni.insert(std::end(energyUni), std::begin(eneTemp), std::end(eneTemp));
//    std::cout<<"after energyuni \t"<< energyUni.size() << std::endl;
    eneTemp.clear();
    sigTemp.clear();
    sigma.clear();
    sigmaMts.clear();
  }
  
}
//------------------------------------------------------------------------------------------------------
double TNudyEndfSigma::Thinning(std::vector<double> &x1, std::vector<double> &x2)
{
  int size  = x1.size();
  int size1 = x1.size();
  if (size <= 0) return 0;
  for (int i = 0; i < size1 - 2; i++) {
    if (x1[i] < 1E3 && dopplerBroad == 0) continue;
    double sigmid1 = x2[i] + (x2[i + 2] - x2[i]) * (x1[i + 1] - x1[i]) / (x1[i + 2] - x1[i]);
    if (std::fabs((x2[i + 1] - sigmid1) / sigmid1) <= sigDiff) {
      x1.erase(x1.begin() + i + 1);
      x2.erase(x2.begin() + i + 1);
    }
    if (x2[i] <= 1E-20) {
      x1.erase(x1.begin() + i);
      x2.erase(x2.begin() + i);
    }
    size1 = x1.size();
  }
  size1 = x1.size();
  if (size == size1) return 0;
  Thinning(x1, x2);
  return 0;
}
//______________________________________________________________________________
void TNudyEndfSigma::ReadFile4(TNudyEndfFile *file)
{
  TIter secIter(file->GetSections());
  TNudyEndfSec *sec;
  while ((sec = (TNudyEndfSec *)secIter.Next())) {
    TIter recIter(sec->GetRecords());
    TNudyEndfCont *header = (TNudyEndfCont *)recIter.Next();
    int MT                = sec->GetMT();
    MtNumbers.push_back(MT);
    int LTT = sec->GetL2();
    int LI  = header->GetL1();
    int LCT = header->GetL2();
    MtLct.push_back(LCT);
    // printf("LCT = %d LTT = %d LI = %d\n",LCT, LTT, LI);
    // Legendre polynomial coefficients
    if (LTT == 1 && LI == 0) {
      TNudyEndfTab2 *tab2 = (TNudyEndfTab2 *)recIter.Next();
      for (int i = 0; i < tab2->GetN2(); i++) {
        TNudyEndfList *tab = (TNudyEndfList *)recIter.Next();
        ein.push_back(tab->GetC2());
        // std::cout<<"energy "<< tab->GetC2() << std::endl;
        for (int j = 0; j < tab->GetNPL(); j++) {
          lCoef1.push_back(tab->GetLIST(j));
        }
        lCoef.push_back(lCoef1);
        lCoef1.clear();
      }
      for (unsigned long i = 0; i < ein.size(); i++) {
        // printf("Ein = %e\n", ein[i]);
        int k1     = 0;
        double fme = 0.0;
        do {
          fme      = 1.0;
          double x = -1. + k1 * 0.02;
          for (unsigned long j = 0; j < lCoef[i].size(); j++) {
            double leg = ROOT::Math::legendre(j + 1, x);
            fme += 0.5 * (2. * (j + 1) + 1.) * lCoef[i][j] * leg;
            // printf("a%d = %e leg= %e\n", j, lCoef[i].At(j),leg);
          }
          if (fme > 0.0) {
            cosFile4.push_back(x);
            cosPdfFile4.push_back(fme);
          }
          // printf("%e %e\n", x, fme);
          k1++;
        } while (k1 < 101);
        for (int l = 0; l < 100; l++) {
          recursionLinearLeg(i, cosFile4[l], cosFile4[l + 1], cosPdfFile4[l], cosPdfFile4[l + 1]);
        }
        fillPdf1d();
      }
      fillPdf2d();
      lCoef.clear();
      // Tabulated probability tables
    } else if (LTT == 2 && LI == 0) {
      TNudyEndfTab2 *tab2 = (TNudyEndfTab2 *)recIter.Next();
      for (int i = 0; i < tab2->GetN2(); i++) {
        TNudyEndfTab1 *tab = (TNudyEndfTab1 *)recIter.Next();
        ein.push_back(tab->GetC2());
        NR = tab->GetNR();
        NP = tab->GetNP();
        // std::cout<<"energy "<< tab->GetC2() << std::endl;
        for (int i = 0; i < tab->GetNR(); i++) {
          nbt1.push_back(tab->GetNBT(i));
          int1.push_back(tab->GetINT(i));
        }
        for (int j = 0; j < tab->GetNP(); j++) {
          cosFile4.push_back(tab->GetX(j));
          cosPdfFile4.push_back(tab->GetY(j));
        }
        for (int l = 0; l < tab->GetNP() - 1; l++) {
          recursionLinearProb(cosFile4[l], cosFile4[l + 1], cosPdfFile4[l], cosPdfFile4[l + 1]);
        }
        fillPdf1d();
        nbt1.clear();
        int1.clear();
      }
      fillPdf2d();
      // Low energy given by legendre polynomial and high energy by tabulated probability tables
    } else if (LTT == 3 && LI == 0) {
      TNudyEndfTab2 *lowE = (TNudyEndfTab2 *)recIter.Next();
      for (int i = 0; i < lowE->GetN2(); i++) {
        TNudyEndfList *tab = (TNudyEndfList *)recIter.Next();
        ein.push_back(tab->GetC2());
        for (int j = 0; j < tab->GetNPL(); j++) {
          lCoef1.push_back(tab->GetLIST(j));
        }
        lCoef.push_back(lCoef1);
        lCoef1.clear();
      }
      for (unsigned long i = 0; i < ein.size(); i++) {
        // printf("Ein = %e\n", ein[i]);
        int k1     = 0;
        double fme = 0.0;
        do {
          fme      = 1.0;
          double x = -1. + k1 * 0.02;
          for (unsigned long j = 0; j < lCoef[i].size(); j++) {
            double leg = ROOT::Math::legendre(j + 1, x);
            fme += 0.5 * (2. * (j + 1) + 1.) * lCoef[i][j] * leg;
            // printf("a%d = %e leg= %e\n", j, lCoef[i][j],leg);
          }
          if (fme > 0.0) {
            cosFile4.push_back(x);
            cosPdfFile4.push_back(fme);
          }
          // printf("%e %e\n", x, fme);
          k1++;
        } while (k1 < 101);

        for (int l = 0; l < 100; l++) {
          recursionLinearLeg(i, cosFile4[l], cosFile4[l + 1], cosPdfFile4[l], cosPdfFile4[l + 1]);
        }
        fillPdf1d();
      }
      lCoef.clear();
      TNudyEndfTab2 *highE = (TNudyEndfTab2 *)recIter.Next();
      for (int i = 0; i < highE->GetN2(); i++) {
        TNudyEndfTab1 *tab = (TNudyEndfTab1 *)recIter.Next();
        ein.push_back(tab->GetC2());
        // std::cout <<"energy "<< ein[ein.size() - 1] << std::endl;
        NR = tab->GetNR();
        NP = tab->GetNP();
        for (int i = 0; i < tab->GetNR(); i++) {
          nbt1.push_back(tab->GetNBT(i));
          int1.push_back(tab->GetINT(i));
        }
        for (int j = 0; j < tab->GetNP(); j++) {
          cosFile4.push_back(tab->GetX(j));
          cosPdfFile4.push_back(tab->GetY(j));
        }
        if (NP > 2) {
          for (int l = 0; l < tab->GetNP() - 1; l++) {
            recursionLinearProb(cosFile4[l], cosFile4[l + 1], cosPdfFile4[l], cosPdfFile4[l + 1]);
          }
        }
        fillPdf1d();
        nbt1.clear();
        int1.clear();
      }
      fillPdf2d();
    } else if (LTT == 0 && LI == 1) {
      ein.push_back(1E-14);
      ein.push_back(1.5E8);
      for (int j = 0; j < 2; j++) {
        cosFile4.push_back(1);
        cosPdfFile4.push_back(0.5);
        cosFile4.push_back(-1);
        cosPdfFile4.push_back(0.5);
        fillPdf1d();
      }
      fillPdf2d();
    }
    // Low energy given by legendre polynomial and high energy by tabulated probability tables
  } // end while loop
}
//--------------------------------------------------------------------------------------
double TNudyEndfSigma::recursionLinearLeg(int i, double x1, double x2, double pdf1, double pdf2)
{
  double pdf = 1.0;
  double mid = 0.5 * (x1 + x2);
  if ((pdf1 == 0.0 && pdf2 == 0.0) || x1 == x2) return 0;
  for (unsigned long j = 0; j < lCoef[i].size(); j++) {
    double leg = ROOT::Math::legendre(j + 1, mid);
    pdf += 0.5 * (2. * (j + 1) + 1.) * lCoef[i][j] * leg;
  }
  double pdfmid1 = pdf1 + (pdf2 - pdf1) * (mid - x1) / (x2 - x1);
  if (pdf > 0 && fabs((pdf - pdfmid1) / pdf) <= 2E-4) {
    return 0;
  }
  cosFile4.push_back(mid);
  cosPdfFile4.push_back(pdf);
  recursionLinearLeg(i, x1, mid, pdf1, pdf);
  recursionLinearLeg(i, mid, x2, pdf, pdf2);
  return 0;
}
//--------------------------------------------------------------------------------------
double TNudyEndfSigma::recursionLinearProb(double x1, double x2, double pdf1, double pdf2)
{
  double pdf = 1.0;
  double mid = 0.5 * (x1 + x2);
  if ((pdf1 == 0.0 && pdf2 == 0.0) || x1 == x2) return 0;
  pdf            = TNudyCore::Instance()->Interpolate(nbt1, int1, NR, cosFile4, cosPdfFile4, NP, mid);
  double pdfmid1 = pdf1 + (pdf2 - pdf1) * (mid - x1) / (x2 - x1);
  if (pdf > 0 && fabs((pdf - pdfmid1) / pdf) <= 2E-4) {
    return 0;
  }
  cosFile4.push_back(mid);
  cosPdfFile4.push_back(pdf);
  recursionLinearProb(x1, mid, pdf1, pdf);
  recursionLinearProb(mid, x2, pdf, pdf2);
  return 0;
}
//-----------------------------------------------------------------------------------------
void TNudyEndfSigma::fillPdf1d()
{
  TNudyCore::Instance()->Sort(cosFile4, cosPdfFile4);
  TNudyCore::Instance()->cdfGenerateT(cosFile4, cosPdfFile4, cosCdfFile4);
  for (unsigned long i = 0; i < cosFile4.size(); i++) {
    cos4.push_back(cosFile4[i]);
    pdf.push_back(cosPdfFile4[i]);
    cdf.push_back(cosCdfFile4[i]);
  }
  cos2d.push_back(cos4);
  pdf2d.push_back(pdf);
  cdf2d.push_back(cdf);
  cosFile4.clear();
  cosPdfFile4.clear();
  cosCdfFile4.clear();
  cos4.clear();
  pdf.clear();
  cdf.clear();
}
//--------------------------------------------------------------------------------------
void TNudyEndfSigma::fillPdf2d()
{
  ein2d.push_back(ein);
  cos3d.push_back(cos2d);
  pdf3d.push_back(pdf2d);
  cdf3d.push_back(cdf2d);
  ein.clear();
  cos2d.clear();
  pdf2d.clear();
  cdf2d.clear();
}
//______________________________________________________________________________
void TNudyEndfSigma::ReWriteFile1(TNudyEndfFile *file)
{
  TIter secIter(file->GetSections());
  TNudyEndfSec *sec;
  while ((sec = (TNudyEndfSec *)secIter.Next())) {
    TIter recIter(sec->GetRecords());
    int MT = sec->GetMT();
    if (MT == 452) { // Total fission neutron multiplicity polynomial expansion
      int LNU = sec->GetL2();

      if (LNU == 1) {
        TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
        for (int j = 0; j < list->GetN1(); j++) {
          cnc.push_back(list->GetLIST(j));
        }
        double ein = 1E-5;
        do {
          double nun = 0;
          for (int i = 0; i < list->GetN1(); i++) {
            nun += cnc[i] * pow(ein, i);
          }
          eintFile1.push_back(ein);
          nutFile1.push_back(nun);
          ein *= 2;
        } while (ein < 21E8);
        cnc.clear();
      } else { // Total fission neutron multiplicity tabulated representation
        TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter.Next();
        NR                  = 1;
        NP                  = eintFile1.size();
        tab1->SetNBT(NP, 0);
        tab1->SetINT(2, 0);
        for (int crs = 0; crs < NP; crs++) {
          tab1->SetX(eintFile1[crs], crs);
          tab1->SetY(nutFile1[crs], crs);
        }
      }
    } else if (MT == 455) { // delayed neutron multiplicity
      int LDG = sec->GetL1();
      int LNU = sec->GetL2();
      if (LNU == 1 && LDG == 0) {

      } else if (LNU == 1 && LDG == 1) {

      } else if (LNU == 2 && LDG == 0) {
        TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter.Next();
        NR                  = 1;
        NP                  = eindFile1.size();
        tab1->SetNBT(NP, 0);
        tab1->SetINT(2, 0);
        for (int crs = 0; crs < NP; crs++) {
          tab1->SetX(eindFile1[crs], crs);
          tab1->SetY(nudFile1[crs], crs);
        }
      } else if (LNU == 2 && LDG == 1) {
      }
    } else if (MT == 456) { // prompt neutron multiplicity
      int LNU = sec->GetL2();
      if (LNU == 1) {
        //      std::cout<<"prompt nu = "<< ZA <<" LNU "<< LNU << std::endl;
        //	TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
        // double nup = list->GetLIST(0);
      } else {
        TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter.Next();
        NR                  = 1;
        NP                  = einFile1.size();
        tab1->SetNBT(NP, 0);
        tab1->SetINT(2, 0);
        for (int crs = 0; crs < NP; crs++) {
          tab1->SetX(einFile1[crs], crs);
          tab1->SetY(nuFile1[crs], crs);
        }
      }
    } else if (MT == 458) {
      TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
      int NPLY            = list->GetL2();
      if (NPLY == 0) {
        double EFR = list->GetLIST(0);
        double ENP = list->GetLIST(2);
        double END = list->GetLIST(4);
        double EGP = list->GetLIST(6);
        double EGD = list->GetLIST(8);
        double EB  = list->GetLIST(10);
        double ENU = list->GetLIST(12);
        //	double ER  = list->GetLIST(14);
        // double ET  = list->GetLIST(16);
        double ein = 1E-5;
        do {
          double EFis = 0;
          EFis        = EFR + ENP + END + EGP + EGD + EB + ENU;
          EFis -= 0.100 * ein; // nuetrino energy dependence
          EFis -= 0.075 * ein; // delayed gamma energy dependence
          EFis -= 0.075 * ein; // delayed beta energy dependence
          //	      for(unsigned long i = 0; i < eintFile1.size(); i++)
          //		std::cout<< eintFile1[i] <<"  "<<nutFile1[i] << std::endl;
          int n0  = 0;
          int max = eintFile1.size() - 1;
          int mid = 0;
          if (ein <= eintFile1[n0]) {
            n0 = 0;
          } else if (ein >= eintFile1[max]) {
            n0 = max - 1;
          } else {
            while (max - n0 > 1) {
              mid = (n0 + max) / 2;
              if (ein < eintFile1[mid])
                max = mid;
              else
                n0 = mid;
              //      std::cout<<"n0 "<< n0 <<" max "<< max << std::endl;
            }
          }
          double nue = TNudyCore::Instance()->LinearInterpolation(eintFile1[n0], nutFile1[n0], eintFile1[n0 + 1],
                                                                  nutFile1[n0 + 1], ein);
          double nu0 = nutFile1[0];
          EFis -= -1.307 * ein + 8.07 * 1E6 * (nue - nu0); // prompt neutron energy dependence
          einfFile1.push_back(ein);
          heatFile1.push_back(EFis);
          ein *= 2;
        } while (ein < 21E8);
      } else {
        double c0[9 * (NPLY + 1)], c1[9 * (NPLY + 1)];
        for (int i = 0; i < 9 * (NPLY + 1); i++) {
          c0[i] = list->GetLIST(i);
        }
        for (int i = 9 * (NPLY + 1); i < 18 * (NPLY + 1); i++) {
          c1[i - 9 * (NPLY + 1)] = list->GetLIST(i);
        }
        double ein = 1E-5;
        do {
          double EFis = 0;
          for (int i = 0; i < 9 * NPLY / 2 - 2; i++) {
            EFis += c0[i * 2] + c1[i * 2] * ein;
          }
          einfFile1.push_back(ein);
          heatFile1.push_back(EFis);
          ein *= 2;
        } while (ein < 21E8);
      }
      einFissHeat.push_back(einfFile1);
      fissHeat.push_back(heatFile1);
      einfFile1.clear();
      heatFile1.clear();
    } else if (MT == 460) {
      int LO = sec->GetL1();
      int NG = sec->GetN1();
      if (LO == 1) {
        for (int ng = 0; ng < NG; ng++) {
          TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)recIter.Next();
          NR                  = 1;
          NP                  = einphFile1.size();
          tab1->SetNBT(NP, 0);
          tab1->SetINT(2, 0);
          for (int crs = 0; crs < NP; crs++) {
            tab1->SetX(einphFile1[crs], crs);
            tab1->SetY(phFile1[crs], crs);
          }
        }
      } else {
        TNudyEndfList *list = (TNudyEndfList *)recIter.Next();
        int NNF             = list->GetN1();
        // double lambda[NNF];
        for (int i = 0; i < NNF; i++) {
          // lambda[i] = list->GetLIST(i);
          //	     std::cout <<"lambda  "<< lambda[i] << std::endl;
        }
      }
    }
  }
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::ReWriteFile3(TNudyEndfFile *file)
{
  TIter secIter0(file->GetSections());
  TNudyEndfSec *sec0;
  int mtcount = 0;
  while ((sec0 = (TNudyEndfSec *)secIter0.Next())) {
    int MT = sec0->GetMT();
    if (MT !=1 && mtcount == 0){
      AddSecFile3 (file, 0, 0, 1, energyUni, sigmaUniTotal);
      break;
    }
  }
  TIter secIter(file->GetSections());
  TNudyEndfSec *sec;  
  while ((sec = (TNudyEndfSec *)secIter.Next())) {
//     std::cout <<"MT  "<<sec->GetMT() <<"  \t"<< MtNumbers[mtcount] << std::endl;
    int MT = sec->GetMT();
//     if (MT != 3 && MT != 4 && MT != 27 && MT != 19 && MT != 20 && MT != 21 && MT != 38 && MT != 101 && MT < 250) {
      TIter recIter(sec->GetRecords());
      TNudyEndfCont *header = (TNudyEndfCont *)recIter.Next();
      int NR                = 1;
      if (MT == 1)
        header->SetCont(header->GetC1(), header->GetC2(), header->GetL1(), header->GetL2(), NR, (int)energyUni.size());
      TNudyEndfTab1 *tab1 = (TNudyEndfTab1 *)(sec->GetRecords()->At(0));
      if (MT == 1) {
        for (int cr = 0; cr < NR; cr++) {
          tab1->SetNBT((int)energyUni.size(), 0);
          tab1->SetINT(2, 0);
        }
        for (unsigned int crs = 0; crs < energyUni.size(); crs++) {
          tab1->SetX(energyUni[crs], crs);
          tab1->SetY(sigmaUniTotal[crs], crs);
//  	  std::cout<<energyUni[crs] <<"  \t"<< sigmaUniTotal[crs] << std::endl;
        }
	mtcount++;
        continue;
      }
      if (MT == MtNumbers[mtcount]) {
        header->SetCont(header->GetC1(), header->GetC2(), header->GetL1(), header->GetL2(), NR,
                        (int)sigmaUniOfMts[mtcount].size());
        tab1->SetNBT((int)sigmaUniOfMts[mtcount].size(), 0);
        tab1->SetINT(2, 0);
        for (unsigned int crs = 0; crs < sigmaUniOfMts[mtcount].size(); crs++) {
          tab1->SetX(energyUni[energyLocationMts[mtcount] + crs], crs);
          tab1->SetY(sigmaUniOfMts[mtcount][crs], crs);
//   	  std::cout << energyUni[energyLocationMts[mtcount] + crs] <<"  \t"<< sigmaUniOfMts[mtcount][crs] << std::endl;
        }
      }
      mtcount++;
//     }
  }
}
//------------------------------------------------------------------------------------------------------
void TNudyEndfSigma::ReWriteFile4(TNudyEndfFile *file)
{
  TIter secIter(file->GetSections());
  TNudyEndfSec *sec;
  while ((sec = (TNudyEndfSec *)secIter.Next())) {
    // if (sec->GetMT() == (int)fReaction) {
    TIter recIter(sec->GetRecords());
  }  
}
//-------------------------------------------------------------------------------------------------------
TNudyEndfSigma::~TNudyEndfSigma()
{
  eintFile1.shrink_to_fit();
  nutFile1.shrink_to_fit();
  einFile1.shrink_to_fit();
  nuFile1.shrink_to_fit();
  eindFile1.shrink_to_fit();
  nudFile1.shrink_to_fit();
  einphFile1.shrink_to_fit();
  phFile1.shrink_to_fit();
  einfFile1.shrink_to_fit();
  heatFile1.shrink_to_fit();
  cnc.shrink_to_fit();
  nui.shrink_to_fit();
  energyUni.shrink_to_fit();
  sigmaUniTotal.shrink_to_fit();
  sigmaOfMts.shrink_to_fit();
  sigmaOfMtsDop.shrink_to_fit();
  sigmaUniOfMts.shrink_to_fit();
  energyLocationMts.shrink_to_fit();
  MtNumbers.shrink_to_fit();
  sigmaMts.shrink_to_fit();
  qvaluetemp.shrink_to_fit();
  eLinElastic.shrink_to_fit();
  eLinCapture.shrink_to_fit();
  eLinFission.shrink_to_fit();
  xLinElastic.shrink_to_fit();
  xLinCapture.shrink_to_fit();
  xLinFission.shrink_to_fit();
  xBroadElastic.shrink_to_fit();
  xBroadCapture.shrink_to_fit();
  xBroadFission.shrink_to_fit();
  nbt1.shrink_to_fit();
  int1.shrink_to_fit();
  eLinearFile3.shrink_to_fit();
  xLinearFile3.shrink_to_fit();
  sigma.shrink_to_fit();
  l.shrink_to_fit();
  NRS.shrink_to_fit();
  NRJ.shrink_to_fit();
  JSM.shrink_to_fit();
  Er.shrink_to_fit();
  J.shrink_to_fit();
  GJ.shrink_to_fit();
  Gamma_r.shrink_to_fit();
  Gamma_n.shrink_to_fit();
  Gamma_g.shrink_to_fit();
  Gamma_f.shrink_to_fit();
  Gamma_x.shrink_to_fit();
  Gamma_fa.shrink_to_fit();
  Gamma_fasq.shrink_to_fit();
  Gamma_fb.shrink_to_fit();
  Gamma_fbsq.shrink_to_fit();
  at1.shrink_to_fit();
  at2.shrink_to_fit();
  at3.shrink_to_fit();
  at4.shrink_to_fit();
  bt1.shrink_to_fit();
  bt2.shrink_to_fit();
  det1.shrink_to_fit();
  dwt1.shrink_to_fit();
  grt1.shrink_to_fit();
  git1.shrink_to_fit();
  def1.shrink_to_fit();
  dwf1.shrink_to_fit();
  grf1.shrink_to_fit();
  gif1.shrink_to_fit();
  dec1.shrink_to_fit();
  dwc1.shrink_to_fit();
  grc1.shrink_to_fit();
  gic1.shrink_to_fit();
  amux.shrink_to_fit();
  amun.shrink_to_fit();
  amug.shrink_to_fit();
  amuf.shrink_to_fit();
  Es.shrink_to_fit();
  D.shrink_to_fit();
  GX.shrink_to_fit();
  GNO.shrink_to_fit();
  GG.shrink_to_fit();
  GF.shrink_to_fit();
  PhiEr.shrink_to_fit();
  ShiftEr.shrink_to_fit();
  eneTemp.shrink_to_fit();
  sigTemp.shrink_to_fit();
}
