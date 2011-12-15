-- Ganon - final boss

-- phase 1: periodically throws flames to the hero
-- phase 2: also periodically throws a set of fire bats in circles or randomly
--          or towards the hero

-- phase 1: 4 torches to light fast in the clockwise order
-- (hint given by the childs)
-- gives a special item to throw to Ganon
-- when touched by the item, Ganon is immobilized,
-- he becomes vulnerable to the sword
-- while he is vulnerable, fire bats appear in circle to protect him
-- when a torch is unlit, Ganon jumps towards the center and a floor where he was
-- is destroyed to make a lava hole
-- once the four lava holes are created and Ganon is hurt again,
-- after a fake end, phase 2 starts.

-- phase 2: the 4 torches enable 4 solid switches on the sides
-- each switch gives a bonus among:
-- - special item to throw
-- - heart or magic flask
-- - kill small enemies
-- - wrong switch, add more small enemies

local phase = 1
local vulnerable = false
local jumping = false
local attacking = false
local nb_floors_destroyed = 0
local nb_flames_created = 0
local nb_bats_created = 0
local cancel_next_attack = false

function event_appear()

  sol.enemy.set_life(1000000)
  sol.enemy.set_damage(16)
  sol.enemy.create_sprite("enemies/ganon")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("thrown_item", "immobilized")
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)
  --sol.enemy.set_can_hurt_hero_running(true)
end

function event_restart()

  if not jumping and not attacking then
    if not vulnerable then
      m = sol.main.path_finding_movement_create(64)
      sol.enemy.start_movement(m)
      if not attack_scheduled then
        sol.main.timer_start(prepare_attack, math.random(3000, 6000))
	attack_scheduled = true
      end
    else
      jump()
    end
  end
end

function event_movement_changed()

  -- take the appropriate sprite direction
  m = sol.enemy.get_movement()
  direction4 = sol.main.movement_get_property(m, "displayed_direction")
  sprite = sol.enemy.get_sprite()
  if direction4 == 1 then
    sol.main.sprite_set_direction(sprite, 1)
  else
    sol.main.sprite_set_direction(sprite, 0)
  end
end

function event_immobilized()

  if not vulnerable then
    vulnerable = true
    sol.enemy.set_attack_consequence("sword", 1)
    sol.enemy.set_attack_consequence("thrown_item", "protected")

    -- make a protection
    if sol.map.enemy_get_group_count(sol.enemy.get_name()) < 5 then
      print("go bats!")
      attacking = false
      prepare_bats()
      cancel_next_attack = true -- otherwise two attacks would be scheduled
    end
  end
end

function jump()

  local x, y = sol.enemy.get_position()
  local hero_x, hero_y = sol.map.hero_get_position()
  local angle = sol.main.get_angle(hero_x, hero_y, x, y)
  local m = sol.main.target_movement_create(128, 240, 245)
  sol.enemy.start_movement(m)
  sol.main.play_sound("jump")
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "jumping")
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("thrown_item", "ignored")
  vulnerable = false
  jumping = true
  sol.enemy.set_can_attack(false)
end

function event_obstacle_reached()

  event_movement_finished(sol.enemy.get_movement())
end

function event_movement_finished(m)

  local sprite = sol.enemy.get_sprite()
  if sol.main.sprite_get_animation(sprite) == "jumping" then
    finish_jump()
  end
end

function finish_jump()

  if phase == 1 then
    -- destroy floors
    local floors = { "floor_left_", "floor_right_", "floor_up_", "floor_down_" }
    nb_floors_destroyed = nb_floors_destroyed + 1
    if nb_floors_destroyed <= #floors then
      destroy_floor(floors[nb_floors_destroyed], 1, 50)
    else
      -- go to phase 2
      sol.enemy.set_life(64)
      phase = 2
    end
  end

  jumping = false
  sol.enemy.set_attack_consequence("thrown_item", "immobilized")
  sol.enemy.set_can_attack(true)
  sol.enemy.restart()
end

function event_sprite_animation_finished(sprite, animation)

  if animation == "jumping" then
    finish_jump()
  end
end

function destroy_floor(prefix, first, last)

  local index = first
  local delay = 30

  function repeat_change()
    if index % 10 == 1 then
      sol.main.play_sound("stone")
    end
    
    sol.map.tile_set_enabled(prefix .. index, false)

    if index ~= last then
      sol.main.timer_start(repeat_change, delay)
    end
    index = index + 1
  end
  repeat_change()
end

function prepare_attack()

  if phase == 1 or math.random(2) == 1 then
    prepare_flames()
  else
    prepare_bats()
  end
end

function prepare_flames()

  if vulnerable or jumping or attacking then
    return
  end

  if cancel_next_attack then
    cancel_next_attack = false
    return
  end

  attacking = true

  local prefix = sol.enemy.get_name() .. "_flame_"
  local nb_to_create = (1 + nb_floors_destroyed) * 5

  function repeat_throw_flame()

    if vulnerable then
      -- immobilized while shooting flames
      attacking = false
      return
    end

    sol.main.play_sound("lamp")
    nb_flames_created = nb_flames_created + 1
    local son_name = prefix .. nb_flames_created
    sol.enemy.create_son(son_name, "red_flame", 0, -24, 0)
    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      sol.main.timer_start(repeat_throw_flame, 150)
    else
      attacking = false
      attack_scheduled = false
      sol.enemy.restart()
    end
  end
  sol.enemy.stop_movement()
  repeat_throw_flame()
end

function prepare_bats()

  if jumping or attacking then
    print("no bats: jumping = ", jumping, " attacking = ", attacking)
    return
  end

  if cancel_next_attack then
    print("no bats: this attack is canceled")
    cancel_next_attack = false
    return
  end

  attacking = true

  local prefix = sol.enemy.get_name() .. "_bat_"
  local nb_to_create = 9

  function repeat_throw_bat()

    sol.main.play_sound("lamp")
    nb_bats_created = nb_bats_created + 1
    local son_name = prefix .. nb_bats_created
    sol.enemy.create_son(son_name, "fire_bat", 0, -21, 0)
    sol.enemy.send_message(son_name, "circle")

    sol.main.timer_start(function()
      if not sol.map.enemy_is_dead(son_name) then
        sol.enemy.send_message(son_name, "go_hero")
      end
    end, 2000 + (nb_to_create * 150))

    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      sol.main.timer_start(repeat_throw_bat, 233)
    else
      attacking = false
      attack_scheduled = false
      if not vulnerable then
	sol.enemy.restart()
      end
    end
  end
  sol.enemy.stop_movement()
  repeat_throw_bat()
end

