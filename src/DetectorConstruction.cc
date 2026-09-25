#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <cmath>
#include <sstream>
#include <vector>

namespace {
// Representative sedimentary phosphate rock, oxide wt% (renormalised in code).
// PLACEHOLDER composition: replace with a real assay / published Moroccan
// phosphorite data before quoting any absolute number.
struct Oxide { const char* el; G4int nEl; G4int nO; G4double wt; };
const std::vector<Oxide> kPhosphateRock = {
  {"Ca", 1, 1, 51.0},  // CaO
  {"P",  2, 5, 31.5},  // P2O5
  {"F",  1, 0,  3.8},  // F (fluorapatite)
  {"C",  1, 2,  6.0},  // CO2 (carbonate)
  {"Si", 1, 2,  3.0},  // SiO2
  {"S",  1, 3,  1.5},  // SO3
  {"Na", 2, 1,  0.9},  // Na2O
  {"Mg", 1, 1,  0.5},  // MgO
  {"Al", 2, 3,  0.5},  // Al2O3
  {"Fe", 2, 3,  0.2},  // Fe2O3
  {"K",  2, 1,  0.1},  // K2O
};
}  // namespace

DetectorConstruction::DetectorConstruction()
  : fDensity(2.0*g/cm3), fThickness(5.0*mm)
{
  // default traces (ppm by mass) -- placeholders, override from macros
  fTracePPM = {{"U", 120.}, {"Th", 10.}, {"Cd", 30.}, {"Sr", 1000.}};
  fMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction() { delete fMessenger; }

void DetectorConstruction::SetTracePPM(const G4String& el, G4double ppm)
{
  if (ppm <= 0.) fTracePPM.erase(el);
  else           fTracePPM[el] = ppm;
}

G4String DetectorConstruction::DescribeSample() const
{
  std::ostringstream os;
  os << "matrix " << fMatrix << "\n"
     << "density_g_cm3 " << fDensity/(g/cm3) << "\n"
     << "thickness_mm " << fThickness/mm << "\n";
  for (const auto& kv : fTracePPM) os << "ppm_" << kv.first << " " << kv.second << "\n";
  return os.str();
}

G4Material* DetectorConstruction::BuildSampleMaterial()
{
  auto* nist = G4NistManager::Instance();
  std::map<G4String, G4double> frac;   // element -> mass fraction

  if (fMatrix == "phosphate") {
    const G4double mO = nist->FindOrBuildElement("O")->GetA()/(g/mole);
    for (const auto& ox : kPhosphateRock) {
      const G4double mEl = nist->FindOrBuildElement(ox.el)->GetA()/(g/mole);
      const G4double mOx = ox.nEl*mEl + ox.nO*mO;
      frac[ox.el] += ox.wt*ox.nEl*mEl/mOx;
      if (ox.nO > 0) frac["O"] += ox.wt*ox.nO*mO/mOx;
    }
  } else {
    G4Material* base = nist->FindOrBuildMaterial(fMatrix);
    if (base == nullptr) {
      G4String msg = "Unknown matrix '" + fMatrix +
                     "'. Use 'phosphate' or a NIST name, e.g. G4_CELLULOSE_CELLOPHANE.";
      G4Exception("DetectorConstruction::BuildSampleMaterial", "XRF001",
                  FatalException, msg.c_str());
    }
    const auto* elv = base->GetElementVector();
    const G4double* fv = base->GetFractionVector();
    for (std::size_t i = 0; i < base->GetNumberOfElements(); ++i)
      frac[(*elv)[i]->GetSymbol()] += fv[i];
  }

  // normalise the major-element matrix, then make room for the traces
  G4double sum = 0.;
  for (const auto& kv : frac) sum += kv.second;
  G4double traceTot = 0.;
  for (const auto& kv : fTracePPM) traceTot += kv.second*1e-6;
  for (auto& kv : frac) kv.second *= (1. - traceTot)/sum;
  for (const auto& kv : fTracePPM) frac[kv.first] += kv.second*1e-6;

  std::ostringstream name;
  name << "Sample_" << fMatrix << "_" << fBuildCount++;
  auto* mat = new G4Material(name.str(), fDensity, static_cast<G4int>(frac.size()));
  for (const auto& kv : frac) mat->AddElement(nist->FindOrBuildElement(kv.first), kv.second);
  return mat;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // allow full rebuild on /run/reinitializeGeometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  auto* nist = G4NistManager::Instance();
  G4Material* vacuum = nist->FindOrBuildMaterial("G4_Galactic");
  G4Material* si     = nist->FindOrBuildMaterial("G4_Si");
  G4Material* be     = nist->FindOrBuildMaterial("G4_Be");
  G4Material* sample = BuildSampleMaterial();

  G4cout << "\n=== Sample ===\n" << DescribeSample() << sample << G4endl;

  // ---- world (vacuum: keeps P/Ca K lines; real XRF often uses He flush or vacuum)
  auto* worldS  = new G4Box("World", 20*cm, 20*cm, 20*cm);
  auto* worldLV = new G4LogicalVolume(worldS, vacuum, "World");
  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  auto* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World",
                                    nullptr, false, 0, true);

  // ---- sample pellet: irradiated face centred at origin, outward normal (1,0,-1)/sqrt2
  //      -> 45 deg incidence (beam along +z) and 45 deg take-off (detector along +x)
  const G4double pelletR = 10*mm;
  auto* sampleS  = new G4Tubs("Sample", 0., pelletR, 0.5*fThickness, 0., twopi);
  auto* sampleLV = new G4LogicalVolume(sampleS, sample, "Sample");
  sampleLV->SetVisAttributes(G4VisAttributes(G4Colour(0.85, 0.75, 0.5)));
  G4RotationMatrix sampleRot;
  sampleRot.rotateY(-45*deg);                       // maps (0,0,-1) -> (1,0,-1)/sqrt2
  const G4ThreeVector frontNormal(1./std::sqrt(2.), 0., -1./std::sqrt(2.));
  const G4ThreeVector sampleCentre = -0.5*fThickness*frontNormal;
  new G4PVPlacement(G4Transform3D(sampleRot, sampleCentre), sampleLV, "Sample",
                    worldLV, false, 0, true);

  // ---- detector: SDD-like Si crystal behind a Be window, axis along +x
  const G4double detR    = 4.0*mm;     // ~50 mm2 active area
  const G4double detT    = 0.5*mm;     // SDD thickness (use ~3 mm for Si(Li))
  const G4double beT     = 12.5*um;    // Be entrance window
  const G4double detDist = 15.0*mm;    // origin -> crystal front face
  const G4double winGap  = 1.0*mm;
  G4RotationMatrix detRot;
  detRot.rotateY(90*deg);              // cylinder axis z -> x

  auto* beS  = new G4Tubs("BeWindow", 0., detR, 0.5*beT, 0., twopi);
  auto* beLV = new G4LogicalVolume(beS, be, "BeWindow");
  new G4PVPlacement(G4Transform3D(detRot, G4ThreeVector(detDist - winGap - 0.5*beT, 0., 0.)),
                    beLV, "BeWindow", worldLV, false, 0, true);

  auto* detS  = new G4Tubs("Detector", 0., detR, 0.5*detT, 0., twopi);
  fDetectorLV = new G4LogicalVolume(detS, si, "Detector");
  fDetectorLV->SetVisAttributes(G4VisAttributes(G4Colour(0.2, 0.4, 0.9)));
  new G4PVPlacement(G4Transform3D(detRot, G4ThreeVector(detDist + 0.5*detT, 0., 0.)),
                    fDetectorLV, "Detector", worldLV, false, 0, true);

  return worldPV;
}
