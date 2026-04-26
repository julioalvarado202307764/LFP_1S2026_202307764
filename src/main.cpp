#include <iostream>
#include <fstream>
#include <sstream>
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "ErrorManager.h"

int main() {
    // 1. Leer el archivo de prueba
    std::string filename = "tests/prueba1.task"; 
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();
    file.close();

 // 2. Instanciar los componentes en orden
    ErrorManager errorManager;
    LexicalAnalyzer lexer(sourceCode);
    ReportGenerator reportGen; // ¡Instanciamos el generador!
    
  // Le pasamos todo al parser
    SyntaxAnalyzer parser(lexer, errorManager, reportGen);

    std::cout << "Iniciando analisis sintactico...\n";
    std::cout << "========================================================\n";

    // 3. ¡Ejecutar el Parser!
    parser.parse();

    // 4. Revisar los resultados
    if (!errorManager.hasErrors()) {
        std::cout << " FELICIDADES! El archivo es sintacticamente CORRECTO. \n";
        // ¡Generamos el DOT!
        reportGen.generateGraphviz("arbol.dot");
        std::cout << " Archivo 'arbol.dot' generado exitosamente.\n";
    } else {
        std::cout << " SE ENCONTRARON ERRORES DURANTE EL ANALISIS \n";
        std::cout << "--------------------------------------------------------\n";
        std::cout << "No.\tTipo\t\tLinea\tCol\tLexema/Descripcion\n";
        std::cout << "--------------------------------------------------------\n";
        
        const auto& errores = errorManager.getErrors();
        for (const auto& err : errores) {
            std::string tipo = (err.type == ErrorType::LEXICO) ? "Lexico" : "Sintactico";
            
            std::cout << err.id << "\t" 
                      << tipo << "\t"
                      << err.line << "\t" 
                      << err.column << "\t"
                      << "'" << err.lexeme << "' - " << err.description << "\n";
        }
    }

    std::cout << "========================================================\n";

    return 0;
}