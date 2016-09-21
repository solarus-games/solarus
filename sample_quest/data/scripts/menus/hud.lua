-- Clickable head-up display.
local hud = {}

local icons = {
    item_1 = {
        surface = sol.surface.create("hud/icon_item_1.png"),
        x = 270,
        y = 225,
        command = "item_1",
        pressed = false,
    },
    attack = {
        surface = sol.surface.create("hud/icon_attack.png"),
        x = 295,
        y = 225,
        command = "attack",
        pressed = false,
    },
    action = {
        surface = sol.surface.create("hud/icon_action.png"),
        x = 282,
        y = 205,
        command = "action",
        pressed = false,
    },
}

function hud:create(game)

  self.game = game
  self.has_hud_button_pushed = false

  -- Clickable circle icons.
  icons.item_1.surface:set_opacity(216)
  icons.attack.surface:set_opacity(216)
end

function hud:on_mouse_pressed(button, x, y)

  if self.game == nil or self.game:get_map() == nil then
    return
  end

  -- If the mouse position is over a button, simulate the corresponding command.
  for _, icon in pairs(icons) do

    local icon_width, icon_height = icon.surface:get_size()
    if not self.is_hud_button_clicked and sol.main.get_distance(x, y, icon.x, icon.y) < icon_width / 2 then
      self.has_hud_button_pushed = true
      self:start_command(icon)
    end
  end
end

function hud:on_mouse_released(button, x, y)

  if self.game == nil or self.game:get_map() == nil then
    return
  end

  self.has_hud_button_pushed = false
  for _, icon in pairs(icons) do
    self:stop_command(icon)
  end
end

function hud:on_draw(screen)

  if self.game == nil or self.game:get_map() == nil then
    return
  end

  for _, icon in pairs(icons) do
    local icon_width, icon_height = icon.surface:get_size()
    icon.surface:draw(screen, icon.x - icon_width / 2, icon.y - icon_height / 2)
  end
end

function hud:start_command(icon)

  if not icon.pressed then
    icon.pressed = true
    self.game:simulate_command_pressed(icon.command)
  end
end

function hud:stop_command(icon)

  if icon.pressed then
    icon.pressed = false
    self.game:simulate_command_released(icon.command)
  end
end

function hud:has_button_pushed()

  return self.has_hud_button_pushed
end

return hud