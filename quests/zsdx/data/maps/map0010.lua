local map = ...
-- Outside world B1

function map:on_started(destination_point_name)

  -- enable dark world
  if map:get_game():get_boolean(905) then
    map:set_tileset(13)
  end

  if map:get_game():get_boolean(921) then
    -- show the vine
    map:npc_remove("vine_start")
    map:npc_remove("vine")
    map:tile_set_group_enabled("vine", false)
  else
    local sprite = map:npc_get_sprite("vine")
    sprite:set_paused(true)
  end
end

-- Function called when the player presses the action key on the vine bottom
function map:on_npc_interaction(npc_name)

  if npc_name == "vine_start" then
    map:start_dialog("outside_world.vine_start")
  end
end

-- Function called when the player uses an item on the vine bottom
function map:on_npc_interaction_item(npc_name, item_name, variant)

  if npc_name == "vine_start" and
      string.find(item_name, "^bottle") and variant == 2 then

    -- using water on the vine bottom
    map:get_hero():freeze()
    map:npc_remove("vine_start")
    map:get_game():set_item(item_name, 1) -- make the bottle empty
    map:get_game():set_boolean(921, true)
    sol.audio.play_sound("item_in_water")
    sol.timer.start(1000, show_vine)
    return true
  end

  return false
end

function show_vine()

  map:move_camera(608, 136, 136, function()
    sol.audio.play_sound("secret")
    map:get_hero():unfreeze()
    map:get_game():set_boolean(921, true)
  end)

  local sprite = map:npc_get_sprite("vine")
  sprite:set_ignore_suspend(true)
  sprite:set_paused(false)
end

