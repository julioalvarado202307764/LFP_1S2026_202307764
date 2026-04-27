#pragma once
#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabWidget>    // Para las pestañas
#include <QTableWidget>  // Para las tablas
#include <QDesktopServices> // Para abrir los HTML
#include <QUrl>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void cargarArchivo();
    void analizarCodigo();
    void abrirReporteTablero(); // Nuevos slots para abrir HTMLs
    void abrirReporteErrores();

private:
    QTextEdit *editorTexto;
    QPushButton *btnCargar;
    QPushButton *btnAnalizar;
    QPushButton *btnVerTablero;
    QPushButton *btnVerErrores;

    QTabWidget *tabWidget;
    QTableWidget *tablaTokens;
    QTableWidget *tablaErrores;

    void configurarTablas(); // Función auxiliar para no ensuciar el constructor
};