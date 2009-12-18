Cette archive contient une version binaire de Zelda Mystery of Solarus DX (démo) pour Linux 32 bits.
Ce fichier détaille comment lancer le jeu. Notez que pour certaines distribution, nous fournissons des packages binaires.
Le code source est également disponible (sous licence GPL) si vous souhaitez compiler le programme vous-même.
Consultez http://www.zelda-solarus.com/jeu-zsdx pour plus de détails.


-----------
Dépendances
-----------

Les bibliothèques dynamiques suivantes sont nécessaires pour exécuter le jeu :

SDL
SDL_image
SDL_ttf
lua5.1
physfs
openal
libsndfile


-------------
Lancer le jeu
-------------

Une fois l'archive décompressée, pour lancer le jeu, lancez l'exécutable zsdx (en supposant que vous êtes placé dans son répertoire) :

$ ./zsdx

Le jeu compte sur la présence du fichier data.zsdx pour trouver ses données. Si data.zsdx n'est pas dans le
répertoire courant, vous devez préciser son emplacement à l'aide de l'option -datapath=<chemin>. Par exemple :

$ ./zsdx -datapath=le/chemin

où le/chemin désigne le répertoire contenant data.zsdx.


-----------------------
Problèmes de son connus
-----------------------

Vous pourriez avoir des problèmes de son, que j'ai rencontrés avec Ubuntu 9.04 (Jaunty) et que j'ai pu régler comme expliqué ci-dessous.
Editez le fichier de configuration de OpenAL (habituellement /etc/openal/alsoft.conf) et essayez les paramètres suivants :

- Si le son se joue avec un délai (typiquement, une fraction de seconde), tentez de réduire la valeur "refresh" à 2048
(la valeur par défaut est 8192 avec d'anciennes version de OpenAL).
- Si le son se joue de manière hachée avec des craquements dans les premières secondes (voire plus longtemps), essayez de forcer
le driver à OSS, en spécifiant dans la liste "driver" la valeur "oss" (sans les guillemets).

N'hésitez pas à partager votre expérience sur le forum, où vous pouvez aussi trouver de l'aide : http://forums.zelda-solarus.com

