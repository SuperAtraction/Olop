#include <QtCore>
#include <QtHttpServer>
#include <QDesktopServices>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QHttpServer>
#include "mainwindow.h"
#include "olop.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MAIN::INIT();

    {QFile file("OlopUpdater.exe");
        if(file.exists()){
            file.remove();
        }}
    if(QDir("Applications/").exists()){
        QDir().mkdir("Applications/");
    }

    QHttpServer httpServer;

    const auto port = httpServer.listen(QHostAddress::Any);
    if (!port) {
        qDebug() << QCoreApplication::translate("QHttpServerExample", "Server failed to listen on a port.");
        exit(0);
    }
    MainWindow *w = new MainWindow(nullptr, "http://localhost:" + QString::number(port) + "/index.html");

    httpServer.route("/index.html", []() {
#ifdef Q_OS_LINUX
        return MAIN::lireFichier(QStringLiteral(":/assets/index.html"))+"<script>"+MAIN::lireFichier(QStringLiteral(":/assets/linux.js"))+"</script>";
#else
            return QHttpServerResponse::fromFile(QStringLiteral(":/assets/index.html"));
#endif
    });

    httpServer.route("/Applist.html", [](){
        return QHttpServerResponse::fromFile(QStringLiteral("Applications-list.txt"));
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

    httpServer.route("/Install/2/<arg>", [&w](const QUrl &Url){
        qDebug() << Url;
        QStringList app = APP::decodeApp(MAIN::lireFichier(MAIN::O_DIR+Url.toDisplayString()+".app"));
        w->setFocus();
        QMessageBox::information(w, "Applications prête à installer", "L'application "+app[1]+" est prête à être installé.\nElle sera installée lorsque vous la démarrerez.");
        return "<script>location.href=\"https://olop.rf.gd/Store/?Installed="+app[1]+"\";</script><noscript><a href=\"https://olop.rf.gd/Store/?Installed="+app[1]+"\">Cliquez ici</a> et activez javascript</noscript>";
    });

    httpServer.route("/stop/", [](){
        exit(0);
        return "";
    });

    httpServer.route("/olop.ico", []() {
        return QHttpServerResponse::fromFile(QStringLiteral(":/assets/olop.ico"));
    });

    httpServer.route("/actions/goupdate/", [](){
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
    });

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

    w->show();

    return app.exec();
}
