#include "SpecialActions.hpp"

SpecialActions::SpecialActions()
{

}

QString SpecialActions::obtenirMotDePasse(QString &motDePasse) {
    // Créer un dialogue d'entrée de texte pour obtenir le mot de passe
    qDebug() << MAIN::mode;
    bool ok;
    QString texte = QInputDialog::getText(nullptr, "Authentification", "Mot de passe :", QLineEdit::Password, QString(), &ok);

    if (ok && !texte.isEmpty()) {
        return texte;
    } else {
        return "---OLOP---ERROR---OLOP---"; // L'utilisateur a annulé ou n'a pas entré de mot de passe
    }
}
