#pragma once
#include <string>
#include <vector>

// Enumeración para distinguir el tipo de error en la tabla final
enum class ErrorType {
    LEXICO,
    SINTACTICO
};

// Estructura ligera para guardar los datos de cada error
struct ErrorItem {
    int id;
    std::string lexeme;
    ErrorType type;
    std::string description;
    int line;
    int column;
};

class ErrorManager {
private:
    std::vector<ErrorItem> errors;
    int nextId = 1;

public:
    // Método para registrar un nuevo error (se usará en el Lexer y en el Parser)
    void addError(ErrorType type, const std::string& lexeme, const std::string& description, int line, int col) {
        errors.push_back({nextId++, lexeme, type, description, line, col});
    }

    // Retorna la lista completa para generar el reporte HTML luego
    const std::vector<ErrorItem>& getErrors() const {
        return errors;
    }

    // Útil para saber si podemos generar el árbol Graphviz o si el archivo estaba corrupto
    bool hasErrors() const {
        return !errors.empty();
    }
};