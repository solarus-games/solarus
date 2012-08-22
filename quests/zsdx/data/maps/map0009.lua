local map = ...
-- Outside world A2

fighting_boss = false -- Agahnim

function map:on_started(destination_point_name)

  local new_music = nil

  if map:get_game():get_boolean(905) then
    -- enable dark world
    new_music = "dark_world"
    map:set_tileset(13)
    map:tile_set_group_enabled("castle_east_bridge", false)
    map:tile_set_group_enabled("castle_east_bridge_off", true)

    if map:get_game():get_boolean(907) then
      map:switch_set_activated("castle_door_switch", true)
    else
      map:tile_set_enabled("castle_door", true)
    end

    map:teletransporter_set_group_enabled("teletransporter_lw", false)

    -- Agahnim fight
    if destination_point_name == "from_dungeon_5_2F_ne"
        and map:get_game():get_boolean(507)
        and not map:get_game():get_boolean(520) then

      new_music = "none"
      map:npc_remove("cannon")
      map:enemy_set_group_enabled("", false) -- disable all simple enemies
    end

  else
    new_music = "overworld"
    map:tile_set_group_enabled("castle_east_bridge_off", false)
    map:teletransporter_set_group_enabled("teletransporter_dw", false)
  end

  sol.audio.play_music(new_music)
end

function map:on_switch_activated(switch_name)

  if switch_name == "castle_door_switch" then
    map:move_camera(296, 552, 250, open_castle_door)
  end
end

function open_castle_door()

  map:tile_set_enabled("castle_door", false)
  map:get_game():set_boolean(907, true)
  sol.audio.play_sound("secret")
  sol.audio.play_sound("door_open")
end

function map:on_npc_interaction(npc_name)

  if npc_name == "cannon" then

    if not map:get_game():get_boolean(905) then
      map:start_dialog("castle.cannon")
    else
      map:hero_freeze()
      local x, y = map:npc_get_position(npc_name)
      map:hero_set_position(x, y, 0)
      map:hero_set_visible(false)
      sol.audio.play_sound("bomb")
      sol.timer.start(2000, cannon_jump)
    end
  end
end

function cannon_jump()

  sol.audio.play_sound("explosion")
  map:create_explosion(296, 384, 0)
  map:hero_start_jumping(6, 424, true)
  map:hero_set_visible(true)
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and map:get_game():get_boolean(507)
      and not map:get_game():get_boolean(520)
      and not fighting_boss then

    -- Agahnim fight
    map:hero_freeze()
    map:tile_set_group_enabled("castle_roof_entrance", false)
    map:stairs_set_enabled("castle_roof_stairs", false)
    map:teletransporter_set_enabled("teletransporter_dw_roof", false)
    sol.audio.play_sound("door_closed")
    sol.timer.start(1000, start_boss)
  end
end

function start_boss()

  sol.audio.play_music("ganon_appears")
  map:enemy_set_enabled("boss", true)
  map:start_dialog("dungeon_5.agahnim_beginning")
  map:hero_unfreeze()
  fighting_boss = true
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "dungeon_5.agahnim_beginning" then
    sol.audio.play_music("ganon_battle")
  end
end

function map:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    map:get_game():set_dungeon_finished(5)
    sol.timer.start(9000, leave_boss)
    sol.audio.play_music("victory")
    map:hero_freeze()
    map:hero_set_direction(3)
  end
end

function leave_boss()

  map:get_hero():teleport(9, "from_dungeon_5_1F")
  sol.timer.start(700, restore_music)
end

function restore_music()
  sol.audio.play_music("dark_world")
end

