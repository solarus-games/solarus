local game = ...

function game:on_started()
  print("player name: ", self:get_player_name())
end

function game:on_finished()
  print("bye!")
end

-- Run the game.
game:start()

