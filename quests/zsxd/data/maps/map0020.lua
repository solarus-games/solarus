----------------------------------
-- Crazy House 1F (south)       --
----------------------------------

function event_map_started(destination_point_name)
	if not sol.game.savegame_get_boolean(101) then
		sol.map.chest_set_enabled("empty_chest_1", false)
	end
end

function event_switch_activated(switch_name)
	-- Faire apparaitre le coffre vide 1
	if switch_name == "empty_chest_1_switch" then
		sol.map.chest_set_enabled("empty_chest_1", true)
		sol.game.savegame_set_boolean(101, true)
		sol.main.play_sound("chest_appears")
	end
end

function event_hero_interaction(entity_name)
	if entity_name == "mario_message_1" then
		sol.main.play_sound("sm64_memario")
		sol.map.dialog_start("crazy_house.mario_message_1")
	end
end
