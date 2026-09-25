#pragma once
#include "G4UserEventAction.hh"
#include "globals.hh"

// Accumulates total energy deposited in the Si crystal per event (pulse-height spectrum).
class EventAction : public G4UserEventAction
{
public:
  void BeginOfEventAction(const G4Event*) override { fEdep = 0.; }
  void EndOfEventAction(const G4Event*) override;
  void AddEdep(G4double e) { fEdep += e; }
private:
  G4double fEdep = 0.;
};
