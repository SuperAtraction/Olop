#ifndef OLOP_HPP
#define OLOP_HPP

#include <QtCore>

// Fichier des actions principales d'Olop

class MAIN {
public:
    static const QString HOME;
    static const QString VERSION;
    static const QString O_DIR;
    static QString osname;

    static int ecrireDansFichier(const QString& cheminFichier, const QString& contenu);
    static int INIT();
    static QString lireFichier(const QString& cheminFichier);
};

class APP {
public:
    static QStringList decodeApp(const QString data);
    static QStringList decodeApp(const QByteArray data);
};

#endif // OLOP_HPP
