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
        // Variables
        var notificationCount = 0;
        var minimizedWindows = [];
        var minimizedWindowListRef;
        var isMinimizedWindowListOpen = false;
        var windowContentIds = [];
        var themeMode = 1;
        var FirstTATheme = 21;
        var secondTATheme = 8;
        var pageCache = {};
        var imageCache = {};
        var useCache = 1;

        function loadPage(page) {
    var content = $("#content");
    content.hide();

    if (useCache === 1 && pageCache[page]) {
        content.html(pageCache[page]);
        content.show();
    } else {
        content.load(page + ".html", function(response, status, xhr) {
            if (status === "error") {
                var errorMessage = "Erreur: " + xhr.status + " " + xhr.statusText;
                showNotification(-1, "Erreur "+xhr.status, errorMessage);
            } else {
                if (useCache === 1) {
                    // Si useCache est défini à 1, stockez le contenu chargé dans le cache
                    pageCache[page] = response;
                }
                content.show();
            }
        });
    }
}

function preloadPage(page) {
    if (useCache === 1) {
    $.get(page + ".html", function(response) {
        pageCache[page] = response;
    });
}
}

function clearPageCache() {
    pageCache = {};
}

function preloadAllPages() {
    return new Promise((resolve) => {
        $.get("pages.txt", function(data) {
            // Divisez les données en lignes
            var pages = data.split("\n");
            
            var promises = []; // Utilisé pour stocker toutes les promesses de préchargement

            // Préchargez chaque page
            for (var i = 0; i < pages.length; i++) {
                // Enlevez les espaces blancs inutiles
                var page = pages[i].trim();
                
                if (page) { // Ignorez les lignes vides
                    promises.push($.get(page + ".html", function(response) {
                        if (useCache === 1) {
                            pageCache[page] = response;
                        }
                    }));
                }
            }

            $.when(...promises).then(() => {
                resolve();
            });
        });
    });
}

function preloadImage(src) {
    return new Promise((resolve, reject) => {
        const img = new Image();
        img.onload = () => {
            imageCache[src] = img;
            resolve(img);
        };
        img.onerror = reject;
        img.src = src;
    });
}

function getImageFromCache(src) {
    if (imageCache[src]) {
        return Promise.resolve(imageCache[src]);
    } else {
        return preloadImage(src);
    }
}

function preloadAllImages() {
    return new Promise((resolve) => {
        $.get("images.txt", function(data) {
            // Divisez les données en lignes
            var images = data.split("\n").filter(Boolean);
            
            var imagesLoaded = 0;

            images.forEach(function(imageUrl) {
                // Supprimez les espaces blancs inutiles
                imageUrl = imageUrl.trim();

                var img = new Image();
                img.onload = function() {
                    // Stockez l'image préchargée dans le cache
                    imageCache[imageUrl] = this.src;
                    imagesLoaded++;

                    // Vérifiez si toutes les images ont été préchargées
                    if (imagesLoaded === images.length) {
                        resolve();
                    }
                };

                img.onerror = function() {
                    console.warn("Failed to preload image:", imageUrl);
                    imagesLoaded++;

                    // Vérifiez si toutes les images ont été préchargées (y compris celles qui n'ont pas été chargées)
                    if (imagesLoaded === images.length) {
                        resolve();
                    }
                };

                img.src = imageUrl;
            });

            // Gérer le cas où il n'y a aucune image à précharger
            if (images.length === 0) {
                resolve();
            }
        });
    });
}

        // Afficher ou masquer le menu
        function toggleMenu() {
    var sidebar = document.getElementById("sidebar");
    var toggleButton = document.getElementById("toggleMenuBtn");
    var content = document.getElementById("content");
    var clock = document.getElementById("clock");

    sidebar.classList.toggle("hidden");
    toggleButton.classList.toggle("closed");
    clock.classList.toggle("open-left");

    content.style.marginLeft = sidebar.classList.contains("hidden") ? "0" : "250px";
}

        // Mise à jour de l'horloge toutes les secondes
        function updateTime() {
            var clock = document.getElementById("clock");
            var date = new Date();
            var hours = date.getHours();
            var minutes = date.getMinutes();
            var seconds = date.getSeconds();
            clock.innerHTML = addLeadingZero(hours) + ":" + addLeadingZero(minutes) + ":" + addLeadingZero(seconds);
            setTimeout(updateTime, 1000);
        }

        // Ajouter un zéro devant les chiffres < 10
        function addLeadingZero(number) {
            return (number < 10) ? "0" + number : number;
        }

        // Afficher une notification
        function showNotification(type, title, message) {
            var icon = "Olop.png";
            if (type === -1) {
                icon = "Warning.png";
            } else if (type === 0) {
                icon = "Info.png";
            } else if (type === 1) {
                icon = "Ok.png";
            }
            var notificationContent = `<span>${message}</span><div style='display: flex; flex-direction: column; align-items: flex-end;'><button type='button' onclick='closeWindow(this, 1, 1)'>FERMER</button></div>`;
var notification = openWindow(icon, title, notificationContent, 1);

            var notificationBar = $('#notificationBar');
            var notificationCountElement = $('#notificationCount');

            notificationBar.hide();
            notificationCount++;
            notificationBar.css('display', 'block');
            notificationCountElement.text(notificationCount);
            notificationBar.fadeIn('slow');
        }

        function updateMinSize(windowElement, useIframe){
            if (!useIframe) { // Si ce n'est pas une iframe, ne faites rien et retournez directement
        return;
    }
    setTimeout(() => {
        initialHeight = windowElement.outerHeight();
initialWidth = windowElement.outerWidth();

if (initialWidth == window.innerWidth) {
    initialWidth = initialWidth / 4;
}

var margin = 5;  // 5% de marge
var windowWidth = $(window).width();
var windowHeight = $(window).height();
var containmentWidth = windowWidth - windowWidth * (margin / 100);
var containmentHeight = windowHeight - windowHeight * (margin / 100);

windowElement.resizable({
    containment: "parent",
    minWidth: initialWidth,
    minHeight: initialHeight,
    resize: function(event, ui) {
        // Calculez les dimensions maximales
        var maxWidth = $(window).width() - ($(window).width() * margin / 100);
        var maxHeight = $(window).height() - ($(window).height() * margin / 100);

        // Appliquez les limites de taille
        if(ui.size.width > maxWidth) {
            ui.size.width = maxWidth;
            ui.position.left = event.originalEvent.pageX - (maxWidth / 2);
        }
        if(ui.size.height > maxHeight) {
            ui.size.height = maxHeight;
            ui.position.top = event.originalEvent.pageY - (maxHeight / 2);
        }

        setTimeout(() => {
    if (useIframe) {
        var iframe = $(ui.element).find('iframe').get(0);
        resizeIframe(iframe);
    }
}, 100);
    }
});
}, 10)}
        // Ouvrir une nouvelle fenêtre
function openWindow(icon, title, content, isnotif, id, useIframe) {
    if (icon === "") {
        icon = "Olop.png";
    }

    var windowElement = createWindowElement(icon, title, content, isnotif, id, useIframe);
    document.body.appendChild(windowElement);
    return windowElement; // Retourne l'élément de fenêtre créé
}

function createWindowElement(icon, title, content, isnotif, id, useIframe) {
    if (!id) {
        id = "window-" + Math.random().toString(36).substr(2, 9); 
    }

    var idW = Math.random().toString(36).substr(2, 9);
    var windowElement = $('<div class="window" id="' + idW + '"></div>');
    var windowTitleBar = $('<div class="window-title-bar"></div>');
    var windowTitle = $('<span class="window-title"></span>');

    function setIcon(src) {
        var iconImage = $('<img class="window-icon" src="' + src + '">');
        windowTitle.append(iconImage).append(title);
    }

    if (imageCache[icon]) {
        setIcon(imageCache[icon]);
    } else {
        var img = new Image();
        img.onload = function() {
            imageCache[icon] = this.src;
            setIcon(this.src);
        };
        img.onerror = function() {
            console.warn("Icon loading failed, setting default icon.");
            setIcon('Olop.png'); 
        };
        img.src = icon;
    }

    var closeButton = $('<button type="button">&#10060;</button>').click(function() {
        if (isnotif === 1) {
            if (notificationCount === 1) {
                $("#notificationBar").fadeOut("slow");
                notificationCount = 0;
            } else {
                notificationCount--;
            }
            document.getElementById("notificationCount").innerText = notificationCount;
        }
        closeWindow(this);
    });

    var windowContentId;
    if (!id) {
        windowContentId = generateUniqueIdForWC();
        while (windowContentIds.includes(windowContentId)) {
            windowContentId = generateUniqueIdForWC();
        }
    } else {
        windowContentId = id;
    }

    var windowContent;
    if (useIframe) {
        windowContent = $('<iframe class="window-content" id="' + id + '" src="' + content + '"></iframe>');
    } else {
        windowContent = $('<div class="window-content" id="' + id + '">' + content + '</div>');
    }

    var minimizeButton = $('<button type="button" class="minimize-button">&#9473;</button>').click(function() {
        minimizeWindow(idW);
    });

    if (document.body.classList.contains('dark-theme')) {
        minimizeButton.addClass('dark-theme');
        windowTitleBar.addClass('dark-theme');
        windowElement.addClass('dark-theme');
    }

    windowTitleBar.append(windowTitle).append("&nbsp;").append(minimizeButton).append('&nbsp;').append(closeButton);
    windowElement.append(windowTitleBar).append(windowContent);
    windowElement.attr('isnotif', isnotif);

    windowElement.ready(function() {
        windowElement.draggable({
            handle: ".window-title-bar",
            containment: "window",
    iframeFix: true
        });

        updateMinSize(windowElement, useIframe);

        const contentObserver = new MutationObserver((mutationsList, observer) => {
            for (const mutation of mutationsList) {
                if (mutation.type === 'childList' || mutation.type === 'characterData' || mutation.type === 'attributes') {
                    setTimeout(() => {
                        updateMinSize(windowElement, useIframe);
                    }, 500);
                }
            }
        });

        const config = { attributes: true, childList: true, characterData: true, subtree: true };
        contentObserver.observe(windowContent.get(0), config);
    });

    return windowElement.get(0);
}

function findWindowAndConvertToIframe(contentId, iframeSrc) {
    var contentElement = document.getElementById(contentId);
    if (contentElement) {
        // Trouver l'élément de la fenêtre parente en remontant dans la hiérarchie des parents
        var windowElement = contentElement.closest('.window');
        if (windowElement) {
            // Convertir le contenu de la fenêtre en iframe
            var windowId = windowElement.id;
            convertWindowContentToIframe(windowId, iframeSrc);
        } else {
            console.error('Aucune fenêtre parente trouvée pour l\'élément avec l\'ID:', contentId);
        }
    } else {
        console.error('Aucun élément trouvé avec l\'ID:', contentId);
    }
}

function convertWindowContentToIframe(windowId, iframeSrc) {
    var windowElement = $('#' + windowId);
    if (windowElement.length > 0) {
        var windowContentElement = windowElement.find('.window-content');
        var windowContentId = windowContentElement.attr('id');

        var iframeElement = $('<iframe></iframe>').attr('id', windowContentId).attr('src', iframeSrc);
        windowContentElement.replaceWith(iframeElement);

        setTimeout(() => {
            updateMinSize($("#"+windowId), true);
        }, 100); // Le délai donne à l'iframe le temps de se charger
    } else {
        console.error('Aucune fenêtre trouvée avec l\'ID:', windowId);
    }
}

function resizeIframe(iframe) {
    var windowContainer = iframe.parentNode;
    $(iframe).height($(windowContainer).height() - $(windowContainer).find('.window-title-bar').outerHeight(true)); 
    $(iframe).width($(windowContainer).width());
}

function generateUniqueIdForWC() {
    return "window_content_" + Math.random().toString(36).substr(2, 9);
}

        function closeWindow(button, isdb, isnotif) {
    if (isdb === 1) {
        // Si la fenêtre est minimisée, fermer et supprimer la fenêtre de la liste des fenêtres minimisées
        button.parentNode.parentNode.parentNode.remove();

        var id = button.parentNode.parentNode.id;
        var index = minimizedWindows.indexOf(id);
        if (index !== -1) {
            minimizedWindows.splice(index, 1);
        }
        var windowContentElement = button.parentNode.parentNode.parentNode.querySelector('.window-content');
    if (windowContentElement) {
        const observer = windowContentElement.observer;
        if (observer) {
            observer.disconnect();
        }
    }
    } else if(isdb === -1){
        button.remove();

        var id = button.id;
        var index = minimizedWindows.indexOf(id);
        if (index !== -1) {
            minimizedWindows.splice(index, 1);
        }
        var windowContentElement = button.querySelector('.window-content');
    if (windowContentElement) {
        const observer = windowContentElement.observer;
        if (observer) {
            observer.disconnect();
        }
    }
    } else {
        button.parentNode.parentNode.remove();
        var id = button.parentNode.parentNode.id;
        var index = minimizedWindows.indexOf(id);
        if (index !== -1) {
            minimizedWindows.splice(index, 1);
        }
        var windowContentElement = button.parentNode.parentNode.querySelector('.window-content');
    if (windowContentElement) {
        const observer = windowContentElement.observer;
        if (observer) {
            observer.disconnect();
        }
    }
    }if(isnotif==1){
        if (notificationCount === 1) {
                $("#notificationBar").fadeOut("slow");
                notificationCount = 0;
            } else {
                notificationCount--;
            }
            document.getElementById("notificationCount").innerText = notificationCount;
    }
}

        // Function to toggle window visibility between minimized and restored
        function minimizeWindow(windowId) {
  var windowElement = $('#' + windowId);

  if (minimizedWindows.includes(windowId)) {
    // Si la fenêtre est déjà minimisée, restaure-la
    var index = minimizedWindows.indexOf(windowId);
    minimizedWindows.splice(index, 1);
    windowElement.show();
  } else {
    // Sinon, minimise-la
    minimizedWindows.push(windowId);
    windowElement.hide();
  }

  // Mettre à jour le contenu du bouton "minimiser"
  updateMinimizedButton();
}

function MinimisedWIndowListHtml() {
    // Créez un nouvel élément div jQuery pour afficher la liste des fenêtres minimisées
    var windowList = $('<div>').addClass('window-list');

for (var i = 0; i < minimizedWindows.length; i++) {
    var windowId = minimizedWindows[i];
    var windowElement = $('#' + windowId);

    var windowItem = $('<div>').addClass('window-item');
    var windowTitle = windowElement.find('.window-title').text();

    // Ajoutez un bouton de restauration pour chaque fenêtre minimisée
    var restoreButton = $('<button>').addClass('window-restore-button')
                                     .text('Restaurer')
                                     .attr('data-window-id', windowId);

                                     var closeButton = $('<button>').addClass('window-close-button')
                                     .text('✖')
                                     .attr('data-window-id', windowId);
    windowItem.text(windowTitle).append("&nbsp;");
    windowItem.append(restoreButton);
    windowItem.append(closeButton);
    windowList.append(windowItem);
}

return windowList.get(0).outerHTML;
}

// Fonction pour afficher les fenêtres minimisées
function viewMinimizedWindows() {
    var windowListHTML = MinimisedWIndowListHtml();
    var minisedwindows = openWindow("", "Fenêtres minimisées", windowListHTML, 0);

    // Le gestionnaire d'événements est attaché à l'élément parent 'minisedwindows' et délégué aux éléments cibles
    $(minisedwindows).on("click", ".window-restore-button", function() {
        var id = $(this).attr('data-window-id');
        var index = minimizedWindows.indexOf(id);
        minimizedWindows.splice(index, 1);
        $("#" + id).show();
        closeWindow(minisedwindows, -1);
        updateMinimizedButton();
    });

    $(minisedwindows).on("click", ".window-close-button", function() {
        var id = $(this).attr('data-window-id');
        var isnotif= $("#"+id).attr('isnotif');
        var index = minimizedWindows.indexOf(id);
        if (index !== -1) {
            minimizedWindows.splice(index, 1);
        }
        closeWindow(document.getElementById(id), -1);
        $(minisedwindows).find('.window-content').html(MinimisedWIndowListHtml()); // Mettre à jour la liste après fermeture

        updateMinimizedButton();
        console.log(isnotif);
        if(isnotif==1){
            if (notificationCount === 1) {
                $("#notificationBar").fadeOut("slow");
                notificationCount = 0;
            } else {
                notificationCount--;
            }
            document.getElementById("notificationCount").innerText = notificationCount;
        }
        var MinimizedWindow = MinimisedWIndowListHtml();
        if (MinimizedWindow !== "<div class=\"window-list\"></div>") {
            $(minisedwindows).find('.window-content').html(MinimizedWindow);
        } else {
            closeWindow(minisedwindows, -1);
        }
    });
}

async function closeMinimizedWindowList() {
    if (isMinimizedWindowListOpen && minimizedWindowListRef) {
        console.log('Close minimized window list:', minimizedWindowListRef.parentNode.parentNode);
        await minimizedWindowListRef.parentNode.parentNode.remove();
        isMinimizedWindowListOpen = false;
    }
}


        function decodeApp(data) {
            var lignesSeparate = data.split('\n').filter(Boolean);
            var result = [];

            var nom;
            var version;
            var url;
            var developpeur;
            var type;
            var os;
            var description;

            if (lignesSeparate.length >= 6) {
                nom = lignesSeparate[0];
                version = lignesSeparate[1];
                url = lignesSeparate[2];
                developpeur = lignesSeparate[3];
                type = lignesSeparate[4];
                os = lignesSeparate[5];

                // Si la description contient plusieurs lignes, les concaténer
                if (lignesSeparate.length > 6) {
                    description = lignesSeparate.slice(6).join("\n");
                }
            }

            result.push("", nom, version, url, developpeur, type, os, description);
            return result;
        }

        function updateMinimizedButton() {
  var minimizedInfo = $("#minimizedInfo");
  var viewMinimizedButton = $("#viewMinimized");

  var numMinimized = minimizedWindows.length;
  if (numMinimized > 0) {
    minimizedInfo.html("Vous avez " + numMinimized + " fenêtre(s) minimisée(s).");
    viewMinimizedButton.show();
  } else {
    minimizedInfo.html("");
    viewMinimizedButton.hide();
  }
}

        async function downloadFile(fileUrl) {
            try {
                const response = await fetch(fileUrl);
                if (!response.ok) {
                    throw new Error('Erreur lors du téléchargement du fichier');
                }
                const data = await response.text();
                return data;
            } catch (error) {
                console.error('Une erreur s\'est produite :', error);
                showNotification(-1, 'Erreur de chargement du .app', error.message);
                return null;
            }
        }

        async function loadPAP(app) {
            try {
                const result = await downloadFile(`/getapp/${app}`);
                const decodedResult = decodeApp(result);
                const nom = decodedResult[1];
                const version = decodedResult[2];
                const url = decodedResult[3];
                const developpeur = decodedResult[4];
                const type = decodedResult[5];
                const os = decodedResult[6];
                const description = decodedResult[7];
                const iconurl = `${url}/favicon.png`;

                const regex = /\/([a-zA-Z0-9]{1,6})\.app$/;
                const match = app.match(regex);
                const appf = match ? match[1] : "";

                try {
                    const iconResult = await downloadFile(`/checkurl/200/${iconurl}`);
                    if (iconResult.indexOf("1") === -1) {
                        openWindow("/Olop.png", nom, getAppDescription(nom, version, developpeur, description, app));
                    } else {
                        openWindow(iconurl, nom, getAppDescription(nom, version, developpeur, description, app));
                    }
                } catch (error) {
                    openWindow("/Olop.png", nom, getAppDescription(nom, version, developpeur, description, app));
                }
            } catch (error) {
                // Gérer l'erreur de downloadFile("/getapp/")
            }
        }

        function getAppDescription(nom, version, developpeur, description, app) {
            return `${description}<div style='text-align: right;'><i>${nom} version ${version}, <br>développé par ${developpeur}</i><br><span style='text-align: left'><button type="button" onclick="deleteApp(this, '`+app+`');">Déinstaller</button></span><button type="button" onclick="closeWindow(this,1);">Annuler</button>&nbsp;<button type="button" onclick="Launch(this, '`+app+`');">Lancer</button></div><br>`;
        }

        function Launch(button, app) {
    $(button).parent().parent().load("/Launch/1/" + app + "-OLOP-" + $(button).parent().parent().attr("id"));
}

function deleteApp(button, app){
    var contentID = button.closest('.window-content').id;
    $.ajax({
    url: "/remove/" + app + "-OLOP-" + contentID,
    type: 'GET',
    success: function(responseText) {
        if (responseText !== "OK") {
            $("#" + contentID).html(responseText);
        }
    },
    error: function(xhr, textStatus, errorThrown) {
        // Gérez les erreurs ici si nécessaire
        console.error("Erreur lors de la requête :", textStatus);
    }
});

}

function tmpHTML(id, time, content){
    var oldContent = $("#"+id).html();
    $("#"+id).html(content);
    setTimeout(() => {
        $("#"+id).fadeOut("slow", function () {
            $("#"+id).html(oldContent);
            $("#"+id).fadeIn("slow");
        });
    }, time);
}

        function update() {
            $("#button-update").html("Recherche de mises à jour...")
            $("#info1").fadeIn("slow");
            $.ajax({
                url: "https://olopsytems.000webhostapp.com/Checkupdate.php?ver=alpha-1.1",
                data: { id: $(this).attr("id") },
                dataType: "text",
                success: function(recup) {
                    console.log(recup);
                    if (recup.indexOf("0") !== -1) {
                        $("#button-update").html("Aucune mise à jour disponible.<button class=\"button\" onclick=\"mask(1);\">x</button>")
                    } else {
                        $("#button-update").html("Mise à jour disponible. Merci de la télécharger." /*, souhaitez-vous l'installer ? <button onClick=\"install();\">Oui</button><button onclick=\"mask(1);\">Non</button>"*/ );
                    }
                },
                error: function(xhr, ajaxOptions, thrownError) {
                    $("#button-update").html("Une erreur est survenue lors de la vérification de mises à jours, veuillez vérifier votre connexion internet.<button onclick=\"update();\">Rééssayer</button><button onclick=\"mask(1);\">x</button>"); //Ce code affichera le message d'erreur, ici Message d'erreur.
                }
            });
        }
        function mask(to) {
            if(to==1){
                $("#info1").fadeOut("slow");
            }
        }

        function openSettings() {
            PANEL.openPanel("SETTINGS");
}

        function toggleTheme(force) {
    var shouldToggle = true;
    if (force === 'dark' && !document.body.classList.contains('dark-theme')) {
        shouldToggle = true;
    } else if (force === 'light' && document.body.classList.contains('dark-theme')) {
        shouldToggle = true;
    } else if (force === 'toggle') {
        shouldToggle = true;
    } else {
        shouldToggle = false;
    }

    if (shouldToggle) {
        var body = document.body;
        var sidebar = document.getElementById('sidebar');
        var content = document.getElementById('content');
        var windows = Array.from(document.querySelectorAll('.window'));
        var titleBars = Array.from(document.querySelectorAll('.window-title-bar'));
        var buttons = Array.from(document.querySelectorAll('.button'));
        var clock = document.getElementById('clock');
        var toggleMenuBtn = document.getElementById('toggleMenuBtn');
        var links = Array.from(document.querySelectorAll("a"));

        // List of elements to toggle dark theme
        var elementsToToggle = [body, sidebar, content, clock, toggleMenuBtn];

        // Push windows and title bars into elements to toggle
        elementsToToggle.push(...windows, ...titleBars, ...buttons, ...links);

        // Toggle dark theme for each element
        elementsToToggle.forEach(element => {
            element.classList.toggle('dark-theme');
        });
    }
}

function initFunction() {
    return new Promise((resolve, reject) => {
            const observer = new MutationObserver((mutationsList) => {
    // Parcourir toutes les mutations qui ont eu lieu
    for (const mutation of mutationsList) {
        // Si le nœud ajouté est un élément
        if (mutation.type === 'childList' && mutation.addedNodes.length > 0) {
            mutation.addedNodes.forEach((node) => {
                if (node instanceof Element) {

                   // Si le nœud est un bouton et n'est PAS à l'intérieur d'une .window-title-bar
                   if (node.tagName === 'BUTTON' && !node.closest('.window-title-bar')) {
                        node.classList.add('button');
                    }

                    // Cherche tous les boutons descendants mais exclut ceux à l'intérieur de .window-title-bar
                    node.querySelectorAll('button:not(.window-title-bar button)').forEach((btn) => {
                        btn.classList.add('button');
                    });

if(document.body.classList.contains('dark-theme')){
                    // vérifiez si le nœud est un élément
                    // Vérifiez si le nœud ajouté est un lien et ajoutez la classe dark-theme s'il y en a un
                        node.classList.add('dark-theme');
                    // Si le nœud ajouté a des descendants de type lien, ajoutez leur classe dark-theme
                    const links = node.querySelectorAll('a');
                    links.forEach((link) => {
                        link.classList.add('dark-theme');
                    });
                }
                }
            });
        }
    }
});

function updateTheme() {
if(themeMode===1){
    var currentHour = new Date().getHours();
    if (currentHour >= FirstTATheme || currentHour < secondTATheme) {
        toggleTheme('dark');
    } else {
        toggleTheme('light');
    }
}}

// Commencer à observer le document avec les configurations spécifiées
observer.observe(document.body, { childList: true, subtree: true });

setTimeout(() => {
    updateTheme();
    setInterval(updateTheme, 60000); // Vérifie le thème chaque minute

    // Exécutez preloadAllPages() et preloadAllImages() simultanément
    Promise.all([preloadAllPages(), preloadAllImages()]).then(() => {
        // Lorsque toutes les pages ET toutes les images sont préchargées, continuez avec le reste de la logique

        // Charger la page 'home'
        loadPage("home");

        updateTime();
        update();
        toggleMenu();
        $.get("avertissement.html", function(data) {
            showNotification(-1, "Avertissement sur Olop", data);
        });
        setInterval(() => {
            if(notificationCount<0){
                notificationCount=0;
                $("#notificationBar").fadeOut("slow");
            }
        }, 5000);
        // Créer l'élément overlay
    let overlay = document.createElement('div');
    overlay.id = 'overlay';
    overlay.className = 'overlay';

    // Ajouter l'overlay au body
    document.body.appendChild(overlay);
    $("#SETTINGS").load("settings.html");
        $("#content").show();
    });
    resolve();

}, 10);
        })}
