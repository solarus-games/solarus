[sign_village]
#       |-----------------------|
line1 = "       Bienvenue"
line2 = "       à Lyriann"

[rupee_house.game_1.intro]
#       |-----------------------|
line1 = "Bienvenue au jeu du"
line2 = "coffre-mystère. Paie 20"
line3 = "rubis pour ouvrir l'un"
next = rupee_house.game_1.intro.2

[rupee_house.game_1.intro.2]
#       |-----------------------|
line1 = "des trois coffres et"
line2 = "tenter de remporter"
line3 = "plus de rubis !"
next = rupee_house.game_1.intro.3

[rupee_house.game_1.intro.3]
#       |-----------------------|
line1 = "Alors, tu veux jouer ?"
line2 = "Oui"
line3 = "Non"
question = true

[rupee_house.game_1.choose_chest]
#       |-----------------------|
line1 = "Va choisir l'un des"
line2 = "trois coffres !"
next = rupee_house.game_1.choose_chest.2

[rupee_house.game_1.choose_chest.2]
#       |-----------------------|
line1 = "Mais ne compte pas sur"
line2 = "moi pour t'aider !"

[rupee_house.game_1.pay_first]
#       |-----------------------|
line1 = "Hé ! Ici il faut payer"
line2 = "avant de jouer !"

[rupee_house.game_1.good_luck]
#       |-----------------------|
line1 = "Va choisir un coffre"
line2 = "et bonne chance !"

[rupee_house.game_1.not_allowed_to_play]
#       |-----------------------|
line1 = "Tu as déjà assez gagné"
line2 = "non ? Nous sommes"
line3 = "ruinés à cause de toi !"

[rupee_house.game_1.play_again_question]
#       |-----------------------|
line1 = "Tu veux retenter ta"
line2 = "chance ?"
next = rupee_house.game_1.play_again_question.2

[rupee_house.game_1.play_again_question.2]
#       |-----------------------|
line1 = "C'est 20 rubis !"
line2 = "Oui"
line3 = "Non"
question = true

[rupee_house.game_1.not_playing]
#       |-----------------------|
line1 = "Vis ta vie ! Sois"
line2 = "un peu plus motivé !"

[rupee_house.game_3.intro]
#       |-----------------------|
line1 = "Le but du jeu est de"
line2 = "traverser le labyrinthe"
line3 = "dans un temps limité"
next = rupee_house.game_3.intro.2

[rupee_house.game_3.intro.2]
#       |-----------------------|
line1 = "puis de pousser les"
line2 = "blocs pour te frayer un"
line3 = "chemin. Cela ne te"
next = rupee_house.game_3.intro.3

[rupee_house.game_3.intro.3]
#       |-----------------------|
line1 = "coûtera que 10 rubis"
line2 = "et il y a un Fragment"
line3 = "de Coeur à gagner !"
next = rupee_house.game_3.intro.4

[rupee_house.game_3.intro.4]
#       |-----------------------|
line1 = "Alors, tu veux jouer ?"
line2 = "Oui"
line3 = "Non"
question = true

[rupee_house.game_3.go]
#       |-----------------------|
line1 = "Et c'est parti !"
line2 = "Bonne chance !"

[rupee_house.game_3.not_playing]
#       |-----------------------|
line1 = "Tant pis ! Reviens une"
line2 = "autre fois !"

[rupee_house.game_3.not_allowed_to_play]
#       |-----------------------|
line1 = "Désolé, tu as déjà"
line2 = "gagné à ce jeu !"

[rupee_house.game_3.restart_question]
#       |-----------------------|
line1 = "Tu veux recommencer à"
line2 = "zéro ?"
next = rupee_house.game_3.restart_question.2

[rupee_house.game_3.restart_question.2]
#       |-----------------------|
line1 = "C'est 10 rubis !"
line2 = "Oui"
line3 = "Non"
question = true

[rupee_house.not_enough_money]
#       |-----------------------|
line1 = "Tu n'as pas assez"
line2 = "d'argent !"



[test]
#       |-----------------------|
line1 = "Bienvenue à la Maison"
line2 = "des Rubis."

[msg]
#       |-----------------------|
icon = -1
line1 = "Voilà un message de"
line2 = "test,"
line3 = "le premier du jeu !"
next = test_icon

[test_icon]
#       |--------------------|
icon = 1
line1 = "Message avec icône !"
next  = test_speed

[test_speed]
line1 = "$3Message annulable ;"
line2 = "$1très lent ;"
line3 = "$2un peu moins lent$3"
next = test_var
cancel = current

[test_var]
line1 = "La variable est"
line3 = "égale à $v !"
next = test_cancel
#cancel = none

[test_cancel]
line1 = "Désormais tout peut"
line2 = "être annulé !"
next = test_pause
cancel = all

[test_pause]
line1 = "Besoin d'une petite"
line2 = "pause ?$0 Et on"
line3 = "continue."
next = test_name

[test_name]
line1 = "Votre nom est :"
line2 = "$v"
next = test_question

[test_question]
#       |--------------------|
line1 = "Que désirez-vous ?"
line2 = "Retour au jeu"
line3 = "Relire"
question = true
next = test_exit
next2 = msg

[test_exit]
line1 = "A bientôt"
