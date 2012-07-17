-- Outside world A2

fighting_boss = false -- Agahnim

function event_map_started(destination_point_name)

  local new_music = nil

  if sol.map.get_game():get_boolean(905) then
    -- enable dark world
    new_music = "dark_world"
    sol.map.tileset_set(13)
    sol.map.tile_set_group_enabled("castle_east_bridge", false)
    sol.map.tile_set_group_enabled("castle_east_bridge_off", true)

    if sol.map.get_game():get_boolean(907) then
      sol.map.switch_set_activated("castle_door_switch", true)
    else
      sol.map.tile_set_enabled("castle_door", true)
    end

    sol.map.teletransporter_set_group_enabled("teletransporter_lw", false)

    -- Agahnim fight
    if destination_point_name == "from_dungeon_5_2F_ne"
        and sol.map.get_game():get_boolean(507)
        and not sol.map.get_game():get_boolean(520) then

      new_music = "none"
      sol.map.npc_remove("cannon")
      sol.map.enemy_set_group_enabled("", false) -- disable all simple enemies
    end

  else
    sol.map.tile_set_group_enabled("castle_east_bridge_off", false)
    sol.map.teletransporter_set_group_enabled("teletransporter_dw", false)
  end

  if new_music ~= nil then
    sol.audio.play_music(new_music)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "castle_door_switch" then
    sol.map.camera_move(296, 552, 250, open_castle_door)
  end
end

function open_castle_door()

  sol.map.tile_set_enabled("castle_door", false)
  sol.map.get_game():set_boolean(907, true)
  sol.audio.play_sound("secret")
  sol.audio.play_sound("door_open")
end

function event_npc_interaction(npc_name)

  if npc_name == "cannon" then

    if not sol.map.get_game():get_boolean(905) then
      sol.map.dialog_start("castle.cannon")
    else
      sol.map.hero_freeze()
      local x, y = sol.map.npc_get_position(npc_name)
      sol.map.hero_set_position(x, y, 0)
      sol.map.hero_set_visible(false)
      sol.audio.play_sound("bomb")
      sol.timer.start(2000, cannon_jump)
    end
  end
end

function cannon_jump()

  sol.audio.play_sound("explosion")
  sol.map.explosion_create(296, 384, 0)
  sol.map.hero_start_jumping(6, 424, true)
  sol.map.hero_set_visible(true)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and sol.map.get_game():get_boolean(507)
      and not sol.map.get_game():get_boolean(520)
      and not fighting_boss then

    -- Agahnim fight
    sol.map.hero_freeze()
    sol.map.tile_set_group_enabled("castle_roof_entrance", false)
    sol.map.stairs_set_enabled("castle_roof_stairs", false)
    sol.map.teletransporter_set_enabled("teletransporter_dw_roof", false)
    sol.audio.play_sound("door_closed")
    sol.timer.start(1000, start_boss)
  end
end

function start_boss()

  sol.audio.play_music("ganon_appears")
  sol.map.enemy_set_enabled("boss", true)
  sol.map.dialog_start("dungeon_5.agahnim_beginning")
  sol.map.hero_unfreeze()
  fighting_boss = true
end

function event_dialog_finished(dialog_id, answer)

  if dialog_id == "dungeon_5.agahnim_beginning" then
    sol.audio.play_music("ganon_battle")
  end
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.map.get_game():set_dungeon_finished(5)
    sol.timer.start(9000, leave_boss)
    sol.audio.play_music("victory")
    sol.map.hero_freeze()
    sol.map.hero_set_direction(3)
  end
end

function leave_boss()

  sol.map.hero_set_map(9, "from_dungeon_5_1F", 1)
  sol.timer.start(700, restore_music)
end

function restore_music()
  sol.audio.play_music("dark_world")
end

