#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "olop.hpp"

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
    connect(this, &MainWindow::resizeEvent, this, &MainWindow::resizeEvent);

    qDebug() << "Olop a été initialisé";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    // Redimensionner le widget QWebEngine avec la fenêtre principale
    ui->Web->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
}

void MainWindow::on_actionQuitter_triggered()
{
    exit(0);
}
