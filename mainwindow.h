#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlocalsocket.h"
#include "qwebenginepage.h"
#include <QMainWindow>
#include <QtCore>
#include <QFile>
#include <QLocalServer>

namespace Ui {
class UiInstance : public QObject {
    Q_OBJECT
public:
    UiInstance(QWebEnginePage* page, QString port) : m_page(page) {
        m_server.listen("Olop-threads-"+port);
        qDebug() << "Olop-threads-"+port;
        connect(&m_server, &QLocalServer::newConnection, this, &UiInstance::handleNewConnection);
    }

private slots:
    void handleNewConnection() {
        QLocalSocket* clientSocket = m_server.nextPendingConnection();
        connect(clientSocket, &QLocalSocket::readyRead, [this, clientSocket]() {
            QString script = QString::fromUtf8(clientSocket->readAll());
            if (script.startsWith("1")) { // vérifier la condition de la réponse
                script = script.mid(1);  // retirez le "1" au début
                m_page->runJavaScript(script);
            }
            clientSocket->disconnectFromServer();
        });
    }

private:
    QLocalServer m_server;
    QWebEnginePage* m_page;
};

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, QString url="");
    ~MainWindow();
    Ui::MainWindow *ui;

private slots:
    void on_actionQuitter_triggered();
    void resizeEvent(QResizeEvent* event);
    void onLoadingHtmlFinished(bool ok, const QString &url);

private:
    Ui::UiInstance *uiInstance;

};

class CustomWebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    CustomWebEnginePage(QObject* parent = nullptr) : QWebEnginePage(parent) {}

signals:
    void customJavaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID);

protected:
    virtual void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override
    {
        emit customJavaScriptConsoleMessage(level, message, lineNumber, sourceID);
    }
};

#endif // MAINWINDOW_H
