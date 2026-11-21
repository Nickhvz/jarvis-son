#include "orchestrator.hpp"
#include "texto_util.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

using Clock = std::chrono::steady_clock;

std::string estado_a_texto(Estado e) {
    switch (e) {
        case Estado::LISTO:          return "LISTO";
        case Estado::GRABANDO:       return "GRABANDO";
        case Estado::TRANSCRIBIENDO: return "TRANSCRIBIENDO (STT)";
        case Estado::GENERANDO:      return "GENERANDO (SLM)";
        case Estado::HABLANDO:       return "HABLANDO (TTS)";
        case Estado::ERROR:          return "ERROR";
    }
    return "DESCONOCIDO";
}

static void mostrar_estado(Estado e) {
    std::cout << "\n[ESTADO] -> " << estado_a_texto(e) << std::endl;
}

static void limpiar_temporales(const ConfigRutas& cfg) {
    std::remove(cfg.audio_entrada_tmp.c_str());
    std::remove(cfg.audio_salida_tmp.c_str());
}

static std::string limpiar_espacios(const std::string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n");
    if (ini == std::string::npos) return "";
    size_t fin = s.find_last_not_of(" \t\r\n");
    std::string res = s.substr(ini, fin - ini + 1);
    std::replace(res.begin(), res.end(), '\n', ' ');
    std::replace(res.begin(), res.end(), '\r', ' ');
    return res;
}

static int ejecutar_proceso(const std::vector<std::string>& args, std::string& salida_stdout, bool silenciar_stderr = true) {
    int pipefd[2];
    if (pipe(pipefd) == -1) return -1;

    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        if (silenciar_stderr) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull != -1) {
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
        } else {
            dup2(pipefd[1], STDERR_FILENO);
        }
        close(pipefd[1]);

        int devnull_in = open("/dev/null", O_RDONLY);
        if (devnull_in != -1) {
            dup2(devnull_in, STDIN_FILENO);
            close(devnull_in);
        }

        std::vector<char*> argv;
        for (const auto& a : args) {
            argv.push_back(const_cast<char*>(a.c_str()));
        }
        argv.push_back(nullptr);

        execvp(argv[0], argv.data());
        _exit(127);
    }

    close(pipefd[1]);
    salida_stdout.clear();
    char buffer[512];
    ssize_t bytes;
    while ((bytes = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        salida_stdout += buffer;
    }
    close(pipefd[0]);

    int status = 0;
    waitpid(pid, &status, 0);
    if (std::getenv("JARVIS_DEBUG")) {
        std::cerr << "[DEBUG] " << args[0] << " -> WIFEXITED=" << WIFEXITED(status)
                  << " WEXITSTATUS=" << (WIFEXITED(status) ? WEXITSTATUS(status) : -1)
                  << " WIFSIGNALED=" << WIFSIGNALED(status)
                  << " bytes_stdout=" << salida_stdout.size() << std::endl;
    }
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return -1;
}

MetricasTurno ejecutar_turno(const ConfigRutas& cfg) {
    MetricasTurno m;
    auto t_inicio_turno = Clock::now();
    std::string out;

    // 1. GRABANDO
    mostrar_estado(Estado::GRABANDO);
    std::cout << "Habla ahora (" << cfg.segundos_grabacion << " segundos)..." << std::endl;
    auto t0 = Clock::now();
    int rc = ejecutar_proceso({
        "bash", cfg.script_captura,
        std::to_string(cfg.segundos_grabacion),
        cfg.audio_entrada_tmp
    }, out, true);
    auto t1 = Clock::now();
    m.ms_captura = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    if (rc != 0) {
        mostrar_estado(Estado::ERROR);
        m.etapa_error = "GRABANDO";
        m.codigo_salida = rc;
        limpiar_temporales(cfg);
        return m;
    }

    // 2. STT (Whisper.cpp)
    mostrar_estado(Estado::TRANSCRIBIENDO);
    t0 = Clock::now();
    rc = ejecutar_proceso({
        cfg.bin_whisper,
        "-m", cfg.modelo_whisper,
        "-f", cfg.audio_entrada_tmp,
        "-l", "es",
        "-nt",
        "-sns",
        "-mc", "0"
    }, out, true);
    t1 = Clock::now();
    m.ms_stt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    std::string transcripcion = limpiar_espacios(out);
    if (rc != 0) {
        mostrar_estado(Estado::ERROR);
        m.etapa_error = "STT (fallo del binario whisper-cli)";
        m.codigo_salida = rc;
        limpiar_temporales(cfg);
        return m;
    }
    if (!jarvis::transcripcion_utilizable(transcripcion)) {
        // Silencio, ruido o etiqueta tipo [MÚSICA]: no es una pregunta.
        std::cout << "[STT] No te escuche (" << transcripcion << "). Intenta de nuevo." << std::endl;
        limpiar_temporales(cfg);
        m.etapa_error = "STT (sin voz utilizable)";
        m.codigo_salida = 2;
        mostrar_estado(Estado::LISTO);
        return m;
    }
    std::cout << "[STT] Transcripcion detectada (" << m.ms_stt << " ms)" << std::endl;

    // 3. SLM (llama.cpp + Qwen3.5-0.8B)
    mostrar_estado(Estado::GENERANDO);
    std::string prompt = "Responde en español en máximo 15 palabras: " + transcripcion;
    t0 = Clock::now();
    rc = ejecutar_proceso({
        cfg.bin_llama,
        "-m", cfg.modelo_slm,
        "-c", "1024",
        "-n", "64",
        "-t", "4",
        "-st",
        "-rea", "off",
        "--simple-io",
        "--no-display-prompt",
        "--log-disable",
        "-p", prompt
    }, out, true);
    t1 = Clock::now();
    m.ms_slm = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    {
        // Descartar banner y eco: quedarse solo con lo que va despues del prompt
        size_t pos = out.find(prompt);
        if (pos != std::string::npos) out = out.substr(pos + prompt.size());

    }
    std::string respuesta = jarvis::limpiar_salida_slm(out);
    if (rc != 0 || respuesta.empty()) {
        mostrar_estado(Estado::ERROR);
        m.etapa_error = "SLM (respuesta vacia o fallo llama-cli)";
        m.codigo_salida = (rc != 0) ? rc : 3;
        limpiar_temporales(cfg);
        return m;
    }
    std::cout << "[SLM] Respuesta generada (" << m.ms_slm << " ms)" << std::endl;

    // 4. TTS (sherpa-onnx + vits-mms-spa)
    mostrar_estado(Estado::HABLANDO);
    t0 = Clock::now();
    rc = ejecutar_proceso({
        "python", cfg.script_tts,
        respuesta,
        cfg.audio_salida_tmp
    }, out, true);
    t1 = Clock::now();
    m.ms_tts = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    if (rc != 0) {
        mostrar_estado(Estado::ERROR);
        m.etapa_error = "TTS (sintesis sherpa-onnx)";
        m.codigo_salida = rc;
        limpiar_temporales(cfg);
        return m;
    }

    std::string play_out;
    if (ejecutar_proceso({"ffplay", "-nodisp", "-autoexit", "-loglevel", "quiet", cfg.audio_salida_tmp}, play_out, true) != 0) { ejecutar_proceso({"termux-media-player", "play", cfg.audio_salida_tmp}, play_out, true); sleep(4); }


    // 5. LIMPIEZA Y CIERRE DE TURNO
    limpiar_temporales(cfg);
    auto t_fin_turno = Clock::now();
    m.ms_total = std::chrono::duration_cast<std::chrono::milliseconds>(t_fin_turno - t_inicio_turno).count();
    m.exito = true;
    mostrar_estado(Estado::LISTO);
    return m;
}
