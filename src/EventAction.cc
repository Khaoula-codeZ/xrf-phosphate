#include "EventAction.hh"
#include "G4AnalysisManager.hh"

void EventAction::EndOfEventAction(const G4Event*)
{
  if (fEdep > 0.) G4AnalysisManager::Instance()->FillH1(0, fEdep);
}
