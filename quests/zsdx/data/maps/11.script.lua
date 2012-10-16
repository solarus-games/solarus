local map = ...
-- Grandma house

-- The player talks to grandma
function grandma:on_interaction()

  local has_smith_sword = map:get_game():get_boolean(30)
  local has_clay_key = map:get_game():get_boolean(28)
  local has_finished_lyriann_cave = map:get_game():get_boolean(37)
  local has_bow = map:get_game():get_boolean(26)
  local has_rock_key = map:get_game():get_boolean(68)

  if not has_smith_sword then
    -- beginning: go get a sword
    map:start_dialog("grandma_house.sword")
  elseif not has_clay_key then
    -- with the sword: find Sahasrahla
    map:start_dialog("grandma_house.find_sahasrahla")
  elseif not has_finished_lyriann_cave then
    -- with the clay key: go to the cave
    map:start_dialog("grandma_house.go_lyriann_cave")
  elseif not map:get_game():is_dungeon_finished(1) then
    -- lyriann cave finished: go to the first dungeon
    map:start_dialog("grandma_house.go_dungeon_1")
  elseif not has_bow then
    -- dungeon 1 finished: go to Sahasrahla's house
    map:start_dialog("grandma_house.go_back_sahasrahla")
  elseif not has_rock_key then
    -- with the bow: go to the twin caves
    map:start_dialog("grandma_house.go_twin_caves")
  elseif not map:get_game():is_dungeon_finished(2) then
    -- with the rock key: go to the second dungeon
    map:start_dialog("grandma_house.go_dungeon_2")
  elseif not map:get_game():is_dungeon_finished(4) then
    -- use the telepathic booth
    map:start_dialog("grandma_house.go_telepathic_booth")
  elseif not map:get_game():is_dungeon_finished(5) then
    -- rupee house broken
    map:start_dialog("grandma_house.dark_world_enabled")
  else
    -- use the telepathic booth again
    map:start_dialog("grandma_house.go_telepathic_booth")
  end

end

