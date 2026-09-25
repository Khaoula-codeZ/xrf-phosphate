#pragma once
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include <map>

class G4LogicalVolume;
class G4Material;
class DetectorMessenger;

// Geometry: pressed sample pellet (front face tilted 45 deg), beam along +z,
// SDD-like Si detector with Be window along +x (90 deg geometry). World = vacuum.
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  ~DetectorConstruction() override;

  G4VPhysicalVolume* Construct() override;

  // sample parameters (take effect after /run/reinitializeGeometry when Idle)
  void SetMatrix(const G4String& m) { fMatrix = m; }
  void SetDensity(G4double d) { fDensity = d; }
  void SetThickness(G4double t) { fThickness = t; }
  void SetTracePPM(const G4String& element, G4double ppm);
  void ClearTraces() { fTracePPM.clear(); }

  G4String DescribeSample() const;       // "key value" lines, also written to *_meta.txt
  G4LogicalVolume* GetDetectorLV() const { return fDetectorLV; }

private:
  G4Material* BuildSampleMaterial();

  G4String fMatrix = "phosphate";
  G4double fDensity;
  G4double fThickness;
  std::map<G4String, G4double> fTracePPM;  // element symbol -> ppm by mass

  G4LogicalVolume* fDetectorLV = nullptr;
  DetectorMessenger* fMessenger = nullptr;
  G4int fBuildCount = 0;
};
