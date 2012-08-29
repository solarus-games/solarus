-- Lua console.

-- TODO: improve the console
-- - implement a history
-- - interrupt keys pressed
-- - allow to get map entities like globals
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

  if key == "f12" then
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
      local code = self:get_text()
      if self.game ~= nil then
	_G.map = self.game:get_map()
      else
	_G.map = nil
      end
      local success, message = pcall(loadstring(code))
      if success then
	self:show_status("done")
	self.status_shown = true
      else
	message = message:gsub(".*:1: ", "")
	self:show_status(message)
	self.status_shown = true
      end
    end
  end
end

function console:on_character_pressed(character)

  if not character:find("%c") then  -- Don't append control characters.

    if self.status_shown then
      self:clear()
    else
      self:append_character(character)
    end
  end
end

function console:on_draw(dst_surface)

  local width, height = dst_surface:get_size()
  dst_surface:fill_color(self.color, 32, height - 64, width - 64, 32)
  self.text_surface:draw(dst_surface, 40, height - 56)
end

return console

