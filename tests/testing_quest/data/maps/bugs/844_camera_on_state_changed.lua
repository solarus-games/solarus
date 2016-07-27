local map = ...

local camera = map:get_camera()
local states = {}

function map:on_started()

  sol.timer.start(map, 100, function()
    camera:start_manual()
    sol.timer.start(map, 100, function()
      camera:start_tracking(hero)
      assert(states["manual"])
      assert(states["tracking"])
      sol.main.exit()
    end)
  end)
end

function camera:on_state_changed(state)

  states[state] = true
end
