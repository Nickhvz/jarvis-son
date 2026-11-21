# Resultados de la evaluación de STT

Evaluación con 20 frases leídas en voz alta en el mismo teléfono (LG K61).

Criterio: una transcripción es utilizable si su WER es de 25 % o menos.

Se ignoran mayúsculas, puntuación y tildes; "1" y "10" cuentan como "uno" y "diez".

## Resumen

| Métrica | Whisper tiny | Whisper base |
|---|---|---|
| Tamaño del modelo | 77.691.713 bytes | 147.951.465 bytes |
| Frases utilizables | 16 de 20 | 17 de 20 |
| WER global | 10,4 % | 9,4 % |
| Frases no utilizables | 7, 11, 19 y 20 | 5, 7 y 11 |
| Tiempo por clip de 6 s (mediana) | 5,56 s | 11,76 s |

## Conclusión

Ninguno de los dos modelos alcanza la meta del plan (18 de 20).

Base mejora una frase, pero tarda 2,1 veces más. La V1 se mantiene con tiny.

## Límites

- Una ejecución válida por modelo, en un solo teléfono.
- No se evaluó con ruido ni con acentos distintos.
- Una primera ejecución se descartó porque las frases se respondieron en vez de leerse.
