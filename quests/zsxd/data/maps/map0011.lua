-- Shop

function event_map_started(destination_point_name)

  -- make an NPC move
   m = sol.main.random_path_movement_create(32)
   sol.map.npc_start_movement("moving_npc", m)
   sol.main.sprite_set_animation(sol.map.npc_get_sprite("moving_npc"), "walking")

end

