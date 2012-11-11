local map = ...
-- Lyriann cave 1F

local tom_initial_x = 0
local tom_initial_y = 0

local function has_seen_tom()
  return map:get_game():get_value("b47")
end

local function has_boomerang_of_tom()
  return map:get_game():get_value("b41")
end

local function has_finished_cavern()
  -- the cavern is considered has finished if the player has found the heart container
  return map:get_game():get_value("b37")
end

local function tom_please_help_dialog_finished(answer)

  map:get_game():set_value("b47", true)
  if answer == 0 then
    map:start_dialog("lyriann_cave.tom.accept_help", function()
      hero:start_treasure("boomerang", 1, 41)
    end)
  end
end

local function tom_go_back()

  give_boomerang_back()
  local x, y = tom:get_position()
  if y ~= tom_initial_y then
    local m = sol.movement.create("path")
    m:set_path{2,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
    m:set_speed(48)
    tom:start_movement(m)
    tom_sprite:set_animation("walking")
  end
end

local function give_boomerang_back()
  map:get_game():get_item("boomerang"):set_variant(0)
  map:get_game():set_value("b41", false)
end

local function start_moving_tom()
  local m = sol.movement.create("path")
  m:set_path{0,0,0,0,6,6,6,6,6,6}
  m:set_speed(48)
  tom:set_position(88, 509)
  tom:start_movement(m)
  tom:get_sprite():set_animation("walking")
end

function map:on_started(destination)

  tom_initial_x, tom_initial_y = tom:get_position()

  if has_finished_cavern() and not has_boomerang_of_tom() then
    tom:remove()
  end

  if map:get_game():get_value("b38") then
    barrier:set_enabled(false)
    open_barrier_switch:set_activated(true)
  end

  for _, enemy in ipairs(map:get_entities("battle_1_enemy")) do
    function enemy:on_dead()
      if not map:has_entities("battle_1_enemy") and battle_1_barrier:is_enabled() then
        map:move_camera(352, 288, 250, function()
          sol.audio.play_sound("secret")
          battle_1_barrier:set_enabled(false)
        end)
      end
    end
  end

  for _, enemy in ipairs(map:get_entities("battle_2_enemy")) do
    function enemy:on_dead()
      if not map:has_entities("battle_2_enemy") and battle_2_barrier:is_enabled() then
        map:move_camera(344, 488, 250, function()
          sol.audio.play_sound("secret")
          battle_2_barrier:set_enabled(false)
        end)
      end
    end
  end
end

function open_barrier_switch:on_activated()
  map:move_camera(136, 304, 250, function()
    sol.audio.play_sound("secret")
    barrier:set_enabled(false)
    map:get_game():set_value("b38", true)
  end)
end

function tom:on_interaction()

  if not has_seen_tom() then
    map:start_dialog("lyriann_cave.tom.first_time", tom_please_help_dialog_finished)
  elseif has_finished_cavern() then
    if has_boomerang_of_tom() then
      map:start_dialog("lyriann_cave.tom.cavern_finished", tom_go_back)
    else
      map:start_dialog("lyriann_cave.tom.see_you_later")
    end
  elseif has_boomerang_of_tom() then
    map:start_dialog("lyriann_cave.tom.not_finished", function(answer)
      if answer == 1 then
        give_boomerang_back()
        map:start_dialog("lyriann_cave.tom.gave_boomerang_back")
      end
    end)
  else
    map:start_dialog("lyriann_cave.tom.not_first_time", tom_please_help_dialog_finished)
  end
end

function tom:on_movement_finished()

  if has_boomerang_of_tom() then
    if has_finished_cavern() then
      map:start_dialog("lyriann_cave.tom.cavern_finished", tom_go_back)
    else
      map:start_dialog("lyriann_cave.tom.leaving.cavern_not_finished", tom_go_back)
    end
  else
    tom:set_position(tom_initial_x, tom_initial_y)
    tom:get_sprite():set_direction(3)
    hero:unfreeze()
  end
end

function leave_cavern_sensor:on_activated()

  if has_boomerang_of_tom() then
    hero:freeze()
    map:start_dialog("lyriann_cave.tom.leaving", function()
      sol.audio.play_sound("warp")
      hero:set_direction(1)
      sol.timer.start(1700, start_moving_tom)
    end)
  end
end

