local map = ...

local function sensor_on_activated(sensor)
  sensor.passed = true
end

for sensor in map:get_entities("sensor_") do
  sensor.on_activated = sensor_on_activated
end

function map:on_opening_transition_finished()

  sol.timer.start(12000, function()
    local num_passed = 0
    for sensor in map:get_entities("sensor_") do
      assert(sensor.passed)
      num_passed = num_passed + 1
    end
    assert_equal(num_passed, 8)
    sol.main.exit()
  end)
end
