local game = ...

-- Define the existing dungeons and their floors for the minimap menu.
game.dungeons = {
  [1] = {
    floor_width = 1040,
    floor_height = 696,
    lowest_floor = -1,
    highest_floor = 1,
    maps = { "23", "24", "25", "27" }
  },
  [2] = {
    floor_width = 1152,
    floor_height = 768,
    lowest_floor = -1,
    highest_floor = 0,
    maps = { "30", "31" }
  },
  [3] = {
    floor_width = 1024,
    floor_height = 1024,
    lowest_floor = 0,
    highest_floor = 4,
    maps = { "40", "41", "42", "43", "85" },
  },
  [4] = {
    floor_width = 960,
    floor_height = 1120,
    lowest_floor = -1,
    highest_floor = 0,
    maps = { "59", "60", "62" }
  },
  [5] = {
    floor_width = 1152,
    floor_height = 1216,
    lowest_floor = -1,
    highest_floor = 1,
    maps = { "63", "64", "65" }
  },
  [6] = {
    floor_width = 1152,
    floor_height = 768,
    lowest_floor = 0,
    highest_floor = 3,
    maps = { "55", "56", "57", "58" }
  },
  [7] = {
    floor_width = 1104,
    floor_height = 1056,
    lowest_floor = 0,
    highest_floor = 2,
    maps = { "101", "102", "103" },
  },
  [8] = {
    floor_width = 2256,
    floor_height = 1808,
    lowest_floor = -4,
    highest_floor = 0,
    maps = { "47", "48", "49", "50", "51", "52", "53" },
  },
  [9] = {
    floor_width = 2576,
    floor_height = 2320,
    lowest_floor = -1,
    highest_floor = 3,
    maps = { "104", "105", "106", "107", "108", "109", "110", "130" },
  },
  [10] = {
    floor_width = 1032,
    floor_height = 1032,
    lowest_floor = -1,
    highest_floor = 4,
    maps = { "17", "97", "98", "99" }
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

