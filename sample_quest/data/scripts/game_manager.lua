local game_manager = {}

function game_manager:start_game()

  local exists = sol.game.exists("save1.dat")
  game = sol.game.load("save1.dat")
  if not exists then
    -- Initialize a new savegame.
    game:set_max_life(12)
    game:set_life(game:get_max_life())
    game:set_ability("lift", 2)
    game:set_ability("sword", 1)
    game:set_starting_location("first_map") -- Starting location.
  end
  game:start()

  local hero = game:get_hero()
  hero:set_tunic_sprite_id("main_heroes/eldran")
end

return game_manager

