local map = ...
-- Lost Woods cave

function map:on_started()
  -- Enable small keys on this map.
  map.small_keys_savegame_variable = "i1201"
end

local function remove_torch_a()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  torch_a:set_enabled(false)
  torch_a_2:set_enabled(false)
  torch_a_switch:set_activated(true)
end

local function remove_torch_b()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  torch_b:set_enabled(false)
  torch_b_2:set_enabled(false)
  torch_b_switch:set_activated(true)
end

local function remove_torch_c()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  torch_c:set_enabled(false)
  torch_c_2:set_enabled(false)
end

local function remove_torch_d()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  torch_d:set_enabled(false)
  torch_d_2:set_enabled(false)
end

local function remove_torch_e()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  torch_e:set_enabled(false)
  torch_e_2:set_enabled(false)
end

local function put_torch_a()
  sol.audio.play_sound("door_closed")
  torch_a:set_enabled(true)
  torch_a_2:set_enabled(true)
  torch_a_switch:set_activated(false)
  torch_a_switch_2:set_activated(false)
end

local function put_torch_b()
  sol.audio.play_sound("door_closed")
  torch_b:set_enabled(true)
  torch_b_2:set_enabled(true)
  torch_b_switch:set_activated(false)
  torch_b_switch_2:set_activated(false)
end

function torch_a_switch:on_activated()
  map:move_camera(176, 264, 250, remove_torch_a)
end

function torch_a_switch_2:on_activated()
  map:move_camera(176, 264, 250, remove_torch_a)
end

function torch_b_switch:on_activated()
  map:move_camera(224, 264, 250, remove_torch_b)
end

function torch_b_switch_2:on_activated()
  map:move_camera(224, 264, 250, remove_torch_b)
end

function torch_c_switch:on_activated()
  map:move_camera(608, 192, 250, remove_torch_c)
end

function torch_d_switch:on_activated()
  map:move_camera(584, 48, 250, remove_torch_d)
end

function torch_e_switch:on_activated()
  map:move_camera(512, 456, 250, remove_torch_e)
end

function close_torch_a_sensor:on_activated()

  if not torch_a:is_enabled()
      and not torch_a_switch_2:is_activated() then
    put_torch_a()
  end
end

function close_torch_b_sensor:on_activated()

  if not torch_b:is_enabled()
      and not torch_b_switch_2:is_activated() then
    put_torch_b()
  end
end

