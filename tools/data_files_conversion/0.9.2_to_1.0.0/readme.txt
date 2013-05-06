The formats of maps, tilesets and of the font list file have changed in
Solarus 1.0.0.

The Lua scripting API has completely changed. Scripts written with the old API
cannot be upgraded automatically, sorry.
The new API is cleaner, much more powerful and very well documented:
http://www.solarus-games.org/doc/1.0.0/lua_api.html

To upgrade your quest data files (except Lua scripts) from Solarus 0.9.2 to
Solarus 1.0.0, first
backup all your Solarus 0.9.2 quest data files. Then, type:
lua update_quest.lua path/to/your_quest
(you need the Lua 5.1 interpreter).

Warning: this converter is experimental!
The format of Solarus 1.0.0 quest data files is not stabilized yet,
so you should really keep your Solarus 0.9.2 files before you run this
upgrade.

