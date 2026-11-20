#ifndef JARVIS_ORCHESTRATOR_HPP
#define JARVIS_ORCHESTRATOR_HPP

#include <string>

// Estados por los que pasa el asistente en cada turno:
// LISTO -> GRABANDO -> TRANSCRIBIENDO (STT) -> GENERANDO (SLM) -> HABLANDO (TTS) -> LISTO.
// ERROR se usa cuando alguna etapa falla.
enum class Estado {
    LISTO,
    GRABANDO,
    TRANSCRIBIENDO,
    GENERANDO,
    HABLANDO,
    ERROR
};

// Resultado de un turno completo: tiempos por etapa y si salió bien.
struct MetricasTurno {
    long ms_captura = 0;      // tiempo de grabación, en milisegundos
    long ms_stt = 0;          // tiempo de transcripción (voz a texto)
    long ms_slm = 0;          // tiempo de generación de la respuesta
    long ms_tts = 0;          // tiempo de síntesis de voz (texto a voz)
    long ms_total = 0;        // tiempo total del turno
    bool exito = false;       // true si el turno terminó sin fallos
    std::string etapa_error;  // etapa donde falló (vacío si no hubo error)
    int codigo_salida = 0;    // código de salida del proceso que falló
};

// Rutas de los programas, modelos y archivos temporales que usa el orquestador.
struct ConfigRutas {
    std::string home_dir;           // carpeta base del usuario
    std::string script_captura;     // script que graba el audio
    std::string bin_whisper;        // programa de transcripción (whisper-cli)
    std::string modelo_whisper;     // modelo de Whisper para el STT
    std::string bin_llama;          // programa que ejecuta el modelo de lenguaje
    std::string modelo_slm;         // modelo de lenguaje (SLM)
    std::string script_tts;         // script que genera la voz
    std::string audio_entrada_tmp;  // WAV temporal con la voz del usuario
    std::string audio_salida_tmp;   // WAV temporal con la respuesta hablada
    int segundos_grabacion = 5;     // duración de cada grabación
};

// Devuelve el nombre legible de un estado, para mostrarlo en pantalla.
std::string estado_a_texto(Estado e);

// Ejecuta un turno completo (grabar, transcribir, responder y hablar)
// y devuelve las métricas del turno.
MetricasTurno ejecutar_turno(const ConfigRutas& cfg);

#endif // JARVIS_ORCHESTRATOR_HPP
