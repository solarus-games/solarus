-- Dungeon 6 4F

fighting_boss = false
nb_spawners_created = 0

-- possible positions of Drakomos lava spawners
spawner_xy = {
  { x = 176, y = 301 },
  { x = 272, y = 325},
  { x = 320, y = 301},
  { x = 392, y = 365},
  { x = 400, y = 237},
  { x = 144, y = 245},
  { x = 152, y = 325},
  { x = 152, y = 365},
  { x = 312, y = 357},
  { x = 392, y = 325},
  { x = 232, y = 301},
  { x = 368, y = 301}
}

function event_map_started(destination_point_name)

  sol.map.door_set_open("ne_door", true)
  sol.map.door_set_open("boss_door", true)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "ne_door_sensor" then

    if sol.map.door_is_open("ne_door") then
      sol.map.door_close("ne_door")
    else
      sol.map.door_open("ne_door")
    end
  elseif sensor_name == "start_boss_sensor"
      and not sol.game.savegame_get_boolean(321)
      and not fighting_boss then

    sol.map.sensor_set_enabled("start_boss_sensor", false)
    start_boss()
  end
end

function start_boss()

  sol.map.hero_freeze()
  sol.map.door_close("boss_door")
  sol.main.timer_start(function()

    sol.main.play_music("boss.spc")
    sol.map.enemy_set_enabled("boss", true)
    sol.map.hero_unfreeze()
    sol.main.timer_start(repeat_lava_spawner, 3000)
    fighting_boss = true

  end,
  1000)
end

function repeat_lava_spawner()

  if not sol.game.savegame_get_boolean(321) then
    nb_spawners_created = nb_spawners_created + 1
    local index = math.random(#spawner_xy)
    sol.map.enemy_create("spawner_"..nb_spawners_created,
    "drakomos_lava_spawner", 1, spawner_xy[index].x, spawner_xy[index].y)
    sol.main.timer_start(repeat_lava_spawner, 5000 + math.random(10000))
  end
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.main.play_music("victory.spc")
    sol.map.hero_freeze()
    sol.map.hero_set_direction(3)
    sol.main.timer_start(start_final_sequence, 9000)
  elseif item_name == "quiver" then
    sol.map.hero_start_victory_sequence()
  end
end

function start_final_sequence()

  sol.main.play_music("dungeon_finished.spc")
  sol.map.hero_set_direction(1)
  sol.map.npc_set_position("tom", 272, 237)
  sol.map.camera_move(272, 232, 100, function()
    sol.map.dialog_start("dungeon_6.tom")
    sol.map.dialog_set_variable("dungeon_6.tom", sol.game.savegame_get_name());
  end)
end

function event_dialog_finished(first_message_id)

  if first_message_id == "dungeon_6.tom" then

    sol.main.play_music("none")
    sol.main.timer_start(function()
      sol.main.play_music("legend.spc")
      sol.map.dialog_start("dungeon_6.tom_revelation")
      sol.map.dialog_set_variable("dungeon_6.tom_revelation", sol.game.savegame_get_name());
    end,
    1000)
  elseif first_message_id == "dungeon_6.tom_revelation" then
    local variant = 2
    if sol.game.savegame_get_boolean(939) then
      variant = 3
    end
    sol.map.treasure_give("quiver", variant, 941)
  end
end

function event_hero_victory_sequence_finished()
  sol.game.set_dungeon_finished(6)
  sol.map.hero_set_map(7, "from_dungeon_6", 1)
end

