#include "SyntaxAnalyzer.h"
#include <iostream>

// --- Constructor ---
// Inicializa las referencias y pide el primer token para arrancar
SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer &lex, ErrorManager &err, ReportGenerator &rep)
    : lexer(lex), errorManager(err), reportGen(rep)
{
    avanzarToken();
}
// --- Filtro Antibasura (Ignora errores léxicos y los registra) ---
// --- Filtro Antibasura (Ignora errores léxicos y los registra) ---
void SyntaxAnalyzer::avanzarToken()
{
    // Pedimos un token
    currentToken = lexer.nextToken();

    // Si es un error léxico, lo registramos y pedimos otro hasta encontrar uno válido (o el fin)
    while (currentToken.getType() == TokenType::ERROR_LEXICO)
    {
        errorManager.addError(ErrorType::LEXICO, currentToken.getLexeme(),
                              "Simbolo o palabra no reconocida.",
                              currentToken.getLine(), currentToken.getColumn());

        currentToken = lexer.nextToken();
    }
}

void SyntaxAnalyzer::syncToCommaOrBracket()
{
    while (currentToken.getType() != TokenType::COMA &&
           currentToken.getType() != TokenType::COR_C &&
           currentToken.getType() != TokenType::FIN_ARCHIVO)
    {
        avanzarToken();
    }
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
        avanzarToken();
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
        avanzarToken();
    }
    // Si salimos del while porque encontramos el token de sincronización, lo consumimos
    if (currentToken.getType() == syncToken)
    {
        avanzarToken();
    }
}

// ======================================================================
// REGLAS DE LA GRAMÁTICA (Descenso Recursivo)
// ======================================================================

// <programa> ::= TABLERO CADENA "{" <columnas> "}"
void SyntaxAnalyzer::programa(int parentId)
{
    match(TokenType::RES_TABLERO, parentId);

    // ¡AQUÍ ATRAPAMOS EL DATO!
    // Guardamos el lexema de la cadena antes de que match() avance al siguiente token
    if (currentToken.getType() == TokenType::CADENA)
    {
        tableroActual.titulo = currentToken.getLexeme();
    }
    match(TokenType::CADENA, parentId);

    match(TokenType::LLAVE_A, parentId);

    int columnasNodeId = reportGen.addNode("<columnas>");
    reportGen.addChild(parentId, columnasNodeId);

    // Le pasamos la lista de columnas de nuestro tableroActual
    columnas(columnasNodeId, tableroActual.columnas);

    match(TokenType::LLAVE_C, parentId);
    if (currentToken.getType() == TokenType::PUNTO_Y_COMA)
    {
        match(TokenType::PUNTO_Y_COMA, parentId);
    }
}

// ===============================================
// BLOQUE DE COLUMNAS
// ===============================================

// <columnas> ::= <columna> <columnas_prima>
void SyntaxAnalyzer::columnas(int parentId, std::vector<ColumnaData> &columnasList)
{
    int columnaNodeId = reportGen.addNode("<columna>");
    reportGen.addChild(parentId, columnaNodeId);
    columna(columnaNodeId, columnasList); // Pasamos la lista hacia abajo

    int colPrimaNodeId = reportGen.addNode("<columnas_prima>");
    reportGen.addChild(parentId, colPrimaNodeId);
    columnas_prima(colPrimaNodeId, columnasList);
}

// <columnas_prima> ::= <columna> <columnas_prima> | ε
void SyntaxAnalyzer::columnas_prima(int parentId, std::vector<ColumnaData> &columnasList)
{
    if (currentToken.getType() == TokenType::RES_COLUMNA)
    {
        int columnaNodeId = reportGen.addNode("<columna>");
        reportGen.addChild(parentId, columnaNodeId);
        columna(columnaNodeId, columnasList);

        int colPrimaNodeId = reportGen.addNode("<columnas_prima>");
        reportGen.addChild(parentId, colPrimaNodeId);
        columnas_prima(colPrimaNodeId, columnasList);
    }
}

// <columna> ::= COLUMNA CADENA "(" <tareas_opt> ")" ";"
void SyntaxAnalyzer::columna(int parentId, std::vector<ColumnaData> &columnasList)
{
    // 1. Preparamos una nueva caja para esta columna
    ColumnaData nuevaColumna;

    match(TokenType::RES_COLUMNA, parentId);

    // 2. Atrapamos el nombre de la columna
    if (currentToken.getType() == TokenType::CADENA)
    {
        nuevaColumna.nombre = currentToken.getLexeme();
    }
    match(TokenType::CADENA, parentId);

    match(TokenType::LLAVE_A, parentId);

    // 3. Procesamos las tareas, pasándole la lista de la nueva columna
    int tareasOptNodeId = reportGen.addNode("<tareas_opt>");
    reportGen.addChild(parentId, tareasOptNodeId);
    tareas_opt(tareasOptNodeId, nuevaColumna.tareas);

    match(TokenType::LLAVE_C, parentId);
    match(TokenType::PUNTO_Y_COMA, parentId);

    // 4. Guardamos la columna terminada en la lista del tablero
    columnasList.push_back(nuevaColumna);
}

// ===============================================
// BLOQUE DE TAREAS
// ===============================================

// <tareas_opt> ::= <lista_tareas> | ε
void SyntaxAnalyzer::tareas_opt(int parentId, std::vector<TareaData> &tareasList)
{
    // El FIRST de <lista_tareas> es la palabra reservada 'tarea'
    if (currentToken.getType() == TokenType::RES_TAREA)
    {
        // 1. Creamos el nodo para la lista
        int list_tareas_id = reportGen.addNode("<lista_tareas>");
        reportGen.addChild(parentId, list_tareas_id);

        // 2. LLAMADA CLAVE: Pasamos el ID del nodo y la lista de datos
        lista_tareas(list_tareas_id, tareasList);
    }
    else
    {
        // 3. CAMINO EPSILON (ε): Si la columna está vacía, dibujamos el símbolo épsilon
        // El 'true' es para que se pinte de color celeste (terminal)
        int epsilonId = reportGen.addNode("ε", true);
        reportGen.addChild(parentId, epsilonId);
    }
}

// <lista_tareas> ::= <tarea> <tareas_prima>
void SyntaxAnalyzer::lista_tareas(int parentId, std::vector<TareaData> &tareasList)
{
    int tar = reportGen.addNode("<tarea>");
    reportGen.addChild(parentId, tar);
    tarea(tar, tareasList); // <- ¡Aquí pasamos la lista!

    int tar_prima = reportGen.addNode("<tareas_prima>");
    reportGen.addChild(parentId, tar_prima);
    tareas_prima(tar_prima, tareasList); // <- ¡Y aquí también!
}

// <tareas_prima> ::= "," <tarea> <tareas_prima> | ε
void SyntaxAnalyzer::tareas_prima(int parentId, std::vector<TareaData> &tareasList)
{
    if (currentToken.getType() == TokenType::COMA)
    {
        // Graficamos y consumimos la coma
        match(TokenType::COMA, parentId);

        if (currentToken.getType() == TokenType::RES_TAREA)
        {
            int tar = reportGen.addNode("<tarea>");
            reportGen.addChild(parentId, tar);
            tarea(tar, tareasList);

            int tar_prima = reportGen.addNode("<tareas_prima>");
            reportGen.addChild(parentId, tar_prima);
            tareas_prima(tar_prima, tareasList);
        }
        else
        {
            // Era una coma final (trailing comma). Aplicamos el camino épsilon (ε)
            int epsilonId = reportGen.addNode("ε", true);
            reportGen.addChild(parentId, epsilonId);
        }
    }
    else
    {
        // CAMINO EPSILON normal
        int epsilonId = reportGen.addNode("ε", true);
        reportGen.addChild(parentId, epsilonId);
    }
}

// <tarea> ::= tarea ":" CADENA "[" <atributos> "]"
void SyntaxAnalyzer::tarea(int parentId, std::vector<TareaData> &tareasList)
{
    // 1. Preparamos una nueva caja para esta tarea
    TareaData nuevaTarea;

    match(TokenType::RES_TAREA, parentId);
    match(TokenType::DOS_PUNTOS, parentId);

    // 2. Atrapamos el nombre de la tarea
    if (currentToken.getType() == TokenType::CADENA)
    {
        nuevaTarea.nombre = currentToken.getLexeme();
    }
    match(TokenType::CADENA, parentId);
    match(TokenType::COR_A, parentId);

    int atributosNodeId = reportGen.addNode("<atributos>");
    reportGen.addChild(parentId, atributosNodeId);

    // 3. Llamamos a atributos() pasándole la nuevaTarea para que la llene
    atributos(atributosNodeId, nuevaTarea);

    if (currentToken.getType() != TokenType::COR_C && currentToken.getType() != TokenType::FIN_ARCHIVO)
    {

        // Registrar que la sintaxis falló adentro de los corchetes
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(),
                              "Falta una coma ',' o se esperaba cerrar con ']'.",
                              currentToken.getLine(), currentToken.getColumn());

        panicModeSync(TokenType::COR_C);
    }
    else
    {
        match(TokenType::COR_C, parentId);
    }

    // 4. Guardamos la tarea en la lista de la columna
    tareasList.push_back(nuevaTarea);
}
// ===============================================
// BLOQUE DE ATRIBUTOS
// ===============================================

// <atributos> ::= <atributo> <atributos_prima>
// <atributos> ::= <atributo> <atributos_prima>
void SyntaxAnalyzer::atributos(int parentId, TareaData &tareaActual)
{
    // 1. Llamamos al primer atributo
    int atributoNodeId = reportGen.addNode("<atributo>");
    reportGen.addChild(parentId, atributoNodeId);
    atributo(atributoNodeId, tareaActual);

    // 2. Llamamos a la recursividad por si hay más atributos separados por coma
    int atrPrimaNodeId = reportGen.addNode("<atributos_prima>");
    reportGen.addChild(parentId, atrPrimaNodeId);
    atributos_prima(atrPrimaNodeId, tareaActual);
}

// <atributos_prima> ::= "," <atributo> <atributos_prima> | ε
void SyntaxAnalyzer::atributos_prima(int parentId, TareaData &tareaActual)
{
    // El FIRST de esta regla es la coma (,)
    if (currentToken.getType() == TokenType::COMA)
    {
        // 1. Graficamos y consumimos la coma
        match(TokenType::COMA, parentId);

        // 2. Graficamos y llamamos al siguiente atributo
        int atributoNodeId = reportGen.addNode("<atributo>");
        reportGen.addChild(parentId, atributoNodeId);
        atributo(atributoNodeId, tareaActual);

        // 3. Llamada recursiva para ver si hay MÁS atributos después
        int atrPrimaNodeId = reportGen.addNode("<atributos_prima>");
        reportGen.addChild(parentId, atrPrimaNodeId);
        atributos_prima(atrPrimaNodeId, tareaActual);
    }
    else
    {
        // CAMINO EPSILON (ε): Ya no hay coma, se acabaron los atributos de esta tarea.
        int epsilonId = reportGen.addNode("ε", true);
        reportGen.addChild(parentId, epsilonId);
    }
}

// <atributo> ::= prioridad ":" <prioridad> | responsable ":" CADENA | fecha_limite ":" FECHA
void SyntaxAnalyzer::atributo(int parentId, TareaData &tareaActual)
{
    if (currentToken.getType() == TokenType::RES_PRIORIDAD)
    {
        match(TokenType::RES_PRIORIDAD, parentId);
        match(TokenType::DOS_PUNTOS, parentId);

        int prioridadNodeId = reportGen.addNode("<prioridad>");
        reportGen.addChild(parentId, prioridadNodeId);
        prioridad(prioridadNodeId, tareaActual);
    }
    else if (currentToken.getType() == TokenType::RES_RESPONSABLE)
    {
        match(TokenType::RES_RESPONSABLE, parentId);
        match(TokenType::DOS_PUNTOS, parentId);

        // Atrapamos el responsable
        if (currentToken.getType() == TokenType::CADENA)
        {
            tareaActual.responsable = currentToken.getLexeme();
        }
        match(TokenType::CADENA, parentId);
    }
    else if (currentToken.getType() == TokenType::RES_FECHA_LIMITE)
    {
        match(TokenType::RES_FECHA_LIMITE, parentId);
        match(TokenType::DOS_PUNTOS, parentId);

        // Atrapamos la fecha
        if (currentToken.getType() == TokenType::FECHA)
        {
            tareaActual.fecha_limite = currentToken.getLexeme();
        }
        match(TokenType::FECHA, parentId);
    }
    else
    {
        // Si no es un atributo válido, lo registramos.
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(),
                              "Se esperaba un atributo (prioridad, responsable o fecha_limite).",
                              currentToken.getLine(), currentToken.getColumn());
    }
    // Si terminamos en un token que NO es coma ni corchete, significa que hubo basura adentro del atributo.
    // Sincronizamos para que la siguiente función pueda leer en paz.
    if (currentToken.getType() != TokenType::COMA && currentToken.getType() != TokenType::COR_C)
    {
        syncToCommaOrBracket();
    }
}

// <prioridad> ::= ALTA | MEDIA | BAJA
void SyntaxAnalyzer::prioridad(int parentId, TareaData &tareaActual)
{
    if (currentToken.getType() == TokenType::PRIO_ALTA)
    {
        tareaActual.prioridad = "ALTA";
        match(TokenType::PRIO_ALTA, parentId);
    }
    else if (currentToken.getType() == TokenType::PRIO_MEDIA)
    {
        tareaActual.prioridad = "MEDIA";
        match(TokenType::PRIO_MEDIA, parentId);
    }
    else if (currentToken.getType() == TokenType::PRIO_BAJA)
    {
        tareaActual.prioridad = "BAJA";
        match(TokenType::PRIO_BAJA, parentId);
    }
    else
    {
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(),
                              "Se esperaba nivel de prioridad (ALTA, MEDIA, BAJA).",
                              currentToken.getLine(), currentToken.getColumn());
        // ¡NADA MÁS AQUÍ!
    }
}