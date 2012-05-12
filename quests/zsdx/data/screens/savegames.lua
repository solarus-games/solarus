-- Savegame selection screen, displayed after the title screen

-- This menu is scripted in an object-oriented style:
-- we create a class savegame_menu and return instance of this class.

local savegame_menu = {}

function savegame_menu:new()
  local object = {}
  setmetatable(object, self)
  self.__index = self
  return object
end

function savegame_menu:on_started()

  -- music
  sol.audio.play_music("game_over")
end

function savegame_menu:on_key_pressed(key)

  if key == "escape" then
    -- stop the program
    sol.main.exit()
  end
end

function savegame_menu:on_display(dst_surface)

  dst_surface:fill_color({255, 255, 0})
end

return savegame_menu:new()

