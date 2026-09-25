#include "PhysicsList.hh"

#include "G4EmLivermorePhysics.hh"
#include "G4EmParameters.hh"
#include "G4ProductionCutsTable.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList()
{
  SetVerboseLevel(1);

  // Livermore: photoelectric, Compton (Doppler broadening), Rayleigh, low-energy e-/hadron ionisation
  RegisterPhysics(new G4EmLivermorePhysics());

  auto* p = G4EmParameters::Instance();
  p->SetFluo(true);                       // characteristic X-rays after photoabsorption
  p->SetPixe(true);                       // inner-shell ionisation by charged particles (PIXE)
  p->SetAuger(false);                     // Auger e- are stopped by the Be window; enable if needed
  p->SetDeexcitationIgnoreCut(true);      // produce fluorescence regardless of production cuts
  p->SetPIXECrossSectionModel("ECPSSR_FormFactor");
  p->SetPIXEElectronCrossSectionModel("Livermore");
  // Optional, more accurate tabulated line energies (check your Geant4 version's API):
  //   /process/em/fluoBearden true

  SetDefaultCutValue(1*um);
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250*eV, 1*GeV);
}
