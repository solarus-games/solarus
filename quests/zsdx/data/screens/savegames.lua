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
  self.background_img = sol.surface.create("menus/selection_menu_background.png")
  self.save_container_img = sol.surface.create("menus/selection_menu_save_container.png")
  self.option_container_img = sol.surface.create("menus/selection_menu_option_container.png")
  self.option1_text = sol.text_surface.create()
  self.option2_text = sol.text_surface.create()
  self.title_text = sol.text_surface.create{
    horizontal_alignment = "center",
    font = "fixed"
  }
  self.cursor_position = 1
  self.cursor_sprite = sol.sprite.create("menus/selection_menu_cursor")
  self.allow_cursor_move = true
  self.finished = false
  self:read_savegames()

  self:init_phase_select_file()

  -- music
  sol.audio.play_music("game_over")
end

function savegame_menu:on_key_pressed(key)

  if key == "escape" then
    -- stop the program
    sol.main.exit()
  else
    -- Phase "select a file".
    if key == "space" or key == "return" then
      sol.audio.play_sound("ok")
      if self.cursor_position == 5 then
        -- The user chooses "Options".
        -- TODO
      elseif self.cursor_position == 4 then
        -- The user chooses "Erase".
        -- TODO
      else
        -- The user chooses a savegame.
        self.slots[self.cursor_position].savegame:start()
      end
    end
  end
end

function savegame_menu:on_direction_pressed(direction8)

  if self.allow_cursor_move then

    -- The cursor moves too much when using a joypad axis.
    self.allow_cursor_move = false
    self:start_timer(100, function()
      self.allow_cursor_move = true
    end)

    if not self.finished then

      -- Phase "select a file".
      if direction8 == 6 then  -- Down.
        self:move_cursor_down()
      elseif direction8 == 2 then  -- Up.
        self:move_cursor_up()
      elseif direction8 == 0 or direction8 == 4 then  -- Right or Left.
        self:move_cursor_left_or_right()
      end
    end
  end
end

function savegame_menu:on_display(dst_surface)

  -- Background color.
  self.surface:fill_color(self.background_color)

  -- Clouds.
  -- TODO

  -- Savegames container.
  self.surface:draw(self.background_img, 37, 38)
  self.surface:draw(self.title_text, 160, 54)

  -- Phase-specific.
  self:display_current_phase()

  -- The menu makes 320*240 pixels, but dst_surface may be larger.
  local width, height = dst_surface:get_size()
  dst_surface:draw(self.surface, width / 2 - 160, height / 2 - 120)
end

function savegame_menu:display_savegame(slot_index)

  local slot = self.slots[slot_index]
  self.surface:draw(self.save_container_img, 57, 48 + slot_index * 27)
  self.surface:draw(slot.player_name_text, 87, 61 + slot_index * 27)
end

function savegame_menu:display_savegame_cursor()

  local x, y
  if self.cursor_position == 5 then
    x = 166
  else
    x = 58
  end
  if self.cursor_position < 4 then
    y = 49 + self.cursor_position * 27
  else
    y = 159
  end
  self.surface:draw(self.cursor_sprite, x, y)
end

function savegame_menu:display_savegame_number(slot_index)

  local slot = self.slots[slot_index]
  self.surface:draw(slot.number_img, 62, 53 + slot_index * 27)
end

function savegame_menu:display_bottom_options()

  local x
  local y = 158
  if self.option1_text:get_text():len() > 0 then
    x = 57
    self.surface:draw(self.option_container_img, x, y)
    self.surface:draw(self.option1_text, 90, 172)
  end
  if self.option2_text:get_text():len() > 0 then
    x = 165
    self.surface:draw(self.option_container_img, x, y)
    self.surface:draw(self.option2_text, 198, 172)
  end
end

function savegame_menu:display_current_phase()

  -- Initial phase (select a file).

  -- Savegame slots.
  for i = 1, 3 do
    self:display_savegame(i)
  end

  -- Options.
  self:display_bottom_options()

  -- Cursor
  self:display_savegame_cursor()

  -- Save numbers.
  for i = 1, 3 do
    self:display_savegame_number(i)
  end
end

function savegame_menu:read_savegames()

  self.slots = {}
  for i = 1, 3 do
    local slot = {}
    slot.savegame = sol.game.load("save" .. i .. ".dat")
    slot.player_name_text = sol.text_surface.create()
    slot.player_name_text:set_text(slot.savegame:get_player_name())
    slot.number_img = sol.surface.create("menus/selection_menu_save" .. i .. ".png")
    self.slots[i] = slot
  end
end

function savegame_menu:init_phase_select_file()

  self.title_text:set_text_key("selection_menu.phase.select_file")
  self:set_bottom_options("selection_menu.erase", "selection_menu.options")
  self.cursor_sprite:set_animation("blue")
end

function savegame_menu:set_bottom_options(key1, key2)

  self.option1_text:set_text_key(key1)
  self.option2_text:set_text_key(key2)
end

function savegame_menu:move_cursor_up()

  sol.audio.play_sound("cursor")
  local cursor_position = self.cursor_position - 1
  if cursor_position == 0 then
    cursor_position = 4
  elseif cursor_position == 4 then
    cursor_position = 3
  end
  self:set_cursor_position(cursor_position)
end

function savegame_menu:move_cursor_down()

  sol.audio.play_sound("cursor")
  local cursor_position = self.cursor_position + 1
  if cursor_position >= 5 then
    cursor_position = 1
  end
  self:set_cursor_position(cursor_position)
end

function savegame_menu:move_cursor_left_or_right()

  if self.cursor_position == 4 then
    sol.audio.play_sound("cursor")
    self:set_cursor_position(5)
  elseif self.cursor_position == 5 then
    sol.audio.play_sound("cursor")
    self:set_cursor_position(4)
  end
end

function savegame_menu:set_cursor_position(cursor_position)

  self.cursor_position = cursor_position
  self.cursor_sprite:set_frame(0)  -- Restart the animation.
end

return savegame_menu

