$(document).ready(function() {
  // Récupérer le code HTML de l'ul.menu
  var menuHTML = $('ul.menu').html();

  // Ajouter une ligne "Bienvenue" au code HTML
  menuHTML += '<li><a href="#" onclick="loadlinuxpage();">Fonctions Linux</a></li>';

  // Remplacer le contenu de l'ul.menu avec le nouveau code HTML
  $('ul.menu').html(menuHTML);
});

function loadlinuxpage(){
  $("#content").load("Linux.html");
}
