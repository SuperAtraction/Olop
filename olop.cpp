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
#include <QWebEngineScript>

QString def = "Olop n'a pas été initialisé\nSi vous êtes le développeur de cette application, veuillez utiliser \"MAIN::INIT();\" au démarrage.";

const QString MAIN::HOME = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0) + "/";
const QString MAIN::VERSION = "alpha-1.1";
const QString MAIN::O_DIR = HOME + "Olop/";
const QString MAIN::APP_DIR = O_DIR + "/App/";
QHttpServer MAIN::httpServer;
MainWindow* MAIN::w;
QString MAIN::osname = def;
QList<QHttpServer*> APP::httpServers;

bool MAIN::ecrireDansFichier(const QString& cheminFichier, const QByteArray& contenu) {
    QFile fichier(cheminFichier);

    if (fichier.open(QIODevice::WriteOnly)) {
        fichier.write(contenu);
        fichier.close();
        return true;
    } else {
        qDebug() << "Impossible d'ouvrir le fichier en écriture :" << fichier.errorString();
        return false;
    }
}

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
        qDebug() << QCoreApplication::translate("Olop", "Server failed to listen on a port.");
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
            MAIN::ecrireDansFichier(MAIN::O_DIR+QString::number(rand)+".app", data);
            QString redirectUrl = "https://olop.rf.gd/Store/InstallPass/?name=" + QUrl::toPercentEncoding(app[1]) + "&version=" + QUrl::toPercentEncoding(app[2]) +"&dev=" + QUrl::toPercentEncoding(app[4]) +"&description=" + QUrl::toPercentEncoding(app[7]) +"&code=" + QUrl::toPercentEncoding(QString::number(rand));

            QString encodedRedirectUrl = QUrl(redirectUrl.toUtf8()).toEncoded();

            return QString("<script type=\"text/javascript\">location.href=\"" + encodedRedirectUrl +"&port=\" + location.port;</script><noscript>Activez javascript</noscript>");
        }
    });

    httpServer.route("/Install/2/<arg>", [](const QUrl &Url){
        qDebug() << Url;
        QStringList app = APP::decodeApp(MAIN::lireFichier(MAIN::O_DIR+Url.toDisplayString()+".app"));
        if(!MAIN::moveFile(MAIN::O_DIR+Url.toDisplayString()+".app", MAIN::O_DIR+"App/"+Url.toDisplayString()+".app")){
            QMessageBox::critical(MAIN::w, "Erreur", "Erreur lors de l'installation de l'application "+app[1]);
            return "Erreur lors de l'installation de l'application "+app[1];
        }
        MAIN::w->ui->Web->page()->runJavaScript("showNotification(0, \"Installation d'une application\", \"L\'application " + app[1] + " est prête à être installée.<br>Elle sera installée lorsque vous la démarrerez.\");");
        MAIN::w->activateWindow();
        MAIN::w->raise();
        MAIN::w->showNormal();

        return "<script>location.href=\"https://olop.rf.gd/Store/?Installed="+QUrl::toPercentEncoding(app[1])+"\";</script><noscript><a href=\"https://olop.rf.gd/Store/?Installed="+app[1]+"\">Cliquez ici</a> et activez javascript</noscript>";
    });

httpServer.route("/Launch/1/<arg>", [=](const QUrl &Url) {
    QWebEnginePage *page = w->ui->Web->page();
    auto args = Url.toDisplayString().split("-OLOP-");
    auto appfile = args[0];
    auto id = args[1];
    auto decodedApp = APP::decodeApp(lireFichier(appfile));
    auto appdir = appfile.split(".app")[0] + "/";
    auto packageurl = decodedApp[3] + "/" + QUrl::toPercentEncoding(decodedApp[1]) + "-package.zip";
    auto appurl = decodedApp[3] + "/" + QUrl::toPercentEncoding(decodedApp[1]) + ".txt";
    qDebug() << decodedApp;

    // Configurez le timer
    auto setupTimerAndCounter = [&](QSharedPointer<QString> message, QPointer<QTimer> timer) {
        timer->setInterval(1000);

        int counter = 0;
        QObject::connect(timer, &QTimer::timeout, [=]() mutable {
            counter = counter % 4;
            QString points = QString(".").repeated(counter++);
            QString jsCode = QString("$(\"#%1\").html(\"" + *message + "%2\");")
                                 .arg(id).arg(points);
            page->runJavaScript(jsCode);
        });
        timer->start();
    };

    QSharedPointer<QString> currentMessage(new QString("Lancement en cours de "+decodedApp[1]));
    QPointer<QTimer> primaryTimer = new QTimer();
    setupTimerAndCounter(currentMessage, primaryTimer);
    APP *app = new APP;

    auto launch = [=](){
        if(!primaryTimer) {
            qDebug() << "primaryTimer is null!";
        } else {
            primaryTimer->stop();
        }
        primaryTimer->deleteLater();  // puis, demandez la suppression
        int port = app->HTTPSERVER(appdir);
        qDebug() << port;
        page->runJavaScript("findWindowAndConvertToIframe(\"" + id + "\", \"http://localhost:" + QString::number(port) + "/index.html\");");
    };

    QTimer::singleShot(10, [=]() {
        if (!QDir(appdir).exists()) {
            MAIN::ecrireDansFichier(QDir::tempPath() + "/tmp.zip", NETWORK::Download(QUrl(packageurl)));
            FILES::unZip(QDir::tempPath() + "/tmp.zip", appdir);

            primaryTimer->stop();
            int port = APP::HTTPSERVER(appdir);
            page->runJavaScript("findWindowAndConvertToIframe(\"" + id + "\", \"http://localhost:" + QString::number(port) + "/index.html\");");
        } else {
            QString newappfile = NETWORK::Download(QUrl(appurl));
            qDebug() << appurl;
            if (!newappfile.isEmpty() && newappfile != "ERR") {
                auto newappdecoded = APP::decodeApp(newappfile);
                if (newappdecoded[2] != decodedApp[2]) {
                    qDebug() << "Mise à jour disponible.";
                    int response;
                    response = QMessageBox::question(w, "Mise à jour", "L'application " + decodedApp[1] + " n'est pas à sa dernière version.\nSouhaitez-vous la mettre à jour ?", QMessageBox::Yes | QMessageBox::No);

                    if (response == QMessageBox::Yes) {
                        std::thread t([=](){
                            qDebug() << "Mise à jour en cours...";
                            *currentMessage="Téléchargement de la mise à jour de "+decodedApp[1]+" en cours";
                            MAIN::ecrireDansFichier(QDir::tempPath() + "/tmp.zip", NETWORK::Download(QUrl(packageurl)));
                            *currentMessage="Installation de la mise à jour de "+decodedApp[1]+" en cours";
                            FILES::unZip(QDir::tempPath() + "/tmp.zip", appdir);
                            qDebug() << "Dézippé";
                            *currentMessage="Finitialisation de la mise à jour de "+decodedApp[1];
                            MAIN::ecrireDansFichier(appfile, newappfile);
                            *currentMessage="Mise à jour terminée. Lancement de "+decodedApp[1]+" en cours";
                            executeInMainThread(launch);
                        });
                        t.detach();
                    }else {
                        launch();
                    }
                }else {
                    launch();
                }
            }else {
                qDebug() << "Erreur d'update (vérifiez votre connexion internet)";
                launch();
            }
        }
    });

    return QString("Chargement...");
});

    httpServer.route("/remove/<arg>", [=](const QUrl Url){
    QWebEnginePage *page = w->ui->Web->page();
    auto args = Url.toDisplayString().split("-OLOP-");
    auto app = args[0];
    auto id = args[1];
    auto decodedApp(APP::decodeApp(lireFichier(app)));
    qDebug() << app;
    auto réponse = QMessageBox::question(w, "Confirmation", "Êtes-vous sûr de vouloir désinstaller "+decodedApp[1]+" ?", QMessageBox::Yes | QMessageBox::No);
    if(réponse == QMessageBox::Yes){
        page->runJavaScript("$(\"#"+id+"\").html(\"Suppression...\");");
        deleteFile(app);
        supprimerDossier(app.split(".app")[0]+"/");
        page->runJavaScript("loadPage(\"home\");");
        return "Suppression terminée.";
    }else {
        page->runJavaScript("tmpHTML(\""+id+"\", 4000, \"La suppression de l'application "+decodedApp[1]+" a été annulée.\");");
        return "OK";
    }
});

    httpServer.route("/stop/", [](){
        exit(0);
        return "";
    });

    httpServer.route("/getapp/<arg>", [](const QUrl &Url) {
        return lireFichier(Url.toDisplayString());
    });

    httpServer.route("/checkurl/200/<arg>", [](const QUrl &Url){
        /*if(NETWORK::checkURLAccess(Url.toDisplayString())){
            return "1";
        }*/
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
        qDebug() << QCoreApplication::translate("Olop", "Couldn't retrieve SSL certificate from file.");
        return 0;
    }
    QFile privateKeyFile(QStringLiteral(":/assets/private.key"));
    if (!privateKeyFile.open(QIODevice::ReadOnly)) {
        qDebug() << QCoreApplication::translate("Olop", "Couldn't open file for reading.");
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

void MAIN::executeInMainThread(std::function<void()> func) {
    QMetaObject::invokeMethod(w, [=]() { func(); }, Qt::QueuedConnection);
}

bool MAIN::isVersionGreater(const QString& newVersion, const QString& oldVersion) {
    QStringList newComponents = newVersion.split(".");
    QStringList oldComponents = oldVersion.split(".");

    int n = qMin(newComponents.size(), oldComponents.size());

    for(int i = 0; i < n; ++i) {
        int newComponent = newComponents[i].toInt();
        int oldComponent = oldComponents[i].toInt();

        if(newComponent > oldComponent) {
            return true;
        } else if(newComponent < oldComponent) {
            return false;
        }
    }
    return newComponents.size() > oldComponents.size();
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

bool MAIN::supprimerDossier(const QString& cheminDossier) {
    QDir dossier(cheminDossier);

    if (!dossier.exists()) {
        qDebug() << "Le dossier n'existe pas!";
        return false;
    }

    // Supprimer tous les fichiers dans le dossier
    Q_FOREACH(QFileInfo info, dossier.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::Files, QDir::DirsFirst)) {
        if (info.isFile()) {
            QFile fichier(info.filePath());

            if (!fichier.remove()) {
                qDebug() << "Impossible de supprimer le fichier" << info.fileName();
                return false;
            }
        }
        else {
            supprimerDossier(info.absoluteFilePath());
        }
    }

    // Supprimer tous les dossiers vides
    Q_FOREACH(QFileInfo info, dossier.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::Dirs, QDir::DirsFirst)) {
        if (!QDir(info.absoluteFilePath()).removeRecursively()) {
            qDebug() << "Impossible de supprimer le dossier" << info.absoluteFilePath();
            return false;
        }
    }

    return dossier.rmdir(cheminDossier);
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

QString MAIN::detectLanguageJS(QWebEnginePage* page) {
    // Code JavaScript pour détecter la langue du système
    QString jsCode = R"(
        function detectLanguage() {
            var userLang = navigator.language || navigator.userLanguage;
            return userLang;
        }
        detectLanguage();
    )";

    // Évaluer le script JavaScript de manière asynchrone sur l'objet QWebEnginePage (page).
    page->runJavaScript(jsCode, [page](const QVariant &result){
        if (result.isValid()) {
            QString langue = result.toString();
            // Faites quelque chose avec la langue détectée ici
            // Obtenir la langue détectée en utilisant JavaScript

            // Construire le chemin du script en fonction de la langue détectée
            QString scriptPath = "lang_" + langue + ".js"; // Par exemple, si detectedLang est "fr", scriptPath sera "lang_fr.js".

            // Charger le script correspondant
            if (!scriptPath.isEmpty()) {
                QString scriptContent = MAIN::lireFichier(scriptPath); // Utilisez votre fonction "lirefichier" pour obtenir le contenu du script.
                if (!scriptContent.isEmpty()) {
                    page->runJavaScript(scriptContent); // Exécutez le contenu du script.
                } else {
                    qWarning() << "Impossible de lire le fichier de script : " << scriptPath;
                }
            }
        } else {
            // Gérer l'erreur ici si nécessaire
            qDebug() << "Erreur lors de la détection de la langue";
        }
    });

    // Retourner une chaîne vide car le résultat ne sera pas immédiatement disponible
    return QString();
}

APP::~APP(){
    for (auto server : httpServers) {
        delete server;
    }

    httpServers.clear();
}

QString APP::LIST(const QString& directoryPath)
{
    QStringList fileList = MAIN::getListOfFilesInDirectory(directoryPath);

    // Formater la liste des fichiers
    QString formattedList;
    QString builder;
    for (const auto& filePath : fileList) {
        QFile f(filePath);
        builder += QStringLiteral("<a href='#' onclick=\"loadPAP('") % f.fileName() % "')\">" % decodeApp(MAIN::lireFichier(filePath))[1] % "</a><br>";
    }

    formattedList = builder;

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

int APP::HTTPSERVER(QString dir){
    QHttpServer* httpServer = new QHttpServer();
    const auto port = httpServer->listen(QHostAddress::Any);
    if (!port) {
        qDebug() << QCoreApplication::translate("Olop", "Server failed to listen on a port.");
        return 0;
    }
    QString URL = NULL;

    httpServer->route("/jquery.js", [=](QUrl Url){
        return QHttpServerResponse::fromFile(QStringLiteral(":/assets/jquery.js"));
    });

    httpServer->route("/setURL/<arg>", [=](QUrl Url){
        return "OK";
    });

    httpServer->route("/<arg>", [=](QUrl Url) {
        QString file = MAIN::lireFichier(dir+"/"+Url.toDisplayString());
        if(file==""){
            QByteArray DOWN = NETWORK::Download(Url);
            if(DOWN =="ERR"){
                DOWN = NETWORK::Download(QUrl(QString(URL+Url.toDisplayString())));
            }
        }else {
            return file;
        }
    });

httpServer->route("/stop", [=]() {
    QTimer::singleShot(300, httpServer, [httpServer]() {
        httpServer->deleteLater();
    });
    return "Application arrêtée";
});


    // Ajouter le nouveau serveur HTTP à la liste
    httpServers.append(httpServer);

    return port;
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

QByteArray NETWORK::Download(const QUrl Url) {
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(Url));
    QByteArray downloadedData;

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, [&]() {
        if (reply->error() == QNetworkReply::NoError) {
            downloadedData = reply->readAll();
            qDebug() << "Taille des données téléchargées: " << downloadedData.size();
                            qWarning() << "Réponse du serveur : " << reply->readAll();
        } else {
            qWarning() << "Erreur lors du téléchargement : " << reply->errorString();
            qWarning() << "Réponse du serveur : " << reply->readAll();
            downloadedData =  QByteArray("ERR");
        }
        loop.quit();
    });
    loop.exec();

    return downloadedData;
}

bool FILES::unZip(const QString &file, const QString &dest) {
#ifdef Q_OS_WIN
    QString program = "7z.exe";
    QStringList arguments;
    arguments << "x" << file << "-o" + dest << "-aoa";
#elif defined(Q_OS_LINUX)
    QString program = "/usr/bin/unzip";
    QStringList arguments;
    arguments << "-o" << file << "-d" << dest;
#else
    // Gérer les autres systèmes d'exploitation
    return false;
#endif

    QProcess unzip;
    unzip.start(program, arguments);
    unzip.waitForFinished();

    // Affichez la sortie pour le dépannage
    qDebug() << "Standard Output:" << unzip.readAllStandardOutput();
    qDebug() << "Standard Error:" << unzip.readAllStandardError();

    return (unzip.exitCode() == 0);
}
