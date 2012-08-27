local map = ...
-- Dungeon 9 5F

function map:on_started(destination_point)

  map:set_doors_open("door_b", true)
  map:set_doors_open("door_c", true)
  map:set_doors_open("door_d", true)
  map:set_entities_enabled("child", false)
end

function map:on_block_moved(block_name)

  -- door A
  if block_name == "door_a_block" then
    if not door_a:is_open() then
      sol.audio.play_sound("secret")
      map:open_doors("door_a")
    end

  -- doors B and C
  elseif block_name == "door_bc_block" then
    sol.audio.play_sound("secret")
    map:open_doors("door_b")
    map:open_doors("door_c")
  end

end

function map:on_hero_on_sensor(sensor_name)

  -- door B
  if sensor_name:find("^close_door_b_sensor") then
    if door_b:is_open()
        and map:has_entities("door_b_enemy") then
      map:close_doors("door_b")
    end

  -- door C
  elseif sensor_name:find("^close_door_c_sensor") then
    if door_c:is_open()
        and map:has_entities("door_c_enemy") then
      map:close_doors("door_c")
    end

  -- door B + C
  elseif sensor_name:find("^close_door_bc_sensor") then
    map:close_doors("door_b")
    map:close_doors("door_c")
    map:set_entities_enabled("close_door_bc_sensor", false)

  -- door D
  elseif sensor_name:find("^close_door_d_sensor") then
    if door_d:is_open()
        and not map:enemy_is_dead("door_d_enemy") then
      map:close_doors("door_d")
      door_d_enemy:set_enabled(true)
    end

  -- childs
  elseif sensor_name == "childs_hint_sensor" then
    hero:freeze()
    hero:set_direction(1)
    map:set_entities_enabled("child", true)
    for i = 1, 8 do
      local sprite = map:npc_get_sprite("child_" .. i)
      sprite:set_ignore_suspend(true)
    end
    sol.timer.start(3000, function()
      hero:unfreeze()
      map:get_entity(sensor_name):set_enabled(false)
      map:start_dialog("dungeon_9.5f_childs_hint")
    end)
  end
end

function map:on_enemy_dead(enemy_name)

  -- door D
  if enemy_name == "door_d_enemy" then
    if not door_d:is_open() then
      sol.audio.play_sound("secret")
      map:open_doors("door_d")
    end

  -- door B
  elseif enemy_name:find("^door_b_enemy") then
    if not map:has_entities("door_b_enemy")
        and not door_b:is_open() then
      sol.audio.play_sound("secret")
      map:open_doors("door_b")
    end

  -- door C
  elseif enemy_name:find("^door_c_enemy") then
    if not map:has_entities("door_c_enemy")
        and not door_c:is_open() then
      sol.audio.play_sound("secret")
      map:open_doors("door_c")
    end
  end
end

