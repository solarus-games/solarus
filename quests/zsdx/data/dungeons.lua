local game = ...

-- Define the existing dungeons and their floors for the minimap menu.
game.dungeons = {
  [1] = {
    floor_width = 1040,
    floor_height = 696,
    lowest_floor = -1,
    highest_floor = 1,
    maps = { "23", "24", "25", "27" },
    boss = {
      floor = -1,
      x = 648,
      y = 144,
      savegame_variable = "b63",
    },
  },
  [2] = {
    floor_width = 1152,
    floor_height = 768,
    lowest_floor = -1,
    highest_floor = 0,
    maps = { "30", "31" },
    boss = {
      floor = -1,
      x = 960,
      y = 365,
      savegame_variable = "b93",
    },
  },
  [3] = {
    floor_width = 1024,
    floor_height = 1024,
    lowest_floor = 0,
    highest_floor = 4,
    maps = { "40", "41", "42", "43", "85" },
    boss = {
      floor = 3,
      x = 252,
      y = 533,
      savegame_variable = "b902",
    },
  },
  [4] = {
    floor_width = 960,
    floor_height = 1120,
    lowest_floor = -1,
    highest_floor = 0,
    maps = { "59", "60", "62" },
    boss = {
      floor = 0,
      x = 200,
      y = 464,
      savegame_variable = "b415",
    },
  },
  [5] = {
    floor_width = 1152,
    floor_height = 1216,
    lowest_floor = -1,
    highest_floor = 1,
    maps = { "63", "64", "65" },
  },
  [6] = {
    floor_width = 1152,
    floor_height = 768,
    lowest_floor = 0,
    highest_floor = 3,
    maps = { "55", "56", "57", "58" },
    boss = {
      floor = 3,
      x = 272,
      y = 224,
      savegame_variable = "b321",
    },
  },
  [7] = {
    floor_width = 1104,
    floor_height = 1056,
    lowest_floor = 0,
    highest_floor = 2,
    maps = { "101", "102", "103" },
    boss = {
      floor = 2,
      x = 544,
      y = 784,
      savegame_variable = "b625",
    },
  },
  [8] = {
    floor_width = 2256,
    floor_height = 1808,
    lowest_floor = -4,
    highest_floor = 0,
    maps = { "47", "48", "49", "50", "51", "52", "53" },
    boss = {
      floor = 0,
      x = 1128,
      y = 125,
      savegame_variable = "b727",
    },
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
    maps = { "17", "97", "98", "99" },
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

function game:is_dungeon_finished(dungeon_index)
  return self:get_value("dungeon_" .. dungeon_index .. "_finished")
end

function game:set_dungeon_finished(dungeon_index, finished)
  if finished == nil then
    finished = true
  end
  self:set_value("dungeon_" .. dungeon_index .. "_finished", finished)
end

function game:has_dungeon_map(dungeon_index)

  dungeon_index = dungeon_index or self:get_dungeon_index()
  return self:get_value("dungeon_" .. dungeon_index .. "_map")
end

function game:has_dungeon_compass(dungeon_index)

  dungeon_index = dungeon_index or self:get_dungeon_index()
  return self:get_value("dungeon_" .. dungeon_index .. "_compass")
end

function game:has_dungeon_big_key(dungeon_index)

  dungeon_index = dungeon_index or self:get_dungeon_index()
  return self:get_value("dungeon_" .. dungeon_index .. "_big_key")
end

function game:has_dungeon_boss_key(dungeon_index)

  dungeon_index = dungeon_index or self:get_dungeon_index()
  return self:get_value("dungeon_" .. dungeon_index .. "_boss_key")
end

