local map = ...

-- Dungeon 3 1F.

local remove_water_delay = 500  -- delay between each step when some water is disappearing

-- Returns whether all five torches are on
local function are_all_torches_on()

  return torch_1 ~= nil
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
      and torch_4:get_sprite():get_animation() == "lit"
      and torch_5:get_sprite():get_animation() == "lit"
end

-- Makes all five torches on forever
local function lock_torches()
  -- the trick: just remove the interactive torches because there are static ones below
  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
  torch_4:remove()
  torch_5:remove()
end

local function remove_c_water()
  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  c_water_tile_out:set_enabled(true)
  c_water_tile_source:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_c_water_2)
end

local function remove_c_water_2()
  c_water_tile_middle:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_c_water_3)
end

local function remove_c_water_3()
  c_water_tile:set_enabled(false)
  c_water_tile_less_1:set_enabled(true)
  sol.timer.start(remove_water_delay, remove_c_water_4)
end

local function remove_c_water_4()
  c_water_tile_less_1:set_enabled(false)
  c_water_tile_less_2:set_enabled(true)
  sol.timer.start(remove_water_delay, remove_c_water_5)
end

local function remove_c_water_5()
  c_water_tile_less_2:set_enabled(false)
  c_water_tile_less_3:set_enabled(true)
  sol.timer.start(remove_water_delay, remove_c_water_6)
end

local function remove_c_water_6()
  c_water_tile_less_3:set_enabled(false)
  map:set_entities_enabled("c_water_on_jumper", false)
  map:set_entities_enabled("c_water_off_obstacle", true)
  map:get_game():set_value("121", true)
  sol.audio.play_sound("secret")
end

-- Called when the map starts
function map:on_started(destination)

  if map:get_game():get_value("904") then
    -- the door before the five torches is open
    ce_door_switch:set_activated(true)
  end

  if map:get_game():get_value("113") then
    -- the door after the five torches is open
    lock_torches()
  end

  if map:get_game():get_value("121") then
    -- the water at the center is removed
    map:set_entities_enabled("c_water_tile", false)
    map:set_entities_enabled("c_water_on_jumper", false)
  else
    map:set_entities_enabled("c_water_off_obstacle", false)
  end

  if map:get_game():get_value("122") then
    -- the east water is removed
    map:set_entities_enabled("e_water_tile", false)
    map:set_entities_enabled("e_water_tile_out", true)
    map:set_entities_enabled("e_water_on_jumper", false)
  else
    map:set_entities_enabled("e_water_off_obstacle", false)
  end

  if map:get_game():get_value("131") then
    -- the north water is removed
    map:set_entities_enabled("n_water_tile", false)
    map:set_entities_enabled("n_water_tile_out", true)
    map:set_entities_enabled("n_water_on_jumper", false)
  else
    map:set_entities_enabled("n_water_off_obstacle", false)
  end

end

-- Called when the opening transition of the map finished
function map:on_opening_transition_finished(destination)

  -- show the welcome message
  if destination:get_name() == "from_outside" then
    map:start_dialog("dungeon_3")
  end
end

function map:on_update()

  if not map:get_game():get_value("113")
    and are_all_torches_on() then

    lock_torches()
    map:move_camera(584, 360, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("torches_door")
    end)
  end
end

function se_door_switch:on_activated()

  if not se_door:is_open() then
    map:move_camera(800, 728, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("se_door")
    end)
  end
end

function ce_door_switch:on_activated()

  if not ce_door:is_open() then
    map:move_camera(736, 552, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("ce_door")
    end)
  end
end

function c_water_switch:on_activated()

  if not map:get_game():get_value("121") then
    map:move_camera(344, 736, 250, remove_c_water, 1000, 3500)
  end
end

