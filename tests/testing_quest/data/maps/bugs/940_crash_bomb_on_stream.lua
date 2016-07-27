local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  local stream_x, stream_y, stream_layer = stream:get_position()
  map:create_bomb({
    x = stream_x,
    y = stream_y,
    layer = stream_layer,
  })
  sol.main.exit()

end
