#pragma once
#include "G4UserStackingAction.hh"
// Keep only photons and excited nuclear states (which de-excite by gamma emission).
// Ground-state daughters are killed so each event is exactly one decay; charged
// particles (alpha, beta) are killed since they deposit locally in the stack.
class StackingAction : public G4UserStackingAction {
public:
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track) override;
};
