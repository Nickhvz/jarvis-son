import re, sys, math
ruta = sys.argv[1] if len(sys.argv) > 1 else "turnos.log"
datos = {"Captura": [], "STT": [], "SLM": [], "TTS": [], "Total": []}
for linea in open(ruta, encoding="utf-8", errors="ignore"):
    if "[METRICAS]" in linea:
        for k in datos:
            m = re.search(rf"{k}: (\d+) ms", linea)
            if m:
                datos[k].append(int(m.group(1)))
def pct(v, p):
    v = sorted(v)
    return v[max(0, math.ceil(p / 100 * len(v)) - 1)]
print(f"{'Etapa':8} {'n':>3} {'p50 (ms)':>10} {'p95 (ms)':>10}")
for k, v in datos.items():
    if v:
        print(f"{k:8} {len(v):>3} {pct(v,50):>10} {pct(v,95):>10}")
