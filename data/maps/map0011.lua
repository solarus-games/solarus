--------------------------
-- Grandma house script --
--------------------------

-- The player talks to grandma
function event_npc_dialog(npc_name)

  has_smith_sword = savegame_get_boolean(30)
  has_clay_key = savegame_get_boolean(28)
  has_finished_lyriann_cave = savegame_get_boolean(37)

  if not has_smith_sword then
    -- beginning: go get a sword
    start_message("grandma_house.sword")
  elseif not has_clay_key then
    -- with the sword: find Sahasrahla
    start_message("grandma_house.find_sahasrahla")
  elseif not has_finished_lyriann_cave then
    start_message("grandma_house.go_lyriann_cave")
  else
    start_message("grandma_house.go_dungeon_1")
  end

  -- TODO other messages later in the game
end

