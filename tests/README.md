Pruebas reproducibles (frases de evaluación, comandos y resultados).


## Archivos de prueba

| Archivo | Contenido |
|---|---|
| `frases_stt.txt` | 20 frases en español para evaluar el reconocimiento de voz (STT). Se leen en voz alta, una por una. |
| `preguntas_slm.txt` | 10 preguntas cortas para evaluar el modelo de lenguaje (SLM). |

## Cómo se usan

Los archivos se copian a la carpeta `$HOME` del teléfono (Termux) y se pasan a los scripts de `scripts/`:

- STT: `./evaluar_stt.sh 6 frases_stt.txt resultados_stt.csv`
- SLM: `./evaluar_slm.sh preguntas_slm.txt resultados_slm.csv`

Para una prueba más corta, se puede crear una lista con las primeras frases:

`head -6 frases_stt.txt > frases_ruido.txt`

## Criterio de evaluación del STT

Una transcripción se considera utilizable si su WER es de 25 % o menos. Los resultados de la evaluación están en `docs/resultados-stt.md`.

## Qué no se sube

No se suben grabaciones de voz, modelos (`.gguf`, `.bin`, `.onnx`) ni archivos de registro como `turnos.log`.
