local game = ...

-- Include the dungeon features.
sol.main.load_file("dungeons")(game)
sol.main.load_file("equipment")(game)
sol.main.load_file("menus/pause")(game)
sol.main.load_file("menus/dialog_box")(game)
sol.main.load_file("hud/hud")(game)

-- Useful functions for this specific quest.

function game:on_started()

  -- Set up the dialog box and the HUD.
  self:initialize_dialog_box()
  self:initialize_hud()
end

-- This event is called when a new map has just become active.
function game:on_map_changed(map)

  -- Notify the hud.
  self:hud_on_map_changed(map)
end

function game:on_paused()
  self:hud_on_paused()
  self:start_pause_menu()
end

function game:on_unpaused()
  self:stop_pause_menu()
  self:hud_on_unpaused()
end

function game:get_player_name()
  return self:get_value("player_name")
end

function game:set_player_name(player_name)
  self:set_value("player_name", player_name)
end

-- Returns whether the current map is in the inside world.
function game:is_in_inside_world()
  return self:get_map():get_world() == "inside_world"
end

-- Returns whether the current map is in the outside world.
function game:is_in_outside_world()
  return self:get_map():get_world() == "outside_world"
end

-- Returns whether the current map is in a dungeon.
function game:is_in_dungeon()
  return self:get_dungeon() ~= nil
end

-- Run the game.
sol.main.game = game
game:start()

