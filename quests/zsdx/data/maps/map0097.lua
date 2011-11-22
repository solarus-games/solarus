-- Dungeon 10 2F

function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(205) then
    sol.map.block_set_enabled("block1_1",false)
    lock_torches_group1()
  end

  if sol.game.savegame_get_boolean(206) then
    sol.map.block_set_enabled("block2_1",false)
    lock_torches_group2()
  end

  if sol.game.savegame_get_boolean(227) then
    sol.map.switch_set_activated("switch1_1", true)
    sol.map.switch_set_activated("switch1_2", true)
    sol.map.switch_set_activated("switch1_3", true)
    sol.map.switch_set_activated("switch1_4", true)
  end
end

function are_group1_torches_on()

  return sol.map.npc_exists("torch1_1")
  and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch1_1")) == "lit"
  and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch1_2")) == "lit"
  and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch1_3")) == "lit"
end

function are_group2_torches_on()

  return sol.map.npc_exists("torch2_1")
  and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch2_1")) == "lit"
  and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch2_2")) == "lit"
  and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch2_3")) == "lit"
end


function lock_torches_group1()

  sol.map.npc_remove("torch1_1")
  sol.map.npc_remove("torch1_2")
  sol.map.npc_remove("torch1_3")
end

function lock_torches_group2()

  sol.map.npc_remove("torch2_1")
  sol.map.npc_remove("torch2_2")
  sol.map.npc_remove("torch2_3")
end

function event_switch_activated(switch_name)

  local error
  if switch_name == "switch1_1"  then
    error = false
    if sol.map.switch_is_activated("switch1_2") 
      or sol.map.switch_is_activated("switch1_3") 
      or sol.map.switch_is_activated("switch1_4") then
      error = true
    end
  end

  if switch_name == "switch1_2" then
    if sol.map.switch_is_activated("switch1_1") == false  then
      error = true
    end
  end

  if switch_name == "switch1_3" then
    if  sol.map.switch_is_activated("switch1_2") == false then
      error = true			
    end
  end

  if switch_name == "switch1_4" then
    if  sol.map.switch_is_activated("switch1_3") == false then
      error = true			
    end
  end

  if error then
    switch1_error()
  end

  if sol.map.switch_is_activated("switch1_1") 
    and sol.map.switch_is_activated("switch1_2") 
    and sol.map.switch_is_activated("switch1_3")
    and sol.map.switch_is_activated("switch1_4") then
    sol.main.play_sound("secret")
    sol.map.pickable_item_create("small_key", 1, 227, 144, 504, 0)
  end
end

function switch1_error()

  sol.main.play_sound("wrong")
  sol.map.switch_set_activated("switch1_1", false)
  sol.map.switch_set_activated("switch1_2", false)
  sol.map.switch_set_activated("switch1_3", false)
  sol.map.switch_set_activated("switch1_4", false)
end

function event_enemy_dead(enemy_name)

  if enemy_name:find("enemy_group2") 
      and sol.map.enemy_is_group_dead("enemy_group2")
      and not sol.game.savegame_get_boolean(221) then

    sol.map.camera_move(328, 320, 250, function()
      sol.map.pickable_item_create("small_key", 1, 221, 328, 325, 1)
      sol.main.play_sound("secret")
    end)
  end
end

function explode_block1() 

  sol.map.explosion_create(536, 256, 1)
  sol.main.play_sound("explosion")
  sol.main.play_sound("secret")
  sol.map.block_set_enabled("block1_1",false)
end

function explode_block2() 

  sol.map.explosion_create(552, 256, 1)
  sol.main.play_sound("explosion")
  sol.main.play_sound("secret")
  sol.map.block_set_enabled("block2_1",false)
end

function event_update()

  if not sol.game.savegame_get_boolean(205)
      and are_group1_torches_on() then
    sol.game.savegame_set_boolean(205, true)
    lock_torches_group1()
    sol.map.camera_move(536, 256, 250, explode_block1)
  end

  if not sol.game.savegame_get_boolean(206)
      and are_group2_torches_on() then
    sol.game.savegame_set_boolean(206, true)
    lock_torches_group2()
    sol.map.camera_move(552, 256, 250, explode_block2)
  end
end

