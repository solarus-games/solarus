-- Control the hero with the mouse.
local mouse_control = {}

function mouse_control:create(game, hud)

  self.game = game
  self.hud = hud
  self.is_mouse_button_pushed = false

  -- Movement of the hero.
  self.directions_pressed = {
      right = false,
      up = false,
      left = false,
      down = false
  }
  self.deadzone_ray = 10 -- Should be set to the max number of pixel that the hero can move in one frame.
end

function mouse_control:on_mouse_pressed(button, x, y)

  self.is_mouse_button_pushed = true;
end

function mouse_control:on_mouse_released(button, x, y)

  self.is_mouse_button_pushed = false;
end

function mouse_control:on_update()

  if self.game == nil or self.game:get_map() == nil then
    return
  end

  if self.is_mouse_button_pushed then

    local mouse_x, mouse_y = sol.input.get_mouse_position()

    -- If the mouse position is not over a button, move the hero.
    if self.hud ~= nil and not self.hud:has_button_pushed() then
      local hero_x, hero_y = self.game:get_hero():get_position()

      -- Compare the position of the hero and the mouse
      -- and simulate the appropriate command for each directions.
      self:update_direction("right", mouse_x > hero_x + self.deadzone_ray)
      self:update_direction("up", mouse_y < hero_y - self.deadzone_ray)
      self:update_direction("left", mouse_x < hero_x - self.deadzone_ray)
      self:update_direction("down", mouse_y > hero_y + self.deadzone_ray)
    end
  else

    for direction, _ in pairs(self.directions_pressed) do
      self:stop_direction(direction)
    end
  end
end

function mouse_control:update_direction(direction, is_movement_needed)

  if is_movement_needed then
    self:start_direction(direction)
  else
    self:stop_direction(direction)
  end
end

function mouse_control:start_direction(direction)

  if not self.directions_pressed[direction] then
    self.directions_pressed[direction] = true
    self.game:simulate_command_pressed(direction)
  end
end

function mouse_control:stop_direction(direction)

  if self.directions_pressed[direction] then
    self.directions_pressed[direction] = false
    self.game:simulate_command_released(direction)
  end
end

return mouse_control
