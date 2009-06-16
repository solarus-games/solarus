-------------------------------------
-- Sahasrahla cave icy room script --
-------------------------------------

function event_map_started()
   interactive_entity_set_direction("frozen_door", 3)
   interactive_entity_set_direction("frozen_door_opposite", 1)
end

function event_interaction(entity_name)
   start_message("sahasrahla_house.frozen_door")
end
