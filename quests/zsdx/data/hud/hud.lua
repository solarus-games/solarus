local game = ...

function game:initialize_hud()

  -- Set up the HUD.
  local hearts_builder = require("hud/hearts")
  local magic_bar_builder = require("hud/magic_bar")
  local rupees_builder = require("hud/rupees")
  local small_keys_builder = require("hud/small_keys")
  local floor_builder = require("hud/floor")
  local pause_icon_builder = require("hud/pause_icon")

  self.hud = {}

  self.hud.hearts = hearts_builder:new(self)
  self.hud.hearts:set_dst_position(-104, 6)

  self.hud.magic_bar = magic_bar_builder:new(self)
  self.hud.magic_bar:set_dst_position(-104, 27)

  self.hud.rupees = rupees_builder:new(self)
  self.hud.rupees:set_dst_position(8, -20)

  self.hud.small_keys = small_keys_builder:new(self)
  self.hud.small_keys:set_dst_position(-36, -18)

  self.hud.floor = floor_builder:new(self)
  self.hud.floor:set_dst_position(5, 70)

  self.hud.pause_icon = pause_icon_builder:new(self)
  self.hud.pause_icon:set_dst_position(0, 7)

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

function game:hud_on_paused()

  if self:is_hud_enabled() then
    for _, menu in pairs(self.hud) do
      if menu.on_paused ~= nil then
        menu:on_paused()
      end
    end
  end
end

function game:hud_on_unpaused()

  if self:is_hud_enabled() then
    for _, menu in pairs(self.hud) do
      if menu.on_unpaused ~= nil then
        menu:on_unpaused()
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

