#include "TARCData.h"

using namespace tarcapp;

TARCDataPerPrimary::TARCDataPerPrimary() { Clear();}

void TARCDataPerPrimary::Clear() {
  fNumChargedSteps = fNumNeutralSteps = fChargedTrackL = fNeutralTrackL = 0.0;
  fNumGammas       = fNumElectrons    = fNumPositrons  = fNumNeutrons   = 0.0;
  fEdep            = 0.0;
}

TARCDataPerPrimary& TARCDataPerPrimary::operator+=(const TARCDataPerPrimary& other) {
  fNumChargedSteps   += other.fNumChargedSteps;
  fNumNeutralSteps   += other.fNumNeutralSteps;
  fChargedTrackL     += other.fChargedTrackL;
  fNeutralTrackL     += other.fNeutralTrackL;
  fNumGammas         += other.fNumGammas;
  fNumElectrons      += other.fNumElectrons;
  fNumPositrons      += other.fNumPositrons;
  fNumProtons        += other.fNumProtons;
  fNumNeutrons       += other.fNumNeutrons;
  fEdep              += other.fEdep;
  return *this;
}

void TARCDataPerPrimary::Print(){
  std::cout << " Edep   = "                 << fEdep          << " [GeV]\n"
            << " Track Length (charged) = " << fChargedTrackL << " [cm] \n"
            << " Track Length (neutral) = " << fNeutralTrackL << " [cm] \n"
            << " Secondary Electrons    = " << fNumElectrons  << "      \n"
            << std::endl;
}

TARCDataPerPrimaryType::TARCDataPerPrimaryType() { Clear();}

void TARCDataPerPrimaryType::TARCDataPerPrimaryType::Clear() {
  fNumPrimaries   = 0.0;
  fNumChargedSteps = fNumNeutralSteps = fNumChargedSteps2 = fNumNeutralSteps2 = 0.0;
  fChargedTrackL  = fNeutralTrackL   = fChargedTrackL2   = fNeutralTrackL2    = 0.0;
  fNumGammas      = fNumGammas2      = fNumElectrons     = fNumElectrons2     = 0.0;
  fNumPositrons   = fNumPositrons2   = fNumProtons       = fNumProtons2       = 0.0;
  fNumNeutrons    = fNumNeutrons2                                             = 0.0;
  fEdep           = fEdep2                                                    = 0.0;
}

void TARCDataPerPrimaryType::AddDataPerPrimary(TARCDataPerPrimary& data){
  AddOnePrimary   ();
  AddChargedSteps (data.GetChargedSteps());
  AddNeutralSteps (data.GetNeutralSteps());
  AddChargedTrackL(data.GetChargedTrackL());
  AddNeutralTrackL(data.GetNeutralTrackL());
  AddGammas       (data.GetGammas());
  AddElectrons    (data.GetElectrons());
  AddPositrons    (data.GetPositrons());
  AddProtons      (data.GetProtons());
  AddNeutrons     (data.GetNeutrons());
  AddEdep         (data.GetEdep());
}

TARCData::TARCData(int nprimarytypes) : fNumPrimaryTypes(nprimarytypes) {
  fDataPerPrimaryType.resize(fNumPrimaryTypes);
  Clear();
}

TARCData::~TARCData(){
  fDataPerPrimaryType.clear();
}

void TARCData::Clear() {
  for (int ipt = 0; ipt < fNumPrimaryTypes; ++ ipt)
    fDataPerPrimaryType[ipt].Clear();
}

void TARCData::AddDataPerPrimaryType(TARCDataPerPrimary& data, int ptypeindex) {
  fDataPerPrimaryType[ptypeindex].AddDataPerPrimary(data);
}

//-------------------DATA Event ----------------------------
TARCDataPerEvent::TARCDataPerEvent(int nprimaryevent) : fNumPrimaryPerEvent(nprimaryevent) {
  fPerPrimaryData.resize(fNumPrimaryPerEvent);
  Clear();
}

void TARCDataPerEvent::Clear() {
  for (int i = 0; i < fNumPrimaryPerEvent; ++i)
    fPerPrimaryData[i].Clear();
}

TARCDataPerEvent& TARCDataPerEvent::operator+=(const TARCDataPerEvent &other) {
  for (int i = 0; i < fNumPrimaryPerEvent; ++i)
    fPerPrimaryData[i] += other.fPerPrimaryData[i];
  return *this;
}


//------------------------ TARC Thread Data Events ------------------------
TARCThreadDataEvents::TARCThreadDataEvents(int nevtbuffered, int nprimaryevent) : fNumBufferedEvents(nevtbuffered) {
  fPerEventData.reserve(fNumBufferedEvents);
  for (int i = 0; i < fNumBufferedEvents; ++i)
    fPerEventData.push_back(TARCDataPerEvent(nprimaryevent));
}

bool TARCThreadDataEvents::Merge(int evtslotindex, const TARCThreadDataEvents& other) {
  fPerEventData[evtslotindex] += other.GetDataPerEvent(evtslotindex);
  return true;
}
