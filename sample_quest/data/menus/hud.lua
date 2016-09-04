-- Control the hero with the mouse.
local hud = {}

function hud:on_started()

  self.has_hud_button_pushed = false

  -- Clickable circle buttons.
  self.circle_buttons = {
      {sol.surface.create("hud/icon_item_1.png"), 270, 225, "item_1", false},
      {sol.surface.create("hud/icon_attack.png"), 295, 225, "attack", false},
      {sol.surface.create("hud/icon_action.png"), 282, 205, "action", false}
  }
  self.circle_buttons[1][1]:set_opacity(216)
  self.circle_buttons[2][1]:set_opacity(216)
end

function hud:on_mouse_pressed(button, x, y)

  if game ~= nil and game:get_map() ~= nil then

    -- If the mouse position is over a button, simulate the corresponding command.
    local mouse_position = {sol.input.get_mouse_position()}
    for i,button in ipairs(self.circle_buttons) do

      if not self.is_hud_button_clicked and sol.main.get_distance(mouse_position[1], mouse_position[2], button[2], button[3]) < button[1]:get_size()/2.0 then
        self.has_hud_button_pushed = true
        self:start_command(button)
      end
    end
  end
end

function hud:on_mouse_released(button, x, y)

  if game ~= nil and game:get_map() ~= nil then
  
    self.has_hud_button_pushed = false
    for i,button in ipairs(self.circle_buttons) do
      self:stop_command(button)
    end
  end
end

function hud:on_draw(screen)

  if game ~= nil and game:get_map() ~= nil then

    for i,button in ipairs(self.circle_buttons) do
      local size = {button[1]:get_size()}

      button[1]:draw(screen, button[2]-size[1]/2.0, button[3]-size[2]/2.0)
    end
  end
end

function hud:start_command(button)

  if not button[5] then
    button[5] = true
    game:simulate_command_pressed(button[4])
  end
end

function hud:stop_command(button)

  if button[5] then
    button[5] = false;
    game:simulate_command_released(button[4])
  end
end

function hud:has_button_pushed()

  return self.has_hud_button_pushed
end

return hud