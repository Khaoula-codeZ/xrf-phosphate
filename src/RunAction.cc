#include "RunAction.hh"
#include "DetectorConstruction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

#include <fstream>

RunAction::RunAction(const DetectorConstruction* det) : fDet(det)
{
  auto* am = G4AnalysisManager::Instance();
  am->SetDefaultFileType("csv");
  am->SetVerboseLevel(1);
  am->SetFileName("out/spectrum");            // override with /analysis/setFileName
  // 10 eV bins, 0-40 keV; change with /analysis/h1/set 0 <nbins> <min> <max> keV
  am->CreateH1("spectrum", "Energy deposited in Si detector", 4000, 0., 40*keV, "keV");
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4AnalysisManager::Instance()->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  auto* am = G4AnalysisManager::Instance();
  const G4String fileName = am->GetFileName();
  am->Write();                                 // worker histograms are merged on the master
  if (IsMaster()) {
    std::ofstream meta(fileName + "_meta.txt");
    meta << "events " << run->GetNumberOfEvent() << "\n" << fDet->DescribeSample();
    G4cout << "Run done: " << run->GetNumberOfEvent() << " primaries -> "
           << fileName << "_h1_spectrum.csv" << G4endl;
  }
  am->CloseFile();
}
