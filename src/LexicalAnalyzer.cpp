#include "LexicalAnalyzer.h"
#include <cctype> // Nos servirá para usar funciones nativas como isalpha(), isdigit(), etc.

LexicalAnalyzer::LexicalAnalyzer(const std::string& code) {
    sourceCode = code;
    currentIndex = 0;
    currentLine = 1;
    currentColumn = 1;
}

TokenType LexicalAnalyzer::checkReservedWord(const std::string& lexeme) {
    if (lexeme == "TABLERO") return TokenType::RES_TABLERO;
    if (lexeme == "COLUMNA") return TokenType::RES_COLUMNA;
    if (lexeme == "tarea") return TokenType::RES_TAREA;
    if (lexeme == "prioridad") return TokenType::RES_PRIORIDAD;
    if (lexeme == "responsable") return TokenType::RES_RESPONSABLE;
    if (lexeme == "fecha_limite") return TokenType::RES_FECHA_LIMITE;
    if (lexeme == "ALTA") return TokenType::PRIO_ALTA;
    if (lexeme == "MEDIA") return TokenType::PRIO_MEDIA;
    if (lexeme == "BAJA") return TokenType::PRIO_BAJA;
    
    // Si no es ninguna de las anteriores, teóricamente en este lenguaje 
    // no hay identificadores libres (como nombres de variables), así que 
    // si no es reservada, en este contexto estricto, podría considerarse un error léxico.
    // Pero por ahora, retornemos un token de error.
    return TokenType::ERROR_LEXICO; 
}  

Token LexicalAnalyzer::nextToken() {
    int state = 0;            // Siempre iniciamos en S0
    std::string lexeme = "";  // Aquí iremos concatenando los caracteres
    
    // Guardamos la posición inicial del token para reportarla
    int startLine = currentLine;
    int startCol = currentColumn;

    while (currentIndex < sourceCode.length()) {
        char c = sourceCode[currentIndex];

        switch (state) {
            case 0: // Estado Inicial (S0 en tu diagrama)
                // 1. Manejar espacios en blanco y saltos de línea (Avanzar sin cambiar de estado)
                if (isspace(c)) {
                    if (c == '\n') {
                        currentLine++;
                        currentColumn = 0; // Se reinicia porque sumaremos 1 al final del while
                    }
                    currentIndex++;
                    currentColumn++;
                    startLine = currentLine;
                    startCol = currentColumn;
                    continue; // Volvemos al inicio del while
                }
                
                // 2. Transición hacia S1 (Letras para palabras reservadas o atributos)
                else if (isalpha(c)) {
                    state = 1;
                    lexeme += c;
                }
                
                // 3. Transición hacia S3 (Inicio de cadena)
                else if (c == '"') {
                    state = 3;
                    lexeme += c;
                }
                
                // 4. Delimitadores (Directo al estado de aceptación S4)
                else if (c == '{' || c == '}' || c == '(' || c == ')' || 
                         c == '[' || c == ']' || c == ':' || c == ',' || c == ';') {
                    lexeme += c;
                    currentIndex++;
                    currentColumn++;
                    // Aquí devolveríamos el token del delimitador correspondiente
                    // Ejemplo rápido (luego lo optimizamos):
                    // if (c == '{') return Token(TokenType::LLAVE_A, lexeme, startLine, startCol);
                }
                
                // ... (Falta la transición S2 para números/fechas y el manejo de errores)
                break;

          case 1: // S1: Capturando letras para palabras reservadas
                // El estado S1 acepta letras y guion bajo (para 'fecha_limite')
                if (isalpha(c) || c == '_') { 
                    lexeme += c;
                    currentIndex++;
                    currentColumn++;
                } else {
                    // ¡Encontramos algo que no es letra! Terminó la palabra.
                    // IMPORTANTE: NO avanzamos el currentIndex. Así el próximo 
                    // nextToken() evalúa este nuevo carácter desde S0.
                    TokenType type = checkReservedWord(lexeme);
                    return Token(type, lexeme, startLine, startCol);
                }
                break;

          case 3: // S3: Capturando cadena de texto (inició con ")
                if (c == '"') {
                    // ¡Encontramos el cierre!
                    lexeme += c;
                    currentIndex++;
                    currentColumn++;
                    return Token(TokenType::CADENA, lexeme, startLine, startCol);
                } else if (c == '\n' || currentIndex >= sourceCode.length()) {
                    // REQUISITO CUMPLIDO: Error crítico por cadena sin cerrar antes de fin de línea.
                    // Cortamos el análisis de la cadena aquí y reportamos el error.
                    return Token(TokenType::ERROR_LEXICO, lexeme, startLine, startCol);
                } else {
                    // Cualquier otro carácter es parte de la cadena
                    lexeme += c;
                    currentIndex++;
                    currentColumn++;
                }
                break;
            // ... los demás estados ...
        }

        // Avanzamos los punteros si el estado consumió el carácter
        currentIndex++;
        currentColumn++;
    }

    // Si salimos del while, llegamos al final del archivo
    return Token(TokenType::FIN_ARCHIVO, "EOF", currentLine, currentColumn);
}

