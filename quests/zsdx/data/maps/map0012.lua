-----------------------------
-- Sahasrahla house script --
-----------------------------

function event_map_started(destination_point)

  if savegame_get_boolean(37) then -- if the Lyriann cave is finished
    door_set_open("door", equipment_is_dungeon_finished(1)) -- don't allow the player to obtain the bow until the first dungeon is finished
  end
end

-- The player talks to Sahasrahla
function event_npc_dialog(npc_name)

  if not has_obtained_clay_key() then

    if not has_obtained_world_map() then
      -- first visit
      dialog_start("sahasrahla_house.beginning")
      dialog_set_variable("sahasrahla_house.beginning", savegame_get_name())

    elseif has_seen_frozen_door() and not has_open_frozen_door() then
      -- the player has seen the frozen door but was not able to unfreeze it
      dialog_start("sahasrahla_house.frozen_door_advice")
	 
    else 
      -- the player has not found the clay key yet
      dialog_start("sahasrahla_house.quest_accepted")
    end

  elseif not has_obtained_world_map() then
    -- the player has obtained the clay key: give him the world map now if he didn't talk the first time
    dialog_start("sahasrahla_house.give_world_map")
  elseif equipment_is_dungeon_finished(1) and not has_obtained_bow() then -- glove
    -- the player should now go downstairs to obtain the bow
    dialog_start("sahasrahla_house.dungeon_1_finished")
  else
    -- Sahsrahla has nothing special to say
    dialog_start("sahasrahla_house.default")
  end
end

function has_obtained_world_map()
  return savegame_get_boolean(33);
end

function has_seen_frozen_door()
  return savegame_get_boolean(34)
end

function has_open_frozen_door()
  return savegame_get_boolean(35)
end

function has_obtained_clay_key()
  return savegame_get_boolean(28);
end

function has_obtained_bow()
  return savegame_get_boolean(26);
end

-- Function called when the dialog box is being closed
-- first_message_id: name of the first message of the sequence that has just finished
-- answer: the answer of the question (0 or 1) or -1 if there was no question
function event_dialog_finished(first_message_id, answer)

  if first_message_id == "sahasrahla_house.beginning" or
    first_message_id == "sahasrahla_house.give_world_map" then
    -- give the world map to the player
    treasure_give("world_map", 1, 33)
  elseif first_message_id == "sahasrahla_house.quest_accepted" and not door_is_open("door") then
    door_open("door")
  end
end

-- Function called when the player has just obtained a treasure
function event_treasure_obtained(item_name, variant, savegame_variable)
   dialog_start("sahasrahla_house.quest_accepted")
end

