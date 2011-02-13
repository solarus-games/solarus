----------------------------------
-- FREAKING CAVE OMG	        --
----------------------------------


function event_map_started(destination_point_name)
	sol.map.light_set(1): -- Few light inside the cave
	
	torch1 = sol.map.interactive_entity_get_sprite("fc_torch_1")
	sol.main.sprite_set_animation(torch1,"lit")
end
