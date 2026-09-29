#pragma once
#include "G4UserSteppingAction.hh"
class DetectorConstruction;
class EventAction;
// Track-length kerma estimator in the air scoring disk.
class SteppingAction : public G4UserSteppingAction {
public:
  SteppingAction(const DetectorConstruction* det, EventAction* ev) : fDet(det), fEv(ev) {}
  void UserSteppingAction(const G4Step* step) override;
private:
  const DetectorConstruction* fDet;
  EventAction* fEv;
};
