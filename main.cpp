#include <QtCore>
#include <QMessageBox>
#include "mainwindow.h"
#include "olop.hpp"
#include "qapplication.h"
#include "OSSpecialClasses/SystemAppInstaller.hpp"

#define Version "alpha-1.2"
int mode = 1;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString timestamp;
    if (mode == 1) {
        timestamp = "["+QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss.zzz")+"]";
    } else {
        timestamp = "";  // Laisser le timestamp vide si mode n'est pas égal à 1
    }

    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s[INFO] %s\n", timestamp.toStdString().c_str(), msg.toStdString().c_str());
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s[ERREUR] %s\n", timestamp.toStdString().c_str(), msg.toStdString().c_str());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s[CRITIQUE] %s\n", timestamp.toStdString().c_str(), msg.toStdString().c_str());
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s[FATAL] %s\n", timestamp.toStdString().c_str(), msg.toStdString().c_str());
        abort();  // arrête le programme
    case QtInfoMsg:
        fprintf(stderr, "%s[INFO] %s\n", timestamp.toStdString().c_str(), msg.toStdString().c_str());
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
        MAIN::INIT(1);
        mode=1;
        SystemAppInstaller::installsystemapp(0, "filetea", "");

        // Déclaration des variables
        bool* IntentionnallyStop = new bool(false);

        // Destruction des pointeurs pour éviter des fuites de mémoire
        QObject::connect(&app, &QCoreApplication::aboutToQuit, [=]() {
            delete IntentionnallyStop;
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
            if (!*IntentionnallyStop && exitCode != 0) {
                qWarning() << "Le processus serveur a planté. Redémarrage...";
                serverProcess->start(appPath, serverArgs);
            }else if(exitCode == 0){

            }
        });

        // Surveillance de l'UI
        QObject::connect(uiProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus) {
            if (exitCode == 0) {
                *IntentionnallyStop = true;
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
            MAIN::INIT(2);
            mode=2;
            QString port = parser.value(serverOption);
            NETWORK::port=port;
            qDebug() << "Lancement d'Olop sur le port "+port;
            MAIN::SERVER(port);
        }else if(parser.isSet(uiOption)){
            MAIN::INIT(3);
            mode=3;
            QString port = parser.value(uiOption);
            NETWORK::port=port;
            MAIN::w = new MainWindow(nullptr, port);
            qDebug() << port;
            MAIN::w->show();
        }
    }
    return app.exec();
}
