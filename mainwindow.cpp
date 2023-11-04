#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "olop.hpp"
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent, QString url) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CustomWebEnginePage* customPage = new CustomWebEnginePage();
    ui->Web->setPage(customPage);

    connect(customPage, &CustomWebEnginePage::customJavaScriptConsoleMessage,
            [](QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
            {
                QString formattedSourceInfo;
                if(sourceID.isEmpty()) {
                    formattedSourceInfo = QString::number(lineNumber) + ":";
                } else {
                    formattedSourceInfo = QFileInfo(sourceID).fileName() + ":" + QString::number(lineNumber);
                }

                switch (level) {
                case QWebEnginePage::InfoMessageLevel:
                    qDebug().noquote() << "[CONSOLE]" << formattedSourceInfo << message;
                    break;
                case QWebEnginePage::WarningMessageLevel:
                case QWebEnginePage::ErrorMessageLevel:
                    qWarning().noquote() << "[CONSOLE]" << formattedSourceInfo << message;
                    break;
                default:
                    qDebug().noquote() << "[CONSOLE]" << message;
                    break;
                }
            });
    ui->Web->setUrl(QUrl("http://localhost:"+url+"/Loading.html"));
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
    connect(this, &MainWindow::resizeEvent, this, &MainWindow::resizeEvent);
    connect(ui->Web, &QWebEngineView::loadFinished, this, [=](bool ok) {
        onLoadingHtmlFinished(ok, "http://localhost:"+url+"/");
    });
    connect(static_cast<CustomWebEnginePage*>(ui->Web->page()), &CustomWebEnginePage::urlRequested, this,
            [=](const QUrl& url) {
                QDesktopServices::openUrl(url);
            });
    // Initialisation de UiInstance
    uiInstance = new Ui::UiInstance(ui->Web->page(), url);

    qDebug() << "Olop a été initialisé";
}

MainWindow::~MainWindow()
{
    delete uiInstance;  // Ajoutez cette ligne
    delete ui;
}

void MainWindow::onLoadingHtmlFinished(bool ok, const QString &url)
{
    if (!ok)
        return;

    QWebEngineView *view = qobject_cast<QWebEngineView *>(sender());
    if (!view)
        return;

    QUrl currentUrl = view->url();
    if (currentUrl.path() == "/Loading.html" && currentUrl.host() == "localhost")
    {
        QString jsCommand = QString("loadOlop('%1');").arg(url);
        ui->Web->page()->runJavaScript(jsCommand);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    // Redimensionner le widget QWebEngine avec la fenêtre principale
    ui->Web->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
    ui->Web->page()->runJavaScript("MAIN.ResizeEvent()");
}

void MainWindow::on_actionQuitter_triggered()
{
    exit(0);
}
