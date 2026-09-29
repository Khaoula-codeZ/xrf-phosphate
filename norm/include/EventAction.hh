#pragma once
#include "G4UserEventAction.hh"
class RunAction;
class EventAction : public G4UserEventAction {
public:
  explicit EventAction(RunAction* run) : fRun(run) {}
  void BeginOfEventAction(const G4Event*) override { fK = 0.; }
  void EndOfEventAction(const G4Event*) override;
  void Add(double k) { fK += k; }
private:
  RunAction* fRun;
  double fK = 0.;
};
