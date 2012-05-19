events = {
  key_pressed = nil,
  key_released = nil,
  joyad_button_pressed = nil,
  joyad_button_released = nil,
  joyad_axis_moved = nil,
  joyad_hat_moved = nil,
  direction_pressed = nil,
}

-- special function: called every tick by the engine!
function sol.events:on_update()

end

function sol.events:on_started()
  -- Just need this here, no need to require globally
  local title_screen = require 'screens/title.oop'

  t = title_screen:new()

  -- Call an initial screen. We need to do this.
  sol.main.start_screen(t)
end

function sol.events:on_key_pressed(key, mods)

  -- print("A key was pressed: " .. key .. "!")
end

