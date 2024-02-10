#include "logger.hpp"

QString logFileName;
int Logger::modev = 0;

void Logger::setupLogging(const QString &fileName, int mode)
{
    modev = mode;
    if (modev == 1) {
        logFileName = fileName;
        QFile file(logFileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Impossible d'ouvrir le fichier de log pour l'écriture.";
            return;
        }
        QTextStream logStream(&file);
        logStream << "------- Nouveau Log Olop ------\n";
        logStream << "Logs du " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm")+"\n";
        file.close();
    }
}

void Logger::logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (modev == 1) {
        QFile logFile(logFileName);
        if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Impossible d'ouvrir le fichier de log pour l'écriture.";
            return;
        }

        QTextStream logStream(&logFile);

        QString timestamp = "[" + QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss.zzz") + "]";

        switch (type) {
        case QtDebugMsg:
            qDebug().noquote() << timestamp << "[INFO]" << msg;
            logStream << timestamp << "[INFO]" << msg << "\n";
            break;
        case QtWarningMsg:
            qWarning().noquote() << timestamp << "[ERREUR]" << msg;
            logStream << timestamp << "[ERREUR]" << msg << "\n";
            break;
        case QtCriticalMsg:
            qCritical().noquote() << timestamp << "[CRITIQUE]" << msg;
            logStream << timestamp << "[CRITIQUE]" << msg << "\n";
            break;
        case QtFatalMsg:
            qFatal().noquote() << timestamp << "[FATAL]" << msg;
            logStream << timestamp << "[FATAL]" << msg << "\n";
            abort();
        case QtInfoMsg:
            qDebug().noquote() << timestamp << "[INFO]" << msg;
            logStream << timestamp << "[INFO]" << msg << "\n";
            break;
        }

        logFile.close();
    } else {
        // Si le mode n'est pas 1, seules les sorties de journalisation Qt sont utilisées
        switch (type) {
        case QtDebugMsg:
            qDebug().noquote() << msg;
            break;
        case QtWarningMsg:
            qWarning().noquote() << msg;
            break;
        case QtCriticalMsg:
            qCritical().noquote() << msg;
            break;
        case QtFatalMsg:
            qFatal().noquote() << msg;
            abort();
        case QtInfoMsg:
            qDebug().noquote() << msg;
            break;
        }
    }
}
