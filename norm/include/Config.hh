#pragma once
// ---------------------------------------------------------------------------
// Scenario: gamma dose rate at 1 m above a phosphogypsum (PG) stack.
// Ra-226 activity of Jorf Lasfar PG: 1097 +/- 31 Bq/kg
//   (Moroccan PG carbonation study, Chem. Eng. Sci. 2023, S0009250923005699;
//    same work: U-238 113 Bq/kg, Pb-210 961 Bq/kg).
// Validation reference: UNSCEAR 2000 dose-rate coefficient for the U-238
//   series in an infinite half-space: 0.462 nGy/h per Bq/kg at 1 m.
// ---------------------------------------------------------------------------
#include "G4SystemOfUnits.hh"
#include <cstdlib>
#include <cstring>

namespace cfg {
constexpr double kStackThickness = 1.0*m;     // >> relaxation length (~infinite depth)
constexpr double kPGDensity      = 1.6;       // g/cm3, dry bulk (result is density-independent
                                              // for a thick stack; mass matters for normalisation)
constexpr double kScoreHeight    = 1.0*m;     // centre height above the PG surface
constexpr double kScoreHalfZ     = 0.10*m;    // disk half-thickness (0.9-1.1 m)
constexpr double kRa226_BqPerKg  = 1097.0;    // Jorf Lasfar phosphogypsum
constexpr double kRefCoef        = 0.462;     // UNSCEAR, nGy/h per Bq/kg (U-238 series)
constexpr double kSvPerGy        = 0.7;       // UNSCEAR outdoor adult effective/absorbed
constexpr double kWorkHoursYear  = 2000.0;    // full-time occupancy on the stack
constexpr double kDecaysPerRa   = 3.0;       // Ra-226, Pb-214, Bi-214 in secular equilibrium
// Geometry scale: NORM_BIG=1 enlarges stack, scoring disk and air volume ~3x
inline bool Big() { const char* b = std::getenv("NORM_BIG"); return b && std::strcmp(b, "1") == 0; }
inline double StackRadius() { return Big() ? 150.0*m : 50.0*m; }
inline double ScoreRadius() { return Big() ?  30.0*m : 10.0*m; }
inline double WorldHalfXY() { return Big() ? 250.0*m : 70.0*m; }
inline double WorldHalfZ()  { return Big() ? 250.0*m : 40.0*m; }
}  // namespace cfg
