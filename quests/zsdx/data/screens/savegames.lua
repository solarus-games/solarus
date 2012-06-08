-- Savegame selection screen, displayed after the title screen.

local savegame_menu = {}

function savegame_menu:new()
  local object = {}
  setmetatable(object, self)
  self.__index = self
  return object
end

function savegame_menu:on_started()

  self.surface = sol.surface.create(320, 240)
  self.background_color = { 104, 144, 240 }
  self.save_container_img = sol.surface.create("menus/selection_menu_save_container.png")

  self:read_savegames()

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

  self.surface:fill_color(self.background_color)

  -- Display the savegame slots.
  for i = 1, 3 do
    self:display_savegame(i)
  end

  -- The menu makes 320*240 pixels, but dst_surface may be larger.
  local width, height = dst_surface:get_size()
  dst_surface:draw(self.surface, width / 2 - 160, height / 2 - 120)
end

function savegame_menu:display_savegame(slot_index)

  local slot = self.slots[slot_index]
  self.surface:draw(self.save_container_img, 57, 75 + slot_index * 27)
  self.surface:draw(slot.player_name_text, 87, 88 + slot_index * 27)
end

function savegame_menu:read_savegames()

  self.slots = {}
  for i = 1, 3 do
    local slot = {}
    slot.savegame = sol.game.load("save" .. i .. ".dat")
    slot.player_name_text = sol.text_surface.create()
    slot.player_name_text:set_text(slot.savegame:get_player_name())
    self.slots[i] = slot
  end
end

return savegame_menu

