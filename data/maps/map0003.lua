-----------------------------
-- Outside world C1 script --
-----------------------------

monkey_jumps = 0

-- Function called when the map starts.
-- The NPCs are initialized.
function event_map_started(destination_point_name)

   if savegame_get_boolean(24) then
      -- remove the monkey from Link's house entrance
      npc_remove("monkey")
   end

   -- make the NPCs walk
   npc_random_walk("hat_man")
   npc_random_walk("how_to_save_npc")
   npc_random_walk("grand_son")
end

function event_hero_on_sensor(sensor_name)

  if string.find(sensor_name, "not_in_demo_sensor") then
    start_message("outside_world.not_in_demo")
  end
end

-- Function called when the player wants to talk to a non-playing character.
-- If the NPC is the monkey, then the monkey sound is played and the dialog starts.
function event_npc_dialog(npc_name)

   if string.find(npc_name, "monkey") then
     
     if not savegame_get_boolean(24) then
       -- monkey first dialog
       play_sound("la_monkey")
       start_message("outside_world.village.monkey")
     else
       play_sound("la_monkey")
       start_message("outside_world.dungeon_2_entrance.monkey")
     end
   end
end

-- Function called when the dialog box is being closed.
-- If the player was talking to the monkey, we do the appropriate action
function event_message_sequence_finished(first_message_id, answer)

   if first_message_id == "outside_world.village.monkey" then

      -- show another message depending on the shield
      if equipment_get_shield() > 0 then
	 start_message("outside_world.village.monkey.with_shield")
      else
	 start_message("outside_world.village.monkey.without_shield")
      end

   elseif first_message_id == "outside_world.village.monkey.with_shield" then
      -- make the monkey leave
      freeze()
      play_sound("la_monkey")
      npc_jump("monkey", 1, 24, false)
      monkey_jumps = 1
      savegame_set_boolean(24, true)

   elseif first_message_id == "outside_world.village.tree_woman" then
      give_treasure(87, -1)
   end
end

-- Function called when the monkey has finished jumping
function event_npc_movement_finished(npc_name)

   if monkey_jumps == 1 then
      -- first jump finished: wait a little amount of time before jumping again
      start_timer(300, "monkey_timer", false)
   elseif monkey_jumps == 2 then
      -- second jump finished: start the last jump
      play_sound("la_monkey")
      npc_jump("monkey", 1, 64, false)
      monkey_jumps = 3
   else
      -- last jump finished: remove the monkey from the map and unfreeze the hero
      npc_remove("monkey")
      unfreeze()
   end
end

-- Function called when the monkey timer expires.
function monkey_timer()
   -- start the second jump
   play_sound("la_monkey")
   npc_jump("monkey", 2, 56, false)
   monkey_jumps = 2
end
