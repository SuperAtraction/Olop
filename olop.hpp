#ifndef OLOP_HPP
#define OLOP_HPP

#include "mainwindow.h"
#include "qhttpserver.h"
#include <QWebEnginePage>
#include <QtCore>

// Fichier des actions principales d'Olop

class MAIN {
public:
    static const QString HOME;
    static const QString VERSION;
    static const QString O_DIR;
    static const QString APP_DIR;
    static QHttpServer httpServer;
    static MainWindow* w;
    static QString osname;

    static bool ecrireDansFichier(const QString& cheminFichier, const QString& contenu);
    static bool INIT();
    static int SERVER();
    static bool copyFile(const QString& sourceFilePath, const QString& destinationFilePath);
    static bool moveFile(const QString& sourceFilePath, const QString& destinationFilePath);
    static bool deleteFile(const QString& filePath);
    static bool mkdir(QString path);
    static QStringList getListOfFilesInDirectory(const QString& directoryPath);
    static QString lireFichier(const QString& cheminFichier);
    static QString detectLanguageJS(QWebEnginePage* page);
};

class APP {
public:
    static QStringList decodeApp(const QString data);
    static QStringList decodeApp(const QByteArray data);
    static QString LIST(const QString& directoryPath);
};

class NETWORK {
public:
    static bool checkURLAccess(const QString& url);
};

#endif // OLOP_HPP
