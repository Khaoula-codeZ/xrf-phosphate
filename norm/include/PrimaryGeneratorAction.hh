#pragma once
#include "G4VUserPrimaryGeneratorAction.hh"
class G4ParticleGun;
class G4ParticleDefinition;
// One radioactive decay per event: Ra-226, Pb-214 or Bi-214 (equal activity, secular
// equilibrium), at rest, uniformly distributed in the stack. Geant4 radioactive decay
// then emits the full ENSDF gamma and X-ray spectrum.
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;
  void GeneratePrimaries(G4Event* event) override;
private:
  G4ParticleGun* fGun;
  G4ParticleDefinition* fIons[3] = {nullptr, nullptr, nullptr};
};
