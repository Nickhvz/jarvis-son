#ifndef JARVIS_ORCHESTRATOR_HPP
#define JARVIS_ORCHESTRATOR_HPP

#include <string>

enum class Estado {
    LISTO,
    GRABANDO,
    TRANSCRIBIENDO,
    GENERANDO,
    HABLANDO,
    ERROR
};

struct MetricasTurno {
    long ms_captura = 0;
    long ms_stt = 0;
    long ms_slm = 0;
    long ms_tts = 0;
    long ms_total = 0;
    bool exito = false;
    std::string etapa_error;
    int codigo_salida = 0;
};

struct ConfigRutas {
    std::string home_dir;
    std::string script_captura;
    std::string bin_whisper;
    std::string modelo_whisper;
    std::string bin_llama;
    std::string modelo_slm;
    std::string script_tts;
    std::string audio_entrada_tmp;
    std::string audio_salida_tmp;
    int segundos_grabacion = 5;
};

std::string estado_a_texto(Estado e);
MetricasTurno ejecutar_turno(const ConfigRutas& cfg);

#endif // JARVIS_ORCHESTRATOR_HPP
