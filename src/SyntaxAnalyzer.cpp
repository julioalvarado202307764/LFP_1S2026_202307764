#include "SyntaxAnalyzer.h"
#include <iostream>

// --- Constructor ---
// Inicializa las referencias y pide el primer token para arrancar
SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer &lex, ErrorManager &err, ReportGenerator &rep)
    : lexer(lex), errorManager(err), reportGen(rep)
{
    currentToken = lexer.nextToken();
}

// --- Método de Arranque ---
void SyntaxAnalyzer::parse()
{
    // Creamos el nodo raíz absoluto del árbol
    int rootId = reportGen.addNode("<programa>");

    // Arrancamos el análisis pasándole el ID de la raíz
    programa(rootId);

    if (currentToken.getType() != TokenType::FIN_ARCHIVO)
    {
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(),
                              "Se esperaba el fin del archivo.",
                              currentToken.getLine(), currentToken.getColumn());
    }
}

// --- El Guardián: match() actualizado ---
// Retorna el ID del nodo terminal creado (útil si luego necesitas extraer info del árbol)
int SyntaxAnalyzer::match(TokenType expectedType, int parentId)
{
    if (currentToken.getType() == expectedType)
    {
        // 1. Creamos el nodo hoja (terminal = true) con el lexema actual
        int leafId = reportGen.addNode(currentToken.getLexeme(), true);

        // 2. Lo conectamos a su padre
        reportGen.addChild(parentId, leafId);

        // 3. Avanzamos
        currentToken = lexer.nextToken();
        return leafId;
    }
    else
    {
        std::string desc = "Error de sintaxis. Token inesperado.";
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(),
                              desc, currentToken.getLine(), currentToken.getColumn());
        return -1; // -1 indica que hubo un error y no se creó nodo
    }
}

// --- Modo Pánico: Recuperación de Errores ---
// Si hay un desastre dentro de una regla, esta función devora tokens
// hasta encontrar un "ancla" (como un ';' o un '}') para poder continuar.
void SyntaxAnalyzer::panicModeSync(TokenType syncToken)
{
    while (currentToken.getType() != syncToken &&
           currentToken.getType() != TokenType::FIN_ARCHIVO)
    {
        currentToken = lexer.nextToken();
    }
    // Si salimos del while porque encontramos el token de sincronización, lo consumimos
    if (currentToken.getType() == syncToken)
    {
        currentToken = lexer.nextToken();
    }
}

// ======================================================================
// REGLAS DE LA GRAMÁTICA (Descenso Recursivo)
// ======================================================================

// <programa> ::= TABLERO CADENA "{" <columnas> "}"
void SyntaxAnalyzer::programa(int parentId)
{
    // Los terminales se conectan directamente al parentId (<programa>)
    match(TokenType::RES_TABLERO, parentId);
    match(TokenType::CADENA, parentId);
    match(TokenType::LLAVE_A, parentId);

    // Para las reglas compuestas, creamos un sub-nodo azul y lo conectamos
    int columnasNodeId = reportGen.addNode("<columnas>");
    reportGen.addChild(parentId, columnasNodeId);

    // Llamamos a la función pasándole SU nuevo nodo padre
    columnas(columnasNodeId);

    match(TokenType::LLAVE_C, parentId);
}

// ===============================================
// BLOQUE DE COLUMNAS
// ===============================================

// <columnas> ::= <columna> <columnas_prima>
void SyntaxAnalyzer::columnas(int parentId)
{
    int columnaNodeId = reportGen.addNode("<columna>");
    reportGen.addChild(parentId, columnaNodeId);
    columna(columnaNodeId);

    int colPrimaNodeId = reportGen.addNode("<columnas_prima>");
    reportGen.addChild(parentId, colPrimaNodeId);
    columnas_prima(colPrimaNodeId);
}

// <columnas_prima> ::= <columna> <columnas_prima> | ε
void SyntaxAnalyzer::columnas_prima(int parentId)
{
    // El conjunto FIRST de <columna> es la palabra reservada COLUMNA
    if (currentToken.getType() == TokenType::RES_COLUMNA)
    {
        int columnaNodeId = reportGen.addNode("<columna>");
        reportGen.addChild(parentId, columnaNodeId);
        columna(columnaNodeId);

        int colPrimaNodeId = reportGen.addNode("<columnas_prima>");
        reportGen.addChild(parentId, colPrimaNodeId);
        columnas_prima(colPrimaNodeId);
    }
    // Si no es COLUMNA, aplicamos transición ε (épsilon).
    // Simplemente no hacemos nada y la recursividad termina solita.
}

// <columna> ::= COLUMNA CADENA "(" <tareas_opt> ")" ";"
void SyntaxAnalyzer::columna(int parentId)
{
    match(TokenType::RES_COLUMNA, parentId);
    match(TokenType::CADENA, parentId);
    match(TokenType::PAR_A, parentId); // '('

    int tareas = reportGen.addNode("<tareas_opt>");
    reportGen.addChild(parentId, tareas);
    tareas_opt(tareas);

    match(TokenType::PAR_C, parentId);        // ')'
    match(TokenType::PUNTO_Y_COMA, parentId); // ';'
}

// ===============================================
// BLOQUE DE TAREAS
// ===============================================

// <tareas_opt> ::= <lista_tareas> | ε
void SyntaxAnalyzer::tareas_opt(int parentId)
{
    // El FIRST de <lista_tareas> es la palabra reservada 'tarea'
    if (currentToken.getType() == TokenType::RES_TAREA)
    {
        int list_tareas = reportGen.addNode("<lista_tareas>");
        reportGen.addChild(parentId, list_tareas);
        lista_tareas(list_tareas);
    }
    // Si es otra cosa (como un ')'), aplica ε y significa que la columna está vacía
}

// <lista_tareas> ::= <tarea> <tareas_prima>
void SyntaxAnalyzer::lista_tareas(int parentId)
{
    int tar = reportGen.addNode("<tarea>");
    reportGen.addChild(parentId, tar);
    tarea(tar);

    int tar_prima = reportGen.addNode("<tareas_prima>");
    reportGen.addChild(parentId, tar_prima);
    tareas_prima(tar_prima);
}

// <tareas_prima> ::= "," <tarea> <tareas_prima> | ε
void SyntaxAnalyzer::tareas_prima(int parentId)
{
    if (currentToken.getType() == TokenType::COMA)
    {
        match(TokenType::COMA, parentId);
        int tar = reportGen.addNode("<tarea>");
        reportGen.addChild(parentId, tar);
        tarea(tar);

        int tar_prima = reportGen.addNode("<tareas_prima>");
        reportGen.addChild(parentId, tar_prima);
        tareas_prima(tar_prima);
    }
    // ε si no hay coma
}

// <tarea> ::= tarea ":" CADENA "[" <atributos> "]"
void SyntaxAnalyzer::tarea(int parentId)
{
    match(TokenType::RES_TAREA, parentId);
    match(TokenType::DOS_PUNTOS, parentId);
    match(TokenType::CADENA, parentId);
    match(TokenType::COR_A, parentId); // '['

    int atrib = reportGen.addNode("<atributos>");
    reportGen.addChild(parentId, atrib);
    atributos(atrib);

    // Aquí implementamos el "Modo Pánico" de forma sencilla.
    // Si hubo un error en los atributos, sincronizamos hasta el corchete de cierre
    // para no arruinar el análisis de la siguiente tarea.
    if (currentToken.getType() != TokenType::COR_C && currentToken.getType() != TokenType::FIN_ARCHIVO)
    {
        panicModeSync(TokenType::COR_C);
    }
    else
    {
        match(TokenType::COR_C, parentId); // ']'
    }
}

// ===============================================
// BLOQUE DE ATRIBUTOS
// ===============================================

// <atributos> ::= <atributo> <atributos_prima>
void SyntaxAnalyzer::atributos(int parentId)
{
    int atrib = reportGen.addNode("<atributo>");
    reportGen.addChild(parentId, atrib);
    atributo(atrib);

    int atributos_prm = reportGen.addNode("<atributos_prima");
    reportGen.addChild(parentId, atributos_prm);
    atributos_prima(atributos_prm);
}

// <atributos_prima> ::= "," <atributo> <atributos_prima> | ε
void SyntaxAnalyzer::atributos_prima(int parentId)
{
    if (currentToken.getType() == TokenType::COMA)
    {
        match(TokenType::COMA, parentId);
        int atrib = reportGen.addNode("<atributo>");
        reportGen.addChild(parentId, atrib);
        atributo(atrib);

        int atributos_prm = reportGen.addNode("<atributos_prima");
        reportGen.addChild(parentId, atributos_prm);
        atributos_prima(atributos_prm);
    }
    // ε si no hay coma
}

// <atributo> ::= prioridad ":" <prioridad> | responsable ":" CADENA | fecha_limite ":" FECHA
void SyntaxAnalyzer::atributo(int parentId)
{
    if (currentToken.getType() == TokenType::RES_PRIORIDAD)
    {
        match(TokenType::RES_PRIORIDAD, parentId);
        match(TokenType::DOS_PUNTOS, parentId);

        int priori = reportGen.addNode("<prioridad>");
        reportGen.addChild(parentId, priori);
        prioridad(priori);
    }
    else if (currentToken.getType() == TokenType::RES_RESPONSABLE)
    {
        match(TokenType::RES_RESPONSABLE, parentId);
        match(TokenType::DOS_PUNTOS, parentId);
    }
    else if (currentToken.getType() == TokenType::RES_FECHA_LIMITE)
    {
        match(TokenType::RES_FECHA_LIMITE, parentId);
        match(TokenType::DOS_PUNTOS, parentId);
        match(TokenType::FECHA, parentId);
    }
    else
    {
        // Si no es ninguno de los 3 atributos válidos, registramos error sintáctico
        std::string desc = "Se esperaba un atributo (prioridad, responsable o fecha_limite).";
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(),
                              desc, currentToken.getLine(), currentToken.getColumn());

        // Consumimos el token malo para no quedarnos en un bucle infinito
        currentToken = lexer.nextToken();
    }
}

// <prioridad> ::= ALTA | MEDIA | BAJA
void SyntaxAnalyzer::prioridad(int parentId)
{
    if (currentToken.getType() == TokenType::PRIO_ALTA)
    {
        match(TokenType::PRIO_ALTA, parentId);
    }
    else if (currentToken.getType() == TokenType::PRIO_MEDIA)
    {
        match(TokenType::PRIO_MEDIA, parentId);
    }
    else if (currentToken.getType() == TokenType::PRIO_BAJA)
    {
        match(TokenType::PRIO_BAJA, parentId);
    }
    else
    {
        std::string desc = "Se esperaba nivel de prioridad (ALTA, MEDIA, BAJA).";
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(),
                              desc, currentToken.getLine(), currentToken.getColumn());
        currentToken = lexer.nextToken();
    }
}