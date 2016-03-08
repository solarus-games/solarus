local map = ...
local game = map:get_game()
local start_location_checker = require("maps/bugs/819_destination_starting_location_mode/start_location_checker")
local first_time = true

function map:on_opening_transition_finished()

  if not first_time then
    return
  end
  first_time = false

  start_location_checker.check(game, "2", "destination_1")
  hero:teleport(map:get_id(), "destination_2", "immediate")
  sol.timer.start(20, function()
    start_location_checker.check(game, "2", "destination_1")
    hero:teleport(map:get_id(), "destination_3", "immediate")
    sol.timer.start(20, function()
      start_location_checker.check(game, "2", "destination_3")
      hero:teleport("bugs/819_destination_starting_location_mode/3", "destination_1", "immediate")
    end)
  end)

end
