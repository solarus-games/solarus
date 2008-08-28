# Ce fichier comporte l'ensemble des messages affichés
# par le moteur de jeu en version française.
# Les messages affichés par les maps sont quant à eux
# définis dans le fichier game.msg.
#
# La syntaxe est comme celle d'un fichier ini de Windows.
# Un message est défini par les éléments suivants :
#
# - Un identificateur entre crochets.
# Les messages du moteur de jeu (donc tous ceux dans ce fichier)
# ont toujours un identificateur qui commence par le caractère '_'.
#
# - Les valeurs line1, line2 et line3 : définissent le texte du
# message, sur une à trois lignes. Le texte de chaque ligne
# doit être mis entre guillemets. Une ligne ne doit pas
# dépasser 23 caractères.
#
# - La valeur next : donne l'identificateur du prochain message
# à afficher. Si cette valeur n'est pas définie, cela signifie
# que ce message était le dernier de la série et la boîte de 
# dialogue va se fermer une fois qu'il sera terminé.
#
# - Par convention, lorsqu'un texte est trop long pour tenir
# dans un seul message, la suite est dans un ou plusieurs messages
# dont l'identificateur est le même auquel on ajoute ".1", puis
# ".2", etc. Lors de la traduction, on peut ainsi faire tenir
# le texte dans un nombre différent de messages sans avoir
# de problèmes d'identificateurs.
#
# - La valeur cancel : définit ce qui se passe lorsque le joueur
# tente de passer le message en appuyant sur la touche C. Il y a
# 3 valeurs possibles :
#    - none (par défaut) : le message ne peut pas être passé.
#    - current : si on appuie sur C, le message courant s'affiche
#                immédiatement en entier (au lieu de défiler).
#                On peut ainsi passer plus vite au message suivant.
#    - all : si on appuie sur C, le message courant s'arrête
#            et les messages suivants ne seront pas affichés. La
#            boîte de dialogue se ferme.
# La valeur de cancel s'applique au message courant et à tous les
# messages suivants, jusqu'à ce que la valeur soit changée ou que
# la boîte de dialogue se ferme.
#
# - La valeur icon : permet d'afficher une icône dans le message.
# La valeur est un numéro qui permet d'identifier l'icône dans
# le fichier image des icônes. L'icône s'affiche à gauche du texte
# et le texte ne doit pas dépasser 20 caractères (au lieu de 23 en
# temps normal).
#
# - La valeur question : permet d'afficher une question avec deux
# réponses au choix. Les deux réponses sont affichées sur les deux
# dernières lignes. Ces deux lignes sont alors limitées à 20
# caractères (au lieu de 23 en temps normal) car un curseur en
# forme de flèche permet au joueur de choisir sa réponse. Le jeu
# ne permet pas d'afficher à la fois une icône et une question.
# S'il y a une question, il est possible de spécifier une valeur
# next2. Si la première réponse est choisie, le prochain message
# affiché est indiqué par next. Si la deuxième réponse est choisie,
# il est alors indiqué par next2.
#
# - Enfin, il existe des séquences particulières que l'on peut
# insérer dans le texte d'un message :
#    - $0 : fait une pause d'une seconde dans le défilement du message.
#    - $1 : met une vitesse de défilement lente
#    - $2 : met une vitesse de défilement moyenne
#    - $3 : met une vitesse de déliement rapide (par défaut)
#    - $v : permet d'insérer la valeur d'une variable dans le texte
#           du message (la valeur doit alors être spécifiée dans
#           le programme qui a demandé l'affichage du message)
#
# - Les commentaires suivants permettent de vérifier le nombre
# limite de caractères dans un message :
#
#       |-----------------------| (sans icône ni question)
#       |--------------------|    (avec icône ou question)

# -------------------------------------
# Description des items de l'inventaire
# -------------------------------------
# 
# Les messages suivants sont affichés dans l'écran d'inventaire
# lorsque le joueur souhaite afficher la description d'un item
# qu'il possède. Ces messages peuvent être annulés en appuyant
# sur C.
# L'identificateur d'un message est de la forme _item_description_i_x
# où i est le numéro de l'item (de 0 à 27) et x est la variante de cet
# item possédée par le joueur.
# Exception : pour le Flacon 2, le numéro de l'item (13) est remplacé
# par celui du Flacon 1 (6) pour éviter de répéter les mêmes messages.

[_item_description_0_1]
# Plume / Feather
#       |--------------------|
line1 = "Utilise la Plume"
line2 = "pour sauter et fran-"
line3 = "chir des obstacles."
icon = 0
cancel = all

[_item_description_1_1]
# Bombes / Bombs
#       |--------------------|
line1 = "Utilise les Bombes"
line2 = "contre les ennemis"
line3 = "ou pour faire"
icon = 1
cancel = all
next = _item_description_1_1.1

[_item_description_1_1.1]
# Bombes / Bombs
#       |--------------------|
line1 = "exploser des"
line2 = "obstacles."
icon = 1

[_item_description_2_1]
# Arc / Bow
#       |--------------------|
line1 = "Tu n'as plus de
line2 = "Flèches !"
icon = 2
cancel = all

[_item_description_2_2]
# Arc & Flèches / Bow & Arrows
#       |--------------------|
line1 = "Tire des Flèches"
line2 = "pour vaincre des"
line3 = "ennemis coriaces."
icon = 3
cancel = all

[_item_description_3_1]
# Boomerang
#       |--------------------|
line1 = "Utilise le Boomerang"
line2 = "pour immobiliser les"
line3 = "ennemis ou attraper"
icon = 4
cancel = all
next = _item_description_3_1.1

[_item_description_3_1.1]
# Boomerang / Boomerang
#       |--------------------|
line1 = "des objets à"
line2 = "distance."
icon = 4

[_item_description_4_1]
# Lanterne / Lamp
#       |--------------------|
line1 = "Eclaire-toi dans le"
line2 = "noir ou allume des"
line3 = "braseros à l'aide"
icon = 5
cancel = all
next = _item_description_4_1.1

[_item_description_4_1.1]
# Lanterne / Lamp
#       |--------------------|
line1 = "de la Lanterne. Mais"
line2 = "surveille ton"
line3 = "compteur magique !"
icon = 5

[_item_description_5_1]
# Grappin / Hook Shot
#       |--------------------|
line1 = "Hisse-toi à des"
line2 = "endroits"
line3 = "inaccessibles grâce"
icon = 6
cancel = all
next = _item_description_5_1.1

[_item_description_5_1.1]
# Grappin / Hook Shot
#       |--------------------|
line1 = "au Grappin."
icon = 6

[_item_description_6_1]
# Flacon Magique vide / Empty bottle
#       |--------------------|
line1 = "Le Flacon Magique"
line2 = "peut contenir di-"
line3 = "verses substances..."
icon = 7
cancel = all

[_item_description_6_2]
# Eau dans Flacon / Water in Bottle
#       |--------------------|
line1 = "Ton Flacon Magique"
line2 = "contient de l'eau."
icon = 8
cancel = all
next = _item_description_6_2.1

[_item_description_6_2.1]
# Eau dans Flacon / Water in Bottle
#       |--------------------|
line1 = "Verse-la à un"
line2 = "endroit approprié..."
icon = 8

[_item_description_6_3]
# Potion rouge / Red potion
#       |--------------------|
line1 = "Bois de la Potion"
line2 = "rouge pour retrouver"
line3 = "tous tes Coeurs."
icon = 9
cancel = all

[_item_description_6_4]
# Potion verte / Red verte
#       |--------------------|
line1 = "Bois de la Potion"
line2 = "verte pour retrouver"
line3 = "toute ta magie."
icon = 10
cancel = all

[_item_description_6_5]
# Potion bleue / Red potion
#       |--------------------|
line1 = "Bois de la Potion"
line2 = "bleue pour retrouver"
line3 = "vie et magie !"
icon = 11
cancel = all

[_item_description_6_6]
# Fée dans Flacon / Fairy in Bottle
#       |--------------------|
line1 = "Cette Fée te viendra"
line2 = "en aide si tu es en"
line3 = "difficulté."
icon = 12
cancel = all

[_item_description_6_6.1]
# Fée dans Flacon / Fairy in Bottle
#       |--------------------|
line1 = "Tu peux aussi"
line2 = "l'utiliser à tout"
line3 = "moment pour"
icon = 12
next = _item_description_6_6.2

[_item_description_6_6.2]
# Fée dans Flacon / Fairy in Bottle
#       |--------------------|
line1 = "récupérer des"
line2 = "Coeurs !"
icon = 12

[_item_description_7_1]
# Bottes de Pégase / Pegasus Shoes
#       |--------------------|
line1 = "Cours très vite et"
line2 = "fracasse des"
line3 = "obstacles grâce aux"
icon = 13
cancel = all
next = _item_description_7_1.1

[_item_description_7_1.1]
# Bottes de Pégase / Pegasus Shoes
#       |--------------------|
line1 = "Bottes de Pégase !"
icon = 13

[_item_description_8_1]
# Miroir Mystique / Mystic Mirror
#       |--------------------|
line1 = "Le Miroir Mystique"
line2 = "aurait de mystérieux"
line3 = "pouvoirs..."
icon = 14
cancel = all
next = _item_description_7_1.1

[_item_description_9_1]
# Canne de Somaria / Cane of Somaria
#       |--------------------|
line1 = "Crée des blocs et"
line2 = "accroche-toi à"
line3 = "l'aide du Grappin."
icon = 15
cancel = all
next = _item_description_9_1.1

[_item_description_9_1.1]
# Canne de Somaria / Cane of Somaria
#       |--------------------|
line1 = "Mais surveille ton"
line2 = "compteur magique !"
icon = 15

[_item_description_10_1]
# Cape magique / Magic Cape
#       |--------------------|
line1 = "La Cape Magique"
line2 = "t'avertit si tu"
line3 = "t'approches d'un"
icon = 16
cancel = all
next = _item_description_10_1.1

[_item_description_10_1.1]
# Cape magique / Magic Cape
#       |--------------------|
line1 = "mur fragile..."
icon = 16

[_item_description_11_1]
# Poigne de Fer / Iron Glove
#       |--------------------|
line1 = "La Poigne de Fer te"
line2 = "permet de soulever"
line3 = "les pierres."
icon = 17
cancel = all

[_item_description_11_2]
# Poigne d'Or / Golden Glove
#       |--------------------|
line1 = "Soulève les pierres"
line2 = "noires grâce à la"
line3 = "Poigne d'Or."
icon = 18
cancel = all

[_item_description_12_1]
# Pierres de Feu / Fire Stones
#       |--------------------|
line1 = "Trouve les trois"
line2 = "Pierres de Feu et"
line3 = "apporte-les"
icon = 19
cancel = all
next = _item_description_12_1.1

[_item_description_12_1.1]
# Pierres de Feu / Fire Stones
#       |--------------------|
line1 = "à Inferno."
icon = 19

[_item_description_14_1]
# Pommes / Apples
#       |--------------------|
line1 = "Mange des Pommes"
line2 = "pour récupérer des"
line3 = "Coeurs."
icon = 20
cancel = all

[_item_description_15_1]
# Pains au Chocolat / Pains au Chocolat
#       |--------------------|
line1 = "Mange un Pain au"
line2 = "Chocolat pour"
line3 = "récupéer 5 Coeurs."
icon = 21
cancel = all

[_item_description_16_1]
# Croissant / Croissant
#       |--------------------|
line1 = "Mange un Croissant"
line2 = "pour récupérer"
line3 = "10 Coeurs."
icon = 22
cancel = all

[_item_description_17_1]
# Tarte aux Pommes / Apple Pie
#       |--------------------|
line1 = "Cette Tarte aux"
line2 = "Pommes a un drôle"
line3 = "d'aspect..."
icon = 23
cancel = all

[_item_description_17_2]
# Lingots d'or / Golden bars
#       |--------------------|
line1 = "Ce sont trois"
line2 = "authentiques"
line3 = "lingots d'or !"
icon = 24
cancel = all

[_item_description_17_3]
# Edelweiss / Edelweiss
#       |--------------------|
line1 = "L'Edelweiss est une"
line2 = "fleur très rare dans"
line3 = "le royaume d'Hyrule."
icon = 25
cancel = all

[_item_description_17_4]
# Clé d'Os / Bone Key
#       |--------------------|
line1 = "A quoi peut servir"
line2 = "la Clé d'Os ?"
icon = 26
cancel = all

[_item_description_18_1]
# Palmes / Flippers
#       |--------------------|
line1 = "Les Palmes te"
line2 = "permettent de nager"
line3 = "dans l'eau profonde."
icon = 27
cancel = all

[_item_description_19_1]
# Clé Rouge / Red Key
#       |--------------------|
line1 = "Cette étrange Clé"
line2 = "est toute rouge."
icon = 28
cancel = all

[_item_description_20_1]
# TODO
#       |--------------------|
line1 = "???"
icon = 29
cancel = all

[_item_description_21_1]
# Clé de Terre / Clay Key
#       |--------------------|
line1 = "La Clé de Terre, qui"
line2 = "permet d'ouvrir la"
line3 = "grotte du village."
icon = 30
cancel = all

[_item_description_22_1]
# Clé de Roc / Rock Key
#       |--------------------|
line1 = "La Clé de Roc. Mais"
line2 = "une seule serrure"
line3 = "est la bonne..."
icon = 31
cancel = all

[_item_description_23_1]
# Clé de Fer / Iron Key
#       |--------------------|
line1 = "La Clé de Fer."
line2 = "Mais ne te trompe"
line3 = "pas de serrure !"
icon = 32
cancel = all

[_item_description_24_1]
# Clé de Pierre / Stone Key
#       |--------------------|
line1 = "La Clé de Pierre."
line2 = "Reste à trouver la"
line3 = "bonne serrure !"
icon = 33
cancel = all

[_item_description_25_1]
# Clé de Bois / Wooden Key
#       |--------------------|
line1 = "La Clé de Bois. Mais"
line2 = "où est donc la bonne"
line3 = "serrure ?"
icon = 34
cancel = all

[_item_description_26_1]
# Clé de Glace / Ice Key
#       |--------------------|
line1 = "La Clé de Glace."
line2 = "A toi de trouver un"
line3 = "lieu gelé pour la"
icon = 35
cancel = all
next = _item_description_26_1.1

[_item_description_26_1.1]
# Clé de Glace / Ice Key
#       |--------------------|
line1 = "faire fonctionner !"
icon = 35
cancel = all

[_item_description_27_1]
# TODO
#       |--------------------|
line1 = "???"
icon = 36
cancel = all

# --------------------
# Obtention d'un objet
# --------------------
# 
# Les messages suivants sont affichés lorsque le joueur obtient
# un objet, généralement en ouvrant un coffre ou en l'achetant
# dans un magasin.

[_got_item_0]
# Plume
#       |--------------------|
icon = 0
line1 = "Tu as trouvé la"
line2 = "Plume ! Utilise-la"
line3 = "pour sauter et"
next = _got_item_0.1

[_got_item_0.1]
# Plume
#       |--------------------|
icon = 0
line1 = "franchir des"
line2 = "obstacles !"

