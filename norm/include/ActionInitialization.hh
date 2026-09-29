#pragma once
#include "G4VUserActionInitialization.hh"
class DetectorConstruction;
class ActionInitialization : public G4VUserActionInitialization {
public:
  explicit ActionInitialization(const DetectorConstruction* det) : fDet(det) {}
  void BuildForMaster() const override;
  void Build() const override;
private:
  const DetectorConstruction* fDet;
};
