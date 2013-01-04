local game = ...

-- Define the existing dungeons and their floors for the minimap menu.
game.dungeons = {
  [3] = {
    floor_width = 1280,
    floor_height = 960,
    lowest_floor = -1,
    highest_floor = 3,
    maps = { "lighthouse_b1", "lighthouse_1f", "lighthouse_2f", "lighthouse_3f", "lighthouse_4f" },
  },
}

-- Returns the index of the current dungeon if any, or nil.
function game:get_dungeon_index()
  local world = self:get_map():get_world()
  local index = tonumber(world:match("^dungeon_([0-9]+)$"))
  return index
end

-- Returns the current dungeon if any, or nil.
function game:get_dungeon()
  local index = self:get_dungeon_index()
  return self.dungeons[index]
end

function game:is_dungeon_finished(dungeon)
  return self:get_value("dungeon_" .. dungeon .. "_finished")
end

function game:set_dungeon_finished(dungeon, finished)
  if finished == nil then
    finished = true
  end
  self:set_value("dungeon_" .. dungeon .. "_finished", finished)
end

