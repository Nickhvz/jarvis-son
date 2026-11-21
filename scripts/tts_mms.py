#!/usr/bin/env python3
import sys, time, wave, array, os
import sherpa_onnx

BASE = os.path.expanduser("~/vits-mms-spa")
texto = sys.argv[1] if len(sys.argv) > 1 else "Hola, soy Jarvis-son. Esta es una prueba de voz local."
salida = sys.argv[2] if len(sys.argv) > 2 else os.path.expanduser("~/voz.wav")

config = sherpa_onnx.OfflineTtsConfig(
    model=sherpa_onnx.OfflineTtsModelConfig(
        vits=sherpa_onnx.OfflineTtsVitsModelConfig(
            model=f"{BASE}/model.onnx",
            tokens=f"{BASE}/tokens.txt",
        ),
        num_threads=4,
        provider="cpu",
    ),
    max_num_sentences=1,
)

t0 = time.time()
tts = sherpa_onnx.OfflineTts(config)
t1 = time.time()
audio = tts.generate(texto, sid=0, speed=1.0)
t2 = time.time()

pcm = array.array("h", (max(-32768, min(32767, int(s * 32767))) for s in audio.samples))
with wave.open(salida, "wb") as w:
    w.setnchannels(1)
    w.setsampwidth(2)
    w.setframerate(audio.sample_rate)
    w.writeframes(pcm.tobytes())

dur = len(audio.samples) / audio.sample_rate
print(f"carga_s={t1-t0:.2f} sintesis_s={t2-t1:.2f} audio_s={dur:.2f} RTF={(t2-t1)/dur:.2f} -> {salida}")
