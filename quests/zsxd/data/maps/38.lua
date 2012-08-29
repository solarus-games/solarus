----------------------------------
-- FREAKING CAVE 1 OMG          --
-- USE BOOLEAN FROM 90 TO 99    --
----------------------------------

-- Global sprites reference
local torch1 = sol.map.npc_get_sprite("fc_torch_1")
local torch2 = sol.map.npc_get_sprite("fc_torch_2")

function event_map_started(destination_point_name)

  torch1 = sol.map.npc_get_sprite("fc_torch_1")
  torch2 = sol.map.npc_get_sprite("fc_torch_2")

  -- Few light inside the cave at start
  if sol.game.savegame_get_boolean(90) == false then
    sol.map.light_set(1)         
    torch1:set_animation("lit")
    torch2:set_animation("lit")
  else        
    sol.map.light_set(0)
    torch1:set_animation("unlit")
    torch2:set_animation("unlit")
    sol.map.sensor_set_enabled("sensor_light_off", false)
  end        

  torch2:set_animation("lit")
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "sensor_light_off" then

    -- Map in deep dark
    sol.map.light_set(0)
    torch1:set_animation("unlit")
    torch2:set_animation("unlit")
    sol.map.sensor_set_enabled("sensor_light_off", false)
    sol.game.savegame_set_boolean(90, true)
  end
end

