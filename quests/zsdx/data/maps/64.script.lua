local map = ...
-- Dungeon 5 1F

sol.main.load_file("maps/prison_guard")(map)

-- Returns whether all torches are on
local function are_all_torches_on()

  return torch_1 ~= nil
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
      and torch_4:get_sprite():get_animation() == "lit"
end

-- Makes all torches on forever
local function lock_torches()
  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
  torch_4:remove()
end

function map:on_started(destination)

  -- torches door
  if map:get_game():get_value("b514") then
    lock_torches()
  end

  -- middle door
  if map:get_game():get_value("b522") then
    c_door_switch:set_activated(true)
  end

  -- block falled from 2F
  if not map:get_game():get_value("b515") then
    block_from_2f:set_enabled(false)
  end
end

function map:on_opening_transition_finished(destination)

  -- show the welcome message
  if destination:get_name() == "from_outside" then
    map:start_dialog("dungeon_5.welcome")
  end
end

function se_room_enemy:on_dead()

  if not se_door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("se_door")
  end
end

for _, enemy in ipairs(map:get_entities("s_room_enemy")) do

  function enemy:on_dead()
    if not map:has_entities("s_room_enemy")
        and not se_door:is_open() then
      sol.audio.play_sound("secret")
      map:open_doors("se_door")
    end
  end
end

function map:on_update()

  if not map:get_game():get_value("b514")
      and are_all_torches_on() then
    sol.audio.play_sound("secret")
    map:open_doors("w_door")
    lock_torches()
  end
end

function c_door_switch:on_activated()

  if not c_door:is_open() then
    map:move_camera(504, 504, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("c_door")
    end)
  end
end

function e_door_switch:on_activated()

  if not e_door:is_open() then
    map:move_camera(1048, 488, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("e_door")
    end)
  end
end

