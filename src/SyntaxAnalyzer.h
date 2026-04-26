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
    void avanzarToken();
    void syncToCommaOrBracket();
    // --- ESTA ES LA CAJA PRINCIPAL ---
    TableroData tableroActual; // Aquí guardaremos todo para luego mandarlo a los reportes
    // Modificamos match para que reciba el ID del nodo padre y retorne el ID del nodo creado
    int match(TokenType expectedType, int parentId);

    // Modificamos las reglas para que reciban el ID de su nodo padre
    void programa(int parentId);
// A columnas() le pasamos la lista de columnas del tablero
    void columnas(int parentId, std::vector<ColumnaData>& columnasList);
    void columnas_prima(int parentId, std::vector<ColumnaData>& columnasList);

    // columna() necesita crear un ColumnaData y meterlo en la lista
    void columna(int parentId, std::vector<ColumnaData>& columnasList);

    // Las tareas necesitan saber a qué lista (de qué columna) pertenecen
    void tareas_opt(int parentId, std::vector<TareaData>& tareasList);
    void lista_tareas(int parentId, std::vector<TareaData>& tareasList);
    void tareas_prima(int parentId, std::vector<TareaData>& tareasList);
    
    // tarea() llenará un TareaData y lo agregará a la lista
    void tarea(int parentId, std::vector<TareaData>& tareasList);

    // Los atributos necesitan saber a qué tarea específica pertenecen
    void atributos(int parentId, TareaData& tareaActual);
    void atributos_prima(int parentId, TareaData& tareaActual);
    void atributo(int parentId, TareaData& tareaActual);
    void prioridad(int parentId, TareaData& tareaActual);

    void panicModeSync(TokenType syncToken);

public:
    // Actualizamos el constructor
    SyntaxAnalyzer(LexicalAnalyzer& lex, ErrorManager& err, ReportGenerator& rep);
    void parse();
    // Un getter para sacar el tablero cuando el análisis termine (para el main.cpp)
    const TableroData& getTableroData() const { return tableroActual; }
};