#!/usr/bin/env bash
# Evalúa el SLM: preguntas cortas, mide tiempo por turno y memoria pico.
# Uso (desde $HOME): ./evaluar_slm.sh [archivo_preguntas] [salida.csv]
PREG="${1:-preguntas_slm.txt}"
SALIDA="${2:-resultados_slm.csv}"
MODELO="$HOME/models/Qwen3.5-0.8B-Q4_0.gguf"
mkdir -p "$HOME/logs_slm"
echo "n;pregunta;respuesta;ms_turno;memoria_pico_kb" > "$SALIDA"
n=0
while IFS= read -r q; do
  [ -z "$q" ] && continue
  n=$((n+1))
  LOG="$HOME/logs_slm/q$n.txt"
  MEM="$HOME/logs_slm/mem$n.txt"
  ini=$(date +%s%3N)
  command time -f "%M" -o "$MEM" llama-cli -m "$MODELO" -c 1024 -n 96 -t 4 -st -rea off -p "Responde en español en una frase corta: $q" > "$LOG" 2>&1 < /dev/null
  fin=$(date +%s%3N)
  ms=$((fin-ini))
  kb=$(tail -n 1 "$MEM")
  resp=$(sed -n '/^> /,/^\[ Prompt/p' "$LOG" | sed '1d;$d' | tr '\n' ' ')
  echo "Pregunta $n: $q"
  echo "  -> $resp (${ms} ms, ${kb} KB)"
  echo "$n;$q;$resp;$ms;$kb" >> "$SALIDA"
done < "$PREG"
echo "Listo: $SALIDA"
