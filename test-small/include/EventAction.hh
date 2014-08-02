//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id$
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "RunAction.hh"
#include <time.h>

//class RunAction;
class EventActionMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
public:
  EventAction();
  virtual ~EventAction();

  void  BeginOfEventAction(const G4Event*);
  void    EndOfEventAction(const G4Event*);
    
  void AddAbs(G4double de, G4double dl) {EnergyAbs += de; TrackLAbs += dl;};
  void AddGap(G4double de, G4double dl) {EnergyGap += de; TrackLGap += dl;};
  void FillPerStep(G4int isgap, G4int layer, G4double edepo, G4double steplength,
                   G4int procIndex);
  void AddOneStep(){ ++fNSteps; }                   
  void SetPrintModulo(G4int    val)  {printModulo = val;};
  static const G4int kNlayers = 10;
  static const G4int kNProc  = 19; // 18+userCuts   

private:
   RunAction*  runAct;
   
   G4double  EnergyAbs, EnergyGap;
   G4double  TrackLAbs, TrackLGap;
   G4int     printModulo;                             
   EventActionMessenger*  eventMessenger;

   // for one event
   G4double  fEdepGap[kNlayers];   // Energy deposition per layer
   G4double  fLengthGap[kNlayers]; // step length per layer
   G4double  fEdepAbs[kNlayers];   // Energy deposition per layer
   G4double  fLengthAbs[kNlayers]; // step length per layer

   G4long    fNSteps;      

   G4long    fProcStat[kNProc]; 

   clock_t   startTime, endTime;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
