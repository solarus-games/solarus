local map = ...
-- Dungeon 3 2F

local remove_water_delay = 500 -- delay between each step when some water is disappearing

function map:on_started(destination_point)

  if map:get_game():get_boolean(127) then
    -- the barrier of the compass chest is removed
    barrier:set_enabled(false)
    barrier_switch:set_activated(true)
  end

  if map:get_game():get_boolean(128) then
    -- the south-east water is drained
    map:set_entities_enabled("se_water_tile", false)
    map:set_entities_enabled("se_water_tile_out", true)
    map:set_entities_enabled("se_water_on_jumper", false)
  else
    map:set_entities_enabled("se_water_off_obstacle", false)
  end

  if map:get_game():get_boolean(908) then
    -- shortcut A
    map:set_entities_enabled("shortcut_a", false)
    shortcut_a_switch:set_activated(true)
  end

  if map:get_game():get_boolean(909) then
    -- shortcut B
    map:set_entities_enabled("shortcut_b", false)
    shortcut_b_switch:set_activated(true)
  end

  -- north chest
  if map:get_game():get_boolean(950) then
    n_switch:set_activated(true)
  else
    n_chest:set_enabled(false)
  end
end

-- Called when the opening transition of the map finished
function map:on_opening_transition_finished(destination_point)

  -- show the welcome message
  if destination_point:get_name() == "from_outside" then
    map:start_dialog("dungeon_3")
  end
end

for _, enemy in ipairs(map:get_entities("e_room_enemy")) do
  function enemy:on_dead()

    if not map:has_entities("e_room_enemy")
        and not e_door:is_open() then
      map:move_camera(856, 472, 250, function()
        sol.audio.play_sound("secret")
        map:open_doors("e_door")
      end)
    end
  end
end

function barrier_switch:on_activated()

  if barrier:is_enabled() then
    map:move_camera(120, 240, 250, function()
      sol.audio.play_sound("secret")
      barrier:set_enabled(false)
      map:get_game():set_boolean(127, true)
    end)
  end
end

function se_water_switch:on_activated()

  if not map:get_game():get_boolean(128) then
    map:move_camera(912, 896, 250, remove_se_water, 1000, 3500)
  end
end

function n_1f_water_switch:on_activated()

  if not map:get_game():get_boolean(131) then
    remove_1f_n_water()
  end
end

function e_1f_water_switch_1:on_activated()

  if e_1f_water_switch_2:is_activated()
      and not map:get_game():get_boolean(122) then
    remove_1f_e_water()
  end
end

function e_1f_water_switch_2:on_activated()

  if e_1f_water_switch_1:is_activated()
      and not map:get_game():get_boolean(122) then
    remove_1f_e_water()
  end
end

function shortcut_a_switch:on_activated()

  map:set_entities_enabled("shortcut_a", false)
  map:get_game():set_boolean(908, true)
  sol.audio.play_sound("secret")
end

function shortcut_b_switch:on_activated()

  map:set_entities_enabled("shortcut_b", false)
  map:get_game():set_boolean(909, true)
  sol.audio.play_sound("secret")
end

function n_switch:on_activated()

  map:move_camera(280, 56, 250, function()
    sol.audio.play_sound("chest_appears")
    n_chest:set_enabled(true)
    map:get_game():set_boolean(950, true)
  end)
end

local function remove_se_water()
  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  se_water_tile_out:set_enabled(true)
  se_water_tile_source:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_se_water_2)
end

local function remove_se_water_2()
  se_water_tile_middle:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_se_water_3)
end

local function remove_se_water_3()
  se_water_tile_initial:set_enabled(false)
  se_water_tile_less_a:set_enabled(true)
  sol.timer.start(remove_water_delay, remove_se_water_4)
end

local function remove_se_water_4()
  se_water_tile_less_a:set_enabled(false)
  se_water_tile_less_b:set_enabled(true)
  sol.timer.start(remove_water_delay, remove_se_water_5)
end

local function remove_se_water_5()
  se_water_tile_less_b:set_enabled(false)
  se_water_tile_less_c:set_enabled(true)
  sol.timer.start(remove_water_delay, remove_se_water_6)
end

local function remove_se_water_6()
  se_water_tile_less_c:set_enabled(false)
  map:set_entities_enabled("se_water_on_jumper", false)
  map:set_entities_enabled("se_water_off_obstacle", true)
  map:get_game():set_boolean(128, true)
  sol.audio.play_sound("secret")
end

local function remove_1f_n_water()

  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  map:start_dialog("dungeon_3.water_drained_somewhere")
  map:get_game():set_boolean(131, true)
end

local function remove_1f_e_water()

  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  map:start_dialog("dungeon_3.water_drained_somewhere")
  map:get_game():set_boolean(122, true)
end

