local map = ...
local game = map:get_game()
local hero = map:get_hero()

local test_maps = {
  "start_in_hole",
  "start_in_deep_water_drown",
  "start_in_deep_water_swim",
  "start_in_shallow_water",
  "start_in_lava",
}

function hero:assert_position_equal(other)

  local x_1, y_1, layer_1 = hero:get_position()
  local x_2, y_2, layer_2 = other:get_position()
  assert(x_1 == x_2)
  assert(y_1 == y_2)
  assert(layer_1 == layer_2)
end

function hero:assert_state_ground_animation(state, ground, animation)

  assert_equal(hero:get_state(), state)
  assert_equal(hero:get_ground_below(), ground)
  assert_equal(hero:get_sprite("tunic"):get_animation(), animation)
end

function map:on_started()

  sol.timer.start(game, 5000, function()
    if #test_maps == 0 then
      -- Finished.
      sol.main.exit()
    else
      -- Next map in the list.
      local next_map_id = test_maps[1]
      table.remove(test_maps, 1)
      hero:teleport("teletransportation_tests/" .. next_map_id)
      return true  -- Repeat the timer.
    end
  end)
end
