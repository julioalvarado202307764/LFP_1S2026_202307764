#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Token.h"
#include "ErrorManager.h"

// Una estructura sencilla para representar los nodos del árbol de derivación
struct ParseNode {
    int id;
    std::string label;
    bool isTerminal; // Para pintar de distinto color los tokens finales
    std::vector<int> childrenIds; // IDs de los nodos hijos
};

class ReportGenerator {
private:
    std::vector<ParseNode> treeNodes;
    int nextNodeId = 0;

public:
    ReportGenerator() {}

    // --- Métodos para construir el árbol desde el Parser ---
    
    // Crea un nodo y retorna su ID
    int addNode(const std::string& label, bool isTerminal = false) {
        int id = nextNodeId++;
        treeNodes.push_back({id, label, isTerminal, {}});
        return id;
    }

    // Conecta un nodo padre con un nodo hijo
    void addChild(int parentId, int childId) {
        treeNodes[parentId].childrenIds.push_back(childId);
    }

    // --- Generación de Archivos ---

    // Genera el archivo arbol.dot con el código de Graphviz
    void generateGraphviz(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;

        file << "digraph ArbolDerivacion {\n";
        file << "    rankdir=TB;\n";
        file << "    node [shape=box, style=filled, fontname=\"Arial\"];\n\n";

        // 1. Declarar todos los nodos con sus colores
        for (const auto& node : treeNodes) {
            std::string fillColor = node.isTerminal ? "\"#D6EAF8\"" : "\"#2E75B6\"";
            std::string fontColor = node.isTerminal ? "black" : "white";
            
            // Escapamos las comillas dobles para Graphviz
            std::string safeLabel = node.label;
            if (safeLabel == "\"") safeLabel = "\\\""; 
            
            file << "    n" << node.id << " [label=\"" << safeLabel 
                 << "\", fillcolor=" << fillColor << ", fontcolor=" << fontColor << "];\n";
        }

        file << "\n    // Conexiones\n";

        // 2. Crear las flechas entre nodos
        for (const auto& node : treeNodes) {
            for (int childId : node.childrenIds) {
                file << "    n" << node.id << " -> n" << childId << ";\n";
            }
        }

        file << "}\n";
        file.close();
    }

    // (Aquí luego agregaremos los métodos para generar los 3 HTML)
    void generateHTMLReports(/* parámetros del tablero */) {
        // Por implementar...
    }
};