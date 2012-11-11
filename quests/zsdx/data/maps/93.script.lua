local map = ...
-- North Fairy Fountain

local function has_fairy_appeared()
  return map:get_game():get_value("b929")
end

local function has_tiger_scrolls()
  return map:get_game():get_value("b930")
end

local function show_fairy()

  great_fairy:set_position(160, 77)
  great_fairy:get_sprite():set_ignore_suspend(true)
  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
  torch_4:remove()
end

local function are_all_torches_on()

  return torch_1 ~= nil
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
      and torch_4:get_sprite():get_animation() == "lit"
end

function map:on_started(destination)

  if has_fairy_appeared() then
    show_fairy()
  end
end

function map:on_update()

  if not has_fairy_appeared()
      and are_all_torches_on() then

    sol.audio.play_sound("secret")
    map:get_game():set_value("b929", true)
    show_fairy()
  end
end

function fairy_sensor:on_activated()

  if has_fairy_appeared() then

    self:set_enabled(false)
    hero:freeze()
    hero:set_direction(1)
    if not has_tiger_scrolls() then
      map:start_dialog("north_fairy_fountain.first_time", function()
        hero:unfreeze()
        hero:start_treasure("level_4_way", 4, 930)
      end)
    else
      map:start_dialog("north_fairy_fountain.restore_health", function()
        hero:unfreeze()
        map:get_game():add_life(map:get_game():get_max_life())
      end)
    end
  end
end

