#include <TPartIndex.h>
#include <string>
#include <algorithm>
#include <TBuffer.h>

using std::transform;
using std::string;

ClassImp(TPartIndex)

    const
    char *TPartIndex::fgPrName[FNPROC] = {"Transport",    "MultScatt",   "Ionisation", "Decay",      "inElastic",
                                          "Elastic",      "RestCapture", "Brehms",     "PairProd",   "Annihilation",
                                          "CoulombScatt", "Photoel",     "Compton",    "Conversion", "Capture",
                                          "Fission",      "Killer",      "Total"};
const short TPartIndex::fgPCode[FNPROC] = {1091, 2010, 2002, 6201, 4121, 4111, 4151, 2003, 2004,
                                           2005, 2001, 2012, 2013, 2014, 4131, 4141, 7403, 999};

const char *TPartIndex::fgEleSymbol[NELEM] = {
    "H",  "He", "Li", "Be", "B",  "C",  "N",  "O",  "F",  "Ne", "Na",  "Mg", "Al",  "Si", "P",   "S",  "Cl",
    "Ar", "K",  "Ca", "Sc", "Ti", "V",  "Cr", "Mn", "Fe", "Co", "Ni",  "Cu", "Zn",  "Ga", "Ge",  "As", "Se",
    "Br", "Kr", "Rb", "Sr", "Y",  "Zr", "Nb", "Mo", "Tc", "Ru", "Rh",  "Pd", "Ag",  "Cd", "In",  "Sn", "Sb",
    "Te", "I",  "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm",  "Eu", "Gd",  "Tb", "Dy",  "Ho", "Er",
    "Tm", "Yb", "Lu", "Hf", "Ta", "W",  "Re", "Os", "Ir", "Pt", "Au",  "Hg", "Tl",  "Pb", "Bi",  "Po", "At",
    "Rn", "Fr", "Ra", "Ac", "Th", "Pa", "U",  "Np", "Pu", "Am", "Cm",  "Bk", "Cf",  "Es", "Fm",  "Md", "No",
    "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn", "Uut", "Fl", "Uup", "Lv", "Uus", "Uuo"};
const char *TPartIndex::fgEleName[NELEM] = {
    "Hydrogen",    "Helium",    "Lithium",      "Beryllium",   "Boron",        "Carbon",        "Nitrogen",
    "Oxygen",      "Fluorine",  "Neon",         "Sodium",      "Magnesium",    "Aluminium",     "Silicon",
    "Phosphorus",  "Sulfur",    "Chlorine",     "Argon",       "Potassium",    "Calcium",       "Scandium",
    "Titanium",    "Vanadium",  "Chromium",     "Manganese",   "Iron",         "Cobalt",        "Nickel",
    "Copper",      "Zinc",      "Gallium",      "Germanium",   "Arsenic",      "Selenium",      "Bromine",
    "Krypton",     "Rubidium",  "Strontium",    "Yttrium",     "Zirconium",    "Niobium",       "Molybdenum",
    "Technetium",  "Ruthenium", "Rhodium",      "Palladium",   "Silver",       "Cadmium",       "Indium",
    "Tin",         "Antimony",  "Tellurium",    "Iodine",      "Xenon",        "Caesium",       "Barium",
    "Lanthanum",   "Cerium",    "Praseodymium", "Neodymium",   "Promethium",   "Samarium",      "Europium",
    "Gadolinium",  "Terbium",   "Dysprosium",   "Holmium",     "Erbium",       "Thulium",       "Ytterbium",
    "Lutetium",    "Hafnium",   "Tantalum",     "Tungsten",    "Rhenium",      "Osmium",        "Iridium",
    "Platinum",    "Gold",      "Mercury",      "Thallium",    "Lead",         "Bismuth",       "Polonium",
    "Astatine",    "Radon",     "Francium",     "Radium",      "Actinium",     "Thorium",       "Protactinium",
    "Uranium",     "Neptunium", "Plutonium",    "Americium",   "Curium",       "Berkelium",     "Californium",
    "Einsteinium", "Fermium",   "Mendelevium",  "Nobelium",    "Lawrencium",   "Rutherfordium", "Dubnium",
    "Seaborgium",  "Bohrium",   "Hassium",      "Meitnerium",  "Darmstadtium", "Roentgenium",   "Copernicium",
    "Ununtrium",   "Flerovium", "Ununpentium",  "Livermorium", "Ununseptium",  "Ununoctium"};

const float TPartIndex::fgWElem[NELEM] = {
    1.008,  4.0026, 6.94,   9.0122, 10.81,  12.011, 14.007, 15.999, 18.998, 20.180, 22.990, 24.305, 26.982, 28.085,
    30.974, 32.06,  35.45,  39.948, 39.098, 40.078, 44.956, 47.867, 50.942, 51.996, 54.938, 55.845, 58.933, 58.693,
    63.546, 65.38,  69.723, 72.63,  74.922, 78.96,  79.904, 83.798, 85.468, 87.62,  88.906, 91.224, 92.906, 95.96,
    97.91,  101.07, 102.91, 106.42, 107.87, 112.41, 114.82, 118.71, 121.76, 127.60, 126.90, 131.29, 132.91, 137.33,
    138.91, 140.12, 140.91, 144.24, 144.91, 150.36, 151.96, 157.25, 158.93, 162.50, 164.93, 167.26, 168.93, 173.05,
    174.97, 178.49, 180.95, 183.84, 186.21, 190.23, 192.22, 195.08, 196.97, 200.59, 204.38, 207.2,  208.98, 208.98,
    209.99, 222.02, 223.02, 226.03, 227.03, 232.04, 231.04, 238.03, 237.05, 244.06, 243.06, 247.07, 247.07, 251.08,
    252.08, 257.10, 258.10, 259.10, 262.11, 265.12, 268.13, 271.13, 270,    277.15, 276.15, 281.16, 280.16, 285.17,
    284.18, 289.19, 288.19, 293,    294,    294};

TPartIndex *TPartIndex::fgPartIndex = 0;

//___________________________________________________________________
TPartIndex::TPartIndex()
    : fNPart(0), fPDG(0), fNpReac(0), fNpCharge(0), fNEbins(0), fEilDelta(0), fEGrid(0),
#ifdef USE_VECGEOM_NAVIGATOR
      fDBPdg(0),
#else
      fDBPdg(TDatabasePDG::Instance()),
#endif
      fPDGToGVMap() {
}

//___________________________________________________________________
TPartIndex::~TPartIndex() {
  delete[] fPDG;
  delete[] fEGrid;
  delete fDBPdg;
  fgPartIndex = 0;
}

//___________________________________________________________________
void TPartIndex::SetEnergyGrid(double emin, double emax, int nbins) {
  fNEbins = nbins;
  fEilDelta = (fNEbins - 1) / log(emax / emin);
  delete[] fEGrid;
  fEGrid = new double[fNEbins];
  double en = emin;
  double edelta = exp(1 / fEilDelta);
  for (int i = 0; i < fNEbins; ++i) {
    fEGrid[i] = en;
    en *= edelta;
  }
}

//___________________________________________________________________
int TPartIndex::ProcIndex(int proccode) const {
  short ip = fgNProc;
  while (ip--)
    if (fgPCode[ip] == proccode)
      break;
  return ip;
}

//___________________________________________________________________
const char *TPartIndex::ProcName(int proc) const {
  if (proc < 0 || proc >= fgNProc)
    return "Unknown";
  return fgPrName[proc];
}

//___________________________________________________________________
void TPartIndex::SetPartTable(const int *vpdg, int np) {
  fNPart = np;
  delete[] fPDG;
  fPDG = new int[fNPart];
  for (int i = 0; i < fNPart; ++i)
    fPDG[i] = vpdg[i];
}

//______________________________________________________________________________
int TPartIndex::PDG(const char *pname) const {
  int nr = fNPart;
#ifdef USE_VECGEOM_NAVIGATOR
  while (nr--)
    if (!strcmp(pname, fDBPdg->GetParticle(fPDG[nr]).Name()))
      return fPDG[nr];
#else
  while (nr--)
    if (!strcmp(pname, fDBPdg->GetParticle(fPDG[nr])->GetName()))
      return fPDG[nr];
#endif
  return -12345678;
}

//______________________________________________________________________________
int TPartIndex::PartIndex(int pdg) const {
  switch (pdg) {
  case 11:
    return fSpecGVIndices[0]; // e-
    break;
  case -11:
    return fSpecGVIndices[1]; // e+
    break;
  case 22:
    return fSpecGVIndices[2]; // gamma
    break;
  case 2212:
    return fSpecGVIndices[3]; // proton
    break;
  default: // look for in the map
    return fPDGToGVMap.find(pdg)->second;
  }
}

//______________________________________________________________________________
void TPartIndex::Print(const char *option) const {
  char line[120];
  string opt = option;
  transform(opt.begin(), opt.end(), opt.begin(), ::tolower);
  if (opt.find("particles") != string::npos) {
    printf("Available particles:\n");
    memset(line, 0, 120);
    for (int i = 0; i < fNPart; ++i) {
#ifdef USE_VECGEOM_NAVIGATOR
      const char *name = fDBPdg->GetParticle(fPDG[i]).Name();
#else
      const char *name = fDBPdg->GetParticle(fPDG[i])->GetName();
#endif
      if (strlen(line) + strlen(name) + 1 > 119) {
        printf("%s\n", line);
        memset(line, 0, 120);
      }
      strcat(line, name);
      strcat(line, " ");
    }
    if (strlen(line))
      printf("%s\n", line);
  }
  if (opt.find("reactions") != string::npos) {
    printf("Available reactions:\n");
    memset(line, 0, 120);
    strcat(line, "Total ");
    for (int i = 0; i < fgNProc; ++i) {
      if (strlen(line) + strlen(fgPrName[i]) + 1 > 119) {
        printf("%s\n", line);
        memset(line, 0, 120);
      }
      strcat(line, fgPrName[i]);
      strcat(line, " ");
    }
    if (strlen(line))
      printf("%s\n", line);
  }
  if (opt.find("version") != string::npos) {
    printf("Xsec database version %d.%d.%d\n", VersionMajor(), VersionMinor(), VersionSub());
  }
}

//______________________________________________________________________________
void TPartIndex::SetPDGToGVMap(std::map<int, int> &theMap) {
  fPDGToGVMap = theMap;
  fSpecGVIndices[0] = fPDGToGVMap.find(11)->second;   // e-
  fSpecGVIndices[1] = fPDGToGVMap.find(-11)->second;  // e+
  fSpecGVIndices[2] = fPDGToGVMap.find(22)->second;   // gamma
  fSpecGVIndices[3] = fPDGToGVMap.find(2212)->second; // proton
}

//______________________________________________________________________________
void TPartIndex::Streamer(TBuffer &R__b) {
  // Stream an object of class TPartIndex.

  if (R__b.IsReading()) {
    delete fDBPdg;
    fDBPdg = 0;
    R__b.ReadClassBuffer(TPartIndex::Class(), this);
    fgPartIndex = this;
    Print("version");
  } else {
    R__b.WriteClassBuffer(TPartIndex::Class(), this);
  }
}

// semi empirical Bethe-Weizsacker mass formula based on the liquid drop model
// with coefficients determined by fitting to experimental mass data (AME2003)and
// taken from J.M. Pearson: Nuclear Mass Models (jina web /pearson_MassSchool)
// accuracy about +-few MeV
//______________________________________________________________________________
double TPartIndex::GetAprxNuclearMass(int Z, int A) {
  const double a_vol = -0.01569755; // volume    term coef. [GeV]
  const double a_surf = 0.01766269; // surface   term coef. [GeV]
  const double a_c = 0.0007070236; // Coulomb   term coef. [GeV]
  const double a_sym = 0.026308165; // asymmetry term coef. [GeV]
  const double a_ss = -0.017003132; // surface-sym. term coef. [GeV]
  const double massp = 0.938272; // mass of proton  [GeV]
  const double massn = 0.939565; // mass of neutron [GeV]

  int N = A - Z;      // #neutrons
  double delta = 0.0; // if A is odd

  if ((N & 1) && (Z & 1)) // (N,Z) are odd
    delta = 0.0025;
  else if ((!(N & 1)) && (!(Z & 1))) // (N,Z) are even
    delta = -0.0025;

  double Ap13 = pow(A, 1 / 3.); // A^{1/3}
  double Ap23 = Ap13 * Ap13;    // A^{2/3}
  double dummy = (N - Z) / (1. * A);

  return Z * massp + N * massn +
         (a_vol * A + a_surf * Ap23 + a_c * Z * Z / Ap13 + (a_sym * A + a_ss * Ap23) * dummy * dummy + delta);
}
