# xrf-phosphate — Geant4 simulation of XRF and PIXE on phosphate rock

Monte Carlo model of energy-dispersive X-ray fluorescence (XRF) and particle-induced
X-ray emission (PIXE) for phosphate ore, focused on trace radionuclides and heavy
metals (U, Th, Cd, Sr). It quantifies two effects that bias real assays:

1. **Matrix effect**: the same U concentration gives a different U Lα signal in
   phosphate rock than in a light (cellulose-binder) standard, so calibrating with
   the wrong matrix biases the result.
2. **Self-absorption**: U Lα intensity versus pellet thickness gives the
   "infinite thickness" a pellet needs for thickness-independent results.

## Setup
- Beam along +z, pellet tilted 45°, detector at 90° (standard EDXRF geometry), in vacuum.
- Detector: SDD-like Si crystal (0.5 mm thick, ~50 mm²) behind a 12.5 µm Be window, 15 mm from the pellet.
- Physics: G4EmLivermorePhysics with fluorescence, PIXE (ECPSSR), Rayleigh, and Compton with Doppler broadening.
- Sample: built-in phosphate rock (oxide wt%) or any NIST material, with traces set in ppm from macros.
- Detector resolution (Fano + electronic noise) is applied in post-processing, so it can be changed without re-running.

## Build and run (Geant4 ≥ 11.0 with data sets)
```bash
mkdir build && cd build && cmake .. && make -j
./xrf macros/xrf.mac              # single spectrum
./xrf macros/pixe.mac             # 3 MeV proton PIXE
./xrf macros/matrix_study.mac     # U calibration: phosphate vs cellulose
./xrf macros/thickness_study.mac  # self-absorption series
./xrf                             # interactive visualisation
```

## Analysis
```bash
python analysis/analyze.py spectrum  out/xrf_phosphate
python analysis/analyze.py spectrum  out/pixe_phosphate --pixe
python analysis/analyze.py matrix    out
python analysis/analyze.py thickness out
```

## Sample commands
| command | example |
|---|---|
| `/xrf/sample/matrix` | `phosphate` or `G4_CELLULOSE_CELLOPHANE` |
| `/xrf/sample/density` | `2.0 g/cm3` |
| `/xrf/sample/thickness` | `5 mm` |
| `/xrf/sample/ppm <El> <ppm>` | `/xrf/sample/ppm U 120` |
| `/xrf/sample/clearTraces`, `/xrf/sample/print` | |

When the application is Idle, follow sample changes with `/run/reinitializeGeometry`.

## Limitations and next steps
- The phosphate composition and trace levels are **placeholders**. Replace them with a real assay.
- The source is a monochromatic 30 keV beam. A realistic tube spectrum can be loaded from SpekPy via `/gps/ene/type Arb`.
- Validation to do: compare simulated line intensities with the fundamental-parameters (Sherman) equation using xraylib.
- Trace-line statistics are low. Forced-detection variance reduction would speed up runs.
