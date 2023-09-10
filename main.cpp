#include <QtCore>
#include <QMessageBox>
#include "mainwindow.h"
#include "olop.hpp"
#include "qapplication.h"

#define Version "alpha-1.2"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Olop");
    QApplication::setApplicationVersion(Version);
    QCommandLineParser parser;
    if(!MAIN::INIT()){
        QMessageBox::critical(nullptr, "Olop - Erreur critique", "Erreur critique d'init !");
        qDebug() << "Erreur critique d'init !";
        exit(-1);
    }

    parser.setApplicationDescription("Olop est un logiciel codé en HTML5/C++");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption serverOption(QStringList() << "server", "Lancer le serveur Olop", "port");
    parser.addOption(serverOption);
    QCommandLineOption uiOption(QStringList() << "ui", "Lancer l'interface Olop", "port");
    parser.addOption(uiOption);
    parser.process(app);

    QString appPath = QCoreApplication::applicationFilePath();


    if (QCoreApplication::arguments().size() == 1) {
        qDebug() << "Démarrage d'Olop...";

        // Déclaration des variables
        bool* serverIntentionallyStopped = new bool(false);

        // Destruction des pointeurs pour éviter des fuites de mémoire
        QObject::connect(&app, &QCoreApplication::aboutToQuit, [=]() {
            delete serverIntentionallyStopped;
        });

        // Système de crash report
        QProcess* serverProcess = new QProcess();
        QStringList serverArgs;
        QString port = QString::number(NETWORK::findAvailablePort());
        serverArgs << "--server" << port;

        QProcess* uiProcess = new QProcess();
        QStringList uiArgs;
        uiArgs << "--ui" << port;

        // Pour le serveur
        QObject::connect(serverProcess, &QProcess::readyReadStandardOutput, [serverProcess]() {
            QByteArray output = serverProcess->readAllStandardOutput();
            qDebug().noquote() << "[SERVEUR]" << QString::fromUtf8(output.trimmed());
        });

        QObject::connect(serverProcess, &QProcess::readyReadStandardError, [serverProcess]() {
            QByteArray errorOutput = serverProcess->readAllStandardError();
            qWarning().noquote() << "[SERVEUR]" << QString::fromUtf8(errorOutput.trimmed());
        });

        // Pour l'UI
        QObject::connect(uiProcess, &QProcess::readyReadStandardOutput, [uiProcess]() {
            QByteArray output = uiProcess->readAllStandardOutput();
            qDebug().noquote() << "[UI]" << QString::fromUtf8(output.trimmed());
        });

        QObject::connect(uiProcess, &QProcess::readyReadStandardError, [uiProcess]() {
            QByteArray errorOutput = uiProcess->readAllStandardError();
            qWarning().noquote() << "[UI]" << QString::fromUtf8(errorOutput.trimmed());
        });

        // Surveillance du serveur
        QObject::connect(serverProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus) {
            if (!*serverIntentionallyStopped) {
                qDebug() << "Le processus serveur a planté. Redémarrage...";
                serverProcess->start(appPath, serverArgs);
            }
        });

        // Surveillance de l'UI
        QObject::connect(uiProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus) {
            if (exitCode == 0) {
                *serverIntentionallyStopped = true;
                serverProcess->terminate();
                serverProcess->waitForFinished();
                qDebug() << "Olop est arrêté. À bientôt !";
                qApp->exit(0);
            } else {
                qDebug() << "Le processus d'interface utilisateur a planté. Redémarrage...";
                uiProcess->start(appPath, uiArgs);
            }
        });

        // Démarrage des instances
        serverProcess->start(appPath, serverArgs);
        uiProcess->start(appPath, uiArgs);
    } else {
        if (parser.isSet(serverOption)) {
            QString port = parser.value(serverOption);
            qDebug() << "Lancement d'Olop sur le port "+port;
            MAIN::SERVER(port);
        }else if(parser.isSet(uiOption)){
            QString port = parser.value(uiOption);
            MAIN::w = new MainWindow(nullptr, port);
            qDebug() << port;
            MAIN::w->show();
        }
    }
    return app.exec();
}
