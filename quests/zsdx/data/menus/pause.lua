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
  self.pause_submenu_index = self:get_value("pause_last_submenu") or 1
  if self.pause_submenu_index <= 0
      or self.pause_submenu_index > #self.pause_submenus then
    self.pause_submenu_index = 1
  end

  sol.audio.play_sound("pause_open")
  sol.menu.start(self, self.pause_submenus[self.pause_submenu_index])
end

function game:stop_pause_menu()

  sol.audio.play_sound("pause_closed")
  sol.menu.stop(self.pause_submenus[self.pause_submenu_index])
  self.pause_submenus = {}
  self:set_value("pause_last_submenu", self.pause_submenu_index)
end

function game:next_pause_submenu()

  sol.audio.play_sound("pause_closed")
  sol.menu.stop(self.pause_submenus[self.pause_submenu_index])
  self.pause_submenu_index = (self.pause_submenu_index % 4) + 1
  sol.menu.start(self, self.pause_submenus[self.pause_submenu_index])
end

function game:previous_pause_submenu()

  sol.audio.play_sound("pause_closed")
  sol.menu.stop(self.pause_submenus[self.pause_submenu_index])
  self.pause_submenu_index = (self.pause_submenu_index + 2) % 4 + 1
  sol.menu.start(self, self.pause_submenus[self.pause_submenu_index])
end

