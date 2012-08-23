local map = ...
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

function map:on_started(destination_point_name)

  map:set_doors_open("ne_door", true)
  map:set_doors_open("boss_door", true)
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "ne_door_sensor" then

    if map:door_is_open("ne_door") then
      map:close_doors("ne_door")
    else
      map:open_doors("ne_door")
    end
  elseif sensor_name == "start_boss_sensor"
      and not map:get_game():get_boolean(321)
      and not fighting_boss then

    map:sensor_set_enabled("start_boss_sensor", false)
    start_boss()
  end
end

function start_boss()

  map:get_hero():freeze()
  map:close_doors("boss_door")
  sol.timer.start(1000, function()
    sol.audio.play_music("boss")
    map:enemy_set_enabled("boss", true)
    map:get_hero():unfreeze()
    sol.timer.start(3000, repeat_lava_spawner)
    fighting_boss = true
  end)
end

function repeat_lava_spawner()

  if not map:get_game():get_boolean(321) then
    nb_spawners_created = nb_spawners_created + 1
    local index = math.random(#spawner_xy)
    map:create_enemy("spawner_"..nb_spawners_created,
    "drakomos_lava_spawner", 1, spawner_xy[index].x, spawner_xy[index].y)
    sol.timer.start(5000 + math.random(10000), repeat_lava_spawner)
  end
end

function map:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.audio.play_music("victory")
    map:get_hero():freeze()
    map:get_hero():set_direction(3)
    sol.timer.start(9000, start_final_sequence)
  elseif item_name == "quiver" then
    map:get_hero():start_victory()
  end
end

function start_final_sequence()

  sol.audio.play_music("dungeon_finished")
  map:get_hero():set_direction(1)
  map:npc_set_position("tom", 272, 237)
  map:move_camera(272, 232, 100, function()
    map:start_dialog("dungeon_6.tom")
    map:set_dialog_variable("dungeon_6.tom", map:get_game():get_player_name())
  end)
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "dungeon_6.tom" then

    sol.audio.stop_music()
    sol.timer.start(1000, function()
      sol.audio.play_music("legend")
      map:start_dialog("dungeon_6.tom_revelation")
      map:set_dialog_variable("dungeon_6.tom_revelation", map:get_game():get_player_name())
    end)
  elseif dialog_id == "dungeon_6.tom_revelation" then
    local variant = 2
    if map:get_game():get_boolean(939) then
      variant = 3
    end
    map:get_hero():start_treasure("quiver", variant, 941)
  end
end

function map:on_hero_victory_finished()
  map:get_game():set_dungeon_finished(6)
  map:get_game():set_boolean(155, false) -- reopen the rupee house
  map:get_hero():teleport(7, "from_dungeon_6")
end

