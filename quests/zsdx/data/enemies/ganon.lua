-- Ganon - final boss

-- phase 1: periodically throws flames to the hero
-- phase 2: also periodically throws a set of fire bats in circles or randomly
--          or towards the hero

-- phase 1: 4 torches to light fast in the clockwise order
-- (hint given by the childs)
-- gives a special item to throw to Ganon
-- when touched by the item, Ganon
-- becomes vulnerable to the sword until a torch stops
-- while he is vulnerable, fire bats appear in circle to protect him
-- when a torch is unlit, Ganon jumps to the center and a floor where he was
-- is destroyed to make a lava hole
-- once the four lava holes are created and Ganon is hurt again,
-- after a fake end, phase 2 starts.

-- phase 2: the 4 torches enable 4 solid switches on the sides
-- each switch gives a predetermined bonus and closes the other switches:
-- - special item to throw
-- - heart or magic flask
-- - kill small enemies
-- - wrong switch, add more small enemies

function event_appear()

  sol.enemy.set_life(16)
  sol.enemy.set_damage(24)
  sol.enemy.create_sprite("enemies/ganon")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_push_hero_on_sword(true)
  --sol.enemy.set_can_hurt_hero_running(true)
end

function event_restart()

  m = sol.main.path_finding_movement_create(32)
  sol.enemy.start_movement(m)
end

function event_movement_changed()

  m = sol.enemy.get_movement()
  direction4 = sol.main.movement_get_property(m, "displayed_direction")
  sprite = sol.enemy.get_sprite()
  if direction4 == 1 then
    sol.main.sprite_set_direction(sprite, 1)
  else
    sol.main.sprite_set_direction(sprite, 0)
  end
end

