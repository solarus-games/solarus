-----------------------------
-- Sahasrahla house script --
-----------------------------

function event_map_started(destination_point)

  if sol.game.savegame_get_boolean(37) then -- if the Lyriann cave is finished
    sol.map.door_set_open("door", sol.game.is_dungeon_finished(1)) -- don't allow the player to obtain the bow until the first dungeon is finished
  end

  if sol.game.is_dungeon_finished(4)
      and not sol.game.is_dungeon_finished(7) then
    -- Sahasrahla has been kidnapped
    sol.map.npc_remove("sahasrahla")
  end
end

-- The player talks to Sahasrahla
function event_npc_dialog(npc_name)

  if not has_obtained_clay_key() then

    if not has_obtained_world_map() then
      -- first visit
      sol.map.dialog_start("sahasrahla_house.beginning")
      sol.map.dialog_set_variable("sahasrahla_house.beginning", sol.game.savegame_get_name())

    elseif has_seen_frozen_door() and not has_open_frozen_door() then
      -- the player has seen the frozen door but was not able to unfreeze it
      sol.map.dialog_start("sahasrahla_house.frozen_door_advice")
	 
    else 
      -- the player has not found the clay key yet
      sol.map.dialog_start("sahasrahla_house.quest_accepted")
    end

  elseif not has_obtained_world_map() then
    -- the player has obtained the clay key: give him the world map now if he didn't talk the first time
    sol.map.dialog_start("sahasrahla_house.give_world_map")
  elseif sol.game.is_dungeon_finished(1) and not has_obtained_bow() then -- glove
    -- the player should now go downstairs to obtain the bow
    sol.map.dialog_start("sahasrahla_house.dungeon_1_finished")
  else
    -- Sahsrahla has nothing special to say
    sol.map.dialog_start("sahasrahla_house.default")
  end
end

function has_obtained_world_map()
  return sol.game.savegame_get_boolean(33);
end

function has_seen_frozen_door()
  return sol.game.savegame_get_boolean(34)
end

function has_open_frozen_door()
  return sol.game.savegame_get_boolean(35)
end

function has_obtained_clay_key()
  return sol.game.savegame_get_boolean(28);
end

function has_obtained_bow()
  return sol.game.savegame_get_boolean(26);
end

-- Function called when the dialog box is being closed
-- first_message_id: name of the first message of the sequence that has just finished
-- answer: the answer of the question (0 or 1) or -1 if there was no question
function event_dialog_finished(first_message_id, answer)

  if first_message_id == "sahasrahla_house.beginning" or
    first_message_id == "sahasrahla_house.give_world_map" then
    -- give the world map to the player
    sol.map.treasure_give("world_map", 1, 33)
  elseif first_message_id == "sahasrahla_house.quest_accepted" and not sol.map.door_is_open("door") then
    sol.map.door_open("door")
  end
end

-- Function called when the player has just obtained a treasure
function event_treasure_obtained(item_name, variant, savegame_variable)
   sol.map.dialog_start("sahasrahla_house.quest_accepted")
end

