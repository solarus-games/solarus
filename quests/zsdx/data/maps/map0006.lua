local map = ...
-- Outside world A4

tom_sprite = nil

function map:on_started(destination_point_name)

  -- enable dark world
  if map:get_game():get_boolean(905) then
    sol.audio.play_music("dark_world")
    map:set_tileset(13)
  else
    sol.audio.play_music("overworld")
  end

  -- dungeon 1 ladder
  if is_ladder_activated() then
    map:tile_set_group_enabled("ladder_step", true)
    map:tile_set_group_enabled("no_ladder", false)
    map:npc_remove("tom")
    map:sensor_set_enabled("tom_appears_sensor", false)
  else
    map:tile_set_group_enabled("ladder_step", false)
    map:tile_set_group_enabled("no_ladder", true)
    tom_sprite = map:npc_get_sprite("tom")
  end

  -- Beaumont cave entrance
  if not is_beaumont_cave_open() then
    map:tile_set_enabled("beaumont_cave_hole", false)
    map:teletransporter_set_enabled("to_beaumont_cave", false)
  end

  -- Dungeon 9 entrance
  if not map:get_game():is_dungeon_finished(8) then
    map:teletransporter_set_enabled("dungeon_9_teletransporter", false)
    map:tile_set_enabled("dungeon_9_entrance", false)
  end
end

function is_ladder_activated()
  return map:get_game():get_boolean(52)
end

function is_beaumont_cave_open()
  return map:get_game():get_boolean(153)
end


function map:on_hero_on_sensor(sensor_name)

  local has_finished_tom_cave = map:get_game():get_boolean(37)

  if sensor_name == "tom_appears_sensor"
      and has_finished_tom_cave
      and not is_ladder_activated() then
    map:start_dialog("outside_world.tom_dungeon_1_entrance.hey")
  elseif sensor_name == "edelweiss_sensor"
      and map:get_game():get_item("level_4_way") == 3 -- the player has the edelweiss
      and not is_beaumont_cave_open() then
    put_edelweiss()
  end
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "outside_world.tom_dungeon_1_entrance.hey" then
    map:hero_freeze()
    map:hero_set_direction(0)
    map:npc_set_position("tom", 528, 245)
    local m = sol.movement.create("path")
    m:set_path{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2}
    m:set_speed(48)
    m:set_ignore_obstacles(true)
    map:npc_start_movement("tom", m)
    tom_sprite:set_animation("walking")
  elseif dialog_id == "outside_world.tom_dungeon_1_entrance.need_help" then
    tom_sprite:set_direction(1)
    sol.timer.start(1500, tom_timer_1)
  elseif dialog_id == "outside_world.tom_dungeon_1_entrance.let_me_see" then
    sol.audio.play_sound("jump")
    local m = sol.movement.create("jump")
    m:set_direction8(4)
    m:set_distance(16)
    m:set_ignore_obstacles(true)
    map:npc_start_movement("tom", m)
  elseif dialog_id == "outside_world.tom_dungeon_1_entrance.open" then
    tom_sprite:set_animation("walking")
    sol.timer.start(300, tom_timer_3)
  elseif dialog_id == "outside_world.beaumont_hill_put_edelweiss" then
    map:hero_freeze()
    sol.timer.start(1000, edelweiss_explode)
  end

end

function map:on_npc_movement_finished(npc_name)

  local x, y = map:npc_get_position("tom")
  if x ~= 352 then
    tom_sprite:set_direction(2)
    map:start_dialog("outside_world.tom_dungeon_1_entrance.need_help")
  else
    tom_sprite:set_direction(1)
    sol.timer.start(1000, tom_timer_2)
  end
end

function tom_timer_1()
  map:start_dialog("outside_world.tom_dungeon_1_entrance.let_me_see")
  tom_sprite:set_direction(2)
end

function tom_timer_2()
  map:start_dialog("outside_world.tom_dungeon_1_entrance.open")
end

function tom_timer_3()
  tom_sprite:set_animation("stopped")
  ladder_step1()
end

function ladder_step1()
  sol.audio.play_sound("door_open")
  map:tile_set_group_enabled("ladder_step1", true)
  map:tile_set_group_enabled("no_ladder_step1", false)
  sol.timer.start(1000, ladder_step2)
end

function ladder_step2()
  sol.audio.play_sound("door_open")
  map:tile_set_group_enabled("ladder_step2", true)
  sol.timer.start(1000, ladder_step3)
end

function ladder_step3()
  sol.audio.play_sound("door_open")
  map:tile_set_group_enabled("ladder_step3", true)
  sol.timer.start(1000, ladder_step4)
end

function ladder_step4()
  map:tile_set_group_enabled("no_ladder", false)
  map:sensor_set_enabled("tom_appears_sensor", false)
  sol.audio.play_sound("secret")
  map:get_game():set_boolean(52, true)
  map:hero_unfreeze()
end

function map:on_npc_interaction(npc_name)
  map:start_dialog("outside_world.tom_dungeon_1_entrance.finished")
end

function put_edelweiss()
  map:start_dialog("outside_world.beaumont_hill_put_edelweiss")
end

function edelweiss_explode()

  sol.audio.play_sound("explosion")
  map:create_explosion(160, 72, 0)
  map:tile_set_enabled("beaumont_cave_hole", true)
  map:teletransporter_set_enabled("to_beaumont_cave", true)
  map:get_game():set_boolean(153, true)
  map:get_game():set_item("level_4_way", 0)
  sol.timer.start(1000, edelweiss_end)
end

function edelweiss_end()
  sol.audio.play_sound("secret")
  map:hero_unfreeze()
end

