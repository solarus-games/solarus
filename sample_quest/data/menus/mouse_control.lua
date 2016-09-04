-- Control the hero with the mouse.
local mouse_control = {}

function mouse_control:create(game, hud)

  self.game = game
  self.hud = hud
  self.is_mouse_button_pushed = false

  -- Movement of the hero.
  self.directions = {
      {"right", false},
      {"up", false},
      {"left", false},
      {"down", false}
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

  if self.game ~= nil and self.game:get_map() ~= nil then
    if self.is_mouse_button_pushed then

      local mouse_position = {sol.input.get_mouse_position()}

      -- If the mouse position is not over a button, move the hero.
      if self.hud ~= nil and not self.hud:has_button_pushed() then
        local hero_position = {self.game:get_hero():get_position()}

        -- Compare the position of the hero and the mouse
        -- and simulate the appropriate command for each directions.
        self:update_direction(1, mouse_position[1] > hero_position[1] + self.deadzone_ray)
        self:update_direction(2, mouse_position[2] < hero_position[2] - self.deadzone_ray)
        self:update_direction(3, mouse_position[1] < hero_position[1] - self.deadzone_ray)
        self:update_direction(4, mouse_position[2] > hero_position[2] + self.deadzone_ray)
      end
    else

      for i=1, 4, 1 do
        self:stop_direction(i)
      end
    end
  end
end

function mouse_control:update_direction(direction_id, is_movement_needed)

  if is_movement_needed then
    self:start_direction(direction_id)
  else
    self:stop_direction(direction_id)
  end
end

function mouse_control:start_direction(direction_id)

  if not self.directions[direction_id][2] then
    self.directions[direction_id][2] = true;
    self.game:simulate_command_pressed(self.directions[direction_id][1])
  end
end

function mouse_control:stop_direction(direction_id)

  if self.directions[direction_id][2] then
    self.directions[direction_id][2] = false;
    self.game:simulate_command_released(self.directions[direction_id][1])
  end
end

return mouse_control