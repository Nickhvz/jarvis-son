Scripts de instalación, descarga verificada de modelos y utilidades.

## Scripts disponibles

| Script | Qué hace |
|---|---|
| `capturar_audio.sh` | Graba un clip con el micrófono, lo convierte a WAV mono de 16 kHz y valida el formato. |
| `evaluar_stt.sh` | Lee cada frase de una lista, graba tu voz, la transcribe con Whisper y guarda los resultados en un CSV. |
| `evaluar_slm.sh` | Hace preguntas cortas al modelo de lenguaje y mide el tiempo por turno y la memoria máxima. |
| `calcular_percentiles.py` | Calcula percentiles de tiempo por etapa (captura, STT, SLM, TTS y total) a partir de `turnos.log`. |
| `tts_mms.py` | Genera un WAV con voz en español usando el modelo MMS. |
| `tts_piper.py` | Genera un WAV con voz en español usando Piper. |

## Cómo usarlos

Los scripts se ejecutan en Termux, desde la carpeta `$HOME`.

- Capturar audio: `./capturar_audio.sh [segundos] [salida.wav]`
- Evaluar STT: `./evaluar_stt.sh [segundos] [archivo_frases] [salida.csv]`
- Evaluar SLM: `./evaluar_slm.sh [archivo_preguntas] [salida.csv]`
- Percentiles: `python3 calcular_percentiles.py turnos.log`
- Voz: `python3 tts_mms.py "texto" [salida.wav]`

No se deben subir al repositorio las grabaciones, los modelos ni `turnos.log`.
