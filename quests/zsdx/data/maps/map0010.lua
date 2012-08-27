local map = ...
-- Outside world B1

function map:on_started(destination_point)

  -- enable dark world
  if map:get_game():get_boolean(905) then
    map:set_tileset(13)
  end

  if map:get_game():get_boolean(921) then
    -- show the vine
    vine_start:remove()
    vine:remove()
    map:set_entities_enabled("vine", false)
  else
    vine:get_sprite():set_paused(true)
  end
end

-- Function called when the player presses the action key on the vine bottom
function vine_start:on_interaction()
  map:start_dialog("outside_world.vine_start")
end

-- Function called when the player uses an item on the vine bottom
function vine_start:on_interaction_item(item)

  if item:get_name():find("^bottle") and item:get_variant() == 2 then

    -- using water on the vine bottom
    hero:freeze()
    self:remove()
    item:set_variant(1)  -- make the bottle empty
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
    hero:unfreeze()
    map:get_game():set_boolean(921, true)
  end)

  local sprite = vine:get_sprite()
  sprite:set_ignore_suspend(true)
  sprite:set_paused(false)
end

