#!/usr/bin/env bash
# Evalúa el STT: lee cada frase, graba, transcribe con whisper-cli y guarda resultados.
# Uso (desde $HOME): ./evaluar_stt.sh [segundos] [archivo_frases] [salida.csv]
SEG="${1:-6}"
FRASES="${2:-frases_stt.txt}"
SALIDA="${3:-resultados_stt.csv}"
W="$HOME/whisper.cpp"
echo "n;referencia;transcripcion;ms_transcripcion" > "$SALIDA"
n=0
while IFS= read -r ref; do
  [ -z "$ref" ] && continue
  n=$((n+1))
  echo ""
  echo "Frase $n: $ref"
  read -r -p "Pulsa Enter y lee la frase en voz alta... " _ < /dev/tty
  "$HOME/capturar_audio.sh" "$SEG" "$HOME/eval_tmp.wav" > /dev/null || { echo "fallo la captura"; continue; }
  ini=$(date +%s%3N)
  txt=$("$W/build/bin/whisper-cli" -m "$W/models/ggml-tiny.bin" -f "$HOME/eval_tmp.wav" -l es -nt -mc 0 2>/dev/null | tr '\n' ' ' | sed 's/^ *//;s/ *$//')
  fin=$(date +%s%3N)
  ms=$((fin-ini))
  echo "  -> $txt  (${ms} ms)"
  echo "$n;$ref;$txt;$ms" >> "$SALIDA"
  rm -f "$HOME/eval_tmp.wav"
done < "$FRASES"
echo "Listo: $SALIDA"
