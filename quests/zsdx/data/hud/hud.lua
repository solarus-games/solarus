local game = ...

function game:initialize_hud()

  -- Set up the HUD.
  local hearts_class = require("hud/hearts")
  local magic_bar_class = require("hud/magic_bar")
  local rupees_class = require("hud/rupees")
  local small_keys_class = require("hud/small_keys")
  local floor_class = require("hud/floor")

  self.hud = {}

  self.hud.hearts = hearts_class:new(self)
  self.hud.hearts:set_dst_position(-104, 6)

  self.hud.magic_bar = magic_bar_class:new(self)
  self.hud.magic_bar:set_dst_position(-104, 27)

  self.hud.rupees = rupees_class:new(self)
  self.hud.rupees:set_dst_position(8, -20)

  self.hud.small_keys = small_keys_class:new(self)
  self.hud.small_keys:set_dst_position(-36, -18)

  self.hud.floor = floor_class:new(self)
  self.hud.floor:set_dst_position(5, 70)

  self:set_hud_enabled(true)
end

function game:hud_on_map_changed(map)

  if self:is_hud_enabled() then
    for _, menu in pairs(self.hud) do
      if menu.on_map_changed ~= nil then
        menu:on_map_changed(map)
      end
    end
  end
end

function game:is_hud_enabled()
  return self.hud_enabled
end

function game:set_hud_enabled(hud_enabled)

  if hud_enabled ~= game.hud_enabled then
    game.hud_enabled = hud_enabled

    for _, menu in pairs(self.hud) do
      if hud_enabled then
	sol.menu.start(self, menu)
      else
	sol.menu.stop(menu)
      end
    end
  end
end

