#pragma once
#include <string>

// Usamos enum class (C++11/17) para tener un tipado fuerte y evitar colisiones de nombres
enum class TokenType {
    // --- Palabras Reservadas ---
    RES_TABLERO,
    RES_COLUMNA,
    RES_TAREA,
    RES_PRIORIDAD,
    RES_RESPONSABLE,
    RES_FECHA_LIMITE,

    // --- Enumeraciones de Prioridad ---
    PRIO_ALTA,
    PRIO_MEDIA,
    PRIO_BAJA,

    // --- Literales ---
    FECHA,          // AAAA-MM-DD
    ENTERO,         // [0-9]+
    CADENA,         // "texto"

    // --- Delimitadores ---
    LLAVE_A,        // {
    LLAVE_C,        // }
    PAR_A,          // (
    PAR_C,          // )
    COR_A,          // [
    COR_C,          // ]
    DOS_PUNTOS,     // :
    COMA,           // ,
    PUNTO_Y_COMA,   // ;

    // --- Especiales ---
    ERROR_LEXICO,   // Para capturar caracteres no reconocidos o cadenas sin cerrar
    FIN_ARCHIVO     // EOF (Crucial para que el parser sepa cuándo detenerse)
};

class Token {
private:
    TokenType type;
    std::string lexeme;
    int line;
    int column;

public:
    // Constructor
    Token(TokenType t, std::string lex, int l, int c)
        : type(t), lexeme(lex), line(l), column(c) {}

    // Getters
    TokenType getType() const { return type; }
    std::string getLexeme() const { return lexeme; }
    int getLine() const { return line; }
    int getColumn() const { return column; }

    // Método utilitario: Te será MUY útil para imprimir la tabla de tokens
    // en los reportes HTML, convirtiendo el enum a un string legible.
    std::string getTypeName() const {
        switch (type) {
            case TokenType::RES_TABLERO: return "Reservada TABLERO";
            case TokenType::ENTERO: return "Número Entero";
            case TokenType::CADENA: return "Cadena de Texto";
            case TokenType::ERROR_LEXICO: return "Error Léxico";
            case TokenType::FIN_ARCHIVO: return "Fin de Archivo";
            // ... (completarías el resto de casos) ...
            default: return "Desconocido";
        }
    }
};