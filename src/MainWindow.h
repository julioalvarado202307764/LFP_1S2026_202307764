#pragma once
#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void cargarArchivo();
    void analizarCodigo();

private:
    QTextEdit *editorTexto;
    QPushButton *btnCargar;
    QPushButton *btnAnalizar;
};