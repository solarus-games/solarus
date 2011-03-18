-- Link's cave

function event_map_started(destination_point_name)

  sol.map.enemy_set_enabled("zelda_enemy", false)
  sol.map.door_set_open("door", true)
  sol.map.teletransporter_set_enabled("teletransporter", false)
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  sol.main.play_music("boss.spc")
  sol.map.npc_set_position("zelda", 224, 85)
  sol.map.hero_freeze()
  sol.main.timer_start(1000, "angry_zelda", false)
  sol.game.add_life(80)
end

function angry_zelda()

  sol.map.dialog_start("link_cave.angry_zelda")
end

function event_dialog_finished(first_message_id)

  if first_message_id == "link_cave.angry_zelda" then

    m = sol.main.jump_movement_create(6, 24)
    sol.main.movement_set_property(m, "ignore_obstacles", true)
    sol.main.movement_set_property(m, "speed", 48)
    sol.map.npc_start_movement("zelda", m)

    zelda_sprite = sol.map.npc_get_sprite("zelda")
    sol.main.sprite_set_animation(zelda_sprite, "walking")

    sol.map.door_close("door")
  end
end

function event_npc_movement_finished(npc_name)

  if npc_name == "zelda" then
    sol.map.npc_set_position("zelda", -100, -100) -- disable the NPC
    x, y = sol.map.npc_get_position("zelda")
    sol.map.hero_unfreeze()
    sol.map.enemy_set_enabled("zelda_enemy", true) -- enable the enemy
  end
end

function event_update()

  if not sol.map.door_is_open("door") and sol.game.get_life() <= 4 then
    -- trigger the teleporter to the end screen
    sol.map.teletransporter_set_enabled("teletransporter", true)
  end
end
