#!/usr/bin/env bash
# Captura un clip, lo normaliza a WAV mono 16 kHz y valida el formato.
# Uso: ./capturar_audio.sh [segundos] [salida.wav]
SEG="${1:-5}"
SALIDA="${2:-$HOME/entrada.wav}"
TMP="$HOME/.captura_tmp.m4a"
rm -f "$TMP"
termux-microphone-record -f "$TMP" -l "$SEG" || exit 1
sleep "$((SEG + 1))"
ffmpeg -y -loglevel error -i "$TMP" -ac 1 -ar 16000 -c:a pcm_s16le "$SALIDA" || { rm -f "$TMP"; exit 2; }
rm -f "$TMP"
ffprobe -hide_banner "$SALIDA" 2>&1 | grep -E "Duration|Audio"
