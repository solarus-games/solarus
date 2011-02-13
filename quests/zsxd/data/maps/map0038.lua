----------------------------------
-- FREAKING CAVE OMG	        --
----------------------------------

-- Global sprites reference
torch1 = sol.map.interactive_entity_get_sprite("fc_torch_1")
torch2 = sol.map.interactive_entity_get_sprite("fc_torch_2")
	

function event_map_started(destination_point_name)
	
	-- Few light inside the cave at start
	sol.map.light_set(1) 	
	sol.main.sprite_set_animation(torch1,"lit")
	sol.main.sprite_set_animation(torch2,"lit")
end

function event_hero_on_sensor(sensor_name)
	
	if sensor_name == "sensor_light_off" then
		
		-- Map in deep dark		
		sol.map.light_set(0)
		sol.main.sprite_set_animation(torch1,"unlit")
		sol.main.sprite_set_animation(torch2,"unlit")
	end
end
