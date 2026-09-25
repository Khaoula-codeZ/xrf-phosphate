#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4ApplicationState.hh"

#include <sstream>

DetectorMessenger::DetectorMessenger(DetectorConstruction* det) : fDet(det)
{
  fDir = new G4UIdirectory("/xrf/");
  fDir->SetGuidance("XRF/PIXE phosphate application.");
  fSampleDir = new G4UIdirectory("/xrf/sample/");
  fSampleDir->SetGuidance("Sample definition. When Idle, follow changes with /run/reinitializeGeometry.");

  fMatrixCmd = new G4UIcmdWithAString("/xrf/sample/matrix", this);
  fMatrixCmd->SetGuidance("'phosphate' (built-in rock) or any NIST material, e.g. G4_CELLULOSE_CELLOPHANE.");
  fMatrixCmd->SetParameterName("matrix", false);

  fDensityCmd = new G4UIcmdWithADoubleAndUnit("/xrf/sample/density", this);
  fDensityCmd->SetGuidance("Bulk density of the pressed pellet.");
  fDensityCmd->SetParameterName("rho", false);
  fDensityCmd->SetRange("rho>0.");
  fDensityCmd->SetDefaultUnit("g/cm3");

  fThickCmd = new G4UIcmdWithADoubleAndUnit("/xrf/sample/thickness", this);
  fThickCmd->SetGuidance("Pellet thickness.");
  fThickCmd->SetParameterName("t", false);
  fThickCmd->SetRange("t>0.");
  fThickCmd->SetDefaultUnit("mm");

  fPPMCmd = new G4UIcommand("/xrf/sample/ppm", this);
  fPPMCmd->SetGuidance("Trace element mass concentration in ppm (0 removes it). e.g. /xrf/sample/ppm U 120");
  auto* pEl = new G4UIparameter("element", 's', false);
  fPPMCmd->SetParameter(pEl);
  auto* pVal = new G4UIparameter("ppm", 'd', false);
  pVal->SetParameterRange("ppm>=0.");
  fPPMCmd->SetParameter(pVal);

  fClearCmd = new G4UIcmdWithoutParameter("/xrf/sample/clearTraces", this);
  fClearCmd->SetGuidance("Remove all trace elements.");

  fPrintCmd = new G4UIcmdWithoutParameter("/xrf/sample/print", this);
  fPrintCmd->SetGuidance("Print current sample definition.");

  for (G4UIcommand* c : {static_cast<G4UIcommand*>(fMatrixCmd), static_cast<G4UIcommand*>(fDensityCmd),
                         static_cast<G4UIcommand*>(fThickCmd), fPPMCmd,
                         static_cast<G4UIcommand*>(fClearCmd), static_cast<G4UIcommand*>(fPrintCmd)}) {
    c->AvailableForStates(G4State_PreInit, G4State_Idle);
    c->SetToBeBroadcasted(false);   // geometry lives on the master
  }
}

DetectorMessenger::~DetectorMessenger()
{
  delete fMatrixCmd; delete fDensityCmd; delete fThickCmd;
  delete fPPMCmd; delete fClearCmd; delete fPrintCmd;
  delete fSampleDir; delete fDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* cmd, G4String value)
{
  if (cmd == fMatrixCmd)       fDet->SetMatrix(value);
  else if (cmd == fDensityCmd) fDet->SetDensity(fDensityCmd->GetNewDoubleValue(value));
  else if (cmd == fThickCmd)   fDet->SetThickness(fThickCmd->GetNewDoubleValue(value));
  else if (cmd == fPPMCmd) {
    std::istringstream is(value);
    G4String el; G4double ppm = 0.;
    is >> el >> ppm;
    fDet->SetTracePPM(el, ppm);
  }
  else if (cmd == fClearCmd)   fDet->ClearTraces();
  else if (cmd == fPrintCmd)   G4cout << fDet->DescribeSample() << G4endl;
}
