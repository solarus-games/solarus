----------------------------------
-- Crazy House 2FA (south)      --
----------------------------------

locked_door_A_value = 0

function event_map_started(destination_point_name)
	-- Coffre farceur	
	sol.map.chest_set_enabled("prankster_chest_bottom", false)
	sol.map.chest_set_enabled("prankster_chest_middle", false)
end

function event_hero_interaction(entity_name)
-- Vieillard --------------------------------------------------
-- TODO: dialogues à finir, script à finir
	if entity_name == "Vieillard" then
		sol.map.dialog_start("crazy_house.vieillard")

-- Guichet 21 -------------------------------------------------
	elseif entity_name == "GC21" then
		if sol.game.savegame_get_integer(1410) == 1 then		
			sol.map.dialog_start("crazy_house.guichet_21_ech_eq_1")
		else
			sol.map.dialog_start("crazy_house.guichet_21_ech_ne_1")
		end

-- Guichet 22 -------------------------------------------------
-- TODO: dialogues presue finis, script à faire
	elseif entity_name == "GC22" then

	end
end

function event_hero_on_sensor(sensor_name)
	-- Mécanisme du coffre farceur dans la salle aux trois portes	
	if sensor_name == "prankster_sensor_top" then
		sol.map.chest_set_enabled("prankster_chest_bottom", false)
		sol.map.chest_set_enabled("prankster_chest_middle", true)
	elseif sensor_name == "prankster_sensor_middle" then
		sol.map.chest_set_enabled("prankster_chest_bottom", true)
		sol.map.chest_set_enabled("prankster_chest_middle", false)	
	elseif sensor_name == "prankster_sensor_bottom" then
		sol.map.chest_set_enabled("prankster_chest_bottom", false)
		sol.map.chest_set_enabled("prankster_chest_middle", true)
	end
end

function event_switch_activated(switch_name)
	-- Mécanisme de la porte qui s'ouvre grâce à deux boutons
  -- dans la salle aux trois portes	
	if switch_name == "locked_door_switch_A"
	or switch_name == "locked_door_switch_B" then
		if locked_door_A_value < 2 then
			locked_door_A_value = locked_door_A_value + 1
			if locked_door_A_value == 2 then
				sol.map.door_set_open("locked_door_A", true)
				sol.main.play_sound("door_open")
			end
		end
	end
end
