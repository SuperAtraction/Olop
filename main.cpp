#include <QtCore>
#include <QMessageBox>
#include "mainwindow.h"
#include "olop.hpp"
#include "qapplication.h"

#define Version "alpha-1.2"

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "[INFO] %s\n", msg.toStdString().c_str());
        break;
    case QtWarningMsg:
        fprintf(stderr, "[ERREUR] %s\n", msg.toStdString().c_str());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "[CRITIQUE] %s\n", msg.toStdString().c_str());
        break;
    case QtFatalMsg:
        fprintf(stderr, "[FATAL] %s\n", msg.toStdString().c_str());
        abort();  // arrête le programme
    case QtInfoMsg:
        fprintf(stderr, "[INFO] %s\n", msg.toStdString().c_str());
        break;
    }
}

void handleProcessOutput(const QByteArray &output, const QString &prefix) {
    QString logMsg = QString(output).trimmed();
    if (logMsg.startsWith("[INFO]"))
        qDebug().noquote() << prefix << logMsg.mid(6).trimmed();
    else if (logMsg.startsWith("[ERREUR]"))
        qWarning().noquote() << prefix << logMsg.mid(8).trimmed();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qInstallMessageHandler(customMessageHandler);  // Installe le gestionnaire
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
        QObject::connect(serverProcess, &QProcess::readyReadStandardOutput, [=]() {
            QByteArray output = serverProcess->readAllStandardOutput();
            handleProcessOutput(output, "[SERVEUR]");
        });

        QObject::connect(serverProcess, &QProcess::readyReadStandardError, [=]() {
            QByteArray errorOutput = serverProcess->readAllStandardError();
            handleProcessOutput(errorOutput, "[SERVEUR]");
        });

        // Pour l'UI
        QObject::connect(uiProcess, &QProcess::readyReadStandardOutput, [=]() {
            QByteArray output = uiProcess->readAllStandardOutput();
            handleProcessOutput(output, "[UI]");
        });

        QObject::connect(uiProcess, &QProcess::readyReadStandardError, [=]() {
            QByteArray errorOutput = uiProcess->readAllStandardError();
            handleProcessOutput(errorOutput, "[UI]");
        });

        // Surveillance du serveur
        QObject::connect(serverProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus) {
            if (!*serverIntentionallyStopped) {
                qWarning() << "Le processus serveur a planté. Redémarrage...";
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
                qWarning() << "Le processus d'interface utilisateur a planté. Redémarrage...";
                uiProcess->start(appPath, uiArgs);
            }
        });

        // Démarrage des instances
        serverProcess->start(appPath, serverArgs);
        uiProcess->start(appPath, uiArgs);
    } else {
        if (parser.isSet(serverOption)) {
            QString port = parser.value(serverOption);
            NETWORK::port=port;
            qDebug() << "Lancement d'Olop sur le port "+port;
            MAIN::SERVER(port);
        }else if(parser.isSet(uiOption)){
            QString port = parser.value(uiOption);
            NETWORK::port=port;
            MAIN::w = new MainWindow(nullptr, port);
            qDebug() << port;
            MAIN::w->show();
        }
    }
    return app.exec();
}
