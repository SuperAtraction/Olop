#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "olop.hpp"

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
                Q_UNUSED(level);
                Q_UNUSED(lineNumber);
                Q_UNUSED(sourceID);
                qDebug().noquote() << "[CONSOLE]" << message;
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

    qDebug() << "Olop a été initialisé";
}

MainWindow::~MainWindow()
{
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
