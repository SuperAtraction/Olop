function changeWindowContent(windowElement, contentUrl) {
    // Enregistrer les dimensions actuelles de la fenêtre
    var currentWidth = windowElement.width();
    var currentHeight = windowElement.height();

    // Charger le nouveau contenu
    windowElement.find('.window-content').load(contentUrl, function(response, status) {
        if(status === "error") {
            var errorMessage = "Une erreur est survenue : " + response;
            console.error(errorMessage);
            // vous pouvez également afficher le message d'erreur dans la fenêtre
        } else {
            // Enregistrer les nouvelles dimensions du contenu
            var newWidth = windowElement.find('.window-content').outerWidth();
            var newHeight = windowElement.find('.window-content').outerHeight();

            // Ajuster les dimensions de la fenêtre si nécessaire
            if (newWidth > currentWidth) {
                windowElement.width(newWidth);
            }
            if (newHeight > currentHeight) {
                windowElement.height(newHeight);
            }
        }
    });
}
