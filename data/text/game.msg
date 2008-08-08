[msg]
#       |-----------------------|
line1 = "Voilà un message de"
line2 = "test,"
line3 = "le premier du jeu !"
next = test_icon

[test_icon]
#       |--------------------|
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
line1 = "Que voulez-vous faire ?"
line2 = "Retour au jeu"
line3 = "Relire"
question = 1
next = test_exit
next2 = msg

[test_exit]
line1 = "A bientôt"
