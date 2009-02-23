[sign_village]
#       |-----------------------|
line1 = "       Bienvenue"
line2 = "     au Hameau Kogo"

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
line1 = "Alors, tu paies ?"
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
