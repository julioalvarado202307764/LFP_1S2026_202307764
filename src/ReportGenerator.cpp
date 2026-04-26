#include "ReportGenerator.h"
#include <map>
#include <iomanip> // Para std::setprecision
// --- REPORTES HTML ---

void ReportGenerator::generateHTMLReports(const TableroData& tablero, const std::vector<ErrorItem>& errors) {
    // Generar los 3 reportes uno por uno
    generateTableroHTML(tablero);
    generateCargaResponsableHTML(tablero);
    generateErroresHTML(errors);
}

void ReportGenerator::generateTableroHTML(const TableroData& tablero) {
    std::ofstream file("Reporte1_Tablero.html");
    if (!file.is_open()) return;

    // 1. Cabecera HTML y CSS embebido
    file << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
         << "<meta charset=\"UTF-8\">\n<title>Tablero Kanban</title>\n"
         << "<style>\n"
         << "  body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f5f7; margin: 0; padding: 20px; }\n"
         << "  h1 { color: #172b4d; text-align: center; }\n"
         << "  .board { display: flex; gap: 20px; overflow-x: auto; padding-bottom: 20px; align-items: flex-start; }\n"
         << "  .column { background-color: #ebecf0; border-radius: 8px; width: 300px; min-width: 300px; padding: 10px; box-sizing: border-box; }\n"
         << "  .column-header { font-weight: bold; color: #172b4d; margin-bottom: 15px; padding-left: 5px; }\n"
         << "  .task-card { background-color: white; border-radius: 6px; padding: 12px; margin-bottom: 10px; box-shadow: 0 1px 2px rgba(0,0,0,0.1); border-left: 4px solid transparent; }\n"
         << "  .task-title { font-weight: 600; font-size: 14px; margin-bottom: 8px; }\n"
         << "  .task-detail { font-size: 12px; color: #5e6c84; margin-bottom: 4px; }\n"
         << "  /* Colores de Prioridad */\n"
         << "  .prio-ALTA { border-left-color: #ff5630; }\n"
         << "  .prio-MEDIA { border-left-color: #ffab00; }\n"
         << "  .prio-BAJA { border-left-color: #36b37e; }\n"
         << "  .badge { display: inline-block; padding: 2px 6px; border-radius: 3px; font-weight: bold; font-size: 11px; margin-bottom: 8px; }\n"
         << "  .badge-ALTA { background-color: #ffebe6; color: #bf2600; }\n"
         << "  .badge-MEDIA { background-color: #fffae6; color: #ff8b00; }\n"
         << "  .badge-BAJA { background-color: #e3fcef; color: #006644; }\n"
         << "</style>\n</head>\n<body>\n";

    // 2. Título del Tablero
    file << "<h1>" << tablero.titulo << "</h1>\n";
    
    // 3. Contenedor principal de las columnas (Flexbox)
    file << "<div class=\"board\">\n";

    for (const auto& col : tablero.columnas) {
        file << "  <div class=\"column\">\n"
             << "    <div class=\"column-header\">" << col.nombre << " (" << col.tareas.size() << ")</div>\n";

        // 4. Tarjetas de Tareas
        for (const auto& tarea : col.tareas) {
            file << "    <div class=\"task-card prio-" << tarea.prioridad << "\">\n"
                 << "      <div class=\"task-title\">" << tarea.nombre << "</div>\n"
                 << "      <span class=\"badge badge-" << tarea.prioridad << "\">" << tarea.prioridad << "</span>\n"
                 << "      <div class=\"task-detail\"><strong>Resp:</strong> " << tarea.responsable << "</div>\n"
                 << "      <div class=\"task-detail\"><strong>Límite:</strong> " << tarea.fecha_limite << "</div>\n"
                 << "    </div>\n";
        }
        
        file << "  </div>\n"; // Cierra columna
    }

    file << "</div>\n</body>\n</html>";
    file.close();
}

// ... (Aquí irían los métodos para Carga Responsable y Errores)
void ReportGenerator::generateCargaResponsableHTML(const TableroData& tablero) {
    std::ofstream file("Reporte2_Carga.html");
    if (!file.is_open()) return;

    // --- 1. Lógica Matemática Mejorada ---
    // Creamos una estructura temporal para guardar el desglose de cada persona
    struct InfoResponsable {
        int total = 0;
        int alta = 0;
        int media = 0;
        int baja = 0;
    };

    std::map<std::string, InfoResponsable> conteoResponsables;
    int totalTareas = 0;

    // Recorremos las tareas y sumamos donde corresponde
    for (const auto& col : tablero.columnas) {
        for (const auto& tarea : col.tareas) {
            conteoResponsables[tarea.responsable].total++;
            totalTareas++;
            
            if (tarea.prioridad == "ALTA") {
                conteoResponsables[tarea.responsable].alta++;
            } else if (tarea.prioridad == "MEDIA") {
                conteoResponsables[tarea.responsable].media++;
            } else if (tarea.prioridad == "BAJA") {
                conteoResponsables[tarea.responsable].baja++;
            }
        }
    }

    // --- 2. HTML y CSS ---
    file << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
         << "<meta charset=\"UTF-8\">\n<title>Carga de Responsables</title>\n"
         << "<style>\n"
         << "  body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f5f7; margin: 0; padding: 20px; }\n"
         << "  h1 { color: #172b4d; text-align: center; }\n"
         << "  .container { max-width: 900px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
         << "  table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n"
         << "  th, td { padding: 12px; text-align: left; border-bottom: 1px solid #dfe1e6; }\n"
         << "  th { background-color: #0078d4; color: white; font-weight: 600; } /* Cabecera azul estilo PDF */\n"
         << "  .dist-container { display: flex; align-items: center; gap: 10px; }\n"
         << "  .badge { background-color: #0078d4; color: white; padding: 2px 8px; border-radius: 4px; font-weight: bold; font-size: 12px; min-width: 35px; text-align: center; }\n"
         << "  .bar-bg { width: 100px; background-color: #ebecf0; border-radius: 4px; height: 12px; overflow: hidden; }\n"
         << "  .bar-fill { height: 100%; background-color: #0078d4; }\n"
         << "</style>\n</head>\n<body>\n";

    file << "<div class=\"container\">\n";
    file << "<h1>Reporte de Carga por Responsable</h1>\n";

    // --- 3. Construcción de la Tabla ---
    if (totalTareas == 0) {
        file << "<p style=\"text-align: center;\">No hay tareas asignadas en este tablero.</p>\n";
    } else {
        file << "<table>\n";
        file << "  <tr><th>Responsable</th><th>Tareas Asignadas</th><th>Alta</th><th>Media</th><th>Baja</th><th>Distribucion</th></tr>\n";

        for (const auto& par : conteoResponsables) {
            std::string nombre = par.first;
            InfoResponsable info = par.second;
            
            // Calculamos el porcentaje
            double porcentaje = (static_cast<double>(info.total) / totalTareas) * 100.0;

            file << "  <tr>\n"
                 << "    <td>" << nombre << "</td>\n"
                 << "    <td>" << info.total << "</td>\n"
                 << "    <td>" << info.alta << "</td>\n"
                 << "    <td>" << info.media << "</td>\n"
                 << "    <td>" << info.baja << "</td>\n"
                 << "    <td>\n"
                 << "      <div class=\"dist-container\">\n"
                 << "        <span class=\"badge\">" << std::fixed << std::setprecision(0) << porcentaje << "%</span>\n"
                 << "        <div class=\"bar-bg\">\n"
                 << "          <div class=\"bar-fill\" style=\"width: " << porcentaje << "%;\"></div>\n"
                 << "        </div>\n"
                 << "      </div>\n"
                 << "    </td>\n"
                 << "  </tr>\n";
        }
        file << "</table>\n";
    }

    file << "</div>\n</body>\n</html>";
    file.close();
}

void ReportGenerator::generateErroresHTML(const std::vector<ErrorItem>& errors) {
    std::ofstream file("Reporte3_Errores.html");
    if (!file.is_open()) return;

    // --- HTML y CSS con temática de Alerta/Error ---
    file << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
         << "<meta charset=\"UTF-8\">\n<title>Reporte de Errores</title>\n"
         << "<style>\n"
         << "  body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f5f7; margin: 0; padding: 20px; }\n"
         << "  h1 { color: #de350b; text-align: center; } /* Título rojo oscuro */\n"
         << "  .container { max-width: 900px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-top: 5px solid #de350b; }\n"
         << "  table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n"
         << "  th, td { padding: 12px; text-align: left; border-bottom: 1px solid #dfe1e6; }\n"
         << "  th { background-color: #ffebe6; color: #bf2600; font-weight: 600; text-transform: uppercase; font-size: 12px; } /* Cabecera rojita */\n"
         << "  .tipo-lexico { font-weight: bold; color: #ff8b00; } /* Naranja para Léxico */\n"
         << "  .tipo-sintactico { font-weight: bold; color: #bf2600; } /* Rojo para Sintáctico */\n"
         << "  .lexema { font-family: 'Courier New', Courier, monospace; background-color: #f4f5f7; padding: 2px 6px; border-radius: 4px; font-weight: bold; }\n"
         << "</style>\n</head>\n<body>\n";

    file << "<div class=\"container\">\n";
    file << "<h1>Tabla de Errores</h1>\n";

    // Si por milagro llaman a este reporte y no hay errores:
    if (errors.empty()) {
        file << "<p style=\"text-align: center; color: #006644; font-weight: bold;\">¡Felicidades! No se encontraron errores en el archivo de entrada.</p>\n";
    } else {
        file << "<table>\n";
        file << "  <tr><th>No.</th><th>Tipo</th><th>Línea</th><th>Columna</th><th>Lexema</th><th>Descripción</th></tr>\n";

        // Iteramos sobre el vector de errores
        for (const auto& err : errors) {
            std::string tipoStr = (err.type == ErrorType::LEXICO) ? "Léxico" : "Sintáctico";
            std::string claseTipo = (err.type == ErrorType::LEXICO) ? "tipo-lexico" : "tipo-sintactico";

            file << "  <tr>\n"
                 << "    <td>" << err.id << "</td>\n"
                 << "    <td class=\"" << claseTipo << "\">" << tipoStr << "</td>\n"
                 << "    <td>" << err.line << "</td>\n"
                 << "    <td>" << err.column << "</td>\n"
                 << "    <td><span class=\"lexema\">" << err.lexeme << "</span></td>\n"
                 << "    <td>" << err.description << "</td>\n"
                 << "  </tr>\n";
        }
        file << "</table>\n";
    }

    file << "</div>\n</body>\n</html>";
    file.close();
}