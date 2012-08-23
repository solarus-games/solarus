local map = ...
-- Sahasrahla house

function map:on_started(destination_point)

  if map:get_game():get_boolean(37) then -- if the Lyriann cave is finished
    map:set_doors_open("door", map:get_game():is_dungeon_finished(1)) -- don't allow the player to obtain the bow until the first dungeon is finished
  end

  if map:get_game():is_dungeon_finished(4)
      and not map:get_game():is_dungeon_finished(7) then
    -- Sahasrahla has been kidnapped
    map:npc_remove("sahasrahla")
  end
end

-- The player talks to Sahasrahla
function map:on_npc_interaction(npc_name)

  if not has_obtained_clay_key() then

    if not has_obtained_world_map() then
      -- first visit
      map:start_dialog("sahasrahla_house.beginning")
      map:set_dialog_variable("sahasrahla_house.beginning", map:get_game():get_player_name())

    elseif has_seen_frozen_door() and not has_open_frozen_door() then
      -- the player has seen the frozen door but was not able to unfreeze it
      map:start_dialog("sahasrahla_house.frozen_door_advice")
	 
    else 
      -- the player has not found the clay key yet
      map:start_dialog("sahasrahla_house.quest_accepted")
    end

  elseif not has_obtained_world_map() then
    -- the player has obtained the clay key: give him the world map now if he didn't talk the first time
    map:start_dialog("sahasrahla_house.give_world_map")
  elseif map:get_game():is_dungeon_finished(1) and not has_obtained_bow() then -- glove
    -- the player should now go downstairs to obtain the bow
    map:start_dialog("sahasrahla_house.dungeon_1_finished")
  else
    -- Sahsrahla has nothing special to say
    map:start_dialog("sahasrahla_house.default")
  end
end

function has_obtained_world_map()
  return map:get_game():get_boolean(33)
end

function has_seen_frozen_door()
  return map:get_game():get_boolean(34)
end

function has_open_frozen_door()
  return map:get_game():get_boolean(35)
end

function has_obtained_clay_key()
  return map:get_game():get_boolean(28)
end

function has_obtained_bow()
  return map:get_game():get_boolean(26)
end

-- Function called when the dialog box is being closed
-- dialog_id: name of the first message of the sequence that has just finished
-- answer: the answer of the question (0 or 1) or -1 if there was no question
function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "sahasrahla_house.beginning" or
    dialog_id == "sahasrahla_house.give_world_map" then
    -- give the world map to the player
    map:get_hero():start_treasure("world_map", 1, 33)
  elseif dialog_id == "sahasrahla_house.quest_accepted" and not map:door_is_open("door") then
    map:open_doors("door")
  end
end

-- Function called when the player has just obtained a treasure
function map:on_obtained_treasure(item_name, variant, savegame_variable)
   map:start_dialog("sahasrahla_house.quest_accepted")
end

