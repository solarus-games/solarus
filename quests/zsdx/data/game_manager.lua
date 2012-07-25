local g = {}

function g:on_started()
  print("player name: ", self:get_player_name())
end

function start_game(game)

  for method_name, method in pairs(g) do
    game[method_name] = method
  end
  game:start()
end

