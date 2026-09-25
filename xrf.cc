// XRF / PIXE of phosphate rock -- Geant4 application
// Usage:  ./xrf                 (interactive + visualisation)
//         ./xrf macros/xrf.mac  (batch)
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

int main(int argc, char** argv)
{
  G4UIExecutive* ui = nullptr;
  if (argc == 1) ui = new G4UIExecutive(argc, argv);

  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

  auto* det = new DetectorConstruction();
  runManager->SetUserInitialization(det);
  runManager->SetUserInitialization(new PhysicsList());
  runManager->SetUserInitialization(new ActionInitialization(det));

  auto* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  auto* uiManager = G4UImanager::GetUIpointer();
  if (!ui) {
    uiManager->ApplyCommand(G4String("/control/execute ") + argv[1]);
  } else {
    uiManager->ApplyCommand("/control/execute macros/vis.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
  return 0;
}
