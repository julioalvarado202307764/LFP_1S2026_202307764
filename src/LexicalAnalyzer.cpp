#include "LexicalAnalyzer.h"
#include <cctype> // Nos servirá para usar funciones nativas como isalpha(), isdigit(), etc.

LexicalAnalyzer::LexicalAnalyzer(const std::string& code) {
    sourceCode = code;
    currentIndex = 0;
    currentLine = 1;
    currentColumn = 1;
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

            case 1: // S1: Capturando letras (LL*)
                if (isalpha(c) || c == '_') { // Agregamos '_' por 'fecha_limite'
                    lexeme += c;
                } else {
                    // ¡Encontramos un carácter que no es letra! 
                    // El token terminó. NO avanzamos el currentIndex para que 
                    // el próximo nextToken() analice este carácter.
                    
                    // Aquí verificaríamos si 'lexeme' es una palabra reservada o un error
                    // return Token(TokenType::RES_TABLERO, lexeme, startLine, startCol);
                }
                break;

            case 3: // S3: Capturando cadena de texto
                // ... la lógica para buscar la comilla de cierre ...
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