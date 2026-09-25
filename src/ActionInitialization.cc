#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction(fDet));
}

void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction());
  SetUserAction(new RunAction(fDet));
  auto* event = new EventAction();
  SetUserAction(event);
  SetUserAction(new SteppingAction(fDet, event));
}
