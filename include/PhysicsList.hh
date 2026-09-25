#pragma once
#include "G4VModularPhysicsList.hh"

// Livermore low-energy EM + atomic de-excitation (fluorescence, PIXE).
class PhysicsList : public G4VModularPhysicsList
{
public:
  PhysicsList();
  ~PhysicsList() override = default;
};
