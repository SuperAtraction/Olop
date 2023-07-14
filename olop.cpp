#include "olop.hpp"
#include "ui_mainwindow.h"
#include <QtHttpServer>
#include <QDesktopServices>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QHttpServer>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

QString def = "Olop n'a pas été initialisé\nSi vous êtes le développeur de cette application, veuillez utiliser \"MAIN::INIT();\" au démarrage.";

const QString MAIN::HOME = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0) + "/";
const QString MAIN::VERSION = "alpha-1.1";
const QString MAIN::O_DIR = HOME + "Olop/";
const QString MAIN::APP_DIR = O_DIR + "/App/";
QHttpServer MAIN::httpServer;
MainWindow* MAIN::w;
QString MAIN::osname = def;

bool MAIN::ecrireDansFichier(const QString& cheminFichier, const QString& contenu) {
    QFile fichier(cheminFichier);

    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream flux(&fichier);
        flux << contenu;
        fichier.close();
        return true;
    } else {
        qDebug() << "Impossible d'ouvrir le fichier en écriture :" << fichier.errorString();
        return false;
    }
}

bool MAIN::INIT() {
    if (!MAIN::mkdir(O_DIR)) {
        return false;
    }
    if (!MAIN::mkdir(APP_DIR)) {
        return false;
    }

#if defined(Q_OS_WIN)
    osname = "Windows";
#elif defined(Q_OS_LINUX)
    osname = "Linux";
#elif defined(Q_OS_MACOS)
    osname = "macOS";
#else
    QMessageBox::critical(nullptr, "Erreur", "Ce système est inconnu (n'est pas WIndows, ni Linux, ni macOS).\nOlop ne peut pas fonctionner correctement et va s'arrêter.");
    exit(-1);
#endif

    return true;
}

int MAIN::SERVER(){
    const auto port = httpServer.listen(QHostAddress::Any);
    if (!port) {
        qDebug() << QCoreApplication::translate("QHttpServerExample", "Server failed to listen on a port.");
        exit(0);
    }

    httpServer.route("/index.html", []() {
#ifdef Q_OS_LINUX
        return MAIN::lireFichier(QStringLiteral(":/assets/index.html"))+"<script>"+MAIN::lireFichier(QStringLiteral(":/assets/linux.js"))+"</script>";
#else
            return QHttpServerResponse::fromFile(QStringLiteral(":/assets/index.html"));
#endif
    });

    httpServer.route("/Applist.html", [](){
        return APP::LIST(MAIN::APP_DIR);
    });

    httpServer.route("/Install/1/<arg>", [](const QUrl &Url){
        QNetworkAccessManager* manager = new QNetworkAccessManager();
        QNetworkRequest request(Url);
        QNetworkReply* reply = manager->get(request);
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        QByteArray data = reply->readAll();
        delete reply;
        delete manager;

        if (data.isEmpty()) {
            return QString("Erreur, vérifiez l'URL et votre connexion internet");
        } else {
            QStringList app = APP::decodeApp(data);
            int rand = QRandomGenerator::global()->bounded(0, 99999 + 1);
            MAIN::ecrireDansFichier(MAIN::O_DIR+QString::number(rand)+".app", QString::fromUtf8(data));
            QString redirectUrl = "https://olop.rf.gd/Store/InstallPass/?name=" + QUrl::toPercentEncoding(app[1]) + "&version=" + QUrl::toPercentEncoding(app[2]) +"&dev=" + QUrl::toPercentEncoding(app[4]) +"&description=" + QUrl::toPercentEncoding(app[7]) +"&code=" + QUrl::toPercentEncoding(QString::number(rand));

            QString encodedRedirectUrl = QUrl(redirectUrl.toUtf8()).toEncoded();

            return QString("<script type=\"text/javascript\">location.href=\"" + encodedRedirectUrl +"&port=\" + location.port;</script><noscript>Activez javascript</noscript>");
        }
    });

    httpServer.route("/Install/2/<arg>", [w](const QUrl &Url){
        qDebug() << Url;
        QStringList app = APP::decodeApp(MAIN::lireFichier(MAIN::O_DIR+Url.toDisplayString()+".app"));
        if(!MAIN::moveFile(MAIN::O_DIR+Url.toDisplayString()+".app", MAIN::O_DIR+"App/"+Url.toDisplayString()+".app")){
            QMessageBox::critical(w, "Erreur", "Erreur lors de l'installation de l'application "+app[1]);
            return "Erreur lors de l'installation de l'application "+app[1];
        }
        w->ui->Web->page()->runJavaScript("showNotification(\"L\'application " + app[1] + " est prête à être installée.<br>Elle sera installée lorsque vous la démarrerez.\");");
        w->activateWindow();
        w->raise();
        w->showNormal();

        return "<script>location.href=\"https://olop.rf.gd/Store/?Installed="+app[1]+"\";</script><noscript><a href=\"https://olop.rf.gd/Store/?Installed="+app[1]+"\">Cliquez ici</a> et activez javascript</noscript>";
    });

    httpServer.route("/stop/", [](){
        exit(0);
        return "";
    });

    httpServer.route("/getapp/<arg>", [](const QUrl &Url) {
        return lireFichier(Url.toDisplayString());
    });

    httpServer.route("/checkurl/200/<arg>", [](const QUrl &Url){
        if(NETWORK::checkURLAccess(Url.toDisplayString())){
            return "1";
        }
        return "0";
    });

    /*httpServer.route("/actions/goupdate/", [](){
        QString downloadUrl = "https://github.com/SuperAtraction/Olop/raw/main/InstallOlop.sh";

        QNetworkAccessManager networkManager;
        QNetworkReply* reply = networkManager.get(QNetworkRequest(QUrl(downloadUrl)));
        QEventLoop eventLoop;
        QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Error downloading file:" << reply->errorString();
            reply->deleteLater();
            QMessageBox::critical(nullptr, "Erreur", "Une erreur est survenue durant la mise à jour");
        }
        return "";
    });*/

    httpServer.route("/query", [] (const QHttpServerRequest &request) {
        return QString("%1/query/").arg(request.value("Host"));
    });

    httpServer.route("/<arg>", [] (const QUrl &url) {
        return QHttpServerResponse::fromFile(QStringLiteral(":/assets/%1").arg(url.path()));
    });

    const auto sslCertificateChain = QSslCertificate::fromPath(QStringLiteral(":/assets/certificate.crt"));
    if (sslCertificateChain.isEmpty()) {
        qDebug() << QCoreApplication::translate("QHttpServerExample", "Couldn't retrieve SSL certificate from file.");
        return 0;
    }
    QFile privateKeyFile(QStringLiteral(":/assets/private.key"));
    if (!privateKeyFile.open(QIODevice::ReadOnly)) {
        qDebug() << QCoreApplication::translate("QHttpServerExample", "Couldn't open file for reading.");
        return 0;
    }
    httpServer.sslSetup(sslCertificateChain.first(), QSslKey(&privateKeyFile, QSsl::Rsa));
    privateKeyFile.close();

    const auto sslPort = httpServer.listen(QHostAddress::Any);
    if (!sslPort) {
        return 0;
    }
    qDebug() << port;
    return port;
}

bool MAIN::mkdir(QString path) {
    QDir dir = path;
    if (!dir.exists()) {
        bool ook = dir.mkpath(dir.path());
        if (!ook) {
            return false;
        }
    }
    return true;
}

QString MAIN::lireFichier(const QString& cheminFichier) {
    QFile fichier(cheminFichier);

    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString(); // Retourne une chaîne vide en cas d'erreur
    }

    QTextStream flux(&fichier);
    QString contenu = flux.readAll();

    fichier.close();

    return contenu;
}

bool MAIN::deleteFile(const QString& filePath) {
    QFile file(filePath);

    if (!file.exists()) {
        qWarning() << "Le fichier n'existe pas :" << filePath;
        return false;
    }

    if (!file.remove()) {
        qWarning() << "Erreur lors de la suppression du fichier :" << file.errorString();
        return false;
    }

    return true;
}

bool MAIN::copyFile(const QString& sourceFilePath, const QString& destinationFilePath) {
    QString content = lireFichier(sourceFilePath);
    if (content.isEmpty()) {
        qWarning() << "Erreur lors de la copie : Le fichier ne peut pas être ouvert";
    }

    return ecrireDansFichier(destinationFilePath, content);
}

bool MAIN::moveFile(const QString& sourceFilePath, const QString& destinationFilePath) {
    if (!copyFile(sourceFilePath, destinationFilePath)) {
        return false;
    }
    if (!deleteFile(sourceFilePath)) {
        return false;
    }

    return true;
}

QStringList MAIN::getListOfFilesInDirectory(const QString& directoryPath)
{
    QDir directory(directoryPath);

    // Vérifier si le répertoire existe
    if (!directory.exists()) {
        qDebug() << "Le répertoire n'existe pas :" << directoryPath;
            return QStringList();
    }

    // Obtenir la liste des fichiers dans le répertoire
    QFileInfoList fileInfoList = directory.entryInfoList(QDir::Files);

    QStringList fileList;
    foreach (const QFileInfo& fileInfo, fileInfoList) {
        fileList.append(fileInfo.absoluteFilePath());
    }

    return fileList;
}

QString APP::LIST(const QString& directoryPath)
{
    QStringList fileList = MAIN::getListOfFilesInDirectory(directoryPath);

    // Formater la liste des fichiers
    QString formattedList;
    for (const QString& filePath : fileList) {
        QFile f(filePath);
        formattedList += "<a href='#' onclick='loadPAP(\"" + f.fileName() + "\")'>" + decodeApp(MAIN::lireFichier(filePath))[1] + "</a><br>";
    }

    return formattedList;
}

QStringList APP::decodeApp(const QString data) {
    QStringList lignesSeparate = data.split('\n', Qt::SkipEmptyParts);
    QStringList result;

    QString nom;
    QString version;
    QString url;
    QString developpeur;
    QString type;
    QString os;
    QString description;

    if (lignesSeparate.size() >= 6) {
        nom = lignesSeparate[0];
        version = lignesSeparate[1];
        url = lignesSeparate[2];
        developpeur = lignesSeparate[3];
        type = lignesSeparate[4];
        os = lignesSeparate[5];

        // Si la description contient plusieurs lignes, les concaténer
        if (lignesSeparate.size() > 6) {
            description = lignesSeparate.mid(6).join("\n");
        }
    }

    result << "Ceci est le .app de " + nom << nom << version << url << developpeur << type << os << description;
    return result;
}

QStringList APP::decodeApp(const QByteArray data) {
    return APP::decodeApp(QString::fromUtf8(data));
}

bool NETWORK::checkURLAccess(const QString& url) {
    QNetworkAccessManager manager;
    QNetworkRequest request(url);

    QNetworkReply* reply = manager.get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        return true; // L'URL est accessible
    }

    return false; // L'URL n'est pas accessible ou une erreur s'est produite
}
