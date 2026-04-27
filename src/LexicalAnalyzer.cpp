#include "LexicalAnalyzer.h"
#include <cctype> // Nos servirá para usar funciones nativas como isalpha(), isdigit(), etc.

LexicalAnalyzer::LexicalAnalyzer(const std::string &code)
{
    sourceCode = code;
    currentIndex = 0;
    currentLine = 1;
    currentColumn = 1;
}

TokenType LexicalAnalyzer::checkReservedWord(const std::string &lexeme)
{
    if (lexeme == "TABLERO")
        return TokenType::RES_TABLERO;
    if (lexeme == "COLUMNA")
        return TokenType::RES_COLUMNA;
    if (lexeme == "tarea")
        return TokenType::RES_TAREA;
    if (lexeme == "prioridad")
        return TokenType::RES_PRIORIDAD;
    if (lexeme == "responsable")
        return TokenType::RES_RESPONSABLE;
    if (lexeme == "fecha_limite")
        return TokenType::RES_FECHA_LIMITE;
    if (lexeme == "ALTA")
        return TokenType::PRIO_ALTA;
    if (lexeme == "MEDIA")
        return TokenType::PRIO_MEDIA;
    if (lexeme == "BAJA")
        return TokenType::PRIO_BAJA;

    // Si no es ninguna de las anteriores, teóricamente en este lenguaje
    // no hay identificadores libres (como nombres de variables), así que
    // si no es reservada, en este contexto estricto, podría considerarse un error léxico.
    // Pero por ahora, retornemos un token de error.
    return TokenType::ERROR_LEXICO;
}

Token LexicalAnalyzer::nextToken()
{
    int state = 0;
    std::string lexeme = "";

    int startLine = currentLine;
    int startCol = currentColumn;

    while (currentIndex < sourceCode.length())
    {
        char c = sourceCode[currentIndex];

        switch (state)
        {
        case 0: // Estado Inicial
            if (isspace(c))
            {
                if (c == '\n')
                {
                    currentLine++;
                    currentColumn = 0;
                }
                currentIndex++;
                currentColumn++;
                startLine = currentLine;
                startCol = currentColumn;
            }
            else if (isalpha(c))
            {
                state = 1;
                lexeme += c;
                currentIndex++;
                currentColumn++;
            }
            else if (c == '"')
            {
                state = 3;
                lexeme += c;
                currentIndex++;
                currentColumn++;
            }
            else if (isdigit(c))
            {
                // ¡Faltaba esto! Transición al estado de números
                state = 2;
                lexeme += c;
                currentIndex++;
                currentColumn++;
            }
            else if (c == '{' || c == '}' || c == '(' || c == ')' ||
                     c == '[' || c == ']' || c == ':' || c == ',' || c == ';')
            {
                lexeme += c;
                currentIndex++;
                currentColumn++;

                // Ahora sí, mapeamos cada símbolo a su Token
                TokenType t;
                if (c == '{')
                    t = TokenType::LLAVE_A;
                else if (c == '}')
                    t = TokenType::LLAVE_C;
                else if (c == '(')
                    t = TokenType::PAR_A;
                else if (c == ')')
                    t = TokenType::PAR_C;
                else if (c == '[')
                    t = TokenType::COR_A;
                else if (c == ']')
                    t = TokenType::COR_C;
                else if (c == ':')
                    t = TokenType::DOS_PUNTOS;
                else if (c == ',')
                    t = TokenType::COMA;
                else if (c == ';')
                    t = TokenType::PUNTO_Y_COMA;

                Token token(t, lexeme, startLine, startCol);
                listaTokens.push_back(token);
                return token;
            }
            else
            {
                lexeme += c;
                currentIndex++;
                currentColumn++;
                Token token(TokenType::ERROR_LEXICO, lexeme, startLine, startCol);
                listaTokens.push_back(token);
                return token;
            }
            break;

        case 1: // S1: Palabras
            if (isalpha(c) || c == '_')
            {
                lexeme += c;
                currentIndex++;
                currentColumn++;
            }
            else
            {
                Token token(checkReservedWord(lexeme), lexeme, startLine, startCol);
                listaTokens.push_back(token);
                return token;
            }
            break;

        case 2: // S2: Números y posible inicio de Fecha
            if (isdigit(c))
            {
                lexeme += c;
                currentIndex++;
                currentColumn++;
            }
            else if (c == '-')
            {
                if (lexeme.length() == 4)
                {
                    lexeme += c;
                    state = 5; // Transición a fecha
                    currentIndex++;
                    currentColumn++;
                }
                else
                {
                    lexeme += c;
                    currentIndex++;
                    currentColumn++;
                    Token token(TokenType::ERROR_LEXICO, lexeme, startLine, startCol);
                    listaTokens.push_back(token);
                    return token;
                }
            }
            else
            {
                // Terminó de leer puros números
                Token token(TokenType::ENTERO, lexeme, startLine, startCol);
                listaTokens.push_back(token);
                return token;
            }
            break;

        case 3: // S3: Cadenas
            if (c == '"')
            {
                lexeme += c;
                currentIndex++;
                currentColumn++;
                Token token(TokenType::CADENA, lexeme, startLine, startCol);
                listaTokens.push_back(token);
                return token;
            }
            else if (c == '\n' || currentIndex >= sourceCode.length())
            {
                Token token(TokenType::ERROR_LEXICO, lexeme, startLine, startCol);
                listaTokens.push_back(token);
                return token;
            }
            else
            {
                lexeme += c;
                currentIndex++;
                currentColumn++;
            }
            break;

        case 5: // S5: Terminando la Fecha (AAAA-MM-DD)
            if (isdigit(c) || c == '-')
            {
                lexeme += c;
                currentIndex++;
                currentColumn++;

                if (lexeme.length() == 10)
                {
                    if (lexeme[7] == '-')
                    {
                        Token token(TokenType::FECHA, lexeme, startLine, startCol);
                        listaTokens.push_back(token);
                        return token;
                    }
                    else
                    {
                        Token token(TokenType::ERROR_LEXICO, lexeme, startLine, startCol);
                        listaTokens.push_back(token);
                        return token;
                    }
                }
            }
            else
            {
                return Token(TokenType::ERROR_LEXICO, lexeme, startLine, startCol);
            }
            break;
        }

        // ¡ELIMINAMOS EL currentIndex++ QUE ESTABA AQUÍ ABAJO!
        // Ahora cada estado es responsable de avanzar el índice solo cuando debe.
    }

    // Por si el archivo termina justo después de una palabra sin espacios al final
    if (state == 1)
    {
        Token token(checkReservedWord(lexeme), lexeme, startLine, startCol);
        listaTokens.push_back(token);
        return token;
    }

    Token token(TokenType::FIN_ARCHIVO, "EOF", currentLine, currentColumn);
    listaTokens.push_back(token);
    return token;
}