local game = ...

local inventory_builder = require("menus/pause_inventory")
local map_builder = require("menus/pause_map")
local quest_status_builder = require("menus/pause_quest_status")
local options_builder = require("menus/pause_options")

function game:start_pause_menu()

  self.pause_submenus = {
    inventory_builder:new(self),
    map_builder:new(self),
    quest_status_builder:new(self),
    options_builder:new(self),
  }

  local submenu_index = self:get_value("pause_last_submenu") or 1
  if submenu_index <= 0
      or submenu_index > #self.pause_submenus then
    submenu_index = 1
  end
  self:set_value("pause_last_submenu", submenu_index)

  sol.audio.play_sound("pause_open")
  sol.menu.start(self, self.pause_submenus[submenu_index])
end

function game:stop_pause_menu()

  sol.audio.play_sound("pause_closed")
  local submenu_index = self:get_value("pause_last_submenu")
  sol.menu.stop(self.pause_submenus[submenu_index])
  self.pause_submenus = {}
  self:set_custom_command_effect("action", nil)
  self:set_custom_command_effect("attack", nil)
end

