local map = ...
local game = map:get_game()

local success = false
function map:on_started()

  local movement = sol.movement.create("straight")
  movement:set_max_distance(32)
  movement:set_speed(32)
  movement:set_angle(0)
  movement:start(npc, function()
    movement:set_speed(32)
    movement:start(npc, function()
      success = true
    end)
  end)

  sol.timer.start(5000, function()
    assert(success)
    sol.main.exit()
  end)
end
