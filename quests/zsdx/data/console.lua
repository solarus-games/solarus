-- Lua console.

-- TODO: improve the console
-- - implement a history
-- - interrupt keys pressed
-- - add a prompt

local console = {
  enabled = false,         -- Indicates whether the console is shown.
  color = { 64, 64, 64 },  -- Background color of the console area.
  input_characters = { },        -- Characters shown, stored as an array of UTF-8 strings (this is necessary to erase the last one).
  input_text_surface = sol.text_surface.create{
    font = "fixed"
  },
  output_text_surface = sol.text_surface.create{
    font = "fixed"
  },
}
sol.main.console = console

function console:start()
  self.enabled = true
  self:build_input_text()
end

function console:stop()
  self.enabled = false
end

function console:get_input_text()
  return self.input_text_surface:get_text():sub(3)  -- Remove the prompt.
end

function console:get_output_text()
  return self.output_text_surface:get_text()
end

function console:set_output_text(text)
  self.output_text_surface:set_text(text)
end

function console:build_input_text()

  local text = "> " .. table.concat(self.input_characters)
  self.input_text_surface:set_text(text)
end

function console:clear()
  self.input_characters = {}
  self:build_input_text()
  self:set_output_text("")
end

function console:append_input_character(character)
  self.input_characters[#self.input_characters + 1] = character
  self:build_input_text()
end

function console:remove_input_character(index)

  if index == nil then
    index = #self.input_characters
  end

  table.remove(self.input_characters, index)
  self:build_input_text()
end

function console:on_key_pressed(key, modifiers)

  local handled = false
  if key == "f12" or key == "escape" then
    self:stop()
    handled = true
  elseif key == "backspace" then
    if self:get_output_text() ~= "" then
      self:clear()
    else
      self:remove_input_character()
    end
    handled = true
  elseif key == "return" or key == "kp return" then
    if self:get_output_text() ~= "" then
      self:clear()
    else
      self:execute_code()
    end
    handled = true
  end

  return handled
end

function console:on_character_pressed(character)

  local handled = false
  if not character:find("%c") then  -- Don't append control characters.

    if self:get_output_text() ~= "" then
      self:clear()
    end
    self:append_input_character(character)
    handled = true
  end

  return handled
end

function console:on_draw(dst_surface)

  local width, height = dst_surface:get_size()
  dst_surface:fill_color(self.color, 32, height - 64, width - 64, 40)
  self.input_text_surface:draw(dst_surface, 40, height - 56)
  self.output_text_surface:draw(dst_surface, 40, height - 40)
end

function console:execute_code()

  -- Set up an environment that gives access to the game, the map and the entities.
  local environment = {}
  setmetatable(environment, { __index = self.environment_index })

  local success = false
  local code, message = loadstring(self:get_input_text())

  if code ~= nil then
    setfenv(code, environment)
    success, message = pcall(code)
  end

  if not success then
    message = message:gsub(".*:1: ", "")
    self:set_output_text(message)
  end
end

function console.environment_index(environment, key)

  local result = nil
  if key == "print" then
    -- Redefine print to output into the console.
    result = console.print
  else
    local game = sol.main.game
    if game ~= nil then
      if key == "game" then
	result = game
      elseif key == "map" then
	result = game:get_map()
      else
	local entity = game:get_map():get_entity(key)
	if entity ~= nil then
	  result = entity
	end
      end
    end
  end

  if result == nil then
    result = _G[key]
  end

  return result
end

function console.print(...)

  local num_args = select("#", ...)
  local text = ""
  for i = 1, num_args do
    local arg = select(i, ...)
    if type(arg) == "string" or type(arg) == "number" then
      text = text .. arg
    else
      text = text .. type(arg)
    end
  end
  console:set_output_text(text)
end

return console

