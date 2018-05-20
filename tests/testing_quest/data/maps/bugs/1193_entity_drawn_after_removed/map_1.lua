local map = ...
local game = map:get_game()

local e = entity
local map_finished = false

function map:on_opening_transition_finished()

  assert(entity:exists())

  sol.timer.start(map, 100, function()
    game:simulate_command_pressed("left")
  end)
end

function map:on_finished()

  assert(e ~= nil)
  assert(entity == nil)
  assert(e:exists())
  e:remove()
  assert(not e:exists())

  map_finished = true
end

function entity:on_pre_draw()

  -- After the map is finished, the entity is removed and should no longer be drawn.
  assert(not map_finished)
  assert(entity ~= nil)
  assert(self == e)
  assert(self:exists())
  assert(e:exists())
end
