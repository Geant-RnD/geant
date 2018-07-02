
#include "TestNudy1Data.h"

#include "Hist1.h"

namespace userapplication {

//
// TestNudy1DataPerPrimary
TestNudy1DataPerPrimary::TestNudy1DataPerPrimary()
{
  Clear();
}

void TestNudy1DataPerPrimary::Clear()
{
  fNumChargedSteps = fNumNeutralSteps = fChargedTrackL = fNeutralTrackL = 0.;
  fNumGammas = fNumElectrons = fNumPositrons = 0.;
  fNumPrimaryTrans = fNumPrimaryRefl = fNumOneTrans = fNumOneRefl = 0.;
  fEdepInTarget = fELeakPrimary = fELeakSecondary = 0.;
}

TestNudy1DataPerPrimary &TestNudy1DataPerPrimary::operator+=(const TestNudy1DataPerPrimary &other)
{
  fNumChargedSteps += other.fNumChargedSteps;
  fNumNeutralSteps += other.fNumNeutralSteps;
  fChargedTrackL += other.fChargedTrackL;
  fNeutralTrackL += other.fNeutralTrackL;
  fNumGammas += other.fNumGammas;
  fNumElectrons += other.fNumElectrons;
  fNumPositrons += other.fNumPositrons;
  fNumPrimaryTrans += other.fNumPrimaryTrans;
  fNumPrimaryRefl += other.fNumPrimaryRefl;
  fNumOneTrans += other.fNumOneTrans;
  fNumOneRefl += other.fNumOneRefl;
  fEdepInTarget += other.fEdepInTarget;
  fELeakPrimary += other.fELeakPrimary;
  fELeakSecondary += other.fELeakSecondary;
  return *this;
}

//
// TestNudy1Data
TestNudy1Data::TestNudy1Data()
{
  Clear();
}

void TestNudy1Data::Clear()
{
  fNumChargedSteps = fNumNeutralSteps = fNumChargedSteps2 = fNumNeutralSteps2 = 0.;
  fChargedTrackL = fNeutralTrackL = fChargedTrackL2 = fNeutralTrackL2 = 0.;
  fNumGammas = fNumElectrons = fNumPositrons = 0.;
  fNumPrimaryTrans = fNumPrimaryRefl = fNumOneTrans = fNumOneRefl = 0.;
  fEdepInTarget = fEdepInTarget2 = 0.;
  fELeakPrimary = fELeakSecondary = fELeakPrimary2 = fELeakSecondary2 = 0.;
}

void TestNudy1Data::AddDataPerPrimary(TestNudy1DataPerPrimary &data)
{
  AddChargedSteps(data.GetChargedSteps());
  AddNeutralSteps(data.GetNeutralSteps());
  AddChargedTrackL(data.GetChargedTrackL());
  AddNeutralTrackL(data.GetNeutralTrackL());
  AddGammas(data.GetGammas());
  AddElectrons(data.GetElectrons());
  AddPositrons(data.GetPositrons());
  AddPrimaryTransmitted(data.GetPrimaryTransmitted());
  AddPrimaryReflected(data.GetPrimaryReflected());
  AddOneTransmitted(data.GetOneTransmitted());
  AddOneReflected(data.GetOneReflected());
  AddEdepInTarget(data.GetEdepInTarget());
  AddELeakPrimary(data.GetELeakPrimary());
  AddELeakSecondary(data.GetELeakSecondary());
}

//
// TestNudy1DataPerEvent
TestNudy1DataPerEvent::TestNudy1DataPerEvent(int nprimperevent) : fNumPrimaryPerEvent(nprimperevent)
{
  fPerPrimaryData.reserve(fNumPrimaryPerEvent);
  for (int i = 0; i < fNumPrimaryPerEvent; ++i) {
    fPerPrimaryData.push_back(TestNudy1DataPerPrimary());
  }
}

void TestNudy1DataPerEvent::Clear()
{
  for (int i = 0; i < fNumPrimaryPerEvent; ++i) {
    fPerPrimaryData[i].Clear();
  }
}

TestNudy1DataPerEvent &TestNudy1DataPerEvent::operator+=(const TestNudy1DataPerEvent &other)
{
  for (int i = 0; i < fNumPrimaryPerEvent; ++i) {
    fPerPrimaryData[i] += other.fPerPrimaryData[i];
  }
  return *this;
}

//
// TestNudy1ThreadDataEvents
TestNudy1ThreadDataEvents::TestNudy1ThreadDataEvents(int nevtbuffered, int nprimperevent) : fNumBufferedEvents(nevtbuffered)
{
  fPerEventData.reserve(fNumBufferedEvents);
  for (int i = 0; i < fNumBufferedEvents; ++i) {
    fPerEventData.push_back(TestNudy1DataPerEvent(nprimperevent));
  }
}

bool TestNudy1ThreadDataEvents::Merge(int evtslotindx, const TestNudy1ThreadDataEvents &other)
{
  fPerEventData[evtslotindx] += other.GetDataPerEvent(evtslotindx);
  return true;
}

//
// TestNudy1ThreadDataRun
TestNudy1ThreadDataRun::TestNudy1ThreadDataRun() : fHisto1(nullptr)
{
}

TestNudy1ThreadDataRun::~TestNudy1ThreadDataRun()
{
  if (fHisto1) {
    delete fHisto1;
  }
  fHisto1 = nullptr;
}

void TestNudy1ThreadDataRun::CreateHisto1(int nbins, double min, double max)
{
  if (fHisto1) {
    delete fHisto1;
  }
  fHisto1 = new Hist1(min, max, nbins);
}

bool TestNudy1ThreadDataRun::Merge(int /*evtslotindx*/, const TestNudy1ThreadDataRun &other)
{
  (*fHisto1) += *(other.GetHisto1());
  return true;
}

} // namespace userapplication
