#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "StackingAction.hh"
void ActionInitialization::BuildForMaster() const { SetUserAction(new RunAction(fDet)); }
void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction());
  auto* run = new RunAction(fDet);
  SetUserAction(run);
  auto* ev = new EventAction(run);
  SetUserAction(ev);
  SetUserAction(new SteppingAction(fDet, ev));
  SetUserAction(new StackingAction());
}
