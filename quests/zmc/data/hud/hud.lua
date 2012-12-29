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
  local action_icon_builder = require("hud/action_icon")

  self.hud = {  -- Array for the hud elements, table for other hud info.
    showing_dialog = false,
    top_left_opacity = 255,
    custom_command_effects = {},
  }

  local menu = hearts_builder:new(self)
  menu:set_dst_position(-104, 6)
  self.hud[#self.hud + 1] = menu

  menu = magic_bar_builder:new(self)
  menu:set_dst_position(-104, 27)
  self.hud[#self.hud + 1] = menu

  menu = rupees_builder:new(self)
  menu:set_dst_position(8, -20)
  self.hud[#self.hud + 1] = menu

  menu = small_keys_builder:new(self)
  menu:set_dst_position(-36, -18)
  self.hud[#self.hud + 1] = menu

  menu = floor_builder:new(self)
  menu:set_dst_position(5, 70)
  self.hud[#self.hud + 1] = menu

  menu = pause_icon_builder:new(self)
  menu:set_dst_position(0, 7)
  self.hud[#self.hud + 1] = menu
  self.hud.pause_icon = menu

  menu = item_icon_builder:new(self, 1)
  menu:set_dst_position(11, 29)
  self.hud[#self.hud + 1] = menu
  self.hud.item_icon_1 = menu

  menu = item_icon_builder:new(self, 2)
  menu:set_dst_position(63, 29)
  self.hud[#self.hud + 1] = menu
  self.hud.item_icon_2 = menu

  menu = attack_icon_builder:new(self)
  menu:set_dst_position(13, 29)
  self.hud[#self.hud + 1] = menu
  self.hud.attack_icon = menu

  menu = action_icon_builder:new(self)
  menu:set_dst_position(26, 51)
  self.hud[#self.hud + 1] = menu
  self.hud.action_icon = menu

  self:set_hud_enabled(true)

  self:check_hud()
end

function game:check_hud()

  local map = self:get_map()
  if map ~= nil then
    -- If the hero is below the top-left icons, make them semi-transparent.
    local hero = map:get_entity("hero")
    local x, y = hero:get_position()
    local opacity = nil

    if self.hud.top_left_opacity == 255
        and not self:is_suspended()
        and x < 88
        and y < 80 then
      opacity = 96
    elseif self.hud.top_left_opacity == 96
        and (self:is_suspended()
        or x >= 88
        or y >= 80) then
      opacity = 255
    end

    if opacity ~= nil then
      self.hud.top_left_opacity = opacity
      self.hud.item_icon_1.surface:set_opacity(opacity)
      self.hud.item_icon_2.surface:set_opacity(opacity)
      self.hud.pause_icon.surface:set_opacity(opacity)
      self.hud.attack_icon.surface:set_opacity(opacity)
      self.hud.action_icon.surface:set_opacity(opacity)
    end

    -- During a dialog, move the action icon and the sword icon.
    if not self.hud.showing_dialog and
        map:is_dialog_enabled() then
      self.hud.showing_dialog = true
      self.hud.action_icon:set_dst_position(0, 54)
      self.hud.attack_icon:set_dst_position(0, 29)
    elseif self.hud.showing_dialog and
        not map:is_dialog_enabled() then
      self.hud.showing_dialog = false
      self.hud.action_icon:set_dst_position(26, 51)
      self.hud.attack_icon:set_dst_position(13, 29)
    end
  end

  sol.timer.start(self, 50, function()
    self:check_hud()
  end)
end

function game:hud_on_map_changed(map)

  if self:is_hud_enabled() then
    for _, menu in ipairs(self.hud) do
      if menu.on_map_changed ~= nil then
        menu:on_map_changed(map)
      end
    end
  end
end

function game:hud_on_paused()

  if self:is_hud_enabled() then
    for _, menu in ipairs(self.hud) do
      if menu.on_paused ~= nil then
        menu:on_paused()
      end
    end
  end
end

function game:hud_on_unpaused()

  if self:is_hud_enabled() then
    for _, menu in ipairs(self.hud) do
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

    for _, menu in ipairs(self.hud) do
      if hud_enabled then
	sol.menu.start(self, menu)
      else
	sol.menu.stop(menu)
      end
    end
  end
end

function game:get_custom_command_effect(command)

  return self.hud.custom_command_effects[command]
end

-- Make the action (or attack) icon show something else than the
-- built-in effect or the action (or attack) command.
-- You are responsible to override the command if you don't want the built-in
-- effect to be performed.
-- Set the effect to nil to show the built-in effect again.
function game:set_custom_command_effect(command, effect)

  self.hud.custom_command_effects[command] = effect
end

