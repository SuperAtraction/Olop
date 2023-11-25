#include "SpecialActions.hpp"
#include "SystemAppInstaller.hpp"
#include "../olop.hpp"

SystemAppInstaller::SystemAppInstaller()
{

}

int SystemAppInstaller::installsystemapp(int type, QString packagename, QString Dest) {
#ifdef Q_OS_LINUX
    // Détecter le gestionnaire de paquets en vérifiant la présence de certains gestionnaires courants
    QStringList gestionnaires = {"apt", "dnf", "zypper", "pacman"}; // Ajouté pour alpine apk

    QString gestionnaireDetecte;
    for (const QString &gestionnaire : gestionnaires) {
        QProcess process;
        process.start("which", QStringList() << gestionnaire);
        process.waitForFinished();

        if (process.exitCode() == 0) {
            gestionnaireDetecte = gestionnaire;
            break;
        }
    }

    if (gestionnaireDetecte.isEmpty()) {
        qDebug() << "Aucun gestionnaire de paquets détecté.";
        return -1;
    }

    // Obtenir le mot de passe de l'utilisateur
    QString motDePasse=SpecialActions::obtenirMotDePasse(motDePasse);
    if (!motDePasse.indexOf("---OLOP---ERROR---OLOP---")) {
        qDebug() << "Annulation par l'utilisateur.";
        return -1;
    }

    // Installer le paquet avec le gestionnaire de paquets détecté et le mot de passe de l'utilisateur
    // Construire la liste des arguments de la commande sudo
    QStringList args;
    args << "-S" << gestionnaireDetecte << "install" << "-y" << packagename;

    // Installer le paquet avec le gestionnaire de paquets détecté et le mot de passe de l'utilisateur
    QProcess process;
    process.start("sh", QStringList() << "-c" << "echo '" + motDePasse + "' | sudo -S " + args.join(" "));
    process.waitForFinished(-1);

    int codeSortie = process.exitCode();
    QByteArray sortieStandard = process.readAllStandardOutput();
    QByteArray sortieErreur = process.readAllStandardError();

    if (codeSortie == 0) {
        qDebug() << "Installation de "+packagename+" réussie";
            return 0;
    } else {
        qDebug() << "Erreur lors de l'installation. Code de sortie :" << codeSortie;
        qDebug() << "Sortie standard du processus :" << sortieStandard;
        qDebug() << "Sortie d'erreur du processus :" << sortieErreur;
        return -1;
    }
#else
    qDebug() << "ERREUR : Impossible d'installer des packages sur un OS inconnu.";
    return -1;
#endif
}
