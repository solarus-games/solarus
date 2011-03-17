----------------------------------
-- FREAKING CAVE 1 OMG	        --
-- USE BOOLEAN FROM 90 TO 99 	--
----------------------------------

-- Global sprites reference
torch1 = sol.map.interactive_entity_get_sprite("fc_torch_1")
torch2 = sol.map.interactive_entity_get_sprite("fc_torch_2")
	

function event_map_started(destination_point_name)
	
	-- Few light inside the cave at start
	if sol.game.savegame_get_boolean(90) == false then
		sol.map.light_set(1) 	
		sol.main.sprite_set_animation(torch1,"lit")
		sol.main.sprite_set_animation(torch2,"lit")
	else	
		sol.map.light_set(0)
		sol.main.sprite_set_animation(torch1,"unlit")
		sol.main.sprite_set_animation(torch2,"unlit")
		sol.map.sensor_set_enabled("sensor_light_off",false)
	end	
	
	sol.main.sprite_set_animation(torch2,"lit")
end

function event_hero_on_sensor(sensor_name)
	
	if sensor_name == "sensor_light_off" then
		
		-- Map in deep dark		
		sol.map.light_set(0)
		sol.main.sprite_set_animation(torch1,"unlit")
		sol.main.sprite_set_animation(torch2,"unlit")
		sol.map.sensor_set_enabled("sensor_light_off",false)
		sol.game.savegame_set_boolean(90,true)
	end
end
