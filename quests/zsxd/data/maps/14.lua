properties{
  x = 0,
  y = 0,
  width = 320,
  height = 240,
  world = "inside_world",
  tileset = "0",
  music = "fortune_teller",
}

tile{
  layer = 1,
  x = 88,
  y = 152,
  width = 24,
  height = 24,
  pattern = 47,
}

tile{
  layer = 1,
  x = 208,
  y = 152,
  width = 24,
  height = 24,
  pattern = 48,
}

tile{
  layer = 1,
  x = 112,
  y = 152,
  width = 96,
  height = 24,
  pattern = 52,
}

tile{
  layer = 1,
  x = 208,
  y = 88,
  width = 24,
  height = 64,
  pattern = 50,
}

tile{
  layer = 1,
  x = 208,
  y = 64,
  width = 24,
  height = 24,
  pattern = 46,
}

tile{
  layer = 1,
  x = 88,
  y = 64,
  width = 24,
  height = 24,
  pattern = 45,
}

tile{
  layer = 1,
  x = 112,
  y = 64,
  width = 96,
  height = 24,
  pattern = 49,
}

tile{
  layer = 1,
  x = 88,
  y = 88,
  width = 24,
  height = 64,
  pattern = 51,
}

tile{
  layer = 1,
  x = 112,
  y = 88,
  width = 8,
  height = 8,
  pattern = 14,
}

tile{
  layer = 1,
  x = 200,
  y = 88,
  width = 8,
  height = 8,
  pattern = 15,
}

tile{
  layer = 1,
  x = 200,
  y = 144,
  width = 8,
  height = 8,
  pattern = 17,
}

tile{
  layer = 1,
  x = 112,
  y = 144,
  width = 8,
  height = 8,
  pattern = 16,
}

tile{
  layer = 1,
  x = 120,
  y = 144,
  width = 80,
  height = 8,
  pattern = 21,
}

tile{
  layer = 1,
  x = 120,
  y = 88,
  width = 80,
  height = 8,
  pattern = 18,
}

tile{
  layer = 1,
  x = 112,
  y = 96,
  width = 8,
  height = 48,
  pattern = 20,
}

tile{
  layer = 1,
  x = 200,
  y = 96,
  width = 8,
  height = 48,
  pattern = 19,
}

tile{
  layer = 1,
  x = 152,
  y = 152,
  width = 16,
  height = 16,
  pattern = 77,
}

tile{
  layer = 1,
  x = 144,
  y = 152,
  width = 8,
  height = 16,
  pattern = 74,
}

tile{
  layer = 1,
  x = 168,
  y = 152,
  width = 8,
  height = 16,
  pattern = 75,
}

tile{
  layer = 1,
  x = 112,
  y = 152,
  width = 32,
  height = 24,
  pattern = 133,
}

tile{
  layer = 1,
  x = 176,
  y = 152,
  width = 32,
  height = 24,
  pattern = 133,
}

tile{
  layer = 1,
  x = 152,
  y = 168,
  width = 16,
  height = 32,
  pattern = 2,
}

tile{
  layer = 1,
  x = 168,
  y = 176,
  width = 8,
  height = 16,
  pattern = 71,
}

tile{
  layer = 1,
  x = 144,
  y = 176,
  width = 8,
  height = 16,
  pattern = 71,
}

tile{
  layer = 1,
  x = 152,
  y = 192,
  width = 16,
  height = 8,
  pattern = 70,
}

tile{
  layer = 1,
  x = 120,
  y = 96,
  width = 80,
  height = 48,
  pattern = 3,
}

tile{
  layer = 1,
  x = 128,
  y = 112,
  width = 16,
  height = 16,
  pattern = 270,
}

tile{
  layer = 1,
  x = 176,
  y = 112,
  width = 16,
  height = 16,
  pattern = 270,
}

tile{
  layer = 1,
  x = 112,
  y = 64,
  width = 16,
  height = 16,
  pattern = 38,
}

tile{
  layer = 1,
  x = 192,
  y = 64,
  width = 16,
  height = 16,
  pattern = 38,
}

tile{
  layer = 1,
  x = 216,
  y = 88,
  width = 16,
  height = 16,
  pattern = 39,
}

tile{
  layer = 1,
  x = 216,
  y = 136,
  width = 16,
  height = 16,
  pattern = 39,
}

tile{
  layer = 1,
  x = 88,
  y = 88,
  width = 16,
  height = 16,
  pattern = 40,
}

tile{
  layer = 1,
  x = 88,
  y = 136,
  width = 16,
  height = 16,
  pattern = 40,
}

tile{
  layer = 1,
  x = 168,
  y = 64,
  width = 16,
  height = 32,
  pattern = 43,
}

tile{
  layer = 1,
  x = 152,
  y = 64,
  width = 16,
  height = 32,
  pattern = 44,
}

tile{
  layer = 1,
  x = 136,
  y = 64,
  width = 16,
  height = 32,
  pattern = 42,
}

destination{
  layer = 1,
  x = 160,
  y = 165,
  name = "from_outside",
  direction = 1,
}

npc{
  layer = 1,
  x = 160,
  y = 125,
  name = "wizard_2",
  direction = 3,
  subtype = 1,
  sprite = "npc/wizard",
  behavior = "dialog#desert.small_house",
}

teletransporter{
  layer = 1,
  x = 152,
  y = 176,
  width = 16,
  height = 16,
  name = "teletransporter",
  transition = 1,
  destination_map = "2",
  destination = "from_desert_small_house",
}

tile{
  layer = 2,
  x = 112,
  y = 168,
  width = 96,
  height = 8,
  pattern = 263,
}

tile{
  layer = 2,
  x = 144,
  y = 168,
  width = 32,
  height = 8,
  pattern = 153,
}

tile{
  layer = 2,
  x = 0,
  y = 0,
  width = 320,
  height = 64,
  pattern = 170,
}

tile{
  layer = 2,
  x = 0,
  y = 176,
  width = 320,
  height = 64,
  pattern = 170,
}

tile{
  layer = 2,
  x = 0,
  y = 64,
  width = 88,
  height = 112,
  pattern = 170,
}

tile{
  layer = 2,
  x = 232,
  y = 64,
  width = 88,
  height = 112,
  pattern = 170,
}

