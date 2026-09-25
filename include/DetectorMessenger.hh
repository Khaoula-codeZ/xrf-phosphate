#pragma once
#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

class DetectorMessenger : public G4UImessenger
{
public:
  explicit DetectorMessenger(DetectorConstruction* det);
  ~DetectorMessenger() override;
  void SetNewValue(G4UIcommand* cmd, G4String value) override;

private:
  DetectorConstruction* fDet;
  G4UIdirectory* fDir;
  G4UIdirectory* fSampleDir;
  G4UIcmdWithAString* fMatrixCmd;
  G4UIcmdWithADoubleAndUnit* fDensityCmd;
  G4UIcmdWithADoubleAndUnit* fThickCmd;
  G4UIcommand* fPPMCmd;
  G4UIcmdWithoutParameter* fClearCmd;
  G4UIcmdWithoutParameter* fPrintCmd;
};
