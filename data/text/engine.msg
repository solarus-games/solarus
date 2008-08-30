# Ce fichier comporte l'ensemble des messages affichés
# par le moteur de jeu en version française.
# Les messages affichés par les maps sont quant à eux
# définis dans le fichier game.msg.
#
# La syntaxe est similaire à celle d'un fichier ini de Windows.
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
# temps normal). Elle reste affichée dans les messages suivants,
# jusqu'à ce que sa valeur soit changée ou que la boîte de dialogue
# se ferme. Une valeur -1 signifie d'enlever l'icône.
#
# - La valeur question : permet d'afficher une question avec deux
# réponses au choix. Les deux réponses sont affichées sur les deux
# dernières lignes. Ces deux lignes sont alors limitées à 20
# caractères (au lieu de 23 en temps normal) car un curseur en
# forme de flèche permet au joueur de choisir sa réponse. Il est
# possible d'avoir à la fois une icône et une question, auquel cas
# le nombre de caractères se limite à 17.
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
#       |-----------------------| sans icône ni question
#       |--------------------|    avec icône ou question
#       |-----------------|       avec icône et question

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

[_warning_magic]
#       |--------------------|
line1 = "Mais surveille ton"
line2 = "compteur magique !"

[_item_description_0_1]
# Plume / Feather
#       |--------------------|
icon = 0
line1 = "Utilise la Plume"
line2 = "pour sauter et fran-"
line3 = "chir des obstacles."
cancel = all

[_item_description_1_1]
# Bombes / Bombs
#       |--------------------|
icon = 1
line1 = "Utilise les Bombes"
line2 = "contre les ennemis"
line3 = "ou pour faire"
cancel = all
next = _item_description_1_1.1

[_item_description_1_1.1]
#       |--------------------|
line1 = "exploser des"
line2 = "obstacles."

[_item_description_2_1]
# Arc / Bow
#       |--------------------|
icon = 2
line1 = "Tu n'as plus de
line2 = "Flèches !"
cancel = all

[_item_description_2_2]
# Arc & Flèches / Bow & Arrows
#       |--------------------|
icon = 3
line1 = "Tire des Flèches"
line2 = "pour vaincre des"
line3 = "ennemis coriaces."
cancel = all

[_item_description_3_1]
# Boomerang
#       |--------------------|
icon = 4
line1 = "Utilise le Boomerang"
line2 = "pour immobiliser les"
line3 = "ennemis ou attraper"
cancel = all
next = _item_description_3_1.1

[_item_description_3_1.1]
#       |--------------------|
line1 = "des objets à"
line2 = "distance."

[_item_description_4_1]
# Lanterne / Lamp
#       |--------------------|
icon = 5
line1 = "Eclaire-toi dans le"
line2 = "noir ou allume des"
line3 = "braseros à l'aide"
cancel = all
next = _item_description_4_1.1

[_item_description_4_1.1]
#       |--------------------|
line1 = "de la Lanterne. Mais"
line2 = "surveille ton"
line3 = "compteur magique !"

[_item_description_5_1]
# Grappin / Hook Shot
#       |--------------------|
icon = 6
line1 = "Hisse-toi à des"
line2 = "endroits"
line3 = "inaccessibles grâce"
cancel = all
next = _item_description_5_1.1

[_item_description_5_1.1]
#       |--------------------|
line1 = "au Grappin."

[_item_description_6_1]
# Flacon Magique vide / Empty bottle
#       |--------------------|
icon = 7
line1 = "Le Flacon Magique"
line2 = "peut contenir di-"
line3 = "verses substances..."
cancel = all

[_item_description_6_2]
# Eau dans Flacon / Water in Bottle
#       |--------------------|
icon = 8
line1 = "Ton Flacon Magique"
line2 = "contient de l'eau."
cancel = all
next = _item_description_6_2.1

[_item_description_6_2.1]
#       |--------------------|
line1 = "Verse-la à un"
line2 = "endroit approprié..."

[_item_description_6_3]
# Potion rouge / Red potion
#       |--------------------|
icon = 9
line1 = "Bois de la Potion"
line2 = "rouge pour retrouver"
line3 = "tous tes Coeurs."
cancel = all

[_item_description_6_4]
# Potion verte / Red potion
#       |--------------------|
icon = 10
line1 = "Bois de la Potion"
line2 = "verte pour retrouver"
line3 = "toute ta magie."
cancel = all

[_item_description_6_5]
# Potion bleue / Red potion
#       |--------------------|
icon = 11
line1 = "Bois de la Potion"
line2 = "bleue pour retrouver"
line3 = "vie et magie !"
cancel = all

[_item_description_6_6]
# Fée dans Flacon / Fairy in Bottle
#       |--------------------|
icon = 12
line1 = "Cette Fée te viendra"
line2 = "en aide si tu es en"
line3 = "difficulté."
cancel = all

[_item_description_6_6.1]
#       |--------------------|
line1 = "Tu peux aussi"
line2 = "l'utiliser à tout"
line3 = "moment pour"
next = _item_description_6_6.2

[_item_description_6_6.2]
#       |--------------------|
line1 = "récupérer des"
line2 = "Coeurs !"

[_item_description_7_1]
# Bottes de Pégase / Pegasus Shoes
#       |--------------------|
icon = 13
line1 = "Cours très vite et"
line2 = "fracasse des"
line3 = "obstacles grâce aux"
cancel = all
next = _item_description_7_1.1

[_item_description_7_1.1]
#       |--------------------|
line1 = "Bottes de Pégase !"

[_item_description_8_1]
# Miroir Mystique / Mystic Mirror
#       |--------------------|
icon = 14
line1 = "Le Miroir Mystique"
line2 = "aurait de mystérieux"
line3 = "pouvoirs..."
cancel = all

[_item_description_9_1]
# Canne de Somaria / Cane of Somaria
#       |--------------------|
icon = 15
line1 = "Crée des blocs et"
line2 = "accroche-toi à"
line3 = "l'aide du Grappin."
cancel = all
next = _warning_magic

[_item_description_10_1]
# Cape magique / Magic Cape
#       |--------------------|
icon = 16
line1 = "La Cape Magique"
line2 = "t'avertit si tu"
line3 = "t'approches d'un"
cancel = all
next = _item_description_10_1.1

[_item_description_10_1.1]
#       |--------------------|
line1 = "mur fragile..."

[_item_description_11_1]
# Poigne de Fer / Iron Glove
#       |--------------------|
icon = 17
line1 = "La Poigne de Fer te"
line2 = "permet de soulever"
line3 = "les pierres."
cancel = all

[_item_description_11_2]
# Poigne d'Or / Golden Glove
#       |--------------------|
icon = 18
line1 = "Soulève les pierres"
line2 = "noires grâce à la"
line3 = "Poigne d'Or."
cancel = all

[_item_description_12_1]
# Pierres de Feu / Fire Stones
#       |--------------------|
icon = 19
line1 = "Trouve les trois"
line2 = "Pierres de Feu et"
line3 = "apporte-les"
cancel = all
next = _item_description_12_1.1

[_item_description_12_1.1]
#       |--------------------|
line1 = "à Inferno."

[_item_description_14_1]
# Pommes / Apples
#       |--------------------|
icon = 20
line1 = "Mange des Pommes"
line2 = "pour récupérer des"
line3 = "Coeurs."
cancel = all

[_item_description_15_1]
# Pains au Chocolat / Pains au Chocolat
#       |--------------------|
icon = 21
line1 = "Mange un Pain au"
line2 = "Chocolat pour"
line3 = "récupéer 3 Coeurs."
cancel = all

[_item_description_16_1]
# Croissant / Croissant
#       |--------------------|
icon = 22
line1 = "Mange un Croissant"
line2 = "pour récupérer"
line3 = "7 Coeurs."
cancel = all

[_item_description_17_1]
# Tarte aux Pommes / Apple Pie
#       |--------------------|
icon = 23
line1 = "Cette Tarte aux"
line2 = "Pommes a un drôle"
line3 = "d'aspect..."
cancel = all

[_item_description_17_2]
# Lingots d'or / Golden bars
#       |--------------------|
icon = 24
line1 = "Ce sont trois"
line2 = "authentiques"
line3 = "lingots d'or !"
cancel = all

[_item_description_17_3]
# Edelweiss / Edelweiss
#       |--------------------|
icon = 25
line1 = "L'Edelweiss est une"
line2 = "fleur très rare dans"
line3 = "le royaume d'Hyrule."
cancel = all

[_item_description_17_4]
# Clé d'Os / Bone Key
#       |--------------------|
icon = 26
line1 = "A quoi peut servir"
line2 = "la Clé d'Os ?"
cancel = all

[_item_description_18_1]
# Palmes / Flippers
#       |--------------------|
icon = 27
line1 = "Les Palmes te"
line2 = "permettent de nager"
line3 = "dans l'eau profonde."
cancel = all

[_item_description_19_1]
# Clé Rouge / Red Key
#       |--------------------|
icon = 28
line1 = "Cette étrange Clé"
line2 = "est toute rouge."
cancel = all

[_item_description_20_1]
# TODO
#       |--------------------|
icon = 29
line1 = "???"
cancel = all

[_item_description_21_1]
# Clé de Terre / Clay Key
#       |--------------------|
icon = 30
line1 = "La Clé de Terre, qui"
line2 = "permet d'ouvrir la"
line3 = "grotte du village."
cancel = all

[_item_description_22_1]
# Clé de Roc / Rock Key
#       |--------------------|
icon = 31
line1 = "La Clé de Roc. Mais"
line2 = "où est donc la bonne"
line3 = "serrure ?"
cancel = all

[_item_description_23_1]
# Clé de Fer / Iron Key
#       |--------------------|
icon = 32
line1 = "La Clé de Fer."
line2 = "Mais ne te trompe"
line3 = "pas de serrure !"
cancel = all

[_item_description_24_1]
# Clé de Pierre / Stone Key
#       |--------------------|
icon = 33
line1 = "La Clé de Pierre."
line2 = "Reste à trouver la"
line3 = "bonne serrure !"
cancel = all

[_item_description_25_1]
# Clé de Bois / Wooden Key
#       |--------------------|
icon = 34
line1 = "La Clé de Bois. Mais"
line2 = "une seule serrure"
line3 = "est la bonne..."
cancel = all

[_item_description_26_1]
# Clé de Glace / Ice Key
#       |--------------------|
icon = 35
line1 = "La Clé de Glace."
line2 = "A toi de trouver un"
line3 = "lieu gelé pour la"
cancel = all
next = _item_description_26_1.1

[_item_description_26_1.1]
#       |--------------------|
line1 = "faire fonctionner !"
icon = 35
cancel = all

[_item_description_27_1]
# TODO
#       |--------------------|
icon = 36
line1 = "???"
cancel = all

# ---------------------
# Obtention d'un trésor
# ---------------------
# 
# Les messages suivants sont affichés lorsque le joueur obtient
# un trésor, généralement en ouvrant un coffre ou en l'achetant
# dans un magasin.

[_treasure_0]
# Plume
#       |--------------------|
icon = 0
line1 = "Tu as trouvé la"
line2 = "Plume !$0 Utilise-la"
line3 = "pour sauter et"
next = _treasure_0.1

[_treasure_0.1]
#       |--------------------|
line1 = "franchir des"
line2 = "obstacles !"

[_treasure_2]
# Arc seul
#       |--------------------|
icon = 2
line1 = "Tu as trouvé l'Arc !$0"
line2 = "Trouve des Flèches"
line3 = "et tu pourras"
next = _treasure_2.1

[_treasure_2.1]
#       |--------------------|
line1 = "vaincre à distance"
line2 = "les ennemis"
line3 = "coriaces !"

[_treasure_3]
# Arc & Flèches
#       |--------------------|
icon = 3
line1 = "Tu as trouvé l'Arc !$0"
line2 = "Il est équipé de 10"
line3 = "Flèches. Tu peux"
next = _treasure_2.1

[_treasure_4]
# Boomerang
#       |--------------------|
icon = 4
line1 = "Tu as trouvé le"
line2 = "Boomerang !$0 Tu peux"
line3 = "immobiliser les"
next = _treasure_4.1

[_treasure_4.1]
#       |--------------------|
line1 = "ennemis ou attraper"
line2 = "des objets à"
line3 = "distance !"

[_treasure_5]
# Lanterne
#       |--------------------|
icon = 5
line1 = "Tu as trouvé la"
line2 = "Lanterne !$0 Elle"
line3 = "t'éclaire dans le"
next = _treasure_5.1

[_treasure_5.1]
#       |--------------------|
line1 = "noir et te permet"
line2 = "d'allumer des"
line3 = "braseros."
next = _warning_magic

[_treasure_6]
# Grappin
#       |--------------------|
icon = 6
line1 = "Tu as trouvé le"
line2 = "Grappin !$0 Immobilise"
line3 = "les ennemis et vole"
next = _treasure_6.1

[_treasure_6.1]
#       |--------------------|
line1 = "vers des endroits"
line2 = "inacessibles !"

[_treasure_7]
# Flacon vide
#       |--------------------|
icon = 7
line1 = "Tu as trouvé un"
line2 = "Flacon Magique !$0"
next = _treasure_7.1

[_treasure_7.1]
#       |--------------------|
line1 = "Remplis-le avec"
line2 = "diverses substances."

[_treasure_8]
# Eau
#       |--------------------|
icon = 8
line1 = "Tu as rempli ton"
line2 = "Flacon Magique avec"
line3 = "de l'eau !"

[_treasure_9]
# Potion rouge
#       |--------------------|
icon = 9
line1 = "Tu as trouvé de la"
line2 = "Potion Rouge !$0"
next = _treasure_9.1

[_treasure_9.1]
#       |--------------------|
line1 = "Bois-la pour"
line2 = "retrouver toute ton"
line3 = "énergie vitale."

[_treasure_10]
# Potion verte
#       |--------------------|
icon = 10
line1 = "Tu as trouvé de la"
line2 = "Potion Verte !$0"
next = _treasure_10.1

[_treasure_10.1]
#       |--------------------|
line1 = "Bois-la pour"
line2 = "retrouver toute ton"
line3 = "énergie magique."

[_treasure_11]
# Potion bleue
#       |--------------------|
icon = 11
line1 = "Tu as trouvé de la"
line2 = "Potion Bleue !$0"
next = _treasure_11.1

[_treasure_11.1]
#       |--------------------|
line1 = "Bois-la pour"
line2 = "retrouver toute ta"
line3 = "vie et ta magie !"

[_treasure_12]
# Fée dans Flacon
#       |--------------------|
icon = 12
line1 = "Tu as capturé une"
line2 = "Fée ! Elle te"
line3 = "viendra en aide si
next = _treasure_12.1
cancel = all

[_treasure_12.1]
#       |--------------------|
line1 = "tu es en difficulté."
line2 = "Tu peux aussi"
line3 = "l'utiliser à tout"
next = _treasure_12.2

[_treasure_12.2]
#       |--------------------|
line1 = "moment pour"
line2 = "récupérer des"
line3 = "Coeurs."

[_treasure_13]
# Bottes de Pégase
#       |--------------------|
icon = 13
line1 = "Tu as trouvé les"
line2 = "Bottes de Pégase !$0"
line3 = "Cours, fonce et"
next = _treasure_13.1

[_treasure_13.1]
#       |--------------------|
line1 = "fracasse les"
line2 = "obstacles !"

[_treasure_14]
# Miroir Mystique
#       |--------------------|
icon = 14
line1 = "Tu as trouvé le"
line2 = "Miroir Mystique !$0"
line3 = "Ses étranges"
next = _treasure_14.1

[_treasure_14.1]
#       |--------------------|
line1 = "pouvoirs t'aideront"
line2 = "sans doute dans ta"
line3 = "quête..."

[_treasure_15]
# Canne de Somaria
#       |--------------------|
icon = 15
line1 = "Tu as trouvé la"
line2 = "Canne de Somaria !$0"
line3 = "Tu peux créer des"
next = _treasure_15.1

[_treasure_15.1]
#       |--------------------|
line1 = "blocs et t'accrocher"
line2 = "à l'aide du Grappin."
next = _warning_magic

[_treasure_16]
# Cape Magique
#       |--------------------|
icon = 16
line1 = "Tu as trouvé la Cape"
line2 = "Magique !$0"
line3 = "Elle t'avertira si"
next = _treasure_16.1

[_treasure_16.1]
#       |--------------------|
line1 = "tu t'approchees d'un"
line2 = "mur fragile."

[_treasure_17]
# Poigne de Fer
#       |--------------------|
icon = 17
line1 = "Tu as trouvé la"
line2 = "Poigne de Fer !$0"
line3 = "Elle te permet de"
next = _treasure_17.1

[_treasure_17.1]
#       |--------------------|
line1 = "soulever les"
line2 = "pierres."

[_treasure_18]
# Poigne d'Or
#       |--------------------|
icon = 18
line1 = "Tu as trouvé la"
line2 = "Poigne d'Or !$0"
line3 = "Tu peux maintenant"
next = _treasure_18.1

[_treasure_18.1]
#       |--------------------|
line1 = "soulever les pierres"
line2 = "noires !"

[_treasure_19]
# Pierre de Feu
#       |--------------------|
icon = 19
line1 = "Tu as trouvé une"
line2 = "Pierre de Feu !"

[_treasure_20_1]
# 1 Pomme
#       |--------------------|
icon = 20
line1 = "Tu as trouvé une"
line2 = "Pomme !$0"
line3 = "Consomme-la plus"
next = _treasure_20_1.1

[_treasure_20_1.1]
#       |--------------------|
line1 = "tard pour récupérer"
line2 = "un Coeur !"

[_treasure_20_2]
# Plusieurs Pommes
#       |--------------------|
icon = 20
line1 = "Tu as trouvé $v"
line2 = "Pommes !$0"
line3 = "Consomme-les plus"
next = _treasure_20_2.1

[_treasure_20_2.1]
#       |--------------------|
line1 = "tard pour récupérer"
line2 = "des Coeurs !"

[_treasure_21_1]
# 1 Pain au Chocolat
#       |--------------------|
icon = 21
line1 = "Tu as trouvé un Pain"
line2 = "au Chocolat !$0"
next = _treasure_21_1.1

[_treasure_21_1.1]
#       |--------------------|
line1 = "Mange un Pain au"
line2 = "Chocolat pour"
line3 = "récupérer 3 Coeurs."

[_treasure_21_2]
# Plusieurs Pains au Chocolat
#       |--------------------|
icon = 21
line1 = "Tu as trouvé $v"
line2 = "Pains au Chocolat !$0"
next = _treasure_21_1.1

[_treasure_22_1]
# 1 Croissant
#       |--------------------|
icon = 22
line1 = "Tu as trouvé un"
line2 = "Croissant !$0 Il est"
line3 = "croustillant !
next = _treasure_22_1.1

[_treasure_22_1.1]
#       |--------------------|
line1 = "Mange un Croissant"
line2 = "pour récupérer"
line3 = "récupérer 7 Coeurs."

[_treasure_22_2]
# Plusieurs Croissants
#       |--------------------|
icon = 22
line1 = "Tu as trouvé $v"
line2 = "Croissants !$0"
next = _treasure_22_1.1

[_treasure_23]
# Tarte aux Pommes
#       |--------------------|
icon = 23
line1 = "Tu as trouvé une"
line2 = "Tarte aux Pommes !$0"
line3 = "$1...$3 Hmmm, à ta"
next = _treasure_23.1

[_treasure_23.1]
#       |--------------------|
line1 = "place, je n'y goûte-"
line2 = "rais pas, elle a un"
line3 = "drôle d'aspect..."

[_treasure_24]
# Lingots d'or
#       |--------------------|
icon = 24
line1 = "Tu as trouvé des"
line2 = "Lingots d'or !!!$0"
line3 = "Ils brillent et ont"
next = _treasure_24.1

[_treasure_24.1]
#       |--------------------|
line1 = "une valeur"
line2 = "inestimable !"

[_treasure_25]
# Edelweiss
#       |--------------------|
icon = 25
line1 = "Tu as trouvé un"
line2 = "Edelweiss !$0"
line3 = "Cette jolie fleur"
next = _treasure_25.1

[_treasure_25.1]
#       |--------------------|
line1 = "des montagnes est"
line2 = "rarissime."

[_treasure_26]
# Clé d'os
#       |--------------------|
icon = 26
line1 = "Tu as trouvé la"
line2 = "Clé d'Os !$0"
next = _treasure_26.1

[_treasure_26.1]
#       |--------------------|
line1 = "Mais à quoi peut-"
line2 = "elle bien servir ?"

[_treasure_27]
# Palmes
#       |--------------------|
icon = 27
line1 = "Tu as trouvé les"
line2 = "Palmes !$0"
line3 = "Tu peux maintenant"
next = _treasure_27.1

[_treasure_27.1]
#       |--------------------|
line1 = "nager en eau"
line2 = "profonde !"

[_treasure_28]
# Clé Rouge
#       |--------------------|
icon = 28
line1 = "Tu as trouvé la"
line2 = "Clé Rouge !$0"

[_treasure_30]
# Clé de Terre
#       |--------------------|
icon = 30
line1 = "Tu as trouvé la"
line2 = "Clé de Terre !$0"
next = _treasure_30.1

[_treasure_30.1]
#       |--------------------|
line1 = "Tu peux maintenant"
line2 = "ouvrir la grotte du"
line3 = "village !"

[_treasure_31]
# Clé de Roc
#       |--------------------|
icon = 31
line1 = "Tu as trouvé la"
line2 = "Clé de Roc !$0"

[_treasure_32]
# Clé de Fer
#       |--------------------|
icon = 32
line1 = "Tu as trouvé la"
line2 = "Clé de Fer !$0 Il doit"
line3 = "bien y avoir une"
next = _treasure_32.1

[_treasure_32.1]
#       |--------------------|
line1 = "porte qui va avec !"

[_treasure_33]
# Clé de Pierre
#       |--------------------|
icon = 33
line1 = "Tu as trouvé la"
line2 = "Clé de Pierre !$0"
line3 = "Mais où est la"
next = _treasure_33.1

[_treasure_33.1]
#       |--------------------|
line1 = "serrure qui"
line2 = "correspond ?"

[_treasure_34]
# Clé de Bois
#       |--------------------|
icon = 34
line1 = "Tu as trouvé la"
line2 = "Clé de Bois !$0"
line3 = "A toi de voir où"
next = _treasure_34.1

[_treasure_34.1]
#       |--------------------|
line1 = "tu peux l'utiliser !"

[_treasure_35]
# Clé de Glace
#       |--------------------|
icon = 35
line1 = "Tu as trouvé la"
line2 = "Clé de Glace !$0"
line3 = "Elle protège sans"
next = _treasure_35.1

[_treasure_35.1]
#       |--------------------|
line1 = "doute l'entrée d'un"
line2 = "lieu glacial..."

[_treasure_50]
# Carte du Monde
#       |--------------------|
icon = 50
line1 = "Tu as trouvé la"
line2 = "Carte du Monde !$0"
line3 = "Tu peux l'afficher"
next = _treasure_50.1

[_treasure_50.1]
#       |--------------------|
line1 = "depuis l'écran"
line2 = "Statut Quête."

[_treasure_52]
# Grand sac de rubis
#       |--------------------|
icon = 52
line1 = "Tu as trouvé le"
line2 = "Grand Sac de Rubis !$0"
line3 = "Il peut contenir"
next = _treasure_55.1

[_treasure_52.1]
#       |--------------------|
line1 = "jusqu'à 199 rubis !"

[_treasure_53]
# Sac de rubis géant
#       |--------------------|
icon = 53
line1 = "Tu as trouvé le"
line2 = "Sac de Rubis Géant !$0"
line3 = "Il peut contenir"
next = _treasure_53.1

[_treasure_53.1]
#       |--------------------|
line1 = "jusqu'à 999 rubis !"

[_treasure_54]
# Petit sac de bombes
#       |--------------------|
icon = 54
line1 = "Tu as trouvé le"
line2 = "Sac de Bombes !$0"
line3 = "Il contient"
next = _treasure_55.1

[_treasure_54.1]
#       |--------------------|
line1 = "10 Bombes !"

[_treasure_55]
# Grand sac de bombes
#       |--------------------|
icon = 55
line1 = "Tu as trouvé le"
line2 = "Grand Sac de Bombes !$0"
line3 = "Il peut contenir"
next = _treasure_55.1

[_treasure_55.1]
#       |--------------------|
line1 = "jusqu'à 30 bombes au"
line2 = "lieu de 10 !"

[_treasure_56]
# Sac de bombes géant
#       |--------------------|
icon = 56
line1 = "Tu as trouvé le Sac"
line2 = "de Bombes Géant !$0"
line3 = "Il peut contenir"
next = _treasure_56.1

[_treasure_56.1]
#       |--------------------|
line1 = "jusqu'à 100 Bombes"
line2 = "au lieu de 30 !"

[_treasure_54]
# Petit sac de bombes
#       |--------------------|
icon = 54
line1 = "Tu as trouvé le Sac"
line2 = "de Bombes !$0 Il"
line3 = "contient 10 Bombes."
next = _treasure_55.1

[_treasure_54.1]
#       |--------------------|
line1 = "Assigne-les à une"
line2 = "touche et fais tout"
line3 = "sauter !"

[_treasure_55]
# Grand sac de bombes
#       |--------------------|
icon = 55
line1 = "Tu as trouvé le"
line2 = "Grand Sac de Bombes !$0"
line3 = "Il peut contenir"
next = _treasure_55.1

[_treasure_55.1]
#       |--------------------|
line1 = "jusqu'à 30 Bombes au"
line2 = "lieu de 10 !"

[_treasure_56]
# Sac de bombes géant
#       |--------------------|
icon = 56
line1 = "Tu as trouvé le Sac"
line2 = "de Bombes Géant !$0"
line3 = "Il peut contenir"
next = _treasure_56.1

[_treasure_56.1]
#       |--------------------|
line1 = "jusqu'à 100 Bombes"
line2 = "au lieu de 30 !"

[_treasure_58]
# Grand carquois 
#       |--------------------|
icon = 58
line1 = "Tu as trouvé le"
line2 = "Grand Carquois !$0"
line3 = "Il peut contenir"
next = _treasure_58.1

[_treasure_58.1]
#       |--------------------|
line1 = "jusqu'à 30 Flèches au"
line2 = "lieu de 10 !"

[_treasure_59]
# Carquois géant
#       |--------------------|
icon = 59
line1 = "Tu as trouvé le"
line2 = "Carquois Géant !$0"
line3 = "Il peut contenir"
next = _treasure_56.1

[_treasure_59.1]
#       |--------------------|
line1 = "jusqu'à 100 Flèches"
line2 = "au lieu de 30 !"

[_treasure_61]
# Cotte de mailles bleue
#       |--------------------|
icon = 61
line1 = "Tu as trouvé la"
line2 = "Cotte de Mailles"
line3 = "bleue !$0 Elle te"
next = _treasure_61.1

[_treasure_61.1]
#       |--------------------|
line1 = "permet de résister"
line2 = "aux températures les"
line3 = "plus glaciales..."

[_treasure_62]
# Cotte de mailles rouge
#       |--------------------|
icon = 62
line1 = "Tu as trouvé la"
line2 = "Cotte de Mailles"
line3 = "rouge !$0 Elle te"
next = _treasure_62.1

[_treasure_62.1]
#       |--------------------|
line1 = "protège de manière"
line2 = "plus efficace contre"
line3 = "les ennemis !"

[_treasure_63]
# Bouclier
#       |--------------------|
icon = 63
line1 = "Tu as trouvé le"
line2 = "Bouclier !$0"
line3 = "Tu peux repousser"
next = _treasure_63.1

[_treasure_63.1]
#       |--------------------|
line1 = "les attaques des"
line2 = "ennemis !"

[_treasure_64]
# Bouclier de Feu
#       |--------------------|
icon = 64
line1 = "Tu as trouvé le"
line2 = "Bouclier de Feu !$0"
line3 = "Tu es désormais"
next = _treasure_64.1

[_treasure_64.1]
#       |--------------------|
line1 = "protégé contre les"
line2 = "attaques"
line3 = "enflammées !"

[_treasure_65]
# Bouclier Miroir
#       |--------------------|
icon = 65
line1 = "Tu as trouvé le"
line2 = "Bouclier Miroir !$0"
line3 = "Tu peux désormais"
next = _treasure_65.1

[_treasure_65.1]
#       |--------------------|
line1 = "contrer les lasers !"

[_treasure_66]
# Epée 1 (Epée du courage)
#       |--------------------|
icon = 66
line1 = "Tu as trouvé l'Epée"
line2 = "du Courage!$0 Tu peux"
line3 = "maintenant croiser"
next = _treasure_66.1

[_treasure_66.1]
#       |--------------------|
line1 = "le fer et affronter"
line2 = "les ennemis !"

[_treasure_67]
# Epée 2 (Glaive Hylien)
#       |--------------------|
icon = 67
line1 = "Tu as trouvé"
line2 = "le Glaive Hylien !$0"
line3 = "Cette nouvelle épée"
next = _treasure_67.1

[_treasure_67.1]
#       |--------------------|
line1 = "est deux fois plus"
line2 = "puissante que"
line3 = "l'ancienne !"

[_treasure_68]
# Epée 3 (Sabre du Héros)
#       |--------------------|
icon = 68
line1 = "Tu as trouvé"
line2 = "le Sabre du Héros !"
line3 = "Cette nouvelle lame"
next = _treasure_67.1

[_treasure_69]
# Epée 4 (Lame Purificatrice)
#       |--------------------|
icon = 69
line1 = "Tu as trouvé la Lame"
line2 = "Purificatrice !"
line3 = "La puissance de"
next = _treasure_69.1

[_treasure_69.1]
#       |--------------------|
line1 = "cette nouvelle épée"
line2 = "dépasse"
line3 = "l'entendement..."

[_treasure_70]
# Carte
#       |--------------------|
icon = 70
line1 = "Tu as trouvé"
line2 = "la Carte !"
line3 = "Utilise-la pour"
next = _treasure_70.1

[_treasure_70.1]
#       |--------------------|
line1 = "connaître le plan"
line2 = "du donjon."

[_treasure_71]
# Boussole
#       |--------------------|
icon = 71
line1 = "Tu as trouvé"
line2 = "la Boussole ! Elle"
line3 = "indique ta position"
next = _treasure_71.1

[_treasure_71.1]
#       |--------------------|
line1 = "sur la Carte, les"
line2 = "emplacements des"
line3 = "coffres et du Boss."
next = _treasure_71.2

[_treasure_71.2]
#       |--------------------|
line1 = "C'est ça"
line2 = "le progrès !"

[_treasure_72]
# Petite clé
#       |--------------------|
icon = 72
line1 = "Tu as trouvé une"
line2 = "Petite Clé !"
next = _treasure_72.1

[_treasure_72.1]
#       |--------------------|
line1 = "Tu peux ouvrir une"
line2 = "porte verrouillée"
line3 = "de ce donjon."

[_treasure_73]
# Grande Clé
#       |--------------------|
icon = 73
line1 = "Tu as trouvé la"
line2 = "Grande Clé ! Tu peux"
line3 = "désormais ouvrir les"
next = _treasure_73.1

[_treasure_73.1]
#       |--------------------|
line1 = "portes spéciales et"
line2 = "découvrir de"
line3 = "nouveaux trésors !"

[_treasure_74]
# Clé du Maître
#       |--------------------|
icon = 74
line1 = "Tu as trouvé la"
line2 = "Clé du Maître ! Tu"
line3 = "peux maintenant"
next = _treasure_74.1

[_treasure_74.1]
#       |--------------------|
line1 = "accéder au repaire"
line2 = "du maître des"
line3 = "lieux !"

[_treasure_79_1]
# 1er Fragment de Coeur
#       |--------------------|
icon = 79
line1 = "Tu as trouvé un"
line2 = "Fragment de Coeur !"
next = _treasure_79_1.1

[_treasure_79_1.1]
#       |--------------------|
icon = 75
line1 = "Collecte-en quatre"
line2 = "pour former un"
line3 = "nouveau Coeur."

[_treasure_79_2]
# 2e Fragment de Coeur
#       |--------------------|
icon = 79
line1 = "Tu as trouvé un"
line2 = "Fragment de Coeur !"
next = _treasure_79_2.1

[_treasure_79_2.1]
#       |--------------------|
icon = 76
line1 = "Encore deux et"
line2 = "tu obtiendras"
line3 = "un nouveau Coeur."

[_treasure_79_3]
# 3e Fragment de Coeur
#       |--------------------|
icon = 79
line1 = "Tu as trouvé un"
line2 = "Fragment de Coeur !"
next = _treasure_79_3.1

[_treasure_79_3.1]
#       |--------------------|
icon = 77
line1 = "Plus qu'un à trouver"
line2 = "pour former un"
line3 = "nouveau Coeur !"

[_treasure_79_4]
# 4e Fragment de Coeur
#       |--------------------|
icon = 79
line1 = "Tu as trouvé un"
line2 = "Fragment de Coeur !"
next = _treasure_79_4.1

[_treasure_79_4.1]
#       |--------------------|
icon = 78
line1 = "Tu as trouvé quatre"
line2 = "Fragments de Coeur."
next = _treasure_79_4.2

[_treasure_79_4.2]
#       |--------------------|
line1 = "Tu obtiens un Coeur"
line2 = "supplémentaire !"

[_treasure_80]
# Réceptable de Coeur
#       |--------------------|
icon = 80
line1 = "Tu as trouvé un"
line2 = "Réceptacle de Coeur !"
next = _treasure_79_4.1

[_treasure_82_1]
# 1 Bombe
#       |--------------------|
icon = 82
line1 = "Tu as trouvé une"
line2 = "Bombe !$0"
line3 = "Utilise une Bombe"
next = _treasure_82_1.1

[_treasure_82_1.1]
#       |--------------------|
line1 = "pour faire exploser"
line2 = "un ennemi ou un"
line3 = "obstacle."

[_treasure_82_2]
# Plusieurs Bombes
#       |--------------------|
icon = 82
line1 = "Tu as trouvé $v"
line2 = "Bombes !$0"
line3 = "Utilise une Bombe"
next = _treasure_82_1.1

[_treasure_83_1]
# 1 Flèche
#       |--------------------|
icon = 83
line1 = "Tu as trouvé une"
line2 = "Flèche !$0"
line3 = "Utilise l'Arc et les"
next = _treasure_83_1.1

[_treasure_83_1.1]
#       |--------------------|
line1 = "Flèches pour vaincre"
line2 = "des ennemis coriaces"
line3 = "à distance."

[_treasure_83_2]
# Plusieurs Flèches
#       |--------------------|
icon = 83
line1 = "Tu as trouvé $v"
line2 = "Flèches !$0"
line3 = "Utilise l'Arc et les"
next = _treasure_83_1.1

[_treasure_84_1]
# 1 Coeur
#       |--------------------|
icon = 84
line1 = "Tu as trouvé un"
line2 = "Coeur !"

[_treasure_84_2]
# Plusieurs Coeurs
#       |--------------------|
icon = 84
line1 = "Tu as trouvé $v"
line2 = "Coeurs !"

[_treasure_85]
# Petite bouteille de magie
#       |--------------------|
icon = 85
line1 = "Tu as trouvé une"
line2 = "petite bouteille"
line3 = "d'énergie magique !"

[_treasure_86]
# Grande bouteille de magie
#       |--------------------|
icon = 86
line1 = "Tu as trouvé une"
line2 = "grande bouteille"
line3 = "d'énergie magique !"

[_treasure_87_1]
# 1 rubis vert
#       |--------------------|
icon = 90
line1 = "Tu as trouvé un"
line2 = "Rubis !"

[_treasure_87_2]
# Plusieurs rubis verts
#       |--------------------|
icon = 90
line1 = "Tu as trouvé $v"
line2 = "Rubis !"

[_treasure_88_1]
# 1 rubis bleu
#       |--------------------|
icon = 91
line1 = "Tu as trouvé un"
line2 = "Rubis bleu.
line3 = "Il vaut 5 Rubis !"

[_treasure_88_2]
# Plusieurs rubis bleus
#       |--------------------|
icon = 91
line1 = "Tu as trouvé $v"
line2 = "Rubis bleus ! Chacun
line3 = "d'eux vaut 5 Rubis !"

[_treasure_89_1]
# 1 rubis rouge
#       |--------------------|
icon = 92
line1 = "Tu as trouvé un"
line2 = "Rubis rouge !
line3 = "Il vaut 20 Rubis !"

[_treasure_88_2]
# Plusieurs rubis bleus
#       |--------------------|
icon = 91
line1 = "Tu as trouvé $v"
line2 = "Rubis rouges."
line3 = "Chacun d'entre eux"
next = _treasure_88_2.1

[_treasure_88_2.1]
#       |--------------------|
line1 = "vaut 20 Rubis !"
