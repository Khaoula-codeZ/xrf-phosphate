#pragma once
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
class G4LogicalVolume;
// Matrix chosen with the environment variable NORM_MATRIX = pg (default) | soil
class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  G4VPhysicalVolume* Construct() override;
  G4LogicalVolume* GetScoringLV() const { return fScoreLV; }
  double GetScoringVolume() const { return fScoreVolume; }
  double GetStackMass() const { return fStackMass; }
  const G4String& GetMatrix() const { return fMatrix; }
private:
  G4String fMatrix = "pg";
  G4LogicalVolume* fScoreLV = nullptr;
  double fScoreVolume = 0.;
  double fStackMass = 0.;
};
