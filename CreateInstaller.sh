#!/bin/bash

# Chemin vers le dossier d'Olop
olop_dir="/home/superatraction/Documents/Qt C++/Olop/"

# Chemin vers le dossier des bibliothèques Qt
qt_lib_dir="/home/superatraction/Qt/6.5.1/gcc_64/lib/"

# Exécuter la commande ldd pour obtenir la liste des bibliothèques utilisées par Olop
libs=$(ldd "$olop_dir/Olop")

# Créer un dossier temporaire pour stocker toutes les bibliothèques
temp_dir=$(mktemp -d)

cp "$olop_dir/Olop" "$temp_dir/Olop"
cp "$olop_dir/Olop.png" "$temp_dir/Olop.png"

# Parcourir chaque ligne de la sortie de ldd
while IFS= read -r line; do
    # Extraire le nom de la bibliothèque de la ligne
    lib=$(echo "$line" | awk '{print $3}')

    # Vérifier si la bibliothèque est située dans le dossier des bibliothèques Qt
    if [[ $lib == *"$qt_lib_dir"* ]]; then
        # Extraire uniquement le nom de la bibliothèque sans le chemin absolu
        lib_name=$(basename "$lib")

        # Copier la bibliothèque vers le dossier temporaire
        cp "$lib" "$temp_dir/$lib_name"
    fi
done <<< "$libs"

# Créer une archive zip contenant toutes les bibliothèques
zip -j "$olop_dir/olop_libs.zip" "$temp_dir"/*

# Supprimer le dossier temporaire
rm -rf "$temp_dir"