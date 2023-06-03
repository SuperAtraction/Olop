#!/bin/bash

# URL du fichier ZIP à télécharger
url="https://raw.githubusercontent.com/SuperAtraction/Olop/main/Olop"

# Chemin de destination pour le téléchargement et l'extraction
destination="/opt/Olop/"

# Chemin vers l'exécutable ou la bibliothèque Qt à analyser
executable=$destination"Olop"

if [ "$EUID" -ne 0 ]
  then echo "Exécuction en root"
  pkexec $0
  echo "Création du raccourci"
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
  echo "Olop et ses dépendances ont étés installés avec succés. Olop va se lancer automatiquement"
  $executable
  exit
fi

mkdir $destination

rm -f $destination"Olop"

# Téléchargement du fichier
wget --no-check-certificate -P $destination $url

# Analyse des dépendances avec ldd
dependencies=$(ldd $executable | awk '{if ($3 == "not") print $1}')

# Installation des bibliothèques manquantes avec le gestionnaire de paquets approprié
if command -v apt-get >/dev/null 2>&1; then
    # Utilisation du gestionnaire de paquets apt
    sudo apt-get update
    for dependency in $dependencies; do
        package=$(dpkg-query -S $dependency 2>/dev/null | cut -d: -f1)
        if [ -n "$package" ]; then
            sudo apt-get install --reinstall -y $package
        fi
    done
elif command -v dnf >/dev/null 2>&1; then
    # Utilisation du gestionnaire de paquets DNF (Fedora, CentOS, etc.)
    sudo dnf check-update
    for dependency in $dependencies; do
        package=$(rpm -qf $dependency)
        if [ -n "$package" ]; then
            sudo dnf reinstall -y $package
        fi
    done
elif command -v pacman >/dev/null 2>&1; then
    # Utilisation du gestionnaire de paquets pacman (Arch Linux, Manjaro, etc.)
    sudo pacman -Syu --needed
    for dependency in $dependencies; do
        package=$(pacman -Qo $dependency | cut -d' ' -f5)
        if [ -n "$package" ]; then
            sudo pacman -S --needed $package
        fi
    done
else
    echo "Gestionnaire de paquets non pris en charge"
    exit 1
fi

chmod +x $executable
chmod -R a+rw /opt/Olop/

exit