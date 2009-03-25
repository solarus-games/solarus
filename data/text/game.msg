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

[rupee_house.game_2.intro]
#       |-----------------------|
line1 = "Bienvenue au tout"
line2 = "nouveau jeu du"
line3 = "bandit manchot !"
next = rupee_house.game_2.intro.2

[rupee_house.game_2.intro.2]
#       |-----------------------|
line1 = "Mise 5 ou 20 rubis et"
line2 = "tente d'aligner trois"
line3 = "symboles identiques"
next = rupee_house.game_2.intro.3

[rupee_house.game_2.intro.3]
#       |-----------------------|
line1 = "pour gagner plusieurs"
line2 = "fois ta mise !"
line3 = ""
next = rupee_house.game_2.intro.4

[rupee_house.game_2.intro.4]
#       |-----------------------|
line1 = "Alors, tu veux jouer ?"
line2 = "Oui"
line3 = "Non"
question = true

[rupee_house.game_2.not_playing]
#       |-----------------------|
line1 = "Une autre fois"
line2 = "peut-être ?"

[rupee_house.game_2.choose_bet]
#       |-----------------------|
line1 = "Combien veux-tu miser ?"
line2 = "5 rubis"
line3 = "20 rubis"
question = true

[rupee_house.game_2.just_paid]
#       |-----------------------|
line1 = "Merci !"
next = rupee_house.game_2.playing

[rupee_house.game_2.playing]
#       |-----------------------|
line1 = "Approche-toi des"
line2 = "roulettes et appuie sur"
line3 = "la touche action"
next = rupee_house.game_2.playing.2

[rupee_house.game_2.playing.2]
#       |-----------------------|
line1 = "pour les arrêter."
line2 = "Bonne chance !"

[rupee_house.game_2.reward.same_any]
#       |-----------------------|
line1 = "Trois symboles identi-"
line2 = "ques, pas mal ! Tu"
line3 = "gagnes 4 fois ta mise."

[rupee_house.game_2.reward.green_rupees]
#       |-----------------------|
line1 = "Trois rubis verts,"
line2 = "joli ! Tu remportes"
line3 = "5 fois ta mise !"

[rupee_house.game_2.reward.blue_rupees]
#       |-----------------------|
line1 = "Trois rubis bleus !"
line2 = "Félicitations, tu"
line3 = "repars avec pas moins"
next = rupee_house.game_2.reward.blue_rupees.2

[rupee_house.game_2.reward.blue_rupees.2]
#       |-----------------------|
line1 = "de 7 fois ta mise !"

[rupee_house.game_2.reward.red_rupees]
#       |-----------------------|
line1 = "Trois rubis rouges !!!"
line2 = "Je suis très"
line3 = "impressionné !"
next = rupee_house.game_2.reward.red_rupees.2

[rupee_house.game_2.reward.red_rupees.2]
#       |-----------------------|
line1 = "Tu repars avec 10 fois"
line2 = "ta mise. Bravo !"

[rupee_house.game_2.reward.different_rupees]
#       |-----------------------|
line1 = "Whaou !!! Tu as obtenu"
line2 = "trois rubis de couleurs"
line3 = "différentes, c'est pas"
next = rupee_house.game_2.reward.different_rupees.2

[rupee_house.game_2.reward.reward.different_rupees.2]
#       |-----------------------|
line1 = "tous les jours qu'on"
line2 = "voit ça ! Tu remportes"
line3 = "15 fois ta mise."

[rupee_house.game_2.reward.yoshi]
#       |-----------------------|
line1 = "$1...$3Trois Yoshi ?!"
line2 = "C'est à peine"
line3 = "croyable ! C'est la"
next = rupee_house.game_2.reward.yoshi.2

[rupee_house.game_2.reward.yoshi.2]
#       |-----------------------|
line1 = "meilleure combinaison"
line2 = "possible... Tu gagnes"
line3 = "20 fois ta mise !"

[rupee_house.game_2.reward.none]
#       |-----------------------|
line1 = "Pas de chance, tu n'as"
line2 = "formé aucune"
line3 = "combinaison."
next = rupee_house.game_2.reward.none.2

[rupee_house.game_2.reward.none.2]
#       |-----------------------|
line1 = "Tu veux réessayer ?"
line2 = "Oui"
line3 = "Non"
question = true

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
line1 = "C'est toi qui vois !"

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

[rupee_house.pay_first]
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
