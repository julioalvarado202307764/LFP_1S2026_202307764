#include "MainWindow.h"
#include <QFile>
#include <QTextStream>

// Incluimos tu poderoso backend
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "ErrorManager.h"
#include "ReportGenerator.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. Configurar la ventana principal
    this->setWindowTitle("Proyecto LFP - Analizador Kanban");
    this->resize(800, 600);

    // 2. Crear los componentes
    editorTexto = new QTextEdit(this);
    editorTexto->setFontFamily("Consolas"); // Fuente de programador
    editorTexto->setFontPointSize(12);

    btnCargar = new QPushButton("Cargar Archivo (.task)", this);
    btnAnalizar = new QPushButton("Analizar Código 🔥", this);

    // 3. Organizar los componentes (Layout)
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(btnCargar);
    layout->addWidget(editorTexto);
    layout->addWidget(btnAnalizar);

    QWidget *widgetCentral = new QWidget(this);
    widgetCentral->setLayout(layout);
    this->setCentralWidget(widgetCentral);

    // 4. Conectar los botones a las funciones
    connect(btnCargar, &QPushButton::clicked, this, &MainWindow::cargarArchivo);
    connect(btnAnalizar, &QPushButton::clicked, this, &MainWindow::analizarCodigo);
}

void MainWindow::cargarArchivo() {
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Abrir archivo Task", "", "Task Files (*.task);;All Files (*)");
    
    if (!nombreArchivo.isEmpty()) {
        QFile archivo(nombreArchivo);
        if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream entrada(&archivo);
            editorTexto->setText(entrada.readAll());
            archivo.close();
        }
    }
}

void MainWindow::analizarCodigo() {
    // Obtenemos el texto del editor, lo pasamos a UTF-8 y luego a std::string
    std::string codigoFuente = editorTexto->toPlainText().toUtf8().constData();

    if (codigoFuente.empty()) {
        QMessageBox::warning(this, "Advertencia", "El editor está vacío. Carga o escribe algo de código.");
        return;
    }

    // ¡INSTANCIAMOS TU BACKEND!
    ErrorManager errorManager;
    LexicalAnalyzer lexer(codigoFuente);
    ReportGenerator reportGen;
    SyntaxAnalyzer parser(lexer, errorManager, reportGen);

    // ¡FUEGO!
    parser.parse();

    // Evaluamos los resultados
    if (!errorManager.hasErrors()) {
        reportGen.generateGraphviz("arbol.dot");
        reportGen.generateHTMLReports(parser.getTableroData(), errorManager.getErrors());
        
        QMessageBox::information(this, "¡Éxito!", "Análisis completado sin errores.\nLos reportes HTML y el árbol de derivación han sido generados.");
    } else {
        // Generamos solo el reporte de errores
        reportGen.generateErroresHTML(errorManager.getErrors());
        
        QMessageBox::critical(this, "Errores Encontrados", "Se encontraron errores Léxicos/Sintácticos.\nRevisa el archivo 'Reporte3_Errores.html' para más detalles.");
    }
}