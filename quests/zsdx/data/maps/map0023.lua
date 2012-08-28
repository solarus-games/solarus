local map = ...
-- Dungeon 1 1F

function map:on_started(destination_point)

  map_chest:set_enabled(false)
  compass_chest:set_enabled(false)
  if map:get_game():get_boolean(54) then
    map_room_switch:set_activated(true)
  end

  close_sw_door_sensor:set_enabled(false)
end

function map:on_opening_transition_finished(destination_point)

  -- show the welcome message
  if destination_point:get_name() == "from_outside" then
    map:start_dialog("dungeon_1")
  end
end

function sw_switch:on_activated()
  map:move_camera(176, 392, 250, open_sw_door)
end

function nw_switch:on_activated()
  map:move_camera(176, 392, 250, open_sw_door)
end

function map_room_switch:on_activated()
  sol.audio.play_sound("chest_appears")
  map_chest:set_enabled(true)
end

function close_sw_door_sensor:on_activated()

  if sw_door:is_open() then
    close_sw_door()
    self:set_enabled(false)
  end
end

function enable_sensor:on_activated()
  close_sw_door_sensor:set_enabled(true)
end

local function open_sw_door()
  sol.audio.play_sound("secret")
  map:open_doors("sw_door")
  sw_switch:set_activated(true)
  nw_switch:set_activated(true)
end

local function close_sw_door()
  map:close_doors("sw_door")
  sw_switch:set_activated(false)
  nw_switch:set_activated(false)
end

for _, sensor in ipairs(map:get_entities("save_solid_ground_sensor")) do
  function sensor:on_activated()
    hero:save_solid_ground()
  end
end

for _, enemy in ipairs(map:get_entities("compass_room_battle")) do
  function enemy:on_dead()
    if not map:has_entities("compass_room_battle")
      and not compass_chest:is_enabled() then
      map:move_camera(408, 456, 250, function()
        sol.audio.play_sound("chest_appears")
        compass_chest:set_enabled(true)
      end)
    end
  end
end

function weak_wall:on_open()

  sol.audio.play_sound("secret")
end

