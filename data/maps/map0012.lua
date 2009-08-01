-----------------------------
-- Sahasrahla house script --
-----------------------------

-- The player talks to Sahasrahla
function event_npc_dialog(npc_name)

  if not has_obtained_clay_key() then

    if not has_obtained_world_map() then
      -- first visit
      start_message("sahasrahla_house.beginning")
      set_message_variable("sahasrahla_house.beginning", get_player_name())
      set_message_variable("sahasrahla_house.beginning", get_player_name())

    elseif has_seen_frozen_door() and not has_open_frozen_door() then
      -- the player has seen the frozen door but was not able to unfreeze it
      start_message("sahasrahla_house.frozen_door_advice")
	 
    else 
      -- the player has not found the clay key yet
      start_message("sahasrahla_house.quest_accepted")
    end

  elseif not has_obtained_world_map() then
    -- the player has obtained the clay key: give him the world map now if he didn't talk the first time
    start_message("sahasrahla_house.give_world_map")
  else
    -- Sahsrahla has nothing special to say
    start_message("sahasrahla_house.default")
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

-- Function called when the dialog box is being closed
-- first_message_id: name of the first message of the sequence that has just finished
-- answer: the answer of the question (0 or 1) or -1 if there was no question
function event_message_sequence_finished(first_message_id, answer)

   if first_message_id == "sahasrahla_house.beginning" or
     first_message_id == "sahasrahla_house.give_world_map" then
      -- give the world map to the player
      give_treasure(50, 33)
   end
end

-- Function called when the player has just obtained a treasure
function event_obtained_treasure(content, savegame_variable)
   start_message("sahasrahla_house.quest_accepted")
end
