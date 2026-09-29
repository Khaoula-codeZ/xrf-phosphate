#pragma once
#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
class DetectorConstruction;
class RunAction : public G4UserRunAction {
public:
  explicit RunAction(const DetectorConstruction* det);
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run* run) override;
  void AddEvent(double k) { fSum += k; fSum2 += k*k; }
private:
  const DetectorConstruction* fDet;
  G4Accumulable<G4double> fSum  = 0.;   // sum over events of E*l*muen/rho  [MeV cm3/g]
  G4Accumulable<G4double> fSum2 = 0.;
};
