#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "Config.hh"
#include "G4AccumulableManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

RunAction::RunAction(const DetectorConstruction* det) : fDet(det)
{
  auto* am = G4AccumulableManager::Instance();
  am->RegisterAccumulable(fSum);
  am->RegisterAccumulable(fSum2);
}

void RunAction::BeginOfRunAction(const G4Run*) { G4AccumulableManager::Instance()->Reset(); }

void RunAction::EndOfRunAction(const G4Run* run)
{
  auto* am = G4AccumulableManager::Instance();
  am->Merge();
  if (!IsMaster()) return;
  const G4int n = run->GetNumberOfEvent();
  if (n == 0) return;

  const double vCm3 = fDet->GetScoringVolume()/cm3;
  const double mean = fSum.GetValue()/n;                          // MeV cm3/g per photon
  const double var  = std::max(0., fSum2.GetValue()/n - mean*mean);
  const double sem  = std::sqrt(var/n);
  const double mevPerG_to_Gy = 1.602176634e-10;
  const double kPerPhoton = mean/vCm3*mevPerG_to_Gy;              // Gy per emitted photon
  const double kErr       = sem/vCm3*mevPerG_to_Gy;

  const double massKg = fDet->GetStackMass()/kg;
  const double yGamma = cfg::kDecaysPerRa;                          // decays per Ra-226 decay
  // coefficient: nGy/h per (Bq/kg)
  const double coef    = kPerPhoton*yGamma*massKg*3600.*1e9;
  const double coefErr = kErr*yGamma*massKg*3600.*1e9;
  const double doseRate = coef*cfg::kRa226_BqPerKg;               // nGy/h
  const double annual   = doseRate*1e-6*cfg::kSvPerGy*cfg::kWorkHoursYear; // mSv/y

  std::ostringstream os;
  os << std::setprecision(4)
     << "primaries                 " << n << "\n"
     << "matrix                    " << fDet->GetMatrix() << (cfg::Big() ? "  (large geometry)" : "") << "\n"
     << "decays per Ra-226 decay   " << yGamma << "\n"
     << "stack mass (kg)           " << massKg << "\n"
     << "kerma per decay (Gy)      " << kPerPhoton << " +/- " << kErr << "\n"
     << "coef (nGy/h per Bq/kg)    " << coef << " +/- " << coefErr << "\n"
     << "UNSCEAR reference         " << cfg::kRefCoef << "  (ratio " << coef/cfg::kRefCoef << ")\n"
     << "Ra-226 in Jorf Lasfar PG  " << cfg::kRa226_BqPerKg << " Bq/kg\n"
     << "dose rate at 1 m (nGy/h)  " << doseRate << " +/- " << coefErr*cfg::kRa226_BqPerKg << "\n"
     << "annual external dose      " << annual << " mSv/y  (" << cfg::kWorkHoursYear
     << " h/y, " << cfg::kSvPerGy << " Sv/Gy)\n";
  G4cout << "\n===== NORM RESULT =====\n" << os.str() << "=======================" << G4endl;
  std::ofstream("norm_result_" + fDet->GetMatrix() + (cfg::Big() ? "_big" : "") + ".txt") << os.str();
}
