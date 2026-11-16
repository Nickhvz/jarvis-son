#ifndef JARVIS_TEXTO_UTIL_HPP
#define JARVIS_TEXTO_UTIL_HPP

#include <string>
#include <cctype>

namespace jarvis {

// Quita espacios/saltos al inicio y al final.
inline std::string recortar(const std::string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n");
    if (ini == std::string::npos) return "";
    size_t fin = s.find_last_not_of(" \t\r\n");
    return s.substr(ini, fin - ini + 1);
}

// Whisper marca los sonidos que no son voz con etiquetas entre corchetes o
// parentesis: [MÚSICA], [Música], (aplausos), [BLANK_AUDIO], [Silencio]...
// Esta funcion devuelve true solo si queda texto "real" fuera de esas etiquetas.
inline bool transcripcion_utilizable(const std::string& texto) {
    int nivel = 0;           // profundidad dentro de [] o ()
    int letras_fuera = 0;    // letras/digitos fuera de etiquetas
    for (size_t i = 0; i < texto.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(texto[i]);
        if (c == '[' || c == '(') { ++nivel; continue; }
        if (c == ']' || c == ')') { if (nivel > 0) --nivel; continue; }
        if (nivel > 0) continue;
        // bytes >= 0x80 son parte de letras UTF-8 (tildes, ñ): cuentan como letra
        if (std::isalnum(c) || c >= 0x80) ++letras_fuera;
    }
    return letras_fuera >= 2;
}

// Quita del inicio de la salida del SLM los marcadores que imprime llama-cli
// y todo lo que viene despues de "Exiting..." o "[ Prompt:".
inline std::string limpiar_salida_slm(const std::string& raw) {
    std::string s = raw;
    size_t fin = s.find("[ Prompt");
    if (fin != std::string::npos) s.erase(fin);
    fin = s.find("Exiting...");
    if (fin != std::string::npos) s.erase(fin);
    // salto de linea -> espacio
    for (auto& ch : s) if (ch == '\n' || ch == '\r') ch = ' ';
    s = recortar(s);
    // algunos builds anteponen "> " como marcador de turno
    if (s.rfind("> ", 0) == 0) s = recortar(s.substr(2));
    return s;
}

} // namespace jarvis

#endif // JARVIS_TEXTO_UTIL_HPP
