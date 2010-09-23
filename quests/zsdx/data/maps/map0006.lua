-----------------------------
-- Outside world D1 script --
-----------------------------

function event_map_started(destination_point_name)
  if is_ladder_activated() then
    tile_set_group_enabled("ladder_step", true)
    tile_set_group_enabled("no_ladder", false)
    npc_remove("tom")
    sensor_remove("tom_appears_sensor")
  else
    tile_set_group_enabled("ladder_step", false)
    tile_set_group_enabled("no_ladder", true)
  end
end

function is_ladder_activated()
  return savegame_get_boolean(52)
end

function event_hero_on_sensor(sensor_name)

  has_finished_tom_cave = savegame_get_boolean(37)

  if sensor_name == "tom_appears_sensor" and
    has_finished_tom_cave and not is_ladder_activated() then
    dialog_start("outside_world.tom_dungeon_1_entrance.hey")
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "outside_world.tom_dungeon_1_entrance.hey" then
    hero_freeze()
    hero_set_direction(0)
    npc_set_position("tom", 528, 245)
    npc_walk("tom", "44444444444444444444222", false, true)
  elseif first_message_id == "outside_world.tom_dungeon_1_entrance.need_help" then
    npc_set_direction("tom", 1)
    timer_start(1500, "tom_timer_1", false)
  elseif first_message_id == "outside_world.tom_dungeon_1_entrance.let_me_see" then
    play_sound("jump")
    npc_jump("tom", 4, 16, true)
  elseif first_message_id == "outside_world.tom_dungeon_1_entrance.open" then
    npc_set_animation("tom", "walking")
    timer_start(300, "tom_timer_3", false)
  end

end

function event_npc_movement_finished(npc_name)

  x,y = npc_get_position("tom")
  if x ~= 352 then
    npc_set_direction("tom", 2)
    dialog_start("outside_world.tom_dungeon_1_entrance.need_help")
  else
    npc_set_direction("tom", 1)
    timer_start(1000, "tom_timer_2", false)
  end
end

function tom_timer_1()
  dialog_start("outside_world.tom_dungeon_1_entrance.let_me_see")
  npc_set_direction("tom", 2)
end

function tom_timer_2()
  dialog_start("outside_world.tom_dungeon_1_entrance.open")
end

function tom_timer_3()
  npc_set_animation("tom", "stopped")
  ladder_step1()
end

function ladder_step1()
  play_sound("door_open")
  tile_set_group_enabled("ladder_step1", true)
  tile_set_group_enabled("no_ladder_step1", false)
  timer_start(1000, "ladder_step2", false)
end

function ladder_step2()
  play_sound("door_open")
  tile_set_group_enabled("ladder_step2", true)
  timer_start(1000, "ladder_step3", false)
end

function ladder_step3()
  play_sound("door_open")
  tile_set_group_enabled("ladder_step3", true)
  timer_start(1000, "ladder_step4", false)
end

function ladder_step4()
  tile_set_group_enabled("no_ladder", false)
  sensor_remove("tom_appears_sensor")
  play_sound("secret")
  savegame_set_boolean(52, true)
  hero_unfreeze()
end

function event_npc_dialog(npc_name)
  dialog_start("outside_world.tom_dungeon_1_entrance.finished")
end

