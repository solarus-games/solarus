-- Control the hero with the mouse.
local mouse_control = {}

function mouse_control:on_started()

  left_button_pressed = false

  -- Clickable circle buttons.
  circle_buttons = {
      {sol.surface.create("hud/icon_object_1.png"), 270, 220, "item_1", false},
      {sol.surface.create("hud/icon_sword.png"), 300, 220, "attack", false},
      {sol.surface.create("hud/icon_action.png"), 285, 195, "action", false}
  }
  circle_buttons[1][1]:set_opacity(216)
  circle_buttons[2][1]:set_opacity(216)

  -- Movement of the hero.
  directions = {
      {"right", false},
      {"up", false},
      {"left", false},
      {"down", false}
  }
  deadzone_ray = 10 -- Should be set to the max number of pixel that the hero can move in one frame.
end

function mouse_control:on_mouse_pressed(button, x, y)

  left_button_pressed = true;
end

function mouse_control:on_mouse_released(button, x, y)

  left_button_pressed = false;
end

function mouse_control:on_update()

  if game ~= nil and game:get_map() ~= nil then
    if left_button_pressed then

      local mouse_position = {sol.input.get_mouse_position()}

      -- If the mouse position is over a button, simulate the corresponding command.
      local button_clicked = false
      for i,button in ipairs(circle_buttons) do

        if sol.main.get_distance(mouse_position[1], mouse_position[2], button[2], button[3]) < button[1]:get_size()/2.0 then
          button_clicked = true
          if not button[5] then
            button[5] = true
            game:simulate_command_pressed(button[4])
          end
        end
      end

      -- If the mouse position is not over a button, move the hero.
      if not button_clicked then
        local hero_position = {game:get_hero():get_position()}

        -- Compare the position of the hero and the mouse
        -- and simulate the appropriate command for each directions.
        self:update_direction(1, mouse_position[1] > hero_position[1] + deadzone_ray)
        self:update_direction(2, mouse_position[2] < hero_position[2] - deadzone_ray)
        self:update_direction(3, mouse_position[1] < hero_position[1] - deadzone_ray)
        self:update_direction(4, mouse_position[2] > hero_position[2] + deadzone_ray)
      end
    else

      for i=1, 4, 1 do
        self:release_direction(i)
      end

      for i,button in ipairs(circle_buttons) do
        if button[5] then
          button[5] = false;
          game:simulate_command_released(button[4])
        end
      end
    end
  end
end

function mouse_control:on_draw(screen)

  if game ~= nil and game:get_map() ~= nil then

    for i,button in ipairs(circle_buttons) do
      local size = {button[1]:get_size()}

      button[1]:draw(screen, button[2]-size[1]/2.0, button[3]-size[2]/2.0)
    end
  end
end

function mouse_control:update_direction(direction_id, condition)

  if condition then
    if not directions[direction_id][2] then
      directions[direction_id][2] = true;
      game:simulate_command_pressed(directions[direction_id][1])
    end
  else
    self:release_direction(direction_id)
  end
end

function mouse_control:release_direction(direction_id)

  if directions[direction_id][2] then
    directions[direction_id][2] = false;
    game:simulate_command_released(directions[direction_id][1])
  end
end

return mouse_control