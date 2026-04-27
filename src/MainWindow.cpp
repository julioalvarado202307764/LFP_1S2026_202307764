#include "MainWindow.h"
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
#include <QCoreApplication>
// Incluimos tu poderoso backend
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "ErrorManager.h"
#include "ReportGenerator.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("Proyecto LFP - Analizador Kanban");
    this->resize(900, 700);

    // 1. Crear los botones superiores
    btnCargar = new QPushButton("📂 Cargar Archivo (.task)", this);
    btnAnalizar = new QPushButton("🚀 Analizar Código", this);
    btnVerTablero = new QPushButton("📊 Ver Tablero HTML", this);
    btnVerErrores = new QPushButton("🚨 Ver Errores HTML", this);

    // Desactivamos los botones de reportes hasta que se analice algo
    btnVerTablero->setEnabled(false);
    btnVerErrores->setEnabled(false);

    // Layout para los botones (horizontal)
    QHBoxLayout *layoutBotones = new QHBoxLayout();
    layoutBotones->addWidget(btnCargar);
    layoutBotones->addWidget(btnAnalizar);
    layoutBotones->addWidget(btnVerTablero);
    layoutBotones->addWidget(btnVerErrores);

    // 2. Crear las Pestañas y Componentes
    tabWidget = new QTabWidget(this);
    
    editorTexto = new QTextEdit();
    editorTexto->setFontFamily("Consolas");
    editorTexto->setFontPointSize(12);

    tablaTokens = new QTableWidget();
    tablaErrores = new QTableWidget();
    configurarTablas(); // Llamamos a nuestra función de diseño

    // Agregar componentes a las pestañas
    tabWidget->addTab(editorTexto, "📝 Código Fuente");
    tabWidget->addTab(tablaTokens, "✅ Tabla de Tokens");
    tabWidget->addTab(tablaErrores, "❌ Tabla de Errores");

    // 3. Organizar todo en la ventana
    QVBoxLayout *layoutPrincipal = new QVBoxLayout();
    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->addWidget(tabWidget);

    QWidget *widgetCentral = new QWidget(this);
    widgetCentral->setLayout(layoutPrincipal);
    this->setCentralWidget(widgetCentral);

    // 4. Conectar señales y slots
    connect(btnCargar, &QPushButton::clicked, this, &MainWindow::cargarArchivo);
    connect(btnAnalizar, &QPushButton::clicked, this, &MainWindow::analizarCodigo);
    connect(btnVerTablero, &QPushButton::clicked, this, &MainWindow::abrirReporteTablero);
    connect(btnVerErrores, &QPushButton::clicked, this, &MainWindow::abrirReporteErrores);
}

void MainWindow::configurarTablas() {
    // Diseño Tabla Tokens
    tablaTokens->setColumnCount(4);
    tablaTokens->setHorizontalHeaderLabels({"Lexema", "Tipo", "Línea", "Columna"});
    tablaTokens->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaTokens->setEditTriggers(QAbstractItemView::NoEditTriggers); // Solo lectura

    // Diseño Tabla Errores
    tablaErrores->setColumnCount(5);
    tablaErrores->setHorizontalHeaderLabels({"Tipo", "Lexema", "Línea", "Columna", "Descripción"});
    tablaErrores->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaErrores->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    // ==========================================
    // 📊 LLENADO DE TABLAS VISUALES EN QT
    // ==========================================
    
    // 1. Limpiar tablas por si el usuario analiza varias veces
    tablaTokens->setRowCount(0);
    tablaErrores->setRowCount(0);

    // 2. Llenar la Tabla de Tokens
    const auto& tokens = lexer.getTokens(); // Traemos la lista de tu Lexer
    for (int i = 0; i < tokens.size(); ++i) {
        tablaTokens->insertRow(i); // Creamos una nueva fila
        
        // Columna 0: Lexema
        tablaTokens->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(tokens[i].getLexeme())));
        
        // Columna 1: Tipo (Nota: si tienes una función para pasar tu enum TokenType a string, úsala aquí)
        tablaTokens->setItem(i, 1, new QTableWidgetItem("Token Válido")); 
        
        // Columna 2: Línea
        tablaTokens->setItem(i, 2, new QTableWidgetItem(QString::number(tokens[i].getLine())));
        
        // Columna 3: Columna
        tablaTokens->setItem(i, 3, new QTableWidgetItem(QString::number(tokens[i].getColumn())));
    }

    // 3. Llenar la Tabla de Errores
    const auto& errores = errorManager.getErrors();
    for (int i = 0; i < errores.size(); ++i) {
        tablaErrores->insertRow(i);
        
        QString tipoStr = (errores[i].type == ErrorType::LEXICO) ? "Léxico" : "Sintáctico";
        
        tablaErrores->setItem(i, 0, new QTableWidgetItem(tipoStr));
        tablaErrores->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(errores[i].lexeme)));
        tablaErrores->setItem(i, 2, new QTableWidgetItem(QString::number(errores[i].line)));
        tablaErrores->setItem(i, 3, new QTableWidgetItem(QString::number(errores[i].column)));
        tablaErrores->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(errores[i].description)));
    }
   // Evaluamos los resultados
    if (!errorManager.hasErrors()) {
        reportGen.generateGraphviz("arbol.dot");
        reportGen.generateHTMLReports(parser.getTableroData(), errorManager.getErrors());
        
        // Encendemos el botón del tablero y apagamos el de errores
        btnVerTablero->setEnabled(true);
        btnVerErrores->setEnabled(false);
        
        QMessageBox::information(this, "¡Éxito!", "Análisis completado sin errores.");
    } else {
        reportGen.generateErroresHTML(errorManager.getErrors());
        
        // Encendemos el botón de errores y apagamos el de tablero
        btnVerTablero->setEnabled(false);
        btnVerErrores->setEnabled(true);
        
        QMessageBox::critical(this, "Errores Encontrados", "Se encontraron errores. Revisa la pestaña de Tabla de Errores.");
    }
}
void MainWindow::abrirReporteTablero() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/Reporte1_Tablero.html"));
}

void MainWindow::abrirReporteErrores() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/Reporte3_Errores.html"));
}