properties{
  width = 320,
  height = 240,
  x = 0,
  y = 0,
  world = "inside_world",
  floor = nil,
  small_keys_variable = nil,
  tileset_id = "6",
  music_id = nil,
}

destination{
  layer = 0,
  x = 8,
  y = 13,
  name = "start_position",
  direction = 3,
  sprite_name = nil,
}

tile{
  layer = 1,
  x = 8,
  y = 13,
  width = 320,
  height = 240,
  pattern_id = 3,
}

tile{
  layer = 1,
  x = -48,
  y = 270,
  width = 320,
  height = 176,
  pattern_id = 2,
}

npc{
  layer = 1,
  x = 16,
  y = 21,
  name = "fresco",
  direction = 3,
  subtype = 0,
  sprite_name = "entities/fresco",
  behavior = "map",
}

dynamic_tile{
  layer = 2,
  x = 0,
  y = 0,
  width = 320,
  height = 240,
  name = "black_screen",
  pattern_id = 3,
  enabled_at_start = true,
}

