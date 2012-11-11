-- This is a Lua items list file for solarus 1.0.0 or greater.
-- This file was converted from the 0.9.2 ini syntax using the script
-- tools/data_files_conversion/0.9.2_to_1.0.0/convert_items.lua.

item{
  name = "feather",
  savegame_variable = "1100",
  can_be_assigned = true,
}

item{
  name = "bombs_counter",
  savegame_variable = "1101",
  can_be_assigned = true,
  counter = "1024",
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
  savegame_variable = "1032",
  nb_variants = 3,
  limit_for_counter = "bombs_counter",
  amount_1 = 10,
  amount_2 = 11,
  amount_3 = 12,
}

item{
  name = "bow",
  savegame_variable = "1102",
  nb_variants = 2,
  can_be_assigned = true,
  counter = "1025",
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
  savegame_variable = "1033",
  nb_variants = 3,
  limit_for_counter = "bow",
  amount_1 = 10,
  amount_2 = 30,
  amount_3 = 99,
}

item{
  name = "boomerang",
  savegame_variable = "1103",
  can_be_assigned = true,
}

item{
  name = "lamp",
  savegame_variable = "1104",
  can_be_assigned = true,
}

item{
  name = "hookshot",
  savegame_variable = "1105",
  can_be_assigned = true,
}

item{
  name = "bottle_1",
  savegame_variable = "1106",
  nb_variants = 6,
  can_be_assigned = true,
}

item{
  name = "pegasus_shoes",
  savegame_variable = "1107",
  can_be_assigned = true,
}

item{
  name = "mystic_mirror",
  savegame_variable = "1108",
  can_be_assigned = true,
}

item{
  name = "cane_of_somaria",
  savegame_variable = "1109",
  can_be_assigned = true,
}

item{
  name = "apples_counter",
  savegame_variable = "1110",
  can_be_assigned = true,
  counter = "1028",
  limit = 10,
}

item{
  name = "apple",
  changes_counter = "apples_counter",
  amount = 3,
}

item{
  name = "pains_au_chocolat_counter",
  savegame_variable = "1111",
  can_be_assigned = true,
  counter = "1026",
  limit = 10,
}

item{
  name = "pain_au_chocolat",
  changes_counter = "pains_au_chocolat_counter",
}

item{
  name = "croissants_counter",
  savegame_variable = "1112",
  can_be_assigned = true,
  counter = "1027",
  limit = 10,
}

item{
  name = "croissant",
  changes_counter = "croissants_counter",
}

item{
  name = "bottle_2",
  savegame_variable = "1113",
  nb_variants = 6,
  can_be_assigned = true,
}

item{
  name = "rock_key",
  savegame_variable = "1114",
}

item{
  name = "bone_key",
  savegame_variable = "1115",
}

item{
  name = "clay_key",
  savegame_variable = "1116",
}

item{
  name = "apple_pie_counter",
  savegame_variable = "1117",
  counter = "1132",
  limit = 50,
}

item{
  name = "apple_pie",
  changes_counter = "apple_pie_counter",
}

item{
  name = "flippers",
  savegame_variable = "1118",
}

item{
  name = "magic_cape",
  savegame_variable = "1119",
}

item{
  name = "bottle_3",
  savegame_variable = "1120",
  nb_variants = 6,
  can_be_assigned = true,
}

item{
  name = "iron_key",
  savegame_variable = "1121",
}

item{
  name = "stone_key",
  savegame_variable = "1122",
}

item{
  name = "wooden_key",
  savegame_variable = "1123",
}

item{
  name = "ice_key",
  savegame_variable = "1124",
}

item{
  name = "glove",
  savegame_variable = "1125",
  nb_variants = 2,
}

item{
  name = "fire_stones_counter",
  savegame_variable = "1126",
  counter = "1029",
  limit = 3,
}

item{
  name = "fire_stone",
  changes_counter = "fire_stones_counter",
}

item{
  name = "bottle_4",
  savegame_variable = "1127",
  nb_variants = 6,
  can_be_assigned = true,
}

item{
  name = "tunic",
  savegame_variable = "1128",
  nb_variants = 3,
  initial_variant = 1,
}

item{
  name = "sword",
  savegame_variable = "1129",
  nb_variants = 4,
}

item{
  name = "shield",
  savegame_variable = "1130",
  nb_variants = 3,
}

item{
  name = "world_map",
  savegame_variable = "1031",
}

item{
  name = "rupee_bag",
  savegame_variable = "1034",
  nb_variants = 3,
  initial_variant = 1,
  limit_for_counter = "money",
  amount_1 = 100,
  amount_2 = 300,
  amount_3 = 999,
}

item{
  name = "magic_bar",
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
  changes_counter = "small_keys",
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
  name = "poivron_counter",
  savegame_variable = "1481",
  counter = "1471",
  limit = 50,
}

item{
  name = "poivron",
  changes_counter = "poivron_counter",
}

item{
  name = "balai_counter",
  savegame_variable = "1482",
  counter = "1472",
  limit = 50,
}

item{
  name = "balai",
  changes_counter = "balai_counter",
}

item{
  name = "cuillere_counter",
  savegame_variable = "1483",
  counter = "1473",
  limit = 50,
}

item{
  name = "cuillere",
  changes_counter = "cuillere_counter",
}

item{
  name = "tapisserie_counter",
  savegame_variable = "1484",
  counter = "1474",
  limit = 50,
}

item{
  name = "tapisserie",
  changes_counter = "tapisserie_counter",
}

item{
  name = "roc_magma_counter",
  savegame_variable = "1485",
  counter = "1475",
  limit = 50,
}

item{
  name = "roc_magma",
  changes_counter = "roc_magma_counter",
}

item{
  name = "sac_riz_counter",
  savegame_variable = "1486",
  counter = "1476",
  limit = 50,
}

item{
  name = "sac_riz",
  changes_counter = "sac_riz_counter",
}

item{
  name = "sac_olive_counter",
  savegame_variable = "1487",
  counter = "1477",
  limit = 50,
}

item{
  name = "sac_olive",
  changes_counter = "sac_olive_counter",
}

item{
  name = "bocal_epice_counter",
  savegame_variable = "1488",
  counter = "1478",
  limit = 50,
}

item{
  name = "bocal_epice",
  changes_counter = "bocal_epice_counter",
}

item{
  name = "hache_counter",
  savegame_variable = "1489",
  counter = "1479",
  limit = 50,
}

item{
  name = "hache",
  changes_counter = "hache_counter",
}

item{
  name = "parfum_counter",
  savegame_variable = "1490",
  counter = "1480",
  limit = 50,
}

item{
  name = "parfum",
  changes_counter = "parfum_counter",
}

item{
  name = "zelda",
  savegame_variable = "1131",
}

