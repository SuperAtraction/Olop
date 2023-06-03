#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent, QString url) :
    QWidget(parent),
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
    ui->webEngineView->setUrl(QUrl(url));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ZoomM_clicked()
{
    ui->webEngineView->setZoomFactor(ui->webEngineView->zoomFactor()+0.1);
    ui->ZoomValue->setText(QString::number(ui->webEngineView->zoomFactor()*100)+" %");
}

void MainWindow::on_ZoomL_clicked()
{
    ui->webEngineView->setZoomFactor(ui->webEngineView->zoomFactor()-0.1);
    ui->ZoomValue->setText(QString::number(ui->webEngineView->zoomFactor()*100)+" %");
}
