#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, QString url) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile file(":/fluent_dark.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString qss = stream.readAll();
        file.close();
        // Appliquer le style QSS à la fenêtre principale
        this->setStyleSheet(qss);
    } else {
        qWarning() << "Impossible d'ouvrir le fichier QSS.";
    }
    ui->Web->setUrl(QUrl(url));

    qDebug() << "Olop a été initialisé";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuitter_triggered()
{
    exit(0);
}

