This package contains the data files of the quest Zelda: Mystery of Solarus XD.
This quest is a free, open-source game that works with Solarus,
an open-source Zelda-like 2D game engine.
To play this game, you need Solarus.
We assume in this readme that Solarus is already installed.

See our development blog (http://www.solarus-engine.org) to get more
information and documentation about Solarus and our games.


--------
Contents
--------

1  Play directly
2  Installation instructions
  2.1  Default settings
  2.2  Change the install directory


----------------
1  Play directly
----------------

You need to specify to the solarus binary the path of the quest data files to
use.
solarus accepts two forms of quest paths:
- a directory having a subdirectory named "data" with all data inside,
- a directory having an archive "data.solarus" with all data inside.

Thus, to run zsxd, if the current directory is the one that
contains the "data" subdirectory (and this readme), you can type

$ solarus .

or without arguments:

$ solarus

if solarus was compiled with the default quest set to ".".


--------------------
2  Install the quest
--------------------


2.1  Default settings
----------------------

If you want to install zsxd, cmake and zip are recommended.
Just type

$ cmake .
$ make

This generates the "data.solarus" archive that contains all data files
of the quest. You can then install it with

# make install

This installs the following files (assuming that the install directory
is /usr/local):
- the quest data archive ("data.solarus") in /usr/local/share/solarus/zsxd/
- a script called "zsxd" in /usr/local/bin/

The zsxd script launches solarus with the appropriate command-line argument
to specify the quest path.
This means that you can launch the zsxd quest with the command:

$ zsxd

which is equivalent to:

$ solarus /usr/local/share/solarus/zsxd


3.2  Change the install directory
---------------------------------

You may want to install zsxd in another directory
(e.g. so that no root access is necessary). You can specify this directory
as a parameter of cmake:

$ cmake -D CMAKE_INSTALL_PREFIX=/home/your_directory .
$ make
$ make install

This installs the files described above, with the
/usr/local prefix replaced by the one you specified.
The script generated runs solarus with the appropriate quest path.

