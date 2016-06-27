local map = ...
local game = map:get_game()

local test_maps = {
  "start_in_hole",
}

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
