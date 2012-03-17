-- Outside world A4

tom_sprite = nil

function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.audio.play_music("dark_world")
    sol.map.tileset_set(13)
  end

  -- dungeon 1 ladder
  if is_ladder_activated() then
    sol.map.tile_set_group_enabled("ladder_step", true)
    sol.map.tile_set_group_enabled("no_ladder", false)
    sol.map.npc_remove("tom")
    sol.map.sensor_set_enabled("tom_appears_sensor", false)
  else
    sol.map.tile_set_group_enabled("ladder_step", false)
    sol.map.tile_set_group_enabled("no_ladder", true)
    tom_sprite = sol.map.npc_get_sprite("tom")
  end

  -- Beaumont cave entrance
  if not is_beaumont_cave_open() then
    sol.map.tile_set_enabled("beaumont_cave_hole", false)
    sol.map.teletransporter_set_enabled("to_beaumont_cave", false)
  end

  -- Dungeon 9 entrance
  if not sol.game.is_dungeon_finished(8) then
    sol.map.teletransporter_set_enabled("dungeon_9_teletransporter", false)
    sol.map.tile_set_enabled("dungeon_9_entrance", false)
  end
end

function is_ladder_activated()
  return sol.game.savegame_get_boolean(52)
end

function is_beaumont_cave_open()
  return sol.game.savegame_get_boolean(153)
end


function event_hero_on_sensor(sensor_name)

  local has_finished_tom_cave = sol.game.savegame_get_boolean(37)

  if sensor_name == "tom_appears_sensor"
      and has_finished_tom_cave
      and not is_ladder_activated() then
    sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.hey")
  elseif sensor_name == "edelweiss_sensor"
      and sol.game.get_item("level_4_way") == 3 -- the player has the edelweiss
      and not is_beaumont_cave_open() then
    put_edelweiss()
  end
end

function event_dialog_finished(dialog_id, answer)

  if dialog_id == "outside_world.tom_dungeon_1_entrance.hey" then
    sol.map.hero_freeze()
    sol.map.hero_set_direction(0)
    sol.map.npc_set_position("tom", 528, 245)
    local m = sol.movement.path_movement_create("44444444444444444444222", 48)
    m:set_property("ignore_obstacles", true)
    sol.map.npc_start_movement("tom", m)
    tom_sprite:set_animation("walking")
  elseif dialog_id == "outside_world.tom_dungeon_1_entrance.need_help" then
    tom_sprite:set_direction(1)
    sol.timer.start(1500, tom_timer_1)
  elseif dialog_id == "outside_world.tom_dungeon_1_entrance.let_me_see" then
    sol.audio.play_sound("jump")
    local m = sol.movement.jump_movement_create(4, 16)
    m:set_property("ignore_obstacles", true)
    sol.map.npc_start_movement("tom", m)
  elseif dialog_id == "outside_world.tom_dungeon_1_entrance.open" then
    tom_sprite:set_animation("walking")
    sol.timer.start(300, tom_timer_3)
  elseif dialog_id == "outside_world.beaumont_hill_put_edelweiss" then
    sol.map.hero_freeze()
    sol.timer.start(1000, edelweiss_explode)
  end

end

function event_npc_movement_finished(npc_name)

  local x, y = sol.map.npc_get_position("tom")
  if x ~= 352 then
    tom_sprite:set_direction(2)
    sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.need_help")
  else
    tom_sprite:set_direction(1)
    sol.timer.start(1000, tom_timer_2)
  end
end

function tom_timer_1()
  sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.let_me_see")
  tom_sprite:set_direction(2)
end

function tom_timer_2()
  sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.open")
end

function tom_timer_3()
  tom_sprite:set_animation("stopped")
  ladder_step1()
end

function ladder_step1()
  sol.audio.play_sound("door_open")
  sol.map.tile_set_group_enabled("ladder_step1", true)
  sol.map.tile_set_group_enabled("no_ladder_step1", false)
  sol.timer.start(1000, ladder_step2)
end

function ladder_step2()
  sol.audio.play_sound("door_open")
  sol.map.tile_set_group_enabled("ladder_step2", true)
  sol.timer.start(1000, ladder_step3)
end

function ladder_step3()
  sol.audio.play_sound("door_open")
  sol.map.tile_set_group_enabled("ladder_step3", true)
  sol.timer.start(1000, ladder_step4)
end

function ladder_step4()
  sol.map.tile_set_group_enabled("no_ladder", false)
  sol.map.sensor_set_enabled("tom_appears_sensor", false)
  sol.audio.play_sound("secret")
  sol.game.savegame_set_boolean(52, true)
  sol.map.hero_unfreeze()
end

function event_npc_interaction(npc_name)
  sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.finished")
end

function put_edelweiss()
  sol.map.dialog_start("outside_world.beaumont_hill_put_edelweiss")
end

function edelweiss_explode()

  sol.audio.play_sound("explosion")
  sol.map.explosion_create(160, 72, 0)
  sol.map.tile_set_enabled("beaumont_cave_hole", true)
  sol.map.teletransporter_set_enabled("to_beaumont_cave", true)
  sol.game.savegame_set_boolean(153, true)
  sol.game.set_item("level_4_way", 0)
  sol.timer.start(1000, edelweiss_end)
end

function edelweiss_end()
  sol.audio.play_sound("secret")
  sol.map.hero_unfreeze()
end

