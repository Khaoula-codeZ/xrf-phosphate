#include "EventAction.hh"
#include "RunAction.hh"
void EventAction::EndOfEventAction(const G4Event*) { if (fK > 0.) fRun->AddEvent(fK); }
