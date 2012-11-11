local game = ...

function game:on_started()

  local hearts_class = require("hud/hearts")
  local magic_bar_class = require("hud/magic_bar")
  local rupees_class = require("hud/rupees")

  -- Set up the HUD.
  self.hud = {}

  self.hud.hearts = hearts_class:new(self)
  self.hud.hearts:set_dst_position(-104, 6)

  self.hud.magic_bar = magic_bar_class:new(self)
  self.hud.magic_bar:set_dst_position(-104, 27)

  self.hud.rupees = rupees_class:new(self)
  self.hud.rupees:set_dst_position(8, -20)

  self:set_hud_enabled(true)
end

-- Useful functions for this quest.

function game:get_player_name()
  return game:get_value("player_name")
end

function game:set_player_name(player_name)
  game:set_value("player_name", player_name)
end

function game:is_dungeon_finished(dungeon)
  return game:get_value("dungeon_" .. dungeon .. ".finished")
end

function game:set_dungeon_finished(dungeon, finished)
  if finished == nil then
    finished = true
  end
  game:set_value("dungeon_" .. dungeon .. ".finished", finished)
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

-- Returns the item name of a bottle with the specified content, or nil.
function game:get_first_bottle_with(variant)

  for i = 1, 4 do
    local item = self:get_item("bottle_" .. i)
    if item:get_variant() == variant then
      return item
    end
  end

  return nil
end

function game:get_first_empty_bottle()
  return self:get_first_bottle_with(1)
end

function game:has_bottle()

  for i = 1, 4 do
    local item = self:get_item("bottle_" .. i)
    if item:has_variant() then
      return item
    end
  end

  return nil
end

function game:has_bottle_with(variant)

  return self:get_first_bottle_with(variant) ~= nil
end

-- Run the game.
sol.main.game = game
game:start()

