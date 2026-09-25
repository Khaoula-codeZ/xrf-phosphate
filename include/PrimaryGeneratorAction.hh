#pragma once
#include "G4VUserPrimaryGeneratorAction.hh"

class G4GeneralParticleSource;

// Source fully configured from macros with /gps/ commands (X-ray beam or proton beam).
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;
  void GeneratePrimaries(G4Event* event) override;
private:
  G4GeneralParticleSource* fGPS;
};
