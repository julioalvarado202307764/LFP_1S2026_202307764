#include <iostream>
#include <fstream>
#include <sstream>
#include "LexicalAnalyzer.h"

int main() {
    // 1. Leer el archivo de prueba
    // Ajusta esta ruta dependiendo de desde dónde ejecutes el binario
    std::string filename = "tests/prueba1.task"; 
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << "\n";
        std::cerr << "Verifica que la ruta sea correcta.\n";
        return 1;
    }

    // Leemos todo el contenido del archivo a un string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();
    file.close();

    // 2. Instanciar el analizador léxico
    LexicalAnalyzer lexer(sourceCode);
    Token token = lexer.nextToken();

    // 3. Imprimir el encabezado de la tabla para la consola
    std::cout << "========================================================\n";
    std::cout << "                  TABLA DE TOKENS                       \n";
    std::cout << "========================================================\n";
    std::cout << "Lexema\t\t\tTipo\t\t\tLinea\tCol\n";
    std::cout << "--------------------------------------------------------\n";

    // 4. Ciclo principal: pedimos tokens hasta llegar al final del archivo
    while (token.getType() != TokenType::FIN_ARCHIVO) {
        std::cout << token.getLexeme() << "\t\t\t"
                  << token.getTypeName() << "\t\t"
                  << token.getLine() << "\t"
                  << token.getColumn() << "\n";

        token = lexer.nextToken();
    }

    std::cout << "========================================================\n";
    std::cout << "Analisis lexico finalizado.\n";

    return 0;
}