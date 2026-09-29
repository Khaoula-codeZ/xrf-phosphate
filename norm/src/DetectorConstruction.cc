#include "DetectorConstruction.hh"
#include "Config.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4PhysicalConstants.hh"
#include <cstdlib>

DetectorConstruction::DetectorConstruction()
{
  if (const char* m = std::getenv("NORM_MATRIX")) fMatrix = m;
  if (fMatrix != "pg" && fMatrix != "soil") fMatrix = "pg";
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  auto* nist = G4NistManager::Instance();
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

  G4Material* stackMat = nullptr;
  if (fMatrix == "soil") {
    // Beck (1972) standard soil: SiO2 67.5, Al2O3 13.5, Fe2O3 4.5, CO2 4.5, H2O 10 wt%
    stackMat = new G4Material("StandardSoil", cfg::kPGDensity*g/cm3, 6);
    stackMat->AddElement(nist->FindOrBuildElement("O"),  0.55808);
    stackMat->AddElement(nist->FindOrBuildElement("Si"), 0.31552);
    stackMat->AddElement(nist->FindOrBuildElement("Al"), 0.07145);
    stackMat->AddElement(nist->FindOrBuildElement("Fe"), 0.03147);
    stackMat->AddElement(nist->FindOrBuildElement("C"),  0.01228);
    stackMat->AddElement(nist->FindOrBuildElement("H"),  0.01120);
  } else {
    // Phosphogypsum: CaSO4.2H2O (Ca 23.28, S 18.62, O 55.76, H 2.34 wt%)
    stackMat = new G4Material("Phosphogypsum", cfg::kPGDensity*g/cm3, 4);
    stackMat->AddElement(nist->FindOrBuildElement("Ca"), 0.2328);
    stackMat->AddElement(nist->FindOrBuildElement("S"),  0.1862);
    stackMat->AddElement(nist->FindOrBuildElement("O"),  0.5576);
    stackMat->AddElement(nist->FindOrBuildElement("H"),  0.0234);
  }

  // world: air above and around the stack
  const double halfXY = cfg::WorldHalfXY();
  auto* worldS  = new G4Box("World", halfXY, halfXY, cfg::WorldHalfZ());
  auto* worldLV = new G4LogicalVolume(worldS, air, "World");
  auto* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", nullptr, false, 0, true);

  // PG stack: top surface at z = 0
  auto* stackS  = new G4Tubs("Stack", 0., cfg::StackRadius(), 0.5*cfg::kStackThickness, 0., twopi);
  auto* stackLV = new G4LogicalVolume(stackS, stackMat, "Stack");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -0.5*cfg::kStackThickness), stackLV, "Stack",
                    worldLV, false, 0, true);
  fStackMass = stackLV->GetMass();

  // air scoring disk centred 1 m above the surface
  auto* scoreS = new G4Tubs("Score", 0., cfg::ScoreRadius(), cfg::kScoreHalfZ, 0., twopi);
  fScoreLV = new G4LogicalVolume(scoreS, air, "Score");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, cfg::kScoreHeight), fScoreLV, "Score",
                    worldLV, false, 0, true);
  fScoreVolume = scoreS->GetCubicVolume();

  G4cout << "\n=== NORM geometry (" << fMatrix << ") ===\n  stack mass = " << fStackMass/kg << " kg"
         << "\n  scoring volume = " << fScoreVolume/m3 << " m3" << G4endl;
  return worldPV;
}
