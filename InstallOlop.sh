#!/bin/bash

# URL du fichier ZIP à télécharger
url="https://github.com/SuperAtraction/Olop/releases/download/alpha-1.1/Olop"

# Chemin de destination pour le téléchargement et l'extraction
destination="/opt/Olop/"

# Chemin vers l'exécutable ou la bibliothèque Qt à analyser
executable=$destination"Olop"

if [ "$EUID" -ne 0 ]; then
    echo "Exécution en tant que root"
    pkexec $0
    exit
fi

mkdir $destination
rm -f $destination"Olop"

# Téléchargement du fichier
wget --no-check-certificate -P $destination $url

# Vérifier si la commande 'unzip' est disponible, sinon l'installer
if ! command -v unzip &>/dev/null; then
    echo "La commande 'unzip' n'est pas installée. Installation en cours..."
    if command -v apt-get &>/dev/null; then
        sudo apt-get update
        sudo apt-get install -y unzip
    elif command -v yum &>/dev/null; then
        sudo yum update
        sudo yum install -y unzip
    elif command -v dnf &>/dev/null; then
        sudo dnf update
        sudo dnf install -y unzip
    elif command -v pacman &>/dev/null; then
        sudo pacman -Sy unzip --noconfirm
    else
        echo "Le gestionnaire de paquets n'a pas été détecté. Veuillez installer 'unzip' manuellement."
        exit 1
    fi
    echo "La commande 'unzip' a été installée avec succès."
fi

# Extraction du ZIP
unzip $destination"Olop.zip" -d $destination

# Configuration de la variable LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/opt/Olop/libs/:$LD_LIBRARY_PATH

# Permissions sur les fichiers
chmod +x $executable
chmod -R a+rw $destination

# Créer le fichier de raccourci
shortcut_file="$HOME/.local/share/applications/olop.desktop"

cat > "$shortcut_file" <<EOF
[Desktop Entry]
Name=Olop
Exec=/opt/Olop/Olop
Icon=/opt/Olop/Olop.png
Terminal=false
Type=Application
Categories=Utility;
EOF

# Mettre les permissions appropriées sur le fichier de raccourci
chmod +x "$shortcut_file"

echo "Le raccourci pour Olop a été créé dans le menu Démarrer."
echo "Olop et ses dépendances ont été installés avec succès. Olop va se lancer automatiquement."
$executable

exit
