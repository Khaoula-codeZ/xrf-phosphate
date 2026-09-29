#include "StackingAction.hh"
#include "G4Track.hh"
#include "G4Gamma.hh"
#include "G4Ions.hh"

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
{
  if (track->GetParentID() == 0) return fUrgent;                      // the decaying primary
  const auto* def = track->GetDefinition();
  if (def == G4Gamma::Definition()) return fUrgent;
  if (def->IsGeneralIon()) {
    const auto* ion = static_cast<const G4Ions*>(def);
    if (ion->GetExcitationEnergy() > 0.) return fUrgent;             // will emit gammas
  }
  return fKill;
}
