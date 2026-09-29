#pragma once
// Mass energy-absorption coefficient of dry air (NIST, Hubbell & Seltzer), cm2/g.
#include <cmath>
namespace air {
constexpr double kE[]  = {0.010,0.015,0.020,0.030,0.040,0.050,0.060,0.080,0.100,0.150,0.200,
                          0.300,0.400,0.500,0.600,0.800,1.000,1.250,1.500,2.000,3.000,4.000,5.000};
constexpr double kMu[] = {4.742,1.334,0.5389,0.1537,0.06833,0.04098,0.03041,0.02407,0.02325,
                          0.02496,0.02672,0.02872,0.02949,0.02966,0.02953,0.02882,0.02789,
                          0.02666,0.02547,0.02345,0.02057,0.01870,0.01740};
constexpr int kN = sizeof(kE)/sizeof(kE[0]);
// log-log interpolation, E in MeV
inline double MuEnRho(double eMeV) {
  if (eMeV <= kE[0]) return kMu[0];
  if (eMeV >= kE[kN-1]) return kMu[kN-1];
  int i = 0; while (kE[i+1] < eMeV) ++i;
  const double f = std::log(eMeV/kE[i]) / std::log(kE[i+1]/kE[i]);
  return std::exp(std::log(kMu[i]) + f*std::log(kMu[i+1]/kMu[i]));
}
}  // namespace air
