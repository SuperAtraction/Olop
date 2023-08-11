#include <QtCore>
#include <QMessageBox>
#include "mainwindow.h"
#include "olop.hpp"
#include "qapplication.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if(!MAIN::INIT()){
        QMessageBox::critical(nullptr, "Olop - Erreur critique", "Erreur critique d'init !");
        exit(-1);
    }
    int port = MAIN::SERVER();

    MAIN::w = new MainWindow(nullptr, "http://localhost:" + QString::number(port) + "/index.html");
    MAIN::w->show();
    qDebug() << port;

    return app.exec();
}
