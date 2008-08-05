[msg]
#    |-----------------------|
line1 = "Voilà un message de"
line2 = "test,"
line3 = "le premier du jeu !"
next = "test1"

[test1]
line1 = "$3Message rapide ;"
line2 = "$1très lent ;"
line3 = "$2un peu moins lent"
next = test2

[test2]
line1 = "La variable est"
line3 = "égale à $v !"
next = test3

[test3]
line1 = "Votre nom est :"
line2 = "$v"
next = test4

[test4]
line1 = "Que voulez-vous faire ?"
line2 = "Relire"
line3 = "Quitter"
question = 1
next = test1
next2 = test5

[test5]
line1 = "A bientôt"
