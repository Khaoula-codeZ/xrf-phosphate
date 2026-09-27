#!/bin/bash
set -u
B=$(pwd)
mkdir -p out out_old runs
mv out/matrix_* out/thick_* out_old/ 2>/dev/null
run_point () {
  mkdir -p "runs/$1/out"
  printf '%s\n' "/run/numberOfThreads 8" "/random/setSeeds $RANDOM $RANDOM" "$2" "/run/initialize" "/control/execute $B/macros/source_xrf.mac" "/run/beamOn $3" > "runs/$1/run.mac"
  echo ">>> $1"
  ( cd "runs/$1" && "$B/xrf" run.mac > log.txt 2>&1 )
  f=$(find "runs/$1" -name '*_h1_spectrum.csv' | head -1)
  if [ -n "$f" ]; then cp "$f" "out/$1_h1_spectrum.csv"; echo "    saved"; else echo "    NO OUTPUT (see runs/$1/log.txt)"; fi
}
for c in 100 250 500 1000 2000; do
  run_point "matrix_phosphate_U$c" "/xrf/sample/matrix phosphate
/xrf/sample/density 2.0 g/cm3
/xrf/sample/thickness 5 mm
/xrf/sample/clearTraces
/xrf/sample/ppm U $c" 20000000
  run_point "matrix_cellulose_U$c" "/xrf/sample/matrix G4_CELLULOSE_CELLOPHANE
/xrf/sample/density 1.3 g/cm3
/xrf/sample/thickness 5 mm
/xrf/sample/clearTraces
/xrf/sample/ppm U $c" 20000000
done
for t in 0.02 0.05 0.1 0.2 0.35 0.5 1.0; do
  run_point "thick_${t}mm" "/xrf/sample/matrix phosphate
/xrf/sample/density 2.0 g/cm3
/xrf/sample/clearTraces
/xrf/sample/ppm U 2000
/xrf/sample/thickness ${t} mm" 15000000
done
echo "ALL DONE"
