# norm — gamma dose rate above a Moroccan phosphogypsum stack

Geant4 model of the external gamma air-kerma rate at 1 m above a phosphogypsum (PG) stack from the
Ra-226 series, the dominant external-dose source in PG from wet-process phosphoric-acid production.

- **Source:** one radioactive decay per event of Ra-226, Pb-214 or Bi-214 (secular equilibrium),
  uniform in a 50 m radius × 1 m thick stack. Geant4 radioactive decay (ENSDF) emits the full gamma
  and X-ray spectrum; charged particles are absorbed locally.
- **Scoring:** track-length air-kerma estimator (NIST μen/ρ of air) in an air disk at 0.9–1.1 m.
- **Matrices:** phosphogypsum (CaSO₄·2H₂O) or Beck (1972) standard soil, density 1.6 g/cm³.
- **Activity:** Ra-226 = 1097 Bq/kg, Jorf Lasfar PG (Chem. Eng. Sci. 2023).
- **Validation:** the standard-soil run is compared with the UNSCEAR (2000) coefficient for the U-238
  series in an infinite half-space, 0.462 nGy/h per Bq/kg.

## Run
```bash
mkdir build && cd build && cmake .. && make -j
NORM_MATRIX=soil ./norm macros/run.mac   # validation
NORM_MATRIX=pg   ./norm macros/run.mac   # phosphogypsum
cat norm_result_soil.txt norm_result_pg.txt
```
