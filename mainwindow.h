#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private slots:
    void on_actionQuitter_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
