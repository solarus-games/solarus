The formats of the quest resource list file (project_db.dat) has changed in
Solarus 1.1.

There are also some changes in the Lua scripting API.
You need to update your scripts accordingly.
To know the exact list of incompatibilities in the Lua API.
see the ChangeLog or our website http://www.solarus-games.org.

Unlike scripts, data files can be upgraded automatically.
To upgrade your quest data files from Solarus 1.0 to
Solarus 1.1, first
backup all your Solarus 1.0 quest data files. Then, type:
lua update_quest.lua path/to/your_quest
(you need the Lua 5.1 interpreter).

Warning: always make a backup of your data files before upgrading them!

