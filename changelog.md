## frontieres version 0.10.3

apport du dernier pr :

- dialogue parametres nuage
réparation bug qui faisait relancer l'enregistrement d'une trajectoire déjà enregistrée, à la création du dialogue

## frontieres version 0.10.2

apport du dernier pr :

- controleur de nuage
enlevé les potentiometres inutiles
gestion des minimums et maximums au niveau des doublespinboxs
mise à jour du controleur depuis les doublespinbox
réparation bug orientation horizontale
- changelog
précédent pr en 0.10.1 et non 0.9.1

## frontieres version 0.10.1

apport du dernier pr :

- nouvelle fonctionnalité controleur de nuage
accessible depuis menu contextuel d'un cloud, permet de gérer hauteur de son et volume dans un controleur carré qu'on peut orienter verticalement (hauteur de son a la verticale, puissance à l'horizontale) ou horizontalement (le contraire) et de règler l'echelle de modification de la hauteur.
on peut faire glisser le controleur ou cliquer ailleur pour le deplacer, arreter le nuage en cliquant droit

## frontieres version 0.9.1

apport du dernier pr :

- gestion des entrées audio :
traduction fr

## frontieres version 0.9.0

apport du dernier pr :

- gestion des entrées audio :
ajout possibilité de modifier la longueur des entrées audio par défaut (menu edition, options, onglet entrées)
ajout d'un fonction permettant de "figer" le contenu d'un echantillon dynamique d'entrée audio pour le transformer en échantillon fixe (menu contextuel sur un echantillon d'entrée préalablement sélectionné)

## frontieres version 0.8.1

apport du dernier pr :

- réparation bug enêtre paramètres nuage : par défaut la trajectoire doit être sur statique, et non sur enregistrée, ce qui faisait planter le programme en tentant de passer en enregistrement.

- complément de traduction pour la fenêtre paramètres nuage

## frontieres version 0.8.0

apport du dernier pr :

- gestion de la place des grains
      depuis la fenetre parametre des nuages, dans un onglet grains
      révision de la fenêtre de parametres des nuages, maintenant sur 3 onglets (nuage, trajectoire, grains)

## frontieres version 0.7.0

apports du dernier pr : 

- gestion d'entrées audio (16 par défaut)
      création d'un echantillon d'entrée audio depuis le menu edition, avec choix de l'entrée
      echantillon mis à jour grâce à un buffer circulaire (10 secondes par défaut)
      sauvegardes des parametres d'echantillons d'entrees audio dans les scenes

apports depuis la derniere release (0.4, fork de borderlands) :

- gestion de la traduction
- possibilité d'ajouter des échantillons (menu edition)
- gestion des echantillons au format mp3
- extension du nombres de sorties audio possibles (16 par defaut)
- gestion de scenes avec sauvegardes (menu fichier)
- gestion de messages osc (parametres des nuages)
- fenetre d'edition des parametres des nuages
- gestion midi polyphonique
- parametrage des banques de sons gérés par midi (menu midi)
- gestion d'envelope de volume des nuages
- application de trajectoires aux nuages (types rebond, circulaire, hypotrochoïde, ou enregistrée manuellement)
- ajout de déclencheurs succeptibles d'activer ou désactiver des nuages ou d'autres déclencheurs
- application de trajectoires aux déclencheurs

