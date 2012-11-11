-- This is a Lua items list file for solarus 1.0.0 or greater.
-- This file was converted from the 0.9.2 ini syntax using the script
-- tools/data_files_conversion/0.9.2_to_1.0.0/convert_items.lua.

item{
  name = "feather",
  savegame_variable = "i1100",
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "bombs_counter",
  savegame_variable = "i1101",
  can_be_assigned = true,
  counter = "i1024",
  brandish_when_picked = true,
}

item{
  name = "bomb",
  nb_variants = 3,
  changes_counter = "bombs_counter",
  can_disappear = true,
  brandish_when_picked = true,
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
  brandish_when_picked = true,
  amount_1 = 10,
  amount_2 = 11,
  amount_3 = 12,
}

item{
  name = "bow",
  savegame_variable = "i1102",
  nb_variants = 2,
  can_be_assigned = true,
  counter = "i1025",
  brandish_when_picked = true,
}

item{
  name = "arrow",
  nb_variants = 3,
  changes_counter = "bow",
  shadow = "small",
  can_disappear = true,
  brandish_when_picked = true,
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
  brandish_when_picked = true,
  amount_1 = 10,
  amount_2 = 30,
  amount_3 = 99,
}

item{
  name = "boomerang",
  savegame_variable = "i1103",
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "lamp",
  savegame_variable = "i1104",
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "hookshot",
  savegame_variable = "i1105",
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "bottle_1",
  savegame_variable = "i1106",
  nb_variants = 6,
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "pegasus_shoes",
  savegame_variable = "i1107",
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "mystic_mirror",
  savegame_variable = "i1108",
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "cane_of_somaria",
  savegame_variable = "i1109",
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "apples_counter",
  savegame_variable = "i1110",
  can_be_assigned = true,
  counter = "i1028",
  limit = 10,
  brandish_when_picked = true,
}

item{
  name = "apple",
  changes_counter = "apples_counter",
  amount = 3,
  brandish_when_picked = true,
}

item{
  name = "pains_au_chocolat_counter",
  savegame_variable = "i1111",
  can_be_assigned = true,
  counter = "i1026",
  limit = 10,
  brandish_when_picked = true,
}

item{
  name = "pain_au_chocolat",
  changes_counter = "pains_au_chocolat_counter",
  brandish_when_picked = true,
}

item{
  name = "croissants_counter",
  savegame_variable = "i1112",
  can_be_assigned = true,
  counter = "i1027",
  limit = 10,
  brandish_when_picked = true,
}

item{
  name = "croissant",
  changes_counter = "croissants_counter",
  brandish_when_picked = true,
}

item{
  name = "bottle_2",
  savegame_variable = "i1113",
  nb_variants = 6,
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "rock_key",
  savegame_variable = "i1114",
  brandish_when_picked = true,
}

item{
  name = "bone_key",
  savegame_variable = "i1115",
  brandish_when_picked = true,
}

item{
  name = "clay_key",
  savegame_variable = "i1116",
  brandish_when_picked = true,
}

item{
  name = "apple_pie_counter",
  savegame_variable = "i1117",
  counter = "i1132",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "apple_pie",
  changes_counter = "apple_pie_counter",
  brandish_when_picked = true,
}

item{
  name = "flippers",
  savegame_variable = "i1118",
  brandish_when_picked = true,
}

item{
  name = "magic_cape",
  savegame_variable = "i1119",
  brandish_when_picked = true,
}

item{
  name = "bottle_3",
  savegame_variable = "i1120",
  nb_variants = 6,
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "iron_key",
  savegame_variable = "i1121",
  brandish_when_picked = true,
}

item{
  name = "stone_key",
  savegame_variable = "i1122",
  brandish_when_picked = true,
}

item{
  name = "wooden_key",
  savegame_variable = "i1123",
  brandish_when_picked = true,
}

item{
  name = "ice_key",
  savegame_variable = "i1124",
  brandish_when_picked = true,
}

item{
  name = "glove",
  savegame_variable = "i1125",
  nb_variants = 2,
  brandish_when_picked = true,
}

item{
  name = "fire_stones_counter",
  savegame_variable = "i1126",
  counter = "i1029",
  limit = 3,
  brandish_when_picked = true,
}

item{
  name = "fire_stone",
  changes_counter = "fire_stones_counter",
  brandish_when_picked = true,
}

item{
  name = "bottle_4",
  savegame_variable = "i1127",
  nb_variants = 6,
  can_be_assigned = true,
  brandish_when_picked = true,
}

item{
  name = "tunic",
  savegame_variable = "i1128",
  nb_variants = 3,
  initial_variant = 1,
  brandish_when_picked = true,
}

item{
  name = "sword",
  savegame_variable = "i1129",
  nb_variants = 4,
  brandish_when_picked = true,
}

item{
  name = "shield",
  savegame_variable = "i1130",
  nb_variants = 3,
  brandish_when_picked = true,
}

item{
  name = "world_map",
  savegame_variable = "i1031",
  brandish_when_picked = true,
}

item{
  name = "rupee_bag",
  savegame_variable = "i1034",
  nb_variants = 3,
  initial_variant = 1,
  limit_for_counter = "money",
  brandish_when_picked = true,
  amount_1 = 100,
  amount_2 = 300,
  amount_3 = 999,
}

item{
  name = "magic_bar",
  nb_variants = 2,
  initial_variant = 0,
  limit_for_counter = "magic",
  brandish_when_picked = true,
  amount_1 = 42,
  amount_2 = 84,
}

item{
  name = "map",
  brandish_when_picked = true,
}

item{
  name = "compass",
  brandish_when_picked = true,
}

item{
  name = "big_key",
  brandish_when_picked = true,
}

item{
  name = "boss_key",
  brandish_when_picked = true,
}

item{
  name = "small_key",
  changes_counter = "small_keys",
  shadow = "small",
  brandish_when_picked = true,
  sound_when_picked = "picked_small_key",
}

item{
  name = "rupee",
  nb_variants = 6,
  changes_counter = "money",
  shadow = "small",
  can_disappear = true,
  brandish_when_picked = true,
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
  brandish_when_picked = true,
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
  brandish_when_picked = true,
}

item{
  name = "heart_container",
  brandish_when_picked = true,
  sound_when_picked = "heart_container",
  sound_when_brandished = "heart_container",
}

item{
  name = "piece_of_heart",
  brandish_when_picked = true,
  sound_when_picked = "piece_of_heart",
  sound_when_brandished = "piece_of_heart",
}

item{
  name = "fairy",
  probability = 2,
  shadow = "none",
  can_disappear = true,
  brandish_when_picked = true,
}

item{
  name = "poivron_counter",
  savegame_variable = "i1481",
  counter = "i1471",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "poivron",
  changes_counter = "poivron_counter",
  brandish_when_picked = true,
}

item{
  name = "balai_counter",
  savegame_variable = "i1482",
  counter = "i1472",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "balai",
  changes_counter = "balai_counter",
  brandish_when_picked = true,
}

item{
  name = "cuillere_counter",
  savegame_variable = "i1483",
  counter = "i1473",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "cuillere",
  changes_counter = "cuillere_counter",
  brandish_when_picked = true,
}

item{
  name = "tapisserie_counter",
  savegame_variable = "i1484",
  counter = "i1474",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "tapisserie",
  changes_counter = "tapisserie_counter",
  brandish_when_picked = true,
}

item{
  name = "roc_magma_counter",
  savegame_variable = "i1485",
  counter = "i1475",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "roc_magma",
  changes_counter = "roc_magma_counter",
  brandish_when_picked = true,
}

item{
  name = "sac_riz_counter",
  savegame_variable = "i1486",
  counter = "i1476",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "sac_riz",
  changes_counter = "sac_riz_counter",
  brandish_when_picked = true,
}

item{
  name = "sac_olive_counter",
  savegame_variable = "i1487",
  counter = "i1477",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "sac_olive",
  changes_counter = "sac_olive_counter",
  brandish_when_picked = true,
}

item{
  name = "bocal_epice_counter",
  savegame_variable = "i1488",
  counter = "i1478",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "bocal_epice",
  changes_counter = "bocal_epice_counter",
  brandish_when_picked = true,
}

item{
  name = "hache_counter",
  savegame_variable = "i1489",
  counter = "i1479",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "hache",
  changes_counter = "hache_counter",
  brandish_when_picked = true,
}

item{
  name = "parfum_counter",
  savegame_variable = "i1490",
  counter = "i1480",
  limit = 50,
  brandish_when_picked = true,
}

item{
  name = "parfum",
  changes_counter = "parfum_counter",
  brandish_when_picked = true,
}

item{
  name = "zelda",
  savegame_variable = "i1131",
  brandish_when_picked = true,
}

