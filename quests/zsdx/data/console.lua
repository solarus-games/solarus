-- Lua console.

-- TODO: improve the console
-- - implement a history
-- - interrupt keys pressed
-- - add a prompt

local console = {
  enabled = false,         -- Indicates whether the console is shown.
  color = { 64, 64, 64 },  -- Background color of the console area.
  characters = { },        -- Characters shown, stored as an array of UTF-8 strings (this is necessary to erase the last one).
  text_surface = sol.text_surface.create{
    font = "fixed"
  },
  game = nil,
}
sol.main.console = console

function console:start()
  self.enabled = true
end

function console:stop()
  self.enabled = false
end

function console:get_text()
  return self.text_surface:get_text()
end

function console:show_status(text)
  self.text_surface:set_text(text)
  self.characters = {}
  self.status_shown = true
end

function console:build_text()

  local text = table.concat(self.characters)
  self.text_surface:set_text(text)
end

function console:clear()
  self.characters = {}
  self.status_shown = false
  self:build_text()
end

function console:append_character(character)
  self.characters[#self.characters + 1] = character
  self:build_text()
end

function console:remove_character(index)

  if index == nil then
    index = #self.characters
  end

  table.remove(self.characters, index)
  self:build_text()
end

function console:get_game()
  return self.game
end

function console:set_game(game)
  self.game = game
end

function console:on_key_pressed(key, modifiers)

  if key == "f12" or key == "escape" then
    self:stop()
  elseif key == "backspace" then
    if self.status_shown then
      self:clear()
    else
      self:remove_character()
    end
  elseif key == "return" or key == "kp return" then
    if self.status_shown then
      self:clear()
    else
      self:execute_code()
    end
  end
end

function console:on_character_pressed(character)

  if not character:find("%c") then  -- Don't append control characters.

    if self.status_shown then
      self:clear()
    end
    self:append_character(character)
  end
end

function console:on_draw(dst_surface)

  local width, height = dst_surface:get_size()
  dst_surface:fill_color(self.color, 32, height - 64, width - 64, 32)
  self.text_surface:draw(dst_surface, 40, height - 56)
end

function console:execute_code()

  -- Set up an environment that gives access to the game, the map and the entities.
  local environment = {}
  setmetatable(environment, { __index = environment_index })

  local success = false
  local code, message = loadstring(self:get_text())

  if code ~= nil then
    setfenv(code, environment)
    success, message = pcall(code)
  end

  if success then
    self:show_status("done")
    self.status_shown = true
  else
    message = message:gsub(".*:1: ", "")
    self:show_status(message)
    self.status_shown = true
  end
end

function environment_index(environment, key)

  local result = nil

  if console.game ~= nil then
    if key == "game" then
      result = console.game
    elseif key == "map" then
      result = console.game:get_map()
    else
      local entity = console.game:get_map():get_entity(key)
      if entity ~= nil then
	result = entity
      end
    end
  end

  if result == nil then
    result = _G[key]
  end

  return result
end

return console

