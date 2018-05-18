local map = ...
local game = map:get_game()
local hero = map:get_hero()

local hit = false
local hit_sprite = false

function map:on_started()

  game:set_ability("sword", 1)

  assert_equal(enemy:get_attack_consequence("sword"), 1)
  assert_equal(enemy:get_attack_consequence_sprite(enemy:get_sprite(), "sword"), 1)

  enemy:set_attack_consequence("sword", function()
    hit = true
  end)

  local reaction = enemy:get_attack_consequence("sword")
  assert_equal(type(reaction), "function")
  local sprite_reaction = enemy:get_attack_consequence_sprite(enemy:get_sprite(), "sword")
  assert_equal(type(sprite_reaction), "function")
end

function map:on_opening_transition_finished()

  game:simulate_command_pressed("attack")

  sol.timer.start(map, 1000, function()
    assert(hit)

    enemy:set_attack_consequence_sprite(enemy:get_sprite(), "sword", function()
      hit_sprite = true
    end)

    sol.timer.start(map, 1000, function()
      assert(hit_sprite)
      assert_equal(hero:get_state(), "sword loading")
      sol.main.exit()
    end)
  end)
end
