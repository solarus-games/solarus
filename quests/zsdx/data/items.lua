-- This is a Lua items list file for solarus 1.0.0 or greater.
-- This file was converted from the 0.9.2 ini syntax using the script
-- tools/data_files_conversion/0.9.2_to_1.0.0/convert_items.lua.

item{
  name = "feather",
  savegame_variable = "i1100",
  can_be_assigned = true,
}

item{
  name = "bombs_counter",
  savegame_variable = "i1101",
  can_be_assigned = true,
  counter = "i1024",
}

item{
  name = "bomb",
  nb_variants = 3,
  changes_counter = "bombs_counter",
  can_disappear = true,
  brandish_when_picked = false,
  amount_1 = 1,
  probability_1 = 8,
  amount_2 = 3,
  probability_2 = 5,
  amount_3 = 8,
  probability_3 = 2,
}

item{
  name = "bomb_bag",
  savegame_variable = "i1032",
  nb_variants = 3,
  limit_for_counter = "bombs_counter",
  amount_1 = 10,
  amount_2 = 30,
  amount_3 = 99,
}

item{
  name = "bow",
  savegame_variable = "i1102",
  nb_variants = 2,
  can_be_assigned = true,
  counter = "i1025",
}

item{
  name = "arrow",
  nb_variants = 3,
  changes_counter = "bow",
  shadow = "small",
  can_disappear = true,
  brandish_when_picked = false,
  amount_1 = 1,
  probability_1 = 8,
  amount_2 = 5,
  probability_2 = 5,
  amount_3 = 10,
  probability_3 = 2,
}

item{
  name = "quiver",
  savegame_variable = "i1033",
  nb_variants = 3,
  limit_for_counter = "bow",
  amount_1 = 10,
  amount_2 = 30,
  amount_3 = 99,
}

item{
  name = "boomerang",
  savegame_variable = "i1103",
  nb_variants = 2,
  can_be_assigned = true,
}

item{
  name = "lamp",
  savegame_variable = "i1104",
  can_be_assigned = true,
}

item{
  name = "hookshot",
  savegame_variable = "i1105",
  can_be_assigned = true,
}

item{
  name = "bottle_1",
  savegame_variable = "i1106",
  nb_variants = 6,
  can_be_assigned = true,
}

item{
  name = "pegasus_shoes",
  savegame_variable = "i1107",
  can_be_assigned = true,
}

item{
  name = "mystic_mirror",
  savegame_variable = "i1108",
  can_be_assigned = true,
}

item{
  name = "cane_of_somaria",
  savegame_variable = "i1109",
  can_be_assigned = true,
}

item{
  name = "apples_counter",
  savegame_variable = "i1110",
  can_be_assigned = true,
  counter = "i1028",
  limit = 10,
}

item{
  name = "apple",
  changes_counter = "apples_counter",
  amount = 3,
}

item{
  name = "pains_au_chocolat_counter",
  savegame_variable = "i1111",
  can_be_assigned = true,
  counter = "i1026",
  limit = 10,
}

item{
  name = "pain_au_chocolat",
  changes_counter = "pains_au_chocolat_counter",
}

item{
  name = "croissants_counter",
  savegame_variable = "i1112",
  can_be_assigned = true,
  counter = "i1027",
  limit = 10,
}

item{
  name = "croissant",
  changes_counter = "croissants_counter",
}

item{
  name = "bottle_2",
  savegame_variable = "i1113",
  nb_variants = 6,
  can_be_assigned = true,
}

item{
  name = "rock_key",
  savegame_variable = "i1114",
}

item{
  name = "bone_key",
  savegame_variable = "i1115",
}

item{
  name = "clay_key",
  savegame_variable = "i1116",
}

item{
  name = "level_4_way",
  savegame_variable = "i1117",
  nb_variants = 4,
}

item{
  name = "flippers",
  savegame_variable = "i1118",
}

item{
  name = "magic_cape",
  savegame_variable = "i1119",
}

item{
  name = "bottle_3",
  savegame_variable = "i1120",
  nb_variants = 6,
  can_be_assigned = true,
}

item{
  name = "iron_key",
  savegame_variable = "i1121",
}

item{
  name = "stone_key",
  savegame_variable = "i1122",
}

item{
  name = "wooden_key",
  savegame_variable = "i1123",
}

item{
  name = "ice_key",
  savegame_variable = "i1124",
}

item{
  name = "glove",
  savegame_variable = "i1125",
  nb_variants = 2,
}

item{
  name = "fire_stones_counter",
  savegame_variable = "i1126",
  counter = "i1029",
  limit = 3,
}

item{
  name = "fire_stone",
  changes_counter = "fire_stones_counter",
}

item{
  name = "bottle_4",
  savegame_variable = "i1127",
  nb_variants = 6,
  can_be_assigned = true,
}

item{
  name = "tunic",
  savegame_variable = "i1128",
  nb_variants = 3,
  initial_variant = 1,
}

item{
  name = "sword",
  savegame_variable = "i1129",
  nb_variants = 4,
  sound_when_picked = "",
  sound_when_brandished = "",
}

item{
  name = "shield",
  savegame_variable = "i1130",
  nb_variants = 3,
}

item{
  name = "world_map",
  savegame_variable = "i1031",
}

item{
  name = "rupee_bag",
  savegame_variable = "i1034",
  nb_variants = 3,
  initial_variant = 1,
  limit_for_counter = "money",
  amount_1 = 100,
  amount_2 = 300,
  amount_3 = 999,
}

item{
  name = "magic_bar",
  savegame_variable = "i1035",
  nb_variants = 2,
  initial_variant = 0,
  limit_for_counter = "magic",
  amount_1 = 42,
  amount_2 = 84,
}

item{
  name = "map",
}

item{
  name = "compass",
}

item{
  name = "big_key",
}

item{
  name = "boss_key",
}

item{
  name = "small_key",
  shadow = "small",
  brandish_when_picked = false,
  sound_when_picked = "picked_small_key",
}

item{
  name = "rupee",
  nb_variants = 6,
  changes_counter = "money",
  shadow = "small",
  can_disappear = true,
  brandish_when_picked = false,
  sound_when_picked = "picked_rupee",
  amount_1 = 1,
  probability_1 = 50,
  amount_2 = 5,
  probability_2 = 15,
  amount_3 = 20,
  probability_3 = 5,
  amount_4 = 50,
  amount_5 = 100,
  amount_6 = 300,
}

item{
  name = "magic_flask",
  nb_variants = 2,
  changes_counter = "magic",
  shadow = "small",
  can_disappear = true,
  brandish_when_picked = false,
  amount_1 = 6,
  probability_1 = 25,
  amount_2 = 42,
  probability_2 = 8,
}

item{
  name = "heart",
  changes_counter = "life",
  amount = 4,
  probability = 100,
  shadow = "small",
  can_disappear = true,
  brandish_when_picked = false,
}

item{
  name = "heart_container",
  sound_when_picked = "heart_container",
  sound_when_brandished = "heart_container",
}

item{
  name = "piece_of_heart",
  sound_when_picked = "piece_of_heart",
  sound_when_brandished = "piece_of_heart",
}

item{
  name = "fairy",
  probability = 2,
  shadow = "none",
  can_disappear = true,
  brandish_when_picked = false,
}

item{
  name = "red_potion",
}

item{
  name = "green_potion",
}

item{
  name = "blue_potion",
}

