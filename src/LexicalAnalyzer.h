#pragma once
#include <string>
#include "Token.h" // Asegúrate de incluir la clase Token que definimos antes

class LexicalAnalyzer {
private:
    std::string sourceCode;
    int currentIndex;
    int currentLine;
    int currentColumn;

public:
    // Constructor: recibe todo el texto del archivo .task
    LexicalAnalyzer(const std::string& code);

    // El método estrella: aplica el AFD para retornar el siguiente token válido
    Token nextToken();
};