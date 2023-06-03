// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtCore>
#include <QtHttpServer>
#include <QDesktopServices>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include "Download.h"
#include "mainwindow.h"

static inline QString host(const QHttpServerRequest &request)
{
    return QString::fromLatin1(request.value("Host"));
}

QString lireFichier(const QString& cheminFichier) {
    QFile fichier(cheminFichier);

    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Gérer l'erreur si le fichier ne peut pas être ouvert
        return QString();
    }

    QTextStream flux(&fichier);
    QString contenu = flux.readAll();

    fichier.close();

    return contenu;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv); // just this line

#if defined(Q_OS_WIN)
    QString osName = "Windows";
#elif defined(Q_OS_LINUX)
    QString osName = "Linux";
#elif defined(Q_OS_MACOS)
    QString osName = "macOS";
#else
    QMessageBox::critical(nullptr, "Erreur", "Ce système est inconnu (n'est pas WIndows, ni linux, ni macos).\nOlop ne peut pas fonctionner correctement");
#endif

    {QFile file("OlopUpdater.exe");
    if(file.exists()){
        file.remove();
    }}
    if(QDir("Applications/").exists()){
        QDir().mkdir("Applications/");
    }

    QHttpServer httpServer;
    httpServer.route("/index.html", []() {
#ifdef Q_OS_LINUX
        return lireFichier(QStringLiteral(":/assets/index.html"))+"<script>"+lireFichier(QStringLiteral(":/assets/linux.js"))+"</script>";
#elif
return QHttpServerResponse::fromFile(QStringLiteral(":/assets/index.html"));
#endif

    });

    httpServer.route("/Applist.html", [](){
        return QHttpServerResponse::fromFile(QStringLiteral("Applications-list.txt"));
    });

    httpServer.route("/stop/", [](){
        QCoreApplication* app = QCoreApplication::instance();
        QTimer::singleShot(3000, app, []() {
               exit(0);
           });
        return "<script>location.href=\"http://olop.rf.gd/Olop/exited.html\"</script>";
    });

    httpServer.route("/olop.ico", []() {
        QString fileName = QStringLiteral("root/olop.ico");
        QFile fichier(fileName);
        fichier.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichier);
        return flux.readAll();
    });

    httpServer.route("/actions/goupdate/", [](){
        download::launch(QUrl("https://filedn.eu/lMYof3q05iKQYQha1nJkqPJ/OlopUpdater.exe"), true, true, "OlopUpdater.exe");
        return "";
    });

    httpServer.route("/query", [] (const QHttpServerRequest &request) {
        return QString("%1/query/").arg(host(request));
    });

    httpServer.route("/json/", [] {
        return QJsonObject{
            {
                {"key1", "1"},
                {"key2", "2"},
                {"key3", "3"}
            }
        };
    });

    httpServer.route("/<arg>", [] (const QUrl &url) {
        return QHttpServerResponse::fromFile(QStringLiteral(":/assets/%1").arg(url.path()));
    });

    // Basic authentication example (RFC 7617)
    httpServer.route("/auth", [](const QHttpServerRequest &request) {
        auto auth = request.value("authorization").simplified();

        if (auth.size() > 6 && auth.first(6).toLower() == "basic ") {
            auto token = auth.sliced(6);
            auto userPass = QByteArray::fromBase64(token);

            if (auto colon = userPass.indexOf(':'); colon > 0) {
                auto userId = userPass.first(colon);
                auto password = userPass.sliced(colon + 1);

                if (userId == "Aladdin" && password == "open sesame")
                    return QHttpServerResponse("text/plain", "Success\n");
            }
        }
        QHttpServerResponse response("text/plain", "Authentication required\n",
                                     QHttpServerResponse::StatusCode::Unauthorized);
        response.setHeader("WWW-Authenticate", R"(Basic realm="Simple example", charset="UTF-8")");
        return response;
    });

    const auto port = httpServer.listen(QHostAddress::Any);
    if (!port) {
        qDebug() << QCoreApplication::translate("QHttpServerExample",
                                                "Server failed to listen on a port.");
        exit(0);
    }

    //! [HTTPS Configuration example]
    const auto sslCertificateChain =
            QSslCertificate::fromPath(QStringLiteral(":/assets/certificate.crt"));
    if (sslCertificateChain.empty()) {
        qDebug() << QCoreApplication::translate("QHttpServerExample",
                                                "Couldn't retrive SSL certificate from file.");
        return 0;
    }
    QFile privateKeyFile(QStringLiteral(":/assets/private.key"));
    if (!privateKeyFile.open(QIODevice::ReadOnly)) {
        qDebug() << QCoreApplication::translate("QHttpServerExample",
                                                "Couldn't open file for reading.");
        return 0;
    }
    httpServer.sslSetup(sslCertificateChain.front(), QSslKey(&privateKeyFile, QSsl::Rsa));
    privateKeyFile.close();

    const auto sslPort = httpServer.listen(QHostAddress::Any);
    if (!sslPort) {
        return 0;
    }
    qDebug() << port;

    MainWindow *w = new MainWindow(nullptr, "http://localhost:"+QString::number(port)+"/index.html");
    w->show();
    return app.exec();
}
