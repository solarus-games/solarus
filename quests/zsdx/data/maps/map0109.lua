-- Dungeon 9 5F

function event_map_started(destination_point_name)

  sol.map.door_set_open("door_b", true)
  sol.map.door_set_open("door_c", true)
  sol.map.door_set_open("door_d", true)
  sol.map.npc_set_group_enabled("child", false)
end

function event_block_moved(block_name)

  -- door A
  if block_name == "door_a_block" then
    if not sol.map.door_is_open("door_a") then
      sol.main.play_sound("secret")
      sol.map.door_open("door_a")
    end

  -- doors B and C
  elseif block_name == "door_bc_block" then
    sol.main.play_sound("secret")
    sol.map.door_open("door_b")
    sol.map.door_open("door_c")
  end

end

function event_hero_on_sensor(sensor_name)

  -- door B
  if sensor_name:find("^close_door_b_sensor") then
    if sol.map.door_is_open("door_b")
        and not sol.map.enemy_is_group_dead("door_b_enemy") then
      sol.map.door_close("door_b")
    end

  -- door C
  elseif sensor_name:find("^close_door_c_sensor") then
    if sol.map.door_is_open("door_c")
        and not sol.map.enemy_is_group_dead("door_c_enemy") then
      sol.map.door_close("door_c")
    end

  -- door B + C
  elseif sensor_name:find("^close_door_bc_sensor") then
    sol.map.door_close("door_b")
    sol.map.door_close("door_c")
    sol.map.sensor_set_group_enabled("close_door_bc_sensor", false)

  -- door D
  elseif sensor_name:find("^close_door_d_sensor") then
    if sol.map.door_is_open("door_d")
        and not sol.map.enemy_is_dead("door_d_enemy") then
      sol.map.door_close("door_d")
      sol.map.enemy_set_enabled("door_d_enemy", true)
    end

  -- childs
  elseif sensor_name == "childs_hint_sensor" then
    sol.map.hero_freeze()
    sol.map.hero_set_direction(1)
    sol.map.npc_set_group_enabled("child", true)
    for i = 1, 8 do
      local sprite = sol.map.npc_get_sprite("child_" .. i)
      sol.main.sprite_set_animation_ignore_suspend(sprite, true)
    end
    sol.main.timer_start(function()
      sol.map.hero_unfreeze()
      sol.map.sensor_set_enabled(sensor_name, false)
      sol.map.dialog_start("dungeon_9.5f_childs_hint")
    end, 3000)
  end
end

function event_enemy_dead(enemy_name)

  -- door D
  if enemy_name == "door_d_enemy" then
    if not sol.map.door_is_open("door_d") then
      sol.main.play_sound("secret")
      sol.map.door_open("door_d")
    end

  -- door B
  elseif enemy_name:find("^door_b_enemy") then
    if sol.map.enemy_is_group_dead("door_b_enemy")
        and not sol.map.door_is_open("door_b") then
      sol.main.play_sound("secret")
      sol.map.door_open("door_b")
    end
 
  -- door C
  elseif enemy_name:find("^door_c_enemy") then
    if sol.map.enemy_is_group_dead("door_c_enemy")
        and not sol.map.door_is_open("door_c") then
      sol.main.play_sound("secret")
      sol.map.door_open("door_c")
    end
  end
end

