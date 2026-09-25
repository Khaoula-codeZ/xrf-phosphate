#!/usr/bin/env python3
"""Analysis for the xrf-phosphate Geant4 application.

  python analysis/analyze.py spectrum out/xrf_phosphate   # labelled spectrum + line table
  python analysis/analyze.py spectrum out/pixe_phosphate --pixe
  python analysis/analyze.py matrix out                   # U calibration: phosphate vs cellulose
  python analysis/analyze.py thickness out                # U La saturation vs pellet thickness

Geant4 scores the ideal energy deposit. The detector response (Fano + electronic
noise) is applied here, so resolution can be changed without re-running.
Quantitative results (matrix, thickness) use net line counts from the ideal
spectrum, where lines are sharp; the broadened spectrum is for display.
"""
import argparse
import glob
import os
import re

import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

# ROI windows (keV) covering each doublet (e.g. La1+La2); sidebands give the background.
LINES = {
    "P Kα": (1.95, 2.08),
    "Ca Kα": (3.62, 3.76),
    "Fe Kα": (6.33, 6.46),
    "Th Lα": (12.75, 13.05),
    "U Lα": (13.35, 13.75),
    "Sr Kα": (14.02, 14.25),
    "Sr Kβ": (15.78, 15.90),
    "U Lβ1": (17.12, 17.32),
    "Cd Kα": (22.90, 23.25),
}
SIDE = 0.10  # keV sideband width


# ---------------------------------------------------------------- I/O
def read_h1(path):
    """Read a Geant4 (tools) CSV H1; returns bin centres [keV], counts."""
    nb = lo = hi = None
    header, rows = None, []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if line.startswith("#"):
                if line.startswith("#axis fixed"):
                    parts = line.split()
                    nb, lo, hi = int(parts[2]), float(parts[3]), float(parts[4])
                continue
            if header is None:
                header = line.split(",")
                continue
            rows.append([float(x) for x in line.split(",")])
    data = np.asarray(rows)
    counts = data[1:-1, header.index("Sw")]  # drop underflow/overflow
    edges = np.linspace(lo, hi, nb + 1)
    return 0.5 * (edges[1:] + edges[:-1]), counts


def read_meta(prefix):
    info = {}
    with open(prefix + "_meta.txt") as f:
        for line in f:
            k, _, v = line.strip().partition(" ")
            if k:
                info[k] = v
    return info


import os as _os
def _spec(prefix):
    for cand in (prefix + "_h1_spectrum.csv", prefix + ".csv_h1_spectrum.csv"):
        if _os.path.exists(cand): return cand
    raise FileNotFoundError(prefix)
def load(prefix):
    e, c = read_h1(_spec(prefix))
    try:
        info = read_meta(prefix)
        n = int(float(info["events"]))
    except (FileNotFoundError, KeyError):
        info, n = {}, int(c.sum()) or 1
    return e, c, n, info


# ---------------------------------------------------------------- physics helpers
def broaden(e, c, fwhm0=0.120, fano=0.115, w=3.62e-3):
    """Gaussian detector response for Si: FWHM^2 = FWHM0^2 + 2.355^2 F w E (keV)."""
    sigma = np.sqrt(fwhm0**2 + 2.355**2 * fano * w * np.clip(e, 0, None)) / 2.355
    out = np.zeros_like(c, dtype=float)
    for i in np.nonzero(c)[0]:
        g = np.exp(-0.5 * ((e - e[i]) / sigma[i]) ** 2)
        out += c[i] * g / g.sum()
    return out


def net_area(e, c, lo, hi, side=SIDE):
    """Net counts in [lo,hi) with linear background from flanking sidebands."""
    roi = (e >= lo) & (e < hi)
    left = (e >= lo - side) & (e < lo)
    right = (e >= hi) & (e < hi + side)
    n, nl, nr = roi.sum(), left.sum(), right.sum()
    gross = c[roi].sum()
    bg = 0.5 * (c[left].mean() + c[right].mean()) * n
    var_bg = (n**2 / 4.0) * (c[left].sum() / nl**2 + c[right].sum() / nr**2)
    return gross - bg, np.sqrt(gross + var_bg)


def scatter_peaks(e0, theta_deg=90.0):
    return e0, e0 / (1 + e0 / 511.0 * (1 - np.cos(np.radians(theta_deg))))


# ---------------------------------------------------------------- commands
def cmd_spectrum(prefix, pixe=False, e0=30.0):
    e, c, n, info = load(prefix)
    cb = broaden(e, c)
    fig, ax = plt.subplots(figsize=(10, 5))
    ax.semilogy(e, np.where(c > 0, c, np.nan), lw=0.5, color="0.75", label="ideal deposit (Geant4)")
    ax.semilogy(e, np.where(cb > 0.1, cb, np.nan), lw=1.2, color="C0", label="with SDD resolution")
    ymax = np.nanmax(cb)
    for lab, (lo, hi) in LINES.items():
        ec = 0.5 * (lo + hi)
        ax.axvline(ec, color="C3", lw=0.5, alpha=0.5)
        ax.text(ec, 0.98, lab, rotation=90, fontsize=8, ha="center", va="top",
                transform=ax.get_xaxis_transform(), backgroundcolor="white")
    if not pixe:
        ray, comp = scatter_peaks(e0)
        for lab, ec in (("Rayleigh", ray), ("Compton", comp)):
            ax.axvline(ec, color="C2", lw=0.5, alpha=0.6)
            ax.text(ec, 0.98, lab, rotation=90, fontsize=8, ha="center", va="top", color="C2",
                    transform=ax.get_xaxis_transform(), backgroundcolor="white")
    ax.set_xlim(1, 32 if not pixe else 26)
    ax.set_ylim(0.5, max(ymax, np.nanmax(c)) * 30)
    ax.set_xlabel("Energy (keV)")
    ax.set_ylabel(f"Counts / 10 eV  ({n:.2e} primaries)")
    ax.set_title(f"{'PIXE' if pixe else 'XRF'} spectrum — {info.get('matrix')}, "
                 f"{info.get('thickness_mm')} mm, {info.get('density_g_cm3')} g/cm³")
    ax.legend(loc="center left", fontsize=8)
    fig.tight_layout()
    out = prefix + "_spectrum.png"
    fig.savefig(out, dpi=200)
    print(f"saved {out}\n")
    print(f"{'line':8s} {'net counts':>12s} {'± err':>9s} {'per 1e6 prim':>13s}")
    for lab, (lo, hi) in LINES.items():
        a, s = net_area(e, c, lo, hi)
        print(f"{lab:8s} {a:12.1f} {s:9.1f} {a / n * 1e6:13.4f}")


import glob as _glob, re as _re

def _net_from_file(path, line="U Lα"):
    e, c = read_h1(path)
    a, sig = net_area(e, c, *LINES[line])
    return a, sig

def _stem(path):
    b = _os.path.basename(path)
    if b.endswith("_h1_spectrum.csv"):
        b = b[: -len("_h1_spectrum.csv")]
    if b.endswith(".csv"):
        b = b[:-4]
    return b

def cmd_matrix(outdir, line="U Lα"):
    groups = {}
    for f in sorted(_glob.glob(_os.path.join(outdir, "matrix_*_h1_spectrum.csv"))):
        m = _re.search(r"matrix_(.+)_U([\d.]+)$", _stem(f))
        if not m:
            continue
        a, sig = _net_from_file(f, line)
        groups.setdefault(m.group(1), {})[float(m.group(2))] = (a, sig)
    if not groups:
        raise SystemExit("no matrix_* spectrum files found in " + outdir)
    fig, ax = plt.subplots(figsize=(6, 4.5))
    slopes = {}
    for i, (mat, d) in enumerate(sorted(groups.items())):
        x = np.array(sorted(d))
        y = np.array([d[v][0] for v in x])
        sig = np.array([d[v][1] for v in x])
        w = 1 / np.maximum(sig, 1e-12) ** 2
        slope = np.sum(w * x * y) / np.sum(w * x * x)
        slope_err = 1 / np.sqrt(np.sum(w * x * x))
        slopes[mat] = (slope, slope_err)
        ax.errorbar(x, y, yerr=sig, fmt="o", color=f"C{i}", label=f"{mat}: {slope:.3f} cts/ppm")
        xx = np.linspace(0, x.max() * 1.05, 50)
        ax.plot(xx, slope * xx, color=f"C{i}", lw=1)
    ax.set_xlabel("U concentration (ppm)")
    ax.set_ylabel(f"{line} net counts")
    ax.set_title("Matrix effect on U calibration")
    ax.legend(fontsize=8)
    fig.tight_layout()
    out = _os.path.join(outdir, "matrix_effect.png")
    fig.savefig(out, dpi=200)
    print("saved", out)
    for mat, (sl, se) in slopes.items():
        print(f"  {mat:12s} sensitivity = {sl:.4f} +/- {se:.4f} counts/ppm  ({len(groups[mat])} points)")
    if "phosphate" in slopes and "cellulose" in slopes:
        k = slopes["cellulose"][0] / slopes["phosphate"][0]
        print(f"\n  sensitivity ratio cellulose/phosphate = {k:.2f}")
        print(f"  -> calibrating with a cellulose standard reports U in phosphate at {100 / k:.0f}% of true value")

def cmd_thickness(outdir, line="U Lα"):
    d = {}
    for f in _glob.glob(_os.path.join(outdir, "thick_*_h1_spectrum.csv")):
        m = _re.search(r"thick_([\d.]+)mm", _stem(f))
        if not m:
            continue
        d[float(m.group(1))] = _net_from_file(f, line)
    if len(d) < 3:
        raise SystemExit(f"found only {len(d)} thickness point(s) in {outdir} (expected ~8). "
                         "The per-thickness spectra overwrote each other -- the run must be repeated.")
    t = np.array(sorted(d))
    y = np.array([d[v][0] for v in t])
    sig = np.array([d[v][1] for v in t])
    from scipy.optimize import curve_fit
    fn = lambda t, i_inf, tau: i_inf * (1 - np.exp(-t / tau))
    p_, cov = curve_fit(fn, t, y, p0=(y.max(), np.median(t)),
                        sigma=np.maximum(sig, 1e-12), absolute_sigma=True)
    i_inf, tau = p_
    t99 = tau * np.log(100)
    fig, ax = plt.subplots(figsize=(6, 4.5))
    ax.errorbar(t, y, yerr=sig, fmt="o", label="Geant4")
    tt = np.logspace(np.log10(t.min() * 0.5), np.log10(t.max() * 1.5), 200)
    ax.plot(tt, fn(tt, *p_), label=f"I_inf(1-e^(-t/tau)),  tau = {tau * 1000:.0f} um")
    ax.axvline(t99, ls="--", color="0.5", label=f"99% of I_inf at {t99:.2f} mm")
    ax.set_xscale("log")
    ax.set_xlabel("Pellet thickness (mm)")
    ax.set_ylabel(f"{line} net counts")
    ax.set_title("Self-absorption / infinite-thickness criterion")
    ax.legend(fontsize=8)
    fig.tight_layout()
    out = _os.path.join(outdir, "thickness_saturation.png")
    fig.savefig(out, dpi=200)
    print("saved", out)
    print(f"  tau = {tau * 1000:.1f} +/- {np.sqrt(cov[1,1]) * 1000:.1f} um ; "
          f"infinite thickness (99%) = {t99:.3f} mm  ({len(d)} points)")

if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("mode", choices=["spectrum", "matrix", "thickness"])
    ap.add_argument("path", help="file prefix (spectrum) or output directory (matrix/thickness)")
    ap.add_argument("--pixe", action="store_true", help="label as PIXE (no scatter peaks)")
    ap.add_argument("--e0", type=float, default=30.0, help="XRF beam energy in keV")
    a = ap.parse_args()
    if a.mode == "spectrum":
        cmd_spectrum(a.path, a.pixe, a.e0)
    elif a.mode == "matrix":
        cmd_matrix(a.path)
    else:
        cmd_thickness(a.path)
