local game = ...

function game:initialize_hud()

  -- Set up the HUD.
  local hearts_builder = require("hud/hearts")
  local magic_bar_builder = require("hud/magic_bar")
  local rupees_builder = require("hud/rupees")
  local small_keys_builder = require("hud/small_keys")
  local floor_builder = require("hud/floor")
  local attack_icon_builder = require("hud/attack_icon")
  local pause_icon_builder = require("hud/pause_icon")
  local item_icon_builder = require("hud/item_icon")

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

  self.hud.item_icon_1 = item_icon_builder:new(self, 1)
  self.hud.item_icon_1:set_dst_position(11, 29)

  self.hud.item_icon_2 = item_icon_builder:new(self, 2)
  self.hud.item_icon_2:set_dst_position(63, 29)

  self.hud.attack_icon = attack_icon_builder:new(self)
  self.hud.attack_icon:set_dst_position(13, 29)

  self:set_hud_enabled(true)

  self:check_hud()
end

function game:check_hud()

  -- If the hero is below the top-left icons, make them semi-transparent.
  if self:get_map() ~= nil then
    local hero = self:get_map():get_entity("hero")
    local x, y = hero:get_position()
    local opacity = nil

    if self.opacity == 255
        and not self:is_suspended()
        and x < 88
        and y < 80 then
      opacity = 96
    elseif self.opacity == 96
        or self:is_suspended()
        or x >= 88
        or y >= 80 then
      opacity = 255
    end

    if opacity ~= nil then
      self.hud.pause_icon.surface:set_opacity(opacity)
      self.hud.item_icon_1.surface:set_opacity(opacity)
      self.hud.item_icon_2.surface:set_opacity(opacity)
    end
  end

  sol.timer.start(self, 50, function()
    self:check_hud()
  end)
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

