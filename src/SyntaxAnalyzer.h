#pragma once
#include "LexicalAnalyzer.h"
#include "ErrorManager.h"

class SyntaxAnalyzer {
private:
    LexicalAnalyzer& lexer;
    ErrorManager& errorManager;
    Token currentToken;

    // --- Método central de validación ---
    // Verifica si el token actual es el esperado. Si lo es, avanza al siguiente.
    // Si no, reporta un error sintáctico.
    void match(TokenType expectedType);

    // --- Métodos del Parser Descendente Recursivo ---
    // Cada producción de tu gramática LL(1) se convierte en una función
    void programa();
    void columnas();
    void columnas_prima();
    void columna();
    void tareas_opt();
    void lista_tareas();
    void tareas_prima();
    void tarea();
    void atributos();
    void atributos_prima();
    void atributo();
    void prioridad();

    // Método auxiliar para el modo pánico (recuperación de errores)
    void panicModeSync(TokenType syncToken);

public:
    // El constructor recibe referencias al lexer y al gestor de errores
    SyntaxAnalyzer(LexicalAnalyzer& lex, ErrorManager& err);

    // Inicia el proceso de análisis
    void parse();
};