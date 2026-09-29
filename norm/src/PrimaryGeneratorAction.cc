#include "PrimaryGeneratorAction.hh"
#include "Config.hh"
#include "G4ParticleGun.hh"
#include "G4IonTable.hh"
#include "G4Event.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"
#include <cmath>

PrimaryGeneratorAction::PrimaryGeneratorAction() : fGun(new G4ParticleGun(1)) {}
PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fGun; }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  if (fIons[0] == nullptr) {            // ions exist only after physics initialisation
    auto* it = G4IonTable::GetIonTable();
    fIons[0] = it->GetIon(88, 226, 0.);  // Ra-226
    fIons[1] = it->GetIon(82, 214, 0.);  // Pb-214
    fIons[2] = it->GetIon(83, 214, 0.);  // Bi-214
  }
  const int k = static_cast<int>(3.*G4UniformRand()) % 3;
  fGun->SetParticleDefinition(fIons[k]);
  fGun->SetParticleEnergy(0.);
  fGun->SetParticleCharge(0.);

  const double r   = cfg::StackRadius()*std::sqrt(G4UniformRand());
  const double phi = twopi*G4UniformRand();
  const double z   = -cfg::kStackThickness*G4UniformRand();
  fGun->SetParticlePosition(G4ThreeVector(r*std::cos(phi), r*std::sin(phi), z));
  fGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
  fGun->GeneratePrimaryVertex(event);
}
