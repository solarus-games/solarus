-- Lua console.

-- TODO: improve the console
-- - implement a history
-- - fix keys (qwerty mapping of symbols)
-- - interrupt keys pressed
-- - allow to get map entities like globals

local console = {
  enabled = false,
  color = { 64, 64, 64 },
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

function console:set_text(text)
  self.text_surface:set_text(text)
end

function console:get_game()
  return self.game
end

function console:set_game(game)
  self.game = game
end

function console:is_printable_character(key)

  return #key == 1
  -- return key:find("^[a-z0-9!\"#$&'()*+,./:<=>?@\[\]\\^_`-]$")
end

function console:on_key_pressed(key, modifiers)

  if key == "f12" then
    self:stop()
  elseif key == "backspace" then
    if self.status_shown then
      self:set_text("")
      self.status_shown = false
    else
      local text = self:get_text()
      self:set_text(text:sub(1, #text - 1))
    end
  elseif key == "return" then
    if self.status_shown then
      self:set_text("")
      self.status_shown = false
    else
      local code = self:get_text()
      if self.game ~= nil then
	_G.map = self.game:get_map()
      else
	_G.map = nil
      end
      local success, message = pcall(loadstring(code))
      if success then
	self:set_text("done")
	self.status_shown = true
      else
	message = message:gsub(".*:1: ", "")
	self:set_text(message)
	self.status_shown = true
      end
    end
  elseif self:is_printable_character(key) then
    if self.status_shown then
      self:set_text("")
      self.status_shown = false
    else
      if modifiers.shift then
	key = key:upper()
      end
      self:set_text(self:get_text() .. key)
    end
  end
end

function console:on_draw(dst_surface)

  local width, height = dst_surface:get_size()
  dst_surface:fill_color(self.color, 32, height - 64, width - 64, 32)
  self.text_surface:draw(dst_surface, 40, height - 56)
end

return console

