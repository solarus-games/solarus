local map = ...
-- Fairy cave

local function has_fairy_appeared()
  return map:get_game():get_boolean(99)
end

local function has_boomerang_from_fairy()
  return map:get_game():get_boolean(100)
end

local function show_fairy()

  great_fairy:set_position(160, 77)
  great_fairy:get_sprite():set_ignore_suspend(true)
  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
  torch_4:remove()
end

-- Returns whether all four torches are on
local function are_all_torches_on()

  return torch_1:exists()
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
      and torch_4:get_sprite():get_animation() == "lit"
end

function map:on_started(destination_point)

  if has_fairy_appeared() then
    show_fairy()
  end
end

function map:on_update()

  if not has_fairy_appeared()
      and are_all_torches_on() then

    sol.audio.play_sound("secret")
    map:get_game():set_boolean(99, true)
    show_fairy()
  end
end

function fairy_sensor:on_activated()

  if has_fairy_appeared() then
    self:set_enabled(false)
    hero:freeze()
    hero:set_direction(1)
    if not has_boomerang_from_fairy() then
      map:start_dialog("fairy_cave.first_time", function()
        hero:unfreeze()
        hero:start_treasure("boomerang", 2, 100)
      end)
    else
      map:start_dialog("fairy_cave.restore_health", function()
        hero:unfreeze()
        map:get_game():add_life(map:get_game():get_max_life())
      end)
    end
  end
end

