#include "orchestrator.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    const char* home_env = std::getenv("HOME");
    std::string home = home_env ? home_env : "/data/data/com.termux/files/home";

    ConfigRutas cfg;
    cfg.home_dir          = home;
    cfg.script_captura    = home + "/capturar_audio.sh";
    cfg.bin_whisper       = home + "/whisper.cpp/build/bin/whisper-cli";
    cfg.modelo_whisper    = home + "/whisper.cpp/models/ggml-tiny.bin";
    cfg.bin_llama         = "llama-cli";
    cfg.modelo_slm        = home + "/models/Qwen3.5-0.8B-Q4_0.gguf";
    cfg.script_tts        = home + "/tts_mms.py";
    cfg.audio_entrada_tmp = home + "/.jarvis_in.wav";
    cfg.audio_salida_tmp  = home + "/.jarvis_out.wav";
    cfg.segundos_grabacion = (argc > 1) ? std::atoi(argv[1]) : 5;

    std::cout << "==========================================" << std::endl;
    std::cout << "   JARVIS-SON V1 - Orquestador C++        " << std::endl;
    std::cout << "   Modo por turnos (sin interrupcion)     " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "[ESTADO] -> " << estado_a_texto(Estado::LISTO) << std::endl;

    while (true) {
        std::cout << "\nPresiona ENTER para iniciar turno (o escribe 'q' + ENTER para salir): ";
        std::string cmd;
        if (!std::getline(std::cin, cmd) || cmd == "q" || cmd == "Q") {
            break;
        }

        MetricasTurno res = ejecutar_turno(cfg);
        if (res.exito) {
            std::cout << "[METRICAS] Captura: " << res.ms_captura << " ms | "
                      << "STT: " << res.ms_stt << " ms | "
                      << "SLM: " << res.ms_slm << " ms | "
                      << "TTS: " << res.ms_tts << " ms | "
                      << "Total: " << res.ms_total << " ms" << std::endl;
        } else if (res.etapa_error == "STT (sin voz utilizable)") {
            std::cout << "[INFO] No se detecto voz. Habla mas cerca del microfono e intenta de nuevo." << std::endl;
        } else {
            std::cerr << "[FALLO] Etapa: " << res.etapa_error
                      << " | Codigo salida: " << res.codigo_salida << std::endl;
            std::cerr << "[DIAGNOSTICO] Verifica permisos/microfono, modelos y vuelve a intentar." << std::endl;
        }
    }

    std::cout << "Saliendo de Jarvis-son V1. Temporales limpios." << std::endl;
    return 0;
}
