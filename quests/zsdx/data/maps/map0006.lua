-----------------------------
-- Outside world D1 script --
-----------------------------

tom_sprite = nil

function event_map_started(destination_point_name)

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
end

function is_ladder_activated()
  return sol.game.savegame_get_boolean(52)
end

function event_hero_on_sensor(sensor_name)

  has_finished_tom_cave = sol.game.savegame_get_boolean(37)

  if sensor_name == "tom_appears_sensor" and
    has_finished_tom_cave and not is_ladder_activated() then
    sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.hey")
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "outside_world.tom_dungeon_1_entrance.hey" then
    sol.map.hero_freeze()
    sol.map.hero_set_direction(0)
    sol.map.npc_set_position("tom", 528, 245)
    m = sol.main.path_movement_create("44444444444444444444222", 48)
    sol.main.movement_set_property(m, "ignore_obstacles", true)
    sol.map.npc_start_movement("tom", m)
    sol.main.sprite_set_animation(tom_sprite, "walking")
  elseif first_message_id == "outside_world.tom_dungeon_1_entrance.need_help" then
    sol.main.sprite_set_direction(tom_sprite, 1)
    sol.main.timer_start(1500, "tom_timer_1", false)
  elseif first_message_id == "outside_world.tom_dungeon_1_entrance.let_me_see" then
    sol.main.play_sound("jump")
    m = sol.main.jump_movement_create(4, 16)
    sol.main.movement_set_property(m, "ignore_obstacles", true)
    sol.map.npc_start_movement("tom", m)
  elseif first_message_id == "outside_world.tom_dungeon_1_entrance.open" then
    sol.main.sprite_set_animation(tom_sprite, "walking")
    sol.main.timer_start(300, "tom_timer_3", false)
  end

end

function event_npc_movement_finished(npc_name)

  x,y = sol.map.npc_get_position("tom")
  if x ~= 352 then
    sol.main.sprite_set_direction(tom_sprite, 2)
    sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.need_help")
  else
    sol.main.sprite_set_direction(tom_sprite, 1)
    sol.main.timer_start(1000, "tom_timer_2", false)
  end
end

function tom_timer_1()
  sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.let_me_see")
  sol.main.sprite_set_direction(tom_sprite, 2)
end

function tom_timer_2()
  sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.open")
end

function tom_timer_3()
  sol.main.sprite_set_animation(tom_sprite, "stopped")
  ladder_step1()
end

function ladder_step1()
  sol.main.play_sound("door_open")
  sol.map.tile_set_group_enabled("ladder_step1", true)
  sol.map.tile_set_group_enabled("no_ladder_step1", false)
  sol.main.timer_start(1000, "ladder_step2", false)
end

function ladder_step2()
  sol.main.play_sound("door_open")
  sol.map.tile_set_group_enabled("ladder_step2", true)
  sol.main.timer_start(1000, "ladder_step3", false)
end

function ladder_step3()
  sol.main.play_sound("door_open")
  sol.map.tile_set_group_enabled("ladder_step3", true)
  sol.main.timer_start(1000, "ladder_step4", false)
end

function ladder_step4()
  sol.map.tile_set_group_enabled("no_ladder", false)
  sol.map.sensor_set_enabled("tom_appears_sensor", false)
  sol.main.play_sound("secret")
  sol.game.savegame_set_boolean(52, true)
  sol.map.hero_unfreeze()
end

function event_npc_dialog(npc_name)
  sol.map.dialog_start("outside_world.tom_dungeon_1_entrance.finished")
end

