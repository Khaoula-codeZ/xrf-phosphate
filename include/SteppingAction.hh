#pragma once
#include "G4UserSteppingAction.hh"

class DetectorConstruction;
class EventAction;

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(const DetectorConstruction* det, EventAction* ev) : fDet(det), fEvent(ev) {}
  void UserSteppingAction(const G4Step* step) override;
private:
  const DetectorConstruction* fDet;
  EventAction* fEvent;
};
