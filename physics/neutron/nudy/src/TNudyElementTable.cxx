#include "Geant/TNudyElementTable.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TButton.h"
#include "Geant/TNudyElementRN.h"
#include <iostream>

#ifdef USE_ROOT
ClassImp(Nudy::TNudyElementTable)
#endif

    namespace Nudy
{
  class TNudyElementRN;
}

namespace Nudy {

TNudyElementTable::TNudyElementTable()
    : fState(0), fOx(40), fOy(40), fTable(gGeoManager->GetElementTable()), fLOD(1), fRNTable(NULL),
      fGeom(new TGeoManager("", ""))
{
  fTable->BuildDefaultElements();
  fTable->ImportElementsRN();
}

void TNudyElementTable::InitializeControls()
{
  int index;
  TString names[] = {"-", "v", "", "<", "o", ">", "", "^", "+"};
  TString func[]  = {"gTNudyElementTable->ZoomOut();",
                    "gTNudyElementTable->MoveDown()",
                    "",
                    "gTNudyElementTable->MoveLeft()",
                    "",
                    "gTNudyElementTable->MoveRight()",
                    "",
                    "gTNudyElementTable->MoveUp()",
                    "gTNudyElementTable->ZoomIn();"};
  for (index = 0; index < 9; index++) {
    double x1 = ((index % 3 + 1) * 30) / (double)(fRNTable->GetWindowWidth());
    double y1 = ((index / 3 + 1) * 30) / (double)(fRNTable->GetWindowHeight());
    double x2 = ((index % 3 + 1) * 30 + 29) / (double)(fRNTable->GetWindowWidth());
    double y2 = ((index / 3 + 1) * 30 + 29) / (double)(fRNTable->GetWindowHeight());
    std::cout << x1 << " , " << y1 << " , " << x2 << " , " << y2 << std::endl;
    x1 += 0.851;
    x2 += 0.851;
    TButton *newControl = new TButton(names[index], func[index], x1, y1, x2, y2);
    fControls.Add(newControl);
  }

  // ZoomInBut = new TButton("ZoomIn","ZoomIn()",0.1,0.1,0.2,0.2);
  // ZoomInBut->Draw();
}
void TNudyElementTable::ZoomIn()
{

  fLOD += 0.2;
  Update();
}
void TNudyElementTable::ZoomOut()
{
  if (fLOD > 0.3) fLOD -= 0.2;
  Update();
}
void TNudyElementTable::MoveUp()
{
  fOy -= 10;
  Update();
}
void TNudyElementTable::MoveDown()
{
  fOy += 10;
  Update();
}
void TNudyElementTable::MoveLeft()
{
  fOx += 10;
  Update();
}
void TNudyElementTable::MoveRight()
{
  fOx -= 10;
  Update();
}
void TNudyElementTable::Update()
{
  Nudy::TNudyElementRN *newEle;
  for (int i = 0; i < fTable->GetElementsRN()->GetEntries(); i++) {
    newEle = (Nudy::TNudyElementRN *)fEleBox.At(i);
    //    std::cout<<newEle->GetZ()*10+fOx<<", "<<newEle->GetA()*10+fOy<<std::endl;
    newEle->SetScale(fLOD);
    newEle->Move((newEle->GetZ() - newEle->GetA()) * 10 + fOx, newEle->GetA() * 10 + fOy);
  }
  fRNTable->Modified();
  fRNTable->Update();
}
void TNudyElementTable::Draw(const char * /*option*/)
{
  fRNTable = new TCanvas("RN Table", "Radio Nucleide Table", 1000, 1000);
  fRNTable->SetBorderMode(0);
  fRNTable->SetFillColor(TColor::GetColor(60, 90, 140));
  fRNTable->Range(0, 0, 1000, 1000);
  // fRNTable->SetEditable(true);
  for (int i = 0; i < fTable->GetElementsRN()->GetEntries(); i++) {
    Nudy::TNudyElementRN *newEle;
    TGeoElementRN *newRN = (TGeoElementRN *)(fTable->GetElementsRN()->At(i));
    newEle =
        new Nudy::TNudyElementRN(newRN, (newRN->MassNo() - newRN->AtomicNo()) * 10 + fOx, newRN->AtomicNo() * 10 + fOy);
    //  std::cout<<"Drawing"<<newRN->AtomicNo()*10+fOx<<","<<newRN->MassNo()*10+fOy<<std::endl;
    //    newEle->Move(newEle->GetZ()*10+fOx,newEle->GetA()*10+fOy);
    newEle->Draw();
    fEleBox.Add(newEle);
  }
  InitializeControls();
  for (int j = 0; j < 9; j++) {
    ((TButton *)fControls.At(j))->Draw();
  }
  // fRNTable->SetEditable(false);
}
TNudyElementTable::~TNudyElementTable()
{
  if (fRNTable) {
    delete fRNTable;
    fRNTable = 0;
  }
  delete fGeom;
  fGeom  = 0;
  fTable = 0;
}

} // namespace
