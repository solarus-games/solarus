-- This menu is displayed when the program starts, before the title screen.
-- The user can select his language.
-- If a language is already set, we go directly to the title screen.

local language_menu = {}

function language_menu:new()
  local object = {}
  setmetatable(object, self)
  self.__index = self
  return object
end

function language_menu:on_started()

  if sol.language.get_language() ~= nil then
    -- A language is already set: skip this screen.
    self:start_title_screen()
  else

    local ids = sol.language.get_languages()
    local default_id = sol.language.get_default_language()
    local index = 1
    local cursor_position = 1
    self.surface = sol.surface.create(320, 240)
    self.finished = false
    self.first_visible_language = 1
    self.max_visible_languages = 10
    self.nb_visible_languages = math.min(#ids, self.max_visible_languages)
    self.languages = {}
    for _, id in ipairs(ids) do
      local language = {}
      language.id = id
      language.text = sol.text_surface.create{
        font = "fixed",
        text = sol.language.get_language_name(id),
        horizontal_alignment = "center"
      }

      if id == default_id then
        cursor_position = index
      end

      self.languages[index] = language
      index = index + 1
    end

    if #self.languages == 1 then
      -- No choice: skip this screen.
      sol.language.set_language(self.languages[1].id)
      self:start_title_screen()
    end

    self:set_cursor_position(cursor_position)
  end
end

function language_menu:on_draw(dst_surface)

  self.surface:fill_color{0, 0, 0}

  local y = 120 - 8 * self.nb_visible_languages
  local first = self.first_visible_language
  local last = self.first_visible_language + self.nb_visible_languages - 1
  for i = first, last do
    self.languages[i].y = y
    y = y + 16
    self.languages[i].text:draw(self.surface, 160, y)
  end

  -- The menu makes 320*240 pixels, but dst_surface may be larger.
  local width, height = dst_surface:get_size()
  self.surface:draw(dst_surface, width / 2 - 160, height / 2 - 120)
end

function language_menu:on_key_pressed(key)

  local handled = false

  if key == "escape" then
    -- Stop the program.
    handled = true
    sol.main.exit()

  elseif key == "space" or key == "return" then

    if not self.finished then
      handled = true
      local language = self.languages[self.cursor_position]
      sol.language.set_language(language.id)
      self.finished = true
      self.surface:fade_out(function()
        self:start_title_screen()
      end)
    end

  elseif key == "right" then
    handled = self:direction_pressed(0)
  elseif key == "up" then
    handled = self:direction_pressed(2)
  elseif key == "left" then
    handled = self:direction_pressed(4)
  elseif key == "down" then
    handled = self:direction_pressed(6)
  end

  return handled
end

function language_menu:on_joypad_axis_moved(axis, state)

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

function language_menu:on_joypad_hat_moved(hat, direction8)

  if direction8 ~= -1 then
    self:direction_pressed(direction8)
  end
end


function language_menu:direction_pressed(direction8)

  local handled = false

  if not self.finished then

    local n = #self.languages
    if direction8 == 2 then  -- Up.
      sol.audio.play_sound("cursor")
      self:set_cursor_position((self.cursor_position + n - 2) % n + 1)
      handled = true
    elseif direction8 == 6 then  -- Down.
      sol.audio.play_sound("cursor")
      self:set_cursor_position(self.cursor_position % n + 1)
      handled = true
    end
  end

  return handled
end

function language_menu:on_joypad_button_pressed(button)

  return self:on_key_pressed("space")
end

function language_menu:set_cursor_position(cursor_position)

  if self.cursor_position ~= nil then
    self.languages[self.cursor_position].text:set_text_color{255, 255, 255}
  end
  self.languages[cursor_position].text:set_text_color{255, 255, 0}

  if cursor_position < self.first_visible_language then
    self.first_visible_language = cursor_position
  end

  if cursor_position >= self.first_visible_language + self.max_visible_languages then
    self.first_visible_language = cursor_position - self.max_visible_languages + 1
  end

  self.cursor_position = cursor_position
end

function language_menu:start_title_screen()

  local title_screen = require("menus/title")
  sol.main:start_menu(title_screen:new())
end

return language_menu

