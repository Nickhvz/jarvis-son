# jarvis-son

Asistente de voz local Jarvis-son V1 (Termux, whisper.cpp, llama.cpp). Proyecto académico de Inteligencia Artificial, Cotecnova.

Flujo por turnos: **micrófono → STT → SLM → TTS**. Sin interrupciones ni palabra de activación.

## Requisitos
- Android 13+, ARM64, unos 3 GB de RAM
- Termux y Termux:API desde fuente oficial (F-Droid o GitHub)
- Unos 2 GB de almacenamiento libre

## Instalación
1. Instalar paquetes:
   ```bash
   pkg update && pkg upgrade
   pkg install git cmake clang ffmpeg termux-api python wget
   ```
2. Compilar whisper.cpp y llama.cpp (o instalar sus binarios) según sus guías oficiales.
3. Descargar los modelos listados en `models/manifest.json` y verificar el SHA-256:
   ```bash
   sha256sum ARCHIVO
   ```
4. Instalar sherpa-onnx para Python y colocar `scripts/tts_mms.py` en `~/`.
5. Compilar:
   ```bash
   mkdir -p build && cd build
   cmake .. && make -j2
   ```
6. Ejecutar:
   ```bash
   JARVIS_DEBUG=1 ./jarvis-son
   ```

## Modelos
Los pesos no se versionan. Ver `models/manifest.json`: STT Whisper tiny (MIT), SLM Qwen3.5-0.8B Q4_0 (Apache-2.0), TTS MMS español (CC-BY-NC-4.0, solo uso no comercial).

## Resultados medidos (12 turnos, un solo teléfono)
| Etapa | p50 (ms) | p95 (ms) |
|---|---|---|
| Captura | 6.785 | 7.096 |
| STT | 5.532 | 12.815 |
| SLM | 12.787 | 26.137 |
| TTS | 43.655 | 154.374 |
| Total | 73.867 | 197.450 |

Memoria máxima observada: 695.964 KB (~680 MiB), medida en un turno aparte. 12 turnos consecutivos sin cierre por memoria. Estos números valen solo para el equipo documentado, no para todos los móviles de 3 GB.

## Limitaciones conocidas
- El TTS es la etapa más lenta e inestable (15 s a 154 s).
- Sin `ffplay` instalado, la reproducción usa `termux-media-player` con una espera fija de 4 s.
- Whisper tiny puede devolver etiquetas como `[MÚSICA]` con silencio; el programa las filtra.
- Sin barge-in, sin escucha continua.
- No se guardan grabaciones ni transcripciones; los temporales se borran al terminar el turno.

## Pruebas
- Percentiles: `python3 scripts/calcular_percentiles.py turnos.log`
- STT: `scripts/evaluar_stt.sh`
