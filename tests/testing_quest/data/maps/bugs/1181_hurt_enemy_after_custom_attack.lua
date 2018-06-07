local map = ...
local game = map:get_game()

function map:on_started()

  game:set_ability("sword", 1)
  enemy:set_invincible()
  enemy:set_attack_consequence("sword", function()
    enemy:hurt(1)
    sol.timer.start(map, 1000, function()
      assert(enemy == nil)
      sol.main.exit()
    end)
  end)
end

function map:on_opening_transition_finished()

  game:simulate_command_pressed("attack")
end
