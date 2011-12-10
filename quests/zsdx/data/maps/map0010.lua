-- Outside world B1

function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.map.tileset_set(13)
  end

  if sol.game.savegame_get_boolean(921) then
    -- show the vine
    sol.map.npc_remove("vine_start")
    sol.map.npc_remove("vine")
    sol.map.tile_set_group_enabled("vine", false)
  else
    local sprite = sol.map.npc_get_sprite("vine")
    sol.main.sprite_set_paused(sprite, true)
  end
end

-- Function called when the player presses the action key on the vine bottom
function event_npc_interaction(npc_name)

  if npc_name == "vine_start" then
    sol.map.dialog_start("outside_world.vine_start")
  end
end

-- Function called when the player uses an item on the vine bottom
function event_npc_interaction_item(npc_name, item_name, variant)

  if npc_name == "vine_start" and
      string.find(item_name, "^bottle") and variant == 2 then

    -- using water on the vine bottom
    sol.map.hero_freeze()
    sol.map.npc_remove("vine_start")
    sol.game.set_item(item_name, 1) -- make the bottle empty
    sol.game.savegame_set_boolean(921, true)
    sol.main.play_sound("item_in_water")
    sol.main.timer_start(show_vine, 1000)
    return true
  end

  return false
end

function show_vine()

  sol.map.camera_move(608, 136, 136, function()
    sol.main.play_sound("secret")
    sol.map.hero_unfreeze()
    sol.game.savegame_set_boolean(921, true)
  end)

  local sprite = sol.map.npc_get_sprite("vine")
  sol.main.sprite_set_animation_ignore_suspend(sprite, true)
  sol.main.sprite_set_paused(sprite, false)
end

