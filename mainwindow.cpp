#include "mainwindow.h"
#include "Tabs/newtab.h"
#include "Tabs/settings.h"
#include "ui_mainwindow.h"
#include <QFile>


MainWindow::MainWindow(QString url) :
    QWidget(),
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

    qDebug() << "Olop a été initialisé";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_NewTab_clicked()
{
    // Créer un nouvel onglet
    QWidget *newTabWidget = new QWidget();
    ui->tabWidget->addTab(newTabWidget, "Nouvel onglet");

    // Ajouter le widget newtab dans le nouvel onglet
    QVBoxLayout *layout = new QVBoxLayout(newTabWidget);
    NewTab *newtab = new NewTab();
    layout->addWidget(newtab);
    newTabWidget->setLayout(layout);

    // Mettre le focus sur le nouvel onglet
    ui->tabWidget->setCurrentWidget(newTabWidget);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    QWidget *tabWidget = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    delete tabWidget;
    if (ui->tabWidget->count() < 1) {
        // Créer un nouvel onglet
        QWidget *newTabWidget = new QWidget();
        ui->tabWidget->addTab(newTabWidget, "Nouvel onglet");

        // Ajouter le widget newtab dans le nouvel onglet
        QVBoxLayout *layout = new QVBoxLayout(newTabWidget);
        NewTab *newtab = new NewTab();
        layout->addWidget(newtab);
        newTabWidget->setLayout(layout);

        // Mettre le focus sur le nouvel onglet
        ui->tabWidget->setCurrentWidget(newTabWidget);
    }
}

