#include "SyntaxAnalyzer.h"
#include <iostream>

// --- Constructor ---
// Inicializa las referencias y pide el primer token para arrancar
SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer& lex, ErrorManager& err) 
    : lexer(lex), errorManager(err) {
    currentToken = lexer.nextToken();
}

// --- Método de Arranque ---
void SyntaxAnalyzer::parse() {
    programa(); // Llamamos a la regla raíz de nuestra gramática
    
    // Al terminar de analizar, deberíamos estar en el fin del archivo
    if (currentToken.getType() != TokenType::FIN_ARCHIVO) {
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(), 
                              "Se esperaba el fin del archivo.", 
                              currentToken.getLine(), currentToken.getColumn());
    }
}

// --- El Guardián: match() ---
void SyntaxAnalyzer::match(TokenType expectedType) {
    if (currentToken.getType() == expectedType) {
        // ¡Todo en orden! El token es el correcto. Pedimos el siguiente.
        currentToken = lexer.nextToken();
    } else {
        // ¡ERROR SINTÁCTICO!
        // Aquí registramos el error sin detener la ejecución de C++.
        std::string desc = "Error de sintaxis. Token inesperado.";
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(), 
                              desc, currentToken.getLine(), currentToken.getColumn());
        
        // Nota: En una versión más pulida, podríamos usar un switch con expectedType 
        // para generar mensajes como "Se esperaba ':' " o "Se esperaba ']' ".
    }
}

// --- Modo Pánico: Recuperación de Errores ---
// Si hay un desastre dentro de una regla, esta función devora tokens 
// hasta encontrar un "ancla" (como un ';' o un '}') para poder continuar.
void SyntaxAnalyzer::panicModeSync(TokenType syncToken) {
    while (currentToken.getType() != syncToken && 
           currentToken.getType() != TokenType::FIN_ARCHIVO) {
        currentToken = lexer.nextToken();
    }
    // Si salimos del while porque encontramos el token de sincronización, lo consumimos
    if (currentToken.getType() == syncToken) {
        currentToken = lexer.nextToken();
    }
}

// ======================================================================
// REGLAS DE LA GRAMÁTICA (Descenso Recursivo)
// ======================================================================

// <programa> ::= TABLERO CADENA "{" <columnas> "}"
void SyntaxAnalyzer::programa() {
    // Para no crashear si el archivo está totalmente mal desde el inicio,
    // podemos envolver bloques propensos a error con try-catch lógicos, 
    // pero por ahora seguiremos la estructura LL(1) pura:
    
    match(TokenType::RES_TABLERO);
    match(TokenType::CADENA);
    match(TokenType::LLAVE_A);   // Corresponde a '{'
    
    // Aquí llamaremos a la siguiente regla
     columnas(); // (Comentado hasta que la implementemos)
    
    match(TokenType::LLAVE_C);   // Corresponde a '}'
}

// ===============================================
// BLOQUE DE COLUMNAS
// ===============================================

// <columnas> ::= <columna> <columnas_prima>
void SyntaxAnalyzer::columnas() {
    columna();
    columnas_prima();
}

// <columnas_prima> ::= <columna> <columnas_prima> | ε
void SyntaxAnalyzer::columnas_prima() {
    // El conjunto FIRST de <columna> es la palabra reservada COLUMNA
    if (currentToken.getType() == TokenType::RES_COLUMNA) {
        columna();
        columnas_prima();
    }
    // Si no es COLUMNA, aplicamos transición ε (épsilon). 
    // Simplemente no hacemos nada y la recursividad termina solita.
}

// <columna> ::= COLUMNA CADENA "(" <tareas_opt> ")" ";"
void SyntaxAnalyzer::columna() {
    match(TokenType::RES_COLUMNA);
    match(TokenType::CADENA);
    match(TokenType::PAR_A);      // '('
    tareas_opt();
    match(TokenType::PAR_C);      // ')'
    match(TokenType::PUNTO_Y_COMA); // ';'
}

// ===============================================
// BLOQUE DE TAREAS
// ===============================================

// <tareas_opt> ::= <lista_tareas> | ε
void SyntaxAnalyzer::tareas_opt() {
    // El FIRST de <lista_tareas> es la palabra reservada 'tarea'
    if (currentToken.getType() == TokenType::RES_TAREA) {
        lista_tareas();
    }
    // Si es otra cosa (como un ')'), aplica ε y significa que la columna está vacía
}

// <lista_tareas> ::= <tarea> <tareas_prima>
void SyntaxAnalyzer::lista_tareas() {
    tarea();
    tareas_prima();
}

// <tareas_prima> ::= "," <tarea> <tareas_prima> | ε
void SyntaxAnalyzer::tareas_prima() {
    if (currentToken.getType() == TokenType::COMA) {
        match(TokenType::COMA);
        tarea();
        tareas_prima();
    }
    // ε si no hay coma
}

// <tarea> ::= tarea ":" CADENA "[" <atributos> "]"
void SyntaxAnalyzer::tarea() {
    match(TokenType::RES_TAREA);
    match(TokenType::DOS_PUNTOS);
    match(TokenType::CADENA);
    match(TokenType::COR_A); // '['
    
    atributos();
    
    // Aquí implementamos el "Modo Pánico" de forma sencilla.
    // Si hubo un error en los atributos, sincronizamos hasta el corchete de cierre
    // para no arruinar el análisis de la siguiente tarea.
    if (currentToken.getType() != TokenType::COR_C && currentToken.getType() != TokenType::FIN_ARCHIVO) {
        panicModeSync(TokenType::COR_C); 
    } else {
        match(TokenType::COR_C); // ']'
    }
}

// ===============================================
// BLOQUE DE ATRIBUTOS
// ===============================================

// <atributos> ::= <atributo> <atributos_prima>
void SyntaxAnalyzer::atributos() {
    atributo();
    atributos_prima();
}

// <atributos_prima> ::= "," <atributo> <atributos_prima> | ε
void SyntaxAnalyzer::atributos_prima() {
    if (currentToken.getType() == TokenType::COMA) {
        match(TokenType::COMA);
        atributo();
        atributos_prima();
    }
    // ε si no hay coma
}

// <atributo> ::= prioridad ":" <prioridad> | responsable ":" CADENA | fecha_limite ":" FECHA
void SyntaxAnalyzer::atributo() {
    if (currentToken.getType() == TokenType::RES_PRIORIDAD) {
        match(TokenType::RES_PRIORIDAD);
        match(TokenType::DOS_PUNTOS);
        prioridad();
    } 
    else if (currentToken.getType() == TokenType::RES_RESPONSABLE) {
        match(TokenType::RES_RESPONSABLE);
        match(TokenType::DOS_PUNTOS);
        match(TokenType::CADENA);
    } 
    else if (currentToken.getType() == TokenType::RES_FECHA_LIMITE) {
        match(TokenType::RES_FECHA_LIMITE);
        match(TokenType::DOS_PUNTOS);
        match(TokenType::FECHA);
    } 
    else {
        // Si no es ninguno de los 3 atributos válidos, registramos error sintáctico
        std::string desc = "Se esperaba un atributo (prioridad, responsable o fecha_limite).";
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(), 
                              desc, currentToken.getLine(), currentToken.getColumn());
                              
        // Consumimos el token malo para no quedarnos en un bucle infinito
        currentToken = lexer.nextToken(); 
    }
}

// <prioridad> ::= ALTA | MEDIA | BAJA
void SyntaxAnalyzer::prioridad() {
    if (currentToken.getType() == TokenType::PRIO_ALTA) {
        match(TokenType::PRIO_ALTA);
    } 
    else if (currentToken.getType() == TokenType::PRIO_MEDIA) {
        match(TokenType::PRIO_MEDIA);
    } 
    else if (currentToken.getType() == TokenType::PRIO_BAJA) {
        match(TokenType::PRIO_BAJA);
    } 
    else {
        std::string desc = "Se esperaba nivel de prioridad (ALTA, MEDIA, BAJA).";
        errorManager.addError(ErrorType::SINTACTICO, currentToken.getLexeme(), 
                              desc, currentToken.getLine(), currentToken.getColumn());
        currentToken = lexer.nextToken(); 
    }
}