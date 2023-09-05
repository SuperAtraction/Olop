var MAIN = {
    ResizeEvent: function() {
        if (panels.length === 0) {
            console.log("Le tableau est vide.");
        } else {
            for (let [index, valeur] of panels.entries()) {
                if (valeur !== undefined) {
                    var panel = $(valeur);
                    PANEL.updatePanelSize(panel.attr("id"));
                    console.log(panel.attr("id"));
            }
        }
    }}
}