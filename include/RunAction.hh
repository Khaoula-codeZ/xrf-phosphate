#pragma once
#include "G4UserRunAction.hh"

class DetectorConstruction;

class RunAction : public G4UserRunAction
{
public:
  explicit RunAction(const DetectorConstruction* det);
  void BeginOfRunAction(const G4Run* run) override;
  void EndOfRunAction(const G4Run* run) override;
private:
  const DetectorConstruction* fDet;
};
