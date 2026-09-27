# xrf-phosphate — Monte Carlo simulation of XRF/PIXE for trace uranium in phosphate ore

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.22958737.svg)](https://doi.org/10.5281/zenodo.22958737)

A Geant4 model of energy-dispersive X-ray fluorescence (XRF) and particle-induced X-ray emission (PIXE)
for quantifying trace radionuclides and heavy metals in Moroccan phosphate rock. It quantifies two
effects that bias real trace-uranium assays: **matrix absorption** and **sample self-absorption**, and
validates the transport model against analytical XRF theory.

## Sample

Beneficiated **Khouribga (Morocco) phosphate rock**, measured composition from Ryszko, Rusek &
Kołodyńska (2023), *Materials* 16, 793, Table 5 (sample PR5), [doi:10.3390/ma16020793](https://doi.org/10.3390/ma16020793):
CaO 55.8, P₂O₅ 31.2, SiO₂ 5.77, F 4.07, SO₃ 2.36, Fe₂O₃ 2.31, Na₂O 0.60, Al₂O₃ 0.51, MgO 0.41, K₂O 0.13,
SrO 0.12 wt% (renormalised), with U 106 ppm and Cd 16 ppm. Pressed-pellet density 2.0 g/cm³.

## Setup

- 30 keV monochromatic photon beam, 45° incidence / 45° take-off (90° scattering geometry), vacuum.
- SDD-like Si detector (0.5 mm, ~50 mm²) behind a 12.5 µm Be window.
- Physics: `G4EmLivermorePhysics` with fluorescence, PIXE (ECPSSR), Rayleigh and Doppler-broadened Compton.
- Detector resolution (Fano + electronic noise) applied in post-processing.
- Each study point runs as an independent process with its own random seed.

## Results

### 1. Matrix effect on uranium calibration

![Matrix effect](figures/matrix_effect.png)

U Lα sensitivity is **0.048 ± 0.004** counts/ppm in phosphate rock versus **0.582 ± 0.013** counts/ppm in a
cellulose-binder standard (5 mm pellets, 2×10⁷ primaries per point): a **12-fold suppression**. Quantifying
phosphate ore against a cellulose calibration would report only **~8% of the true uranium content**,
showing why matrix-matched standards or fundamental-parameter corrections are essential.

### 2. Self-absorption and infinite-thickness criterion

![Thickness saturation](figures/thickness_saturation.png)

U Lα intensity versus pellet thickness follows I∞(1 − e^(−t/τ)) with **τ = 123 ± 24 µm**, reaching 99% of
the infinite-thickness signal at ~0.6 mm. Pellets of **≥ 1 mm** are recommended for thickness-independent
trace-U measurements.

### 3. Validation against analytical XRF theory

![Validation](figures/selfabsorption_validation.png)

The analytical thick-target attenuation depth, computed from tabulated mass-attenuation coefficients
(xraydb) for the same matrix and geometry, is **τ = 156 µm**. The Monte Carlo value agrees within
1.4σ (ratio 0.79), confirming that the transport model reproduces first-principles X-ray physics.

## Build and run (Geant4 ≥ 11.0)

```bash
mkdir build && cd build && cmake .. && make -j
./xrf macros/xrf.mac     # single spectrum
./xrf macros/pixe.mac    # 3 MeV proton PIXE
bash ../run_studies.sh   # matrix-effect + thickness studies
python ../analysis/analyze.py matrix out
python ../analysis/analyze.py thickness out
python ../analysis/validate_selfabsorption.py out   # requires: pip install xraydb
```

Sample commands: `/xrf/sample/matrix`, `/xrf/sample/density`, `/xrf/sample/thickness`,
`/xrf/sample/ppm <El> <ppm>`, `/xrf/sample/clearTraces`.

## Limitations

- Monochromatic excitation; a realistic tube spectrum (e.g. from SpekPy) can be loaded with `/gps/ene/type Arb`.
- Trace-line statistics are limited; forced-detection variance reduction would reduce run times.
- Detector geometry is generic; matching a specific instrument would allow direct comparison with measurements.

## Author

Khaoula Younous — Mohammed V University, Rabat · ORCID [0009-0003-1492-6787](https://orcid.org/0009-0003-1492-6787)
