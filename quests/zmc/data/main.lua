function sol.main:on_started()

  local game = sol.game.load("save.dat")

  if not sol.game.exists("save.dat") then
    game:set_starting_location("test_map", "initial_position")
    game:set_ability("tunic", 1)
  end

  game:start()
end

