#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qwebenginepage.h"
#include <QMainWindow>
#include <QtCore>
#include <QFile>

namespace Ui {
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
