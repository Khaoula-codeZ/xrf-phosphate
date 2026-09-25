import argparse, glob, math, os, re
import numpy as np
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import xraydb

E0_EV, EF_EV, RHO, ANG_IN, ANG_OUT, U_PPM = 30000.0, 13615.0, 2.0, 45.0, 45.0, 2000.0
ULINE, SIDE = (13.35, 13.75), 0.10
_AW = dict(Ca=40.078,P=30.974,O=15.999,F=18.998,C=12.011,Si=28.085,S=32.06,Na=22.990,Mg=24.305,Al=26.982,Fe=55.845,K=39.098)
_OX = [("Ca",1,1,51.0),("P",2,5,31.5),("F",1,0,3.8),("C",1,2,6.0),("Si",1,2,3.0),("S",1,3,1.5),("Na",2,1,0.9),("Mg",1,1,0.5),("Al",2,3,0.5),("Fe",2,3,0.2),("K",2,1,0.1)]

def fractions():
    fr={}
    for el,ne,no,wt in _OX:
        m=ne*_AW[el]+no*_AW["O"]; fr[el]=fr.get(el,0)+wt*ne*_AW[el]/m
        if no: fr["O"]=fr.get("O",0)+wt*no*_AW["O"]/m
    s=sum(fr.values()); fr={k:v/s for k,v in fr.items()}
    u=U_PPM*1e-6; fr={k:v*(1-u) for k,v in fr.items()}; fr["U"]=u; return fr

def mu(e,fr): return sum(w*xraydb.mu_elam(el,e,kind="total") for el,w in fr.items())

def tau_ana():
    fr=fractions(); m0,mf=mu(E0_EV,fr),mu(EF_EV,fr)
    a=RHO*(m0/math.sin(math.radians(ANG_IN))+mf/math.sin(math.radians(ANG_OUT)))
    return 10.0/a,m0,mf

def read_h1(p):
    nb=lo=hi=None; hdr=None; rows=[]
    for line in open(p):
        line=line.strip()
        if not line: continue
        if line.startswith("#"):
            if line.startswith("#axis fixed"):
                q=line.split(); nb,lo,hi=int(q[2]),float(q[3]),float(q[4])
            continue
        if hdr is None: hdr=line.split(","); continue
        rows.append([float(x) for x in line.split(",")])
    d=np.asarray(rows); c=d[1:-1,hdr.index("Sw")]; ed=np.linspace(lo,hi,nb+1)
    return 0.5*(ed[1:]+ed[:-1]),c

def net(e,c,lo,hi,side=SIDE):
    roi=(e>=lo)&(e<hi); L=(e>=lo-side)&(e<lo); R=(e>=hi)&(e<hi+side)
    n,nl,nr=roi.sum(),L.sum(),R.sum(); g=c[roi].sum(); bg=0.5*(c[L].mean()+c[R].mean())*n
    v=(n**2/4)*(c[L].sum()/nl**2+c[R].sum()/nr**2); return g-bg,math.sqrt(g+v)

def main(outdir):
    d={}
    for f in glob.glob(os.path.join(outdir,"thick_*_h1_spectrum.csv")):
        b=os.path.basename(f)[:-len("_h1_spectrum.csv")]
        if b.endswith(".csv"): b=b[:-4]
        m=re.search(r"thick_([\d.]+)mm",b)
        if not m: continue
        e,c=read_h1(f); d[float(m.group(1))]=net(e,c,*ULINE)
    if len(d)<3: raise SystemExit(f"need >=3 points, found {len(d)}")
    t=np.array(sorted(d)); y=np.array([d[v][0] for v in t]); s=np.array([d[v][1] for v in t])
    fn=lambda t,i,tau:i*(1-np.exp(-t/tau))
    p,cov=curve_fit(fn,t,y,p0=(y.max(),np.median(t)),sigma=np.maximum(s,1e-12),absolute_sigma=True)
    i_inf,tmc=p; tmc_e=math.sqrt(cov[1,1]); tana,m0,mf=tau_ana()
    fig,ax=plt.subplots(figsize=(6.5,4.5))
    ax.errorbar(t,y,yerr=s,fmt="o",color="C0",label="Geant4",zorder=3)
    tt=np.logspace(np.log10(t.min()*0.5),np.log10(t.max()*1.6),300)
    ax.plot(tt,fn(tt,i_inf,tmc),color="C0",lw=1.4,label=f"MC fit: tau={tmc*1000:.0f}$\\pm${tmc_e*1000:.0f} um")
    ax.plot(tt,fn(tt,i_inf,tana),color="C3",ls="--",lw=1.6,label=f"xraydb theory: tau={tana*1000:.0f} um")
    ax.set_xscale("log"); ax.set_xlabel("Pellet thickness (mm)"); ax.set_ylabel("U L$\\alpha$ net counts")
    ax.set_title("Self-absorption: Geant4 vs analytical XRF theory"); ax.legend(fontsize=8); fig.tight_layout()
    out=os.path.join(outdir,"selfabsorption_validation.png"); fig.savefig(out,dpi=200)
    print(f"mu(30keV)={m0:.3f}  mu(13.6keV)={mf:.3f} cm2/g")
    print(f"tau MC     = {tmc*1000:.1f} +/- {tmc_e*1000:.1f} um")
    print(f"tau theory = {tana*1000:.1f} um   (ratio {tmc/tana:.2f})")
    print(f"saved {out}")

if __name__=="__main__":
    ap=argparse.ArgumentParser(); ap.add_argument("outdir"); main(ap.parse_args().outdir)
