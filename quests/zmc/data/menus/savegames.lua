-- Savegame selection screen, displayed after the title screen.

local savegame_menu = {}

function savegame_menu:new()
  local object = {}
  setmetatable(object, self)
  self.__index = self
  return object
end

function savegame_menu:on_started()

  -- Create all graphic objects.
  self.surface = sol.surface.create(320, 240)
  self.background_color = { 104, 144, 240 }
  self.background_img = sol.surface.create("menus/selection_menu_background.png")
  self.cloud_img = sol.surface.create("menus/selection_menu_cloud.png")
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
  self.phase = nil

  -- Start the clouds.
  self.cloud_positions = {
    { x =  20, y =  40 },
    { x =  50, y = 160 },
    { x = 160, y =  30 },
    { x = 270, y = 200 },
    { x = 200, y = 120 },
    { x =  90, y = 120 },
    { x = 300, y = 100 },
    { x = 240, y =  10 },
    { x =  60, y = 190 },
    { x = 150, y = 120 },
    { x = 310, y = 220 },
    { x =  70, y =  20 },
    { x = 130, y = 180 },
    { x = 200, y = 700 },
    { x =  20, y = 120 },
    { x = 170, y = 220 }
  }
  self:repeat_move_clouds()

  -- Run the menu.
  self:read_savegames()
  sol.audio.play_music("game_over")
  self:init_phase_select_file()

  -- Show an opening transition.
  self.surface:fade_in()
end

function savegame_menu:on_key_pressed(key)

  local handled = false
  if key == "escape" then
    -- Stop the program.
    handled = true
    sol.main.exit()
  elseif key == "right" then
    handled = self:direction_pressed(0)
  elseif key == "up" then
    handled = self:direction_pressed(2)
  elseif key == "left" then
    handled = self:direction_pressed(4)
  elseif key == "down" then
    handled = self:direction_pressed(6)
  elseif not self.finished then

    -- Phase-specific direction_pressed method.
    local method_name = "key_pressed_phase_" .. self.phase
    handled = self[method_name](self, key)
  end

  return handled
end

function savegame_menu:on_joypad_button_pressed(button)

  local handled = true
  if not self.finished then
    -- Phase-specific joypad_button_pressed method.
    local method_name = "joypad_button_pressed_phase_" .. self.phase
    handled = self[method_name](self, button)
  else
    handled = false
  end

  return handled
end

function savegame_menu:on_joypad_axis_moved(axis, state)

  if axis % 2 == 0 then  -- Horizontal axis.
    if state > 0 then
      self:direction_pressed(0)
    elseif state < 0 then
      self:direction_pressed(4)
    end
  else  -- Vertical axis.
    if state > 0 then
      self:direction_pressed(2)
    else
      self:direction_pressed(6)
    end
  end
end

function savegame_menu:on_joypad_hat_moved(hat, direction8)

  if direction8 ~= -1 then
    self:direction_pressed(direction8)
  end
end

function savegame_menu:direction_pressed(direction8)

  local handled = true
  if self.allow_cursor_move and not self.finished then

    -- The cursor moves too much when using a joypad axis.
    self.allow_cursor_move = false
    sol.timer.start(self, 100, function()
      self.allow_cursor_move = true
    end)

    -- Phase-specific direction_pressed method.
    local method_name = "direction_pressed_phase_" .. self.phase
    handled = self[method_name](self, direction8)
  else
    handled = false
  end
end

function savegame_menu:on_draw(dst_surface)

  -- Background color.
  self.surface:fill_color(self.background_color)

  -- Clouds.
  local width, height = self.surface:get_size()
  for _, position in ipairs(self.cloud_positions) do
    local x, y = position.x, position.y
    self.cloud_img:draw(self.surface, x, y)

    if position.x >= width - 80 then
      x = position.x - width
      y = position.y
      self.cloud_img:draw(self.surface, x, y)

      if position.y <= 0 then
        x = position.x - width
        y = position.y + height
        self.cloud_img:draw(self.surface, x, y)
      end
    end

    if position.y <= 0 then
      x = position.x
      y = position.y + height
      self.cloud_img:draw(self.surface, x, y)
    end
  end

  -- Savegames container.
  self.background_img:draw(self.surface, 37, 38)
  self.title_text:draw(self.surface, 160, 54)

  -- Phase-specific draw method.
  local method_name = "draw_phase_" .. self.phase
  self[method_name](self)

  -- The menu makes 320*240 pixels, but dst_surface may be larger.
  local width, height = dst_surface:get_size()
  self.surface:draw(dst_surface, width / 2 - 160, height / 2 - 120)
end

function savegame_menu:draw_savegame(slot_index)

  local slot = self.slots[slot_index]
  self.save_container_img:draw(self.surface, 57, 48 + slot_index * 27)
  slot.player_name_text:draw(self.surface, 87, 61 + slot_index * 27)

  if slot.hearts_view ~= nil then
    slot.hearts_view:set_dst_position(168, 51 + slot_index * 27)
    slot.hearts_view:on_draw(self.surface)
  end
end

function savegame_menu:draw_savegame_cursor()

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
  self.cursor_sprite:draw(self.surface, x, y)
end

function savegame_menu:draw_savegame_number(slot_index)

  local slot = self.slots[slot_index]
  slot.number_img:draw(self.surface, 62, 53 + slot_index * 27)
end

function savegame_menu:draw_bottom_buttons()

  local x
  local y = 158
  if self.option1_text:get_text():len() > 0 then
    x = 57
    self.option_container_img:draw(self.surface, x, y)
    self.option1_text:draw(self.surface, 90, 172)
  end
  if self.option2_text:get_text():len() > 0 then
    x = 165
    self.option_container_img:draw(self.surface, x, y)
    self.option2_text:draw(self.surface, 198, 172)
  end
end

function savegame_menu:read_savegames()

  self.slots = {}
  for i = 1, 3 do
    local slot = {}
    slot.file_name = "save" .. i .. ".dat"
    slot.savegame = sol.game.load(slot.file_name)
    slot.number_img = sol.surface.create("menus/selection_menu_save" .. i .. ".png")

    slot.player_name_text = sol.text_surface.create()
    if sol.game.exists(slot.file_name) then
      -- Existing file.
      slot.player_name_text:set_text(slot.savegame:get_value("player_name"))

      -- Hearts.
      local hearts_class = require("hud/hearts")
      slot.hearts_view = hearts_class:new(slot.savegame)
    else
      -- New file.
      local name = "- " .. sol.language.get_string("selection_menu.empty") .. " -"
      slot.player_name_text:set_text(name)
    end

    self.slots[i] = slot
  end
end

function savegame_menu:set_bottom_buttons(key1, key2)

  if key1 ~= nil then
    self.option1_text:set_text_key(key1)
  else
    self.option1_text:set_text("")
  end

  if key2 ~= nil then
    self.option2_text:set_text_key(key2)
  else
    self.option2_text:set_text("")
  end
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

function savegame_menu:repeat_move_clouds()

  local width, height = self.surface:get_size()
  for _, position in ipairs(self.cloud_positions) do

    position.x = position.x + 1
    if position.x >= width then
      position.x = 0
    end

    position.y = position.y - 1
    if position.y <= -44 then
      position.y = height - 44
    end
  end

  sol.timer.start(self, 100, function()
    self:repeat_move_clouds()
  end)
end

---------------------------
-- Phase "select a file" --
---------------------------
function savegame_menu:init_phase_select_file()

  self.phase = "select_file"
  self.title_text:set_text_key("selection_menu.phase.select_file")
  self:set_bottom_buttons("selection_menu.erase", "selection_menu.options")
  self.cursor_sprite:set_animation("blue")
end

function savegame_menu:key_pressed_phase_select_file(key)

  local handled = false
  if key == "space" or key == "return" then
    sol.audio.play_sound("ok")
    if self.cursor_position == 5 then
      -- The user chooses "Options".
      self:init_phase_options()
    elseif self.cursor_position == 4 then
      -- The user chooses "Erase".
      self:init_phase_erase_file()
    else
      -- The user chooses a savegame.
      local slot = self.slots[self.cursor_position]
      if sol.game.exists(slot.file_name) then
        -- The file exists: run it after a fade-out effect.
        self.finished = true
        self.surface:fade_out(function()
	  self:start_game(slot.savegame)
        end)
      else
        -- It's a new savegame: choose the player's name.
        self:init_phase_choose_name()
      end
    end
    handled = true
  end

  return handled
end

function savegame_menu:joypad_button_pressed_phase_select_file(button)

  return self:key_pressed_phase_select_file("space")
end

function savegame_menu:direction_pressed_phase_select_file(direction8)

  local handled = true
  if direction8 == 6 then  -- Down.
    self:move_cursor_down()
  elseif direction8 == 2 then  -- Up.
    self:move_cursor_up()
  elseif direction8 == 0 or direction8 == 4 then  -- Right or Left.
    self:move_cursor_left_or_right()
  else
    handled = false
  end
  return handled
end

function savegame_menu:draw_phase_select_file()

  -- Savegame slots.
  for i = 1, 3 do
    self:draw_savegame(i)
  end

  -- Bottom buttons.
  self:draw_bottom_buttons()

  -- Cursor.
  self:draw_savegame_cursor()

  -- Save numbers.
  for i = 1, 3 do
    self:draw_savegame_number(i)
  end
end

--------------------------
-- Phase "erase a file" --
--------------------------
function savegame_menu:init_phase_erase_file()

  self.phase = "erase_file"
  self.title_text:set_text_key("selection_menu.phase.erase_file")
  self:set_bottom_buttons("selection_menu.cancel", nil)
  self.cursor_sprite:set_animation("red")
end

function savegame_menu:key_pressed_phase_erase_file(key)

  local handled = true
  if key == "space" or key == "return" then
    if self.cursor_position == 4 then
      -- The user chooses "Cancel".
      sol.audio.play_sound("ok")
      self:init_phase_select_file()
    elseif self.cursor_position > 0 and self.cursor_position <= 3 then
      -- The user chooses a savegame to delete.
      local slot = self.slots[self.cursor_position]
      if not sol.game.exists(slot.file_name) then
        -- The savegame doesn't exist: error sound.
        sol.audio.play_sound("wrong")
      else
        -- The savegame exists: confirm deletion.
        sol.audio.play_sound("ok")
        self:init_phase_confirm_erase()
      end
    end
  else
    handled = false
  end
  return handled
end

function savegame_menu:joypad_button_pressed_phase_erase_file(button)

  return self:key_pressed_phase_erase_file("space")
end

function savegame_menu:direction_pressed_phase_erase_file(direction8)

  local handled = true
  if direction8 == 6 then  -- Down.
    self:move_cursor_down()
  elseif direction8 == 2 then  -- Up.
    self:move_cursor_up()
  else
    handled = false
  end
  return handled
end

function savegame_menu:draw_phase_erase_file()

  -- Savegame slots.
  for i = 1, 3 do
    self:draw_savegame(i)
  end

  -- Bottom buttons.
  self:draw_bottom_buttons()

  -- Cursor.
  self:draw_savegame_cursor()

  -- Save numbers.
  for i = 1, 3 do
    self:draw_savegame_number(i)
  end
end

---------------------------
-- Phase "Are you sure?" --
---------------------------
function savegame_menu:init_phase_confirm_erase()

  self.phase = "confirm_erase"
  self.title_text:set_text_key("selection_menu.phase.confirm_erase")
  self:set_bottom_buttons("selection_menu.big_no", "selection_menu.big_yes")
  self.save_number_to_erase = self.cursor_position
  self.cursor_position = 4  -- Select "no" by default.
end

function savegame_menu:key_pressed_phase_confirm_erase(key)

  local handled = true
  if key == "space" or key == "return" then
   if self.cursor_position == 5 then
      -- The user chooses "yes".
      sol.audio.play_sound("boss_killed")
      local slot = self.slots[self.save_number_to_erase]
      sol.game.delete(slot.file_name)
      self.cursor_position = self.save_number_to_erase
      self:read_savegames()
      self:init_phase_select_file()
    elseif self.cursor_position == 4 then
      -- The user chooses "no".
      sol.audio.play_sound("ok")
      self:init_phase_select_file()
    end
  else
    handled = false
  end
  return handled
end

function savegame_menu:joypad_button_pressed_phase_confirm_erase(button)

  -- To erase a savegame file, we don't allow joypad buttons.
end

function savegame_menu:direction_pressed_phase_confirm_erase(direction8)

  local handled = false
  if direction8 == 0 or direction8 == 4 then  -- Right or Left.
    self:move_cursor_left_or_right()
    handled = true
  end
  return handled
end

function savegame_menu:draw_phase_confirm_erase()

  -- Current savegame slot.
  self:draw_savegame(self.save_number_to_erase)
  self:draw_savegame_number(self.save_number_to_erase)

  -- Bottom buttons.
  self:draw_bottom_buttons()

  -- Cursor.
  self:draw_savegame_cursor()
end

----------------------
-- Phase "options" --
----------------------
function savegame_menu:init_phase_options()

  self.phase = "options"
  self.title_text:set_text_key("selection_menu.phase.options")
  self.modifying_option = false
  self.options_cursor_position = 1

  -- Option texts and values.
  self.options = {
    {
      name = "language",
      values = sol.language.get_languages(),
      initial_value = sol.language.get_language()
    },
    {
      name = "video_mode",
      values = sol.video.get_modes(),
      initial_value = sol.video.get_mode()
    },
    {
      name = "music_volume",
      values = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100},
      initial_value = math.floor((sol.audio.get_music_volume() + 5) / 10) * 10
    },
    {
      name = "sound_volume",
      values = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100},
      initial_value = math.floor((sol.audio.get_sound_volume() + 5) / 10) * 10
    }
  }

  for _, option in ipairs(self.options) do

    option.current_index = nil

    -- Text surface of the label.
    option.label_text = sol.text_surface.create{
      font = "fixed",
      text_key = "selection_menu.options." .. option.name
    }

    -- Text surface of the value.
    option.value_text = sol.text_surface.create{
      font = "fixed",
      horizontal_alignment = "right"
    }
  end

  for _, option in ipairs(self.options) do
    -- Initial value.
    for i, value in ipairs(option.values) do
      if value == option.initial_value then
	self:set_option_value(option, i)
      end
    end
  end

  -- Sprites.
  self.left_arrow_sprite = sol.sprite.create("menus/arrow")
  self.left_arrow_sprite:set_animation("blink")
  self.left_arrow_sprite:set_direction(2)

  self.right_arrow_sprite = sol.sprite.create("menus/arrow")
  self.right_arrow_sprite:set_animation("blink")
  self.right_arrow_sprite:set_direction(0)

  self:set_bottom_buttons("selection_menu.back", nil)
  self:set_options_cursor_position(1)
end

function savegame_menu:key_pressed_phase_options(key)

  local handled = true
  if key == "space" or key == "return" then
    if self.options_cursor_position > #self.options then
      -- Back.
      sol.audio.play_sound("ok")
      self:init_phase_select_file()
    else
      -- Set an option.
      local option = self.options[self.options_cursor_position]
      if not self.modifying_option then
	sol.audio.play_sound("ok")
	self.left_arrow_sprite:set_frame(0)
	self.right_arrow_sprite:set_frame(0)
	option.label_text:set_text_color{255, 255, 255}
	option.value_text:set_text_color{255, 255, 0}
	self.title_text:set_text_key("selection_menu.phase.options.changing")
	self.modifying_option = true
      else
	sol.audio.play_sound("danger")
	option.label_text:set_text_color{255, 255, 0}
	option.value_text:set_text_color{255, 255, 255}
	self.left_arrow_sprite:set_frame(0)
	self.right_arrow_sprite:set_frame(0)
	self.title_text:set_text_key("selection_menu.phase.options")
	self.modifying_option = false
      end
    end
  else
    handled = false
  end
  return handled
end

function savegame_menu:joypad_button_pressed_phase_options(button)
  return self:key_pressed_phase_options("space")
end

function savegame_menu:direction_pressed_phase_options(direction8)

  local handled = false
  if not self.modifying_option then
    -- Just moving the options cursor (not modifying any option).

    if direction8 == 2 then  -- Up.
      sol.audio.play_sound("cursor")
      self.left_arrow_sprite:set_frame(0)
      local position = self.options_cursor_position - 1
      if position == 0 then
        position = #self.options + 1
      end
      self:set_options_cursor_position(position)
      handled = true

    elseif direction8 == 6 then  -- Down.
      sol.audio.play_sound("cursor")
      self.left_arrow_sprite:set_frame(0)
      local position = self.options_cursor_position + 1
      if position > #self.options + 1 then
        position = 1
      end
      self:set_options_cursor_position(position)
      handled = true
    end

  else
    -- An option is currently being modified.

    if direction8 == 0 then  -- Right.
      local option = self.options[self.options_cursor_position]
      local index = (option.current_index % #option.values) + 1
      self:set_option_value(option, index)
      sol.audio.play_sound("cursor")
      self.left_arrow_sprite:set_frame(0)
      self.right_arrow_sprite:set_frame(0)
      handled = true

    elseif direction8 == 4 then  -- Left.
      local option = self.options[self.options_cursor_position]
      local index = (option.current_index + #option.values - 2) % #option.values + 1
      self:set_option_value(option, index)
      sol.audio.play_sound("cursor")
      self.left_arrow_sprite:set_frame(0)
      self.right_arrow_sprite:set_frame(0)
      handled = true

    end
  end
  return handled
end

function savegame_menu:draw_phase_options()

  -- All options.
  for i, option in ipairs(self.options) do
    local y = 70 + i * 16
    option.label_text:draw(self.surface, 64, y)
    option.value_text:draw(self.surface, 266, y)
  end

  -- Bottom buttons.
  self:draw_bottom_buttons()

  -- Cursor.
  if self.options_cursor_position > #self.options then
    -- The cursor is on the bottom button.
    self:draw_savegame_cursor()
  else
    -- The cursor is on an option line.
    local y = 64 + self.options_cursor_position * 16
    if self.modifying_option then
      local option = self.options[self.options_cursor_position]
      local width, _ = option.value_text:get_size()
      self.left_arrow_sprite:draw(self.surface, 256 - width, y)
      self.right_arrow_sprite:draw(self.surface, 268, y)
    else
      self.right_arrow_sprite:draw(self.surface, 54, y)
    end
  end
end

function savegame_menu:set_options_cursor_position(position)

  if self.options_cursor_position <= #self.options then
    -- An option line was previously selected.
    local option = self.options[self.options_cursor_position]
    option.label_text:set_text_color{255, 255, 255}
  end

  self.options_cursor_position = position
  if position > #self.options then
    self:set_cursor_position(4)
  end

  if position <= #self.options then
    -- An option line is now selected.
    local option = self.options[self.options_cursor_position]
    option.label_text:set_text_color{255, 255, 0}
  end
end

-- Sets the value of an option.
function savegame_menu:set_option_value(option, index)

  if option.current_index ~= index then
    option.current_index = index
    local value = option.values[index]

    if option.name == "language" then
      option.value_text:set_text(sol.language.get_language_name(value))
      if value ~= sol.language.get_language() then
	sol.language.set_language(value)
	self:reload_options_strings()
      end

    elseif option.name == "video_mode" then
      option.value_text:set_text_key("options.video_mode." .. value)
      sol.video.set_mode(value)

    elseif option.name == "music_volume" then
      option.value_text:set_text(value)
      sol.audio.set_music_volume(value)

    elseif option.name == "sound_volume" then
      option.value_text:set_text(value)
      sol.audio.set_sound_volume(value)
    end
  end
end

-- Reloads all strings displayed on the menu.
-- This function is called when the language has just been changed.
function savegame_menu:reload_options_strings()

  -- Update the label of each option.
  for _, option in ipairs(self.options) do

    option.label_text:set_text_key("selection_menu.options." .. option.name)

    -- And the value of the video mode.
    if option.name == "video_mode" and option.current_index ~= nil then
      local mode = option.values[option.current_index]
      option.value_text:set_text_key("options.video_mode." .. mode)
    end
  end

  -- Other menu elements
  self.title_text:set_text_key("selection_menu.phase.options")
  self:set_bottom_buttons("selection_menu.back", nil)
  self:read_savegames()  -- To update "- Empty -" mentions.
end

------------------------------
-- Phase "choose your name" --
------------------------------
function savegame_menu:init_phase_choose_name()

  self.phase = "choose_name"
  self.title_text:set_text_key("selection_menu.phase.choose_name")
  self.cursor_sprite:set_animation("letters")
  self.player_name = ""
  self.player_name_text = sol.text_surface.create()
  self.letter_cursor = { x = 0, y = 0 }
  self.letters_img = sol.surface.create("menus/selection_menu_letters.png")
  self.name_arrow_sprite = sol.sprite.create("menus/arrow")
  self.name_arrow_sprite:set_direction(0)
  self.can_add_letter_player_name = true
end

function savegame_menu:key_pressed_phase_choose_name(key)

  local handled = false
  local finished = false
  if key == "return" then
    -- Directly validate the name.
    finished = self:validate_player_name()
    handled = true

  elseif key == "space" or key == "c" then

    if self.can_add_letter_player_name then
      -- Choose a letter
      finished = self:add_letter_player_name()
      self.player_name_text:set_text(self.player_name)
      self.can_add_letter_player_name = false
      sol.timer.start(self, 300, function()
        self.can_add_letter_player_name = true
      end)
      handled = true
    end
  end

  if finished then
    self:init_phase_select_file()
  end

  return handled
end

function savegame_menu:joypad_button_pressed_phase_choose_name(button)

  return self:key_pressed_phase_choose_name("space")
end

function savegame_menu:direction_pressed_phase_choose_name(direction8)

  local handled = true
  if direction8 == 0 then  -- Right.
    sol.audio.play_sound("cursor")
    self.letter_cursor.x = (self.letter_cursor.x + 1) % 13

  elseif direction8 == 2 then  -- Up.
    sol.audio.play_sound("cursor")
    self.letter_cursor.y = (self.letter_cursor.y + 4) % 5

  elseif direction8 == 4 then  -- Left.
    sol.audio.play_sound("cursor")
    self.letter_cursor.x = (self.letter_cursor.x + 12) % 13

  elseif direction8 == 6 then  -- Down.
    sol.audio.play_sound("cursor")
    self.letter_cursor.y = (self.letter_cursor.y + 1) % 5

  else
    handled = false
  end
  return handled
end

function savegame_menu:draw_phase_choose_name()

  -- Letter cursor.
  self.cursor_sprite:draw(self.surface,
      51 + 16 * self.letter_cursor.x,
      93 + 18 * self.letter_cursor.y)

  -- Name and letters.
  self.name_arrow_sprite:draw(self.surface, 57, 76)
  self.player_name_text:draw(self.surface, 67, 85)
  self.letters_img:draw(self.surface, 57, 98)
end

function savegame_menu:add_letter_player_name()

  local size = self.player_name:len()
  local letter_cursor = self.letter_cursor
  local letter_to_add = nil
  local finished = false

  if letter_cursor.y == 0 then  -- Uppercase letter from A to M.
    letter_to_add = string.char(string.byte("A") + letter_cursor.x)

  elseif letter_cursor.y == 1 then  -- Uppercase letter from N to Z.
    letter_to_add = string.char(string.byte("N") + letter_cursor.x)

  elseif letter_cursor.y == 2 then  -- Lowercase letter from a to m.
    letter_to_add = string.char(string.byte("a") + letter_cursor.x)

  elseif letter_cursor.y == 3 then  -- Lowercase letter from n to z.
    letter_to_add = string.char(string.byte("n") + letter_cursor.x)

  elseif letter_cursor.y == 4 then  -- Digit or special command.
    if letter_cursor.x <= 9 then
      -- Digit.
      letter_to_add = string.char(string.byte("0") + letter_cursor.x)
    else
      -- Special command.

      if letter_cursor.x == 10 then  -- Remove the last letter.
        if size == 0 then
          sol.audio.play_sound("wrong")
        else
          sol.audio.play_sound("danger")
          self.player_name = self.player_name:sub(1, size - 1)
        end

      elseif letter_cursor.x == 11 then  -- Validate the choice.
        finished = self:validate_player_name()

      elseif letter_cursor.x == 12 then  -- Cancel.
        sol.audio.play_sound("danger")
        finished = true
      end
    end
  end

  if letter_to_add ~= nil then
    -- A letter was selected.
    if size < 6 then
      sol.audio.play_sound("danger")
      self.player_name = self.player_name .. letter_to_add
    else
      sol.audio.play_sound("wrong")
    end
  end

  return finished
end

function savegame_menu:validate_player_name()

  if self.player_name:len() == 0 then
    sol.audio.play_sound("wrong")
    return false
  end

  sol.audio.play_sound("ok")

  local savegame = self.slots[self.cursor_position].savegame
  self:set_initial_values(savegame)
  savegame:save()
  self:read_savegames()
  return true
end

function savegame_menu:set_initial_values(savegame)

  savegame:set_starting_location("test_map", "initial_position")
  savegame:set_value("player_name", self.player_name)

  -- Initially give 3 hearts, the first tunic and the first wallet.
  savegame:set_max_life(12)
  savegame:set_life(savegame:get_max_life())
  savegame:get_item("tunic"):set_variant(1)
  savegame:set_ability("tunic", 1)
  savegame:get_item("rupee_bag"):set_variant(1)
end

function savegame_menu:start_game(game)

  local play_game = sol.main.load_file("play_game")
  play_game(game)
end

return savegame_menu

