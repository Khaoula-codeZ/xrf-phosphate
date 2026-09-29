// Gamma dose rate above a phosphogypsum stack (Ra-226 series), Geant4.
// Usage: ./norm macros/run.mac
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4RadioactiveDecayPhysics.hh"

int main(int argc, char** argv)
{
  if (argc < 2) { G4cout << "usage: ./norm macros/run.mac" << G4endl; return 1; }
  auto* rm = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  auto* det = new DetectorConstruction();
  rm->SetUserInitialization(det);
  G4PhysListFactory factory;
  auto* phys = factory.GetReferencePhysList("FTFP_BERT_EMZ");   // EM option4
  phys->RegisterPhysics(new G4RadioactiveDecayPhysics());    // full ENSDF decay radiation
  rm->SetUserInitialization(phys);
  rm->SetUserInitialization(new ActionInitialization(det));
  G4UImanager::GetUIpointer()->ApplyCommand(G4String("/control/execute ") + argv[1]);
  delete rm;
  return 0;
}
