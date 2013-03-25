-- A Lua console that can be enabled with F12 at any time during the program.

local console = {
  enabled = false,
  color = { 64, 64, 64 },  -- Background color of the console area.
  history = { {} },        -- An array of commands typed (the last one is the current one).
                           -- Each command as an array of UTF-8 strings
                           -- (this is necessary to be able to erase characters).
  history_capacity = 50,   -- Maximum size of the history.
  history_position = 1,    -- Current position when browsing the history.
  input_text_surface = sol.text_surface.create{
    font = "fixed"
  },
  output_text_surface = sol.text_surface.create{
    font = "fixed"
  },
}

function console:start()
  self.enabled = true
  self:build_input_text()
  sol.menu.start(sol.main, self)
end

function console:stop()
  self.enabled = false
  sol.menu.stop(self)
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

  local text = "> " .. table.concat(self.history[self.history_position])
  self.input_text_surface:set_text(text)
end

function console:clear()

  self.history[self.history_position] = {}
  self:build_input_text()
  self:set_output_text("")
end

function console:append_input_character(character)

  local characters = self.history[self.history_position]
  characters[#characters + 1] = character
  self:build_input_text()
end

function console:remove_input_character(index)

  local characters = self.history[self.history_position]
  if index == nil then
    index = #characters
  end

  table.remove(characters, index)
  self:build_input_text()
end

function console:on_key_pressed(key, modifiers)

  local handled = true
  if key == "f12" or key == "escape" then
    self:stop()
  elseif key == "backspace" then
    if self:get_output_text() ~= "" then
      self:clear()
    else
      self:remove_input_character()
    end
  elseif (key == "return" or key == "kp return")
      and not modifiers.alt and not modifiers.control then
    if self:get_output_text() ~= "" then
      self:clear()
    else
      self:execute_code()
    end
  elseif key == "up" then
    self:history_up()
  elseif key == "down" then
    self:history_down()
  else
    handled = false
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

  self:history_add_command()

  -- Set up an environment that gives access to the game, the map and the entities.
  local environment = {}
  setmetatable(environment, {
    __index = self.environment_index,
    __newindex = _G
  })

  local success = false
  local code, message = loadstring(self:get_input_text())

  if code ~= nil then
    setfenv(code, environment)
    success, message = pcall(code)
  end

  if success then
    if self:get_output_text() == "" then
      self:set_output_text("Done")
    end
  else
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
    text = text .. " "
  end
  console:set_output_text(text)
end

function console:history_up()

  if self:get_output_text() ~= "" then
    self:clear()
  end

  if self.history_position > 1 then
    self.history_position = self.history_position - 1
    self:build_input_text()
  end
end

function console:history_down()

  if self:get_output_text() ~= "" then
    self:clear()
  end

  if self.history_position < #self.history then
    self.history_position = self.history_position + 1
    self:build_input_text()
  end
end

function console:history_add_command()

  if #self.history >= self.history_capacity then
    table.remove(self.history, 1)
  end

  if self.history_position < #self.history then
    -- We are browsing the history: save the selected command as the current one.
    local copy = {}
    for _, character in ipairs(self.history[self.history_position]) do
      copy[#copy + 1] = character
    end
    self.history[#self.history] = copy
  end
  -- Create a new empty command.
  self.history_position = #self.history + 1
  self.history[self.history_position] = {}
end

return console

