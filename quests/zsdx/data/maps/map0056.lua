-- Dungeon 6 3F

fighting_boss = false

function event_map_started(destination_point_name)

  sol.map.door_set_open("miniboss_door", true)
  sol.map.enemy_set_group_enabled("miniboss", false)
  if sol.game.savegame_get_boolean(320) then
    sol.map.tile_set_group_enabled("miniboss_fake_floor", false)
  end
end

function are_all_torches_on()

  return sol.map.interactive_entity_exists("torch_1")
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_2")) == "lit"
end

-- Makes all torches on forever
function lock_torches()
  sol.map.interactive_entity_remove("torch_1")
  sol.map.interactive_entity_remove("torch_2")
end

function event_update()

  if not sol.map.door_is_open("torches_door")
      and are_all_torches_on() then

    sol.map.camera_move(360, 104, 250, open_torches_door)
  end
end

function open_torches_door()

  sol.main.play_sound("secret")
  sol.map.door_open("torches_door")
  lock_torches()
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor"
      and not sol.game.savegame_get_boolean(320)
      and not fighting_miniboss then

    sol.map.hero_freeze()
    sol.map.door_close("miniboss_door")
    fighting_miniboss = true
    sol.main.timer_start(function()
      sol.main.play_music("boss.spc")
      sol.map.enemy_set_group_enabled("miniboss", true)
      sol.map.tile_set_group_enabled("miniboss_fake_floor", false)
      sol.map.hero_unfreeze()
    end,
    1000)
  end
end

function event_enemy_dead(enemy_name)

  if string.find(enemy_name, "^miniboss")
      and sol.map.enemy_is_group_dead("miniboss") then

    sol.main.play_music("dark_world_dungeon.spc")
    sol.map.door_open("miniboss_door")
    sol.game.savegame_set_boolean(320, true)
  end
end


