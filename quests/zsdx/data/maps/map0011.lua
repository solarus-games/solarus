--------------------------
-- Grandma house script --
--------------------------

-- The player talks to grandma
function event_npc_dialog(npc_name)

  has_smith_sword = savegame_get_boolean(30)
  has_clay_key = savegame_get_boolean(28)
  has_finished_lyriann_cave = savegame_get_boolean(37)
  has_bow = savegame_get_boolean(26)
  has_rock_key = savegame_get_boolean(68)

  if not has_smith_sword then
    -- beginning: go get a sword
    dialog_start("grandma_house.sword")
  elseif not has_clay_key then
    -- with the sword: find Sahasrahla
    dialog_start("grandma_house.find_sahasrahla")
  elseif not has_finished_lyriann_cave then
    -- with the clay key: go to the cave
    dialog_start("grandma_house.go_lyriann_cave")
  elseif not equipment_is_dungeon_finished(1) then
    -- lyriann cave finished: go to the first dungeon
    dialog_start("grandma_house.go_dungeon_1")
  elseif not has_bow then
    -- dungeon 1 finished: go to Sahasrahla's house
    dialog_start("grandma_house.go_back_sahasrahla")
  elseif not has_rock_key then
    -- with the bow: go to the twin caves
    dialog_start("grandma_house.go_twin_caves")
  elseif not equipment_is_dungeon_finished(2) then
    -- with the rock key: go to the second dungeon
    dialog_start("grandma_house.go_dungeon_2")
  else
    -- then use the telepathic booth
    dialog_start("grandma_house.go_telepathic_booth")
  end

end

