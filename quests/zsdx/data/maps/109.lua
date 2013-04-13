local map = ...
-- Dungeon 9 5F

function map:on_started(destination)

  map:set_doors_open("door_b", true)
  map:set_doors_open("door_c", true)
  map:set_doors_open("door_d", true)
  map:set_entities_enabled("child_", false)
end

-- door A
function door_a_block:on_moved()

  if door_a:is_closed() then
    sol.audio.play_sound("secret")
    map:open_doors("door_a")
  end
end

-- doors B and C
function door_bc_block:on_moved()
  sol.audio.play_sound("secret")
  map:open_doors("door_b")
  map:open_doors("door_c")
end

-- door B
function close_door_b_sensor_1:on_activated()

  if door_b:is_open()
      and map:has_entities("door_b_enemy") then
    map:close_doors("door_b")
  end
end
close_door_b_sensor_2.on_activated = close_door_b_sensor_1.on_activated

-- door C
function close_door_c_sensor_1:on_activated()

  if door_c:is_open()
      and map:has_entities("door_c_enemy") then
    map:close_doors("door_c")
  end
end
close_door_c_sensor_2.on_activated = close_door_c_sensor_1.on_activated

-- door B + C
function close_door_bc_sensor_1:on_activated()

  map:close_doors("door_b")
  map:close_doors("door_c")
  map:set_entities_enabled("close_door_bc_sensor", false)
end
close_door_bc_sensor_2.on_activated = close_door_bc_sensor_1.on_activated
close_door_bc_sensor_3.on_activated = close_door_bc_sensor_1.on_activated

-- door D
function close_door_d_sensor:on_activated()

  if door_d:is_open()
      and door_d_enemy ~= nil then
    map:close_doors("door_d")
    door_d_enemy:set_enabled(true)
  end
end

-- childs
function childs_hint_sensor:on_activated()

  hero:freeze()
  hero:set_direction(1)
  map:set_entities_enabled("child_", true)
  for i = 1, 8 do
    local sprite = map:npc_get_sprite("child_" .. i)
    sprite:set_ignore_suspend(true)
  end
  sol.timer.start(3000, function()
    hero:unfreeze()
    self:set_enabled(false)
    map:start_dialog("dungeon_9.5f_childs_hint")
  end)
end

-- door D
function door_d_enemy:on_dead()

  if not door_d:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door_d")
  end
end

-- door B
function door_b_enemy_1:on_dead()

  if not map:has_entities("door_b_enemy")
      and not door_b:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door_b")
  end
end
door_b_enemy_2.on_dead = door_b_enemy_1.on_dead

-- door C
function door_c_enemy_1:on_dead()

  if not map:has_entities("door_c_enemy")
      and not door_c:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door_c")
  end
end
door_c_enemy_2.on_dead = door_c_enemy_1.on_dead

