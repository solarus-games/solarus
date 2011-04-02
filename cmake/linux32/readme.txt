Cette archive contient une version binaire de Zelda Mystery of Solarus XD pour Linux 32 bits.
Ce fichier détaille comment lancer le jeu. Notez que pour certaines distributions, nous fournissons des packages binaires.
Le code source est également disponible (sous licence GPL) si vous souhaitez compiler le programme vous-même.
Consultez http://www.zelda-solarus.com/jeu-zsxd pour plus de détails.


-----------
Dépendances
-----------

Les bibliothèques dynamiques suivantes sont nécessaires pour exécuter le jeu :

SDL
SDL_image
SDL_ttf
openal
vorbisfile
modplug
lua5.1
physfs


-------------
Lancer le jeu
-------------

Une fois l'archive décompressée, pour démarrer le jeu, lancez simplement l'exécutable solarus
(en supposant que vous êtes placé dans son répertoire) :

$ ./solarus

L'exécutable compte sur la présence du fichier data.solarus pour trouver les données. Si data.solarus n'est pas dans le
répertoire courant, vous devez préciser son emplacement à l'aide de l'option -datapath=<chemin>. Par exemple :

$ ./solarus -datapath=le/chemin

où le/chemin désigne le répertoire contenant data.solarus.

