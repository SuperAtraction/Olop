#ifndef OLOP_HPP
#define OLOP_HPP

#include "mainwindow.h"
#include "qhttpserver.h"
#include <QWebEnginePage>
#include <QtCore>

// Fichier des actions principales d'Olop

class MAIN {
public:
    ~MAIN();
    static const QString HOME;
    static QString VERSION;
    static const QString O_DIR;
    static const QString APP_DIR;
    static QHttpServer httpServer;
    static MainWindow* w;
    static QString osname;
    static int mode;

    static bool ecrireDansFichier(const QString& cheminFichier, const QByteArray& contenu);
    static bool ecrireDansFichier(const QString& cheminFichier, const QString& contenu);
    static bool INIT(int mode=0);
    static int SERVER(QString ports);
    static void executeInMainThread(std::function<void()> func);
    static bool isVersionGreater(const QString& newVersion, const QString& oldVersion);
    static bool copyFile(const QString& sourceFilePath, const QString& destinationFilePath);
    static bool moveFile(const QString& sourceFilePath, const QString& destinationFilePath);
    static bool deleteFile(const QString& filePath);
    static bool supprimerDossier(const QString& cheminDossier);
    static bool mkdir(QString path);
    static QStringList getListOfFilesInDirectory(const QString& directoryPath, bool includesubdir=false, bool includeDirs = false);
    static QByteArray lireFichier(const QString& cheminFichier);
    static QString detectLanguageJS(QWebEnginePage* page);
    static bool Exit(int code);
    static bool ReportCrash(int exitcode, QString log);
};

class APP {
public:
    ~APP();
    static QStringList decodeApp(const QString data);
    static QStringList decodeApp(const QByteArray data);
    static QString LIST(const QString& directoryPath);
    static int HTTPSERVER(QString dir);
    static QList<QHttpServer*> httpServers;
};

class NETWORK {
public:
    static bool checkURLAccess(const QString& url);
    static int findAvailablePort();
    static QByteArray Download(const QUrl Url);
    static QString port;
#ifdef Q_OS_LINUX
    static int goalpha();
#endif
};

class FILES {
public:
    static bool unZip(const QString &file, const QString &dest, std::function<void()> cleaningCallback = nullptr);
    static int gocurrentdirandcheckpermissions();
    static QString getcurrentdir();
    static int checkpermission(QString path);
};

#endif // OLOP_HPP
