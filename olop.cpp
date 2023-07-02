#include "olop.hpp"

QString def = "Olop n'a pas été initialisé\nSi vous êtes le développeur de cette application, veuillez à utiliser \"MAIN::INIT();\" au démmarage.";

const QString MAIN::HOME = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0) + "/";
const QString MAIN::VERSION = "alpha-1.1";
const QString MAIN::O_DIR = HOME+"Olop/";
QString MAIN::osname = def;

int MAIN::ecrireDansFichier(const QString& cheminFichier, const QString& contenu) {
    QFile fichier(cheminFichier);

    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream flux(&fichier);
        flux << contenu;
        fichier.close();
        return 0;
    } else {
        // Gérer l'erreur si le fichier ne peut pas être ouvert en écriture
        qDebug() << "Impossible d'ouvrir le fichier en écriture :" << fichier.errorString();
        return 1;
    }
}

int MAIN::INIT(){
    QDir o_dir(MAIN::O_DIR);
    if(!o_dir.exists()){
        bool ook = o_dir.mkpath(o_dir.path());
        if(!ook){
            return 1;
        }
    }

#if defined(Q_OS_WIN)
    osName = "Windows";
#elif defined(Q_OS_LINUX)
    osname = "Linux";
#elif defined(Q_OS_MACOS)
    osName = "macOS";
#else
    QMessageBox::critical(nullptr, "Erreur", "Ce système est inconnu (n'est pas WIndows, ni linux, ni macos).\nOlop ne peut pas fonctionner correctement et va s'arrêter.");
#endif

    return 0;
}

QString MAIN::lireFichier(const QString& cheminFichier) {
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

    result << "Ceci est le .app de "+nom << nom << version << url << developpeur << type << os << description;
    return result;
}

QStringList APP::decodeApp(const QByteArray data){
    return APP::decodeApp(QString::fromUtf8(data));
}
