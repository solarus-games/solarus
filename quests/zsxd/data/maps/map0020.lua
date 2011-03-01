----------------------------------
-- Crazy House 1F (south)       --
----------------------------------

function event_map_started(destination_point_name)
	if not sol.game.savegame_get_boolean(101) then
		sol.map.chest_set_enabled("CV3", false)
	end
end

function event_switch_activated(switch_name)
	if switch_name == "CS3" then
		-- Faire apparaitre le coffre vide 1
		sol.map.chest_set_enabled("CV3", true)
		sol.game.savegame_set_boolean(101, true)
		sol.main.play_sound("chest_appears")
	end
end

function event_hero_interaction(entity_name)
	if entity_name == "mario_message_1" then
		-- Tableau de mario qui parle
		sol.main.play_sound("sm64_memario")
		sol.map.dialog_start("crazy_house.mario_message_1")
	elseif entity_name == "receptionist_1a" then
		-- Receptionniste des guichets sud est
		sol.map.dialog_start("crazy_house.receptionist_1")
	end
end

function event_dialog_finished(first_message_id, answer)
	if first_message_id == "crazy_house.receptionist_1" then
		if answer == 0 then
			-- Knil accepte d'attendre
			sol.map.door_set_open("waiting_room_door", true)
			sol.main.play_sound("door_open")
		else
			-- Knil refuse
			
		end
	end
end
