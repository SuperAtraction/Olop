let panelWidth = "30%";
let panelMinWidth = "300px";
let panelMaxWidth = "500px";
var panels = [];

var PANEL = {
    updatePanelSize: function(id) {
        const panel = document.getElementById(id);
        panel.style.width = panelWidth;
        panel.style.minWidth = panelMinWidth;
        panel.style.maxWidth = panelMaxWidth;
    },

    openPanel: function(id) {
        const panel = document.getElementById(id);
        const title = "<h2>" + $(panel).attr("titre");

        // Mettez à jour la taille du panneau avant de le faire glisser
        this.updatePanelSize(id);  // Notez que j'ai changé updatePanelSize par 'this.updatePanelSize'

        if (!$(panel).hasClass("initialized")) {
            var content = $(panel).html();
            const headerContent = title + " <button class='panel-close' onclick='PANEL.closePanel(\"" + id + "\");'>&#10060;</button></h2><hr>";
            $(panel).html("<span class='header'>" + headerContent + "</span><div class='scroll-content'>" + content + "</div>");
            $(panel).addClass("initialized");
            panels.push(panel);
        }

        // Maintenant, faites glisser le panneau
        panel.style.right = '50px';
    },

    closePanel: function(id) {
        const panel = document.getElementById(id);
        const computedStyle = window.getComputedStyle(panel);
        const panelActualWidth = panel.offsetWidth;
        const offset = panelActualWidth + 35;
        panel.style.right = '-' + offset + "px";

        // Supprimer le panneau de la liste 'panels'
        const index = panels.indexOf(panel);
        if (index !== -1) {
            panels.splice(index, 1);
        }
    }
};
