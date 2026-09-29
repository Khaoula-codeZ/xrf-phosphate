#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "AirMuEn.hh"
#include "G4Step.hh"
#include "G4Gamma.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (step->GetTrack()->GetDefinition() != G4Gamma::Definition()) return;
  const auto* pre = step->GetPreStepPoint();
  if (pre->GetTouchableHandle()->GetVolume()->GetLogicalVolume() != fDet->GetScoringLV()) return;
  const double eMeV = pre->GetKineticEnergy()/MeV;
  const double lCm  = step->GetStepLength()/cm;
  fEv->Add(eMeV*lCm*air::MuEnRho(eMeV));   // MeV cm3/g ; divide by V[cm3] -> MeV/g
}
