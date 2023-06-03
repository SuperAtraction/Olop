#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, QString url = "http://olop.rf.gd/Errors?id=1");
    ~MainWindow();

private slots:
    void on_ZoomM_clicked();

    void on_ZoomL_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
