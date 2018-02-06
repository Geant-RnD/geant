#ifndef TARCDATA_H
#define TARCDATA_H

#include <vector>
#include <iostream>

namespace tarcapp {
  class TARCDataPerPrimary {
  public:
    TARCDataPerPrimary();
    ~TARCDataPerPrimary(){};

    void   AddChargedStep()                  {fNumChargedSteps += 1.0; }
    double GetChargedSteps() const           {return fNumChargedSteps;}

    void AddNeutralStep()                    {fNumNeutralSteps += 1.0;}
    double GetNeutralSteps() const           {return fNumNeutralSteps;}

    void AddChargedTrackL(double val)        {fChargedTrackL += val;}
    double GetChargedTrackL() const          {return fChargedTrackL;}

    void AddNeutralTrackL(double val)        {fNeutralTrackL += val;}
    double GetNeutralTrackL() const          {return fNeutralTrackL;}

    void AddGamma()                          {fNumGammas += 1.0;}
    double GetGammas() const                 {return fNumGammas;}

    void AddElectron()                       {fNumElectrons += 1.0;}
    double GetElectrons() const              {return fNumElectrons;}

    void AddPositron()                       {fNumPositrons += 1.0;}
    double GetPositrons() const              {return fNumPositrons;}

    void AddProton()                         {fNumProtons += 1.0;}
    double GetProtons() const                {return fNumProtons;}

    void AddNeutron()                        {fNumNeutrons += 1.0;}
    double GetNeutrons() const               {return fNumNeutrons;}

    void AddEdep(double val)                 {fEdep += val;}
    double GetEdep() const                   {return fEdep;}

    void Print();
    void Clear();
    TARCDataPerPrimary& operator+=(const TARCDataPerPrimary& other);

  private:
    double fNumNeutralSteps;       // mean number of charged steps per primary
    double fNumChargedSteps;       // mean number of neutral steps per primary
    double fChargedTrackL;         // mean number of charged Track length per primary
    double fNeutralTrackL;         // mean number of neutral Track length per primary
    double fNumGammas;             // mean number of secondary gamma per primary
    double fNumElectrons;          // mean number of secondary Electrons per primary
    double fNumPositrons;          // mean number of secondary Positrons per primary
    double fNumProtons;            // mean number of secondary protons per primary
    double fNumNeutrons;
    double fEdep;                  // mean Energy deposited per primary
  };

// Data structure to accumulate per-primary data for a given primary type during the simulation. Used in the global
// TARCData object and there will be as many object from this class as many different primary particle candidates
  class TARCDataPerPrimaryType{
  public:
    TARCDataPerPrimaryType();
    ~TARCDataPerPrimaryType(){};

    void AddOnePrimary()                    { fNumPrimaries += 1.0;}
    double GetNumPrimaries() const          { return fNumPrimaries;}

    void AddChargedSteps(double val)        {fNumChargedSteps += val; fNumChargedSteps2 += val * val;}
    double GetChargedSteps() const          {return fNumChargedSteps;}
    double GetChargedSteps2() const         {return fNumChargedSteps2;}

    void AddNeutralSteps(double val)        {fNumNeutralSteps += val; fNumNeutralSteps2 += val * val;}
    double GetNeutralSteps() const          {return fNumNeutralSteps;}
    double GetNeutralSteps2() const         {return fNumNeutralSteps2;}

    void AddChargedTrackL(double val)       {fChargedTrackL += val; fChargedTrackL2 += val * val;}
    double GetChargedTrackL() const         {return fChargedTrackL;}
    double GetChargedTrackL2() const        {return fChargedTrackL2;}

    void AddNeutralTrackL(double val)       {fNeutralTrackL += val; fNeutralTrackL2 += val * val;}
    double GetNeutralTrackL() const         {return fNeutralTrackL;}
    double GetNeutralTrackL2() const        {return fNeutralTrackL2;}

    void AddGammas(double val)              {fNumGammas += val; fNumGammas2 += val * val;}
    double GetGammas() const                {return fNumGammas;}
    double GetGammas2() const               {return fNumGammas2;}

    void AddElectrons(double val)           {fNumElectrons += val; fNumElectrons2 += val * val;}
    double GetElectrons() const             {return fNumElectrons;}
    double GetElectrons2() const            {return fNumElectrons2;}

    void AddPositrons(double val)           {fNumPositrons += val; fNumPositrons2 += val * val;}
    double GetPositrons() const             {return fNumPositrons;}
    double GetPositrons2() const            {return fNumPositrons2;}

    void AddProtons(double val)             {fNumProtons += val; fNumProtons2 += val * val;}
    double GetProtons() const               {return fNumProtons;}
    double GetProtons2() const              {return fNumProtons2;}

    void AddNeutrons(double val)             {fNumNeutrons += val; fNumNeutrons2 += val * val;}
    double GetNeutrons() const               {return fNumNeutrons;}
    double GetNeutrons2() const              {return fNumNeutrons2;}

    void AddEdep(double val)                {fEdep += val; fEdep2 += val * val;}
    double GetEdep() const                  {return fEdep;}
    double GetEdep2() const                 {return fEdep2;}

    void Print();
    void Clear();
    void AddDataPerPrimary(TARCDataPerPrimary& data);

  private:
    double fNumPrimaries;          // Number of Primaries from this type
    double fNumChargedSteps;       // mean number of neutral steps per primary
    double fNumChargedSteps2;       // mean number of neutral steps per primary squared
    double fNumNeutralSteps;       // mean number of charged steps per primary
    double fNumNeutralSteps2;       // mean number of charged steps per primary squared

    double fChargedTrackL;         // mean number of charged Track length per primary
    double fChargedTrackL2;         // mean number of charged Track length per primary squared
    double fNeutralTrackL;         // mean number of neutral Track length per primary
    double fNeutralTrackL2;         // mean number of neutral Track length per primary squared

    double fNumGammas;             // mean number of secondary gamma per primary
    double fNumGammas2;             // mean number of secondary gamma per primary squared
    double fNumElectrons;          // mean number of secondary Electrons per primary
    double fNumElectrons2;          // mean number of secondary Electrons per primary squared
    double fNumPositrons;          // mean number of secondary Positrons per primary
    double fNumPositrons2;          // mean number of secondary Positrons per primary squared
    double fNumProtons;            // mean number of secondary protons per primary
    double fNumProtons2;            // mean number of secondary protons per primary squared
    double fNumNeutrons;
    double fNumNeutrons2;

    double fEdep;                  // mean Energy deposited per primary
    double fEdep2;                  // mean Energy deposited per primary squared
  };

  // Global data structure to accumulate per primary data for all primary Types
  class TARCData {
  public:
    TARCData(int primarytypes = 1);
    ~TARCData();

    void Clear();

    // add data after one primary finished tracking with a given primary type index
    void AddDataPerPrimaryType(TARCDataPerPrimary& data, int primarytypeindex);

    const TARCDataPerPrimaryType& GetDataPerPrimaryType(const int primarytypeindex) const {
      return fDataPerPrimaryType[primarytypeindex];
    }

    int GetNumberOfPrimaryTypes() const { return fNumPrimaryTypes;}

  private:
    int fNumPrimaryTypes;
    std::vector<TARCDataPerPrimaryType> fDataPerPrimaryType;
  };

  // Data Structure per-event for (contains as many per-primary data structure as number of primaries in the event)
  class TARCDataPerEvent{
  public:
    TARCDataPerEvent(int nPrimaryPerEvent = 1);
    ~TARCDataPerEvent(){}

    void Clear();
    int GetNumberOfPrimaryPerEvent() const {return fNumPrimaryPerEvent;}
    TARCDataPerPrimary& GetDataPerPrimary(int primaryindex) { return fPerPrimaryData[primaryindex];}
    TARCDataPerEvent& operator+=(const TARCDataPerEvent& other);

  private:
    int fNumPrimaryPerEvent;
    std::vector<TARCDataPerPrimary> fPerPrimaryData; // as many as primary particle in an event
  };


  //--------------------------------------------------------------------------------------------------------------------//
  //----  Taken from CMSapp as example
  //--------------------------------------------------------------------------------------------------------------------//
// Thread local data structure for tarcapp to collecet/handle thread local multiple per-event data structures (as many
// per-event data structures as number of events are transported on the same time). Each of the currently transported
// events occupies one possible event-slot and per-event data can be identified by the index of this event-slot. This
// user defined thread local data needs to implement both the Merge and Clear methods for a given event-slot index:
// these methods are called when a data per-event,that corresponds to the completed event(with a given event-slot index),
// is merged from all threads.
class TARCThreadDataEvents {
public:
  TARCThreadDataEvents(int nevtbuffered, int nprimaryperevent);
 ~TARCThreadDataEvents() {}

  void  Clear(int evtslotindex) { fPerEventData[evtslotindex].Clear(); }
  bool  Merge(int evtslotindex, const TARCThreadDataEvents &other);

  TARCDataPerEvent& GetDataPerEvent(int evtslotindex) { return fPerEventData[evtslotindex]; }
  const TARCDataPerEvent& GetDataPerEvent(int evtslotindex) const { return fPerEventData[evtslotindex]; }

private:
  int                             fNumBufferedEvents;
  std::vector<TARCDataPerEvent>   fPerEventData;
};

} // namespace


#endif
