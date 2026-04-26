#pragma once
#include "LexicalAnalyzer.h"
#include "ErrorManager.h"
#include "ReportGenerator.h" // ¡No olvides incluir esto!

class SyntaxAnalyzer {
private:
    LexicalAnalyzer& lexer;
    ErrorManager& errorManager;
    ReportGenerator& reportGen; // Nueva referencia
    Token currentToken;

    // Modificamos match para que reciba el ID del nodo padre y retorne el ID del nodo creado
    int match(TokenType expectedType, int parentId);

    // Modificamos las reglas para que reciban el ID de su nodo padre
    void programa(int parentId);
    void columnas(int parentId);
    void columnas_prima(int parentId);
    void columna(int parentId);
    void tareas_opt(int parentId);
    void lista_tareas(int parentId);
    void tareas_prima(int parentId);
    void tarea(int parentId);
    void atributos(int parentId);
    void atributos_prima(int parentId);
    void atributo(int parentId);
    void prioridad(int parentId);

    void panicModeSync(TokenType syncToken);

public:
    // Actualizamos el constructor
    SyntaxAnalyzer(LexicalAnalyzer& lex, ErrorManager& err, ReportGenerator& rep);

    void parse();
};