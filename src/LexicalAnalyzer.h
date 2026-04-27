#pragma once
#include <string>
#include "Token.h" // Asegúrate de incluir la clase Token que definimos antes
#include <vector>

class LexicalAnalyzer {
private:
    std::string sourceCode;
    int currentIndex;
    int currentLine;
    int currentColumn;
    std::vector<Token> listaTokens;
    // Función auxiliar para verificar palabras reservadas
    TokenType checkReservedWord(const std::string& lexeme);
public:
    // Constructor: recibe todo el texto del archivo .task
    std::vector<Token> getTokens() const { return listaTokens; }
    LexicalAnalyzer(const std::string& code);

    // El método estrella: aplica el AFD para retornar el siguiente token válido
    Token nextToken();
};