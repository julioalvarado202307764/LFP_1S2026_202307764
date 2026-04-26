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
    // Constructor por defecto (necesario para declarar variables Token vacías)
    Token() : type(TokenType::FIN_ARCHIVO), lexeme(""), line(0), column(0) {}
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
            case TokenType::RES_COLUMNA: return "Reservada COLUMNA";
            case TokenType::RES_TAREA: return "Reservada TAREA";
            case TokenType::RES_PRIORIDAD: return "Reservada PRIORIDAD";
            case TokenType::RES_RESPONSABLE: return "Reservada RESPONSABLE";
            case TokenType::RES_FECHA_LIMITE: return "Reservada FECHA_LIMITE";
            case TokenType::PRIO_ALTA: return "Prioridad ALTA";
            case TokenType::PRIO_MEDIA: return "Prioridad MEDIA";
            case TokenType::PRIO_BAJA: return "Prioridad BAJA";
            case TokenType::FECHA: return "Fecha";
            case TokenType::ENTERO: return "Numero Entero";
            case TokenType::CADENA: return "Cadena de Texto";
            case TokenType::LLAVE_A: return "Llave Abre {";
            case TokenType::LLAVE_C: return "Llave Cierra }";
            case TokenType::PAR_A: return "Parentesis Abre (";
            case TokenType::PAR_C: return "Parentesis Cierra )";
            case TokenType::COR_A: return "Corchete Abre [";
            case TokenType::COR_C: return "Corchete Cierra ]";
            case TokenType::DOS_PUNTOS: return "Dos Puntos :";
            case TokenType::COMA: return "Coma ,";
            case TokenType::PUNTO_Y_COMA: return "Punto y Coma ;";
            case TokenType::ERROR_LEXICO: return "Error Lexico";
            case TokenType::FIN_ARCHIVO: return "Fin de Archivo";
            default: return "Desconocido";
        }
    }
};