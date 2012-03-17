-- Ganon - final boss

-- Phase 1: Ganon periodically throws flames toward the hero.
-- 4 torches have to be light fast enough and in the clockwise order
-- (hint given by the childs) to make spawn a special item to throw to Ganon.
-- When touched by the item, Ganon is immobilized
-- and he becomes vulnerable to the sword.
-- While he is vulnerable, fire bats appear in circle to protect him.
-- After being immobilized, Ganon jumps towards the center and a
-- part of the floor is destroyed to make a lava hole.
-- Once four lava holes are created and Ganon is hurt again,
-- phase 2 starts.

-- Phase 2: Ganon also throws fire bats periodically.
-- The 4 torches now enable 4 solid switches on the sides.
-- Each switch may give a bonus among:
-- - special item to throw
-- - heart or magic flask
-- - kill all small enemies
-- - wrong switch, add more fire bats

local phase = 1
local vulnerable = false
local jumping = false
local attacking = false
local nb_floors_destroyed = 0
local nb_flames_created = 0
local nb_bats_created = 0
local cancel_next_attack = false
local sprite
local timers = {}

function self:on_appeared()

  self:set_life(1000000)
  self:set_damage(16)
  sprite = self:create_sprite("enemies/ganon")
  self:set_optimization_distance(0)
  self:set_size(32, 32)
  self:set_origin(16, 29)
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("thrown_item", "immobilized")
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
end

function self:on_restarted()

  if not jumping and not attacking then
    if not vulnerable then
      local m = sol.movement.path_finding_movement_create{speed = 64}
      self:start_movement(m)
      self:set_hurt_style("normal")
      if not attack_scheduled then
	schedule_attack()
      end
    else
      jump()
    end
  end
end

function self:on_movement_changed(movement)

  -- take the appropriate sprite direction
  local direction4 = movement:get_displayed_direction()
  if direction4 == 1 then
    sprite:set_direction(1)
  else
    sprite:set_direction(0)
  end
end

function self:on_immobilized()

  if not vulnerable then
    vulnerable = true
    self:set_attack_consequence("sword", 1)
    self:set_attack_consequence("thrown_item", "protected")
    self:set_hurt_style("boss")

    -- make a protection
    if #sol.map:find(self:get_id() .. "_bats_") < 9 then
      attacking = false
      throw_bats()
      cancel_next_attack = true -- otherwise two attacks would be scheduled
    end
  end
end

function jump()

  local x, y = self:get_position()
  local hero_x, hero_y = sol.map:get_hero():get_position()
  local angle = sol.main.get_angle(hero_x, hero_y, x, y)
  local m = sol.movement.target_movement_create{
    speed = 128, target_x = 240, target_y = 245}
  self:start_movement(m)
  sol.audio.play_sound("jump")
  sprite:set_animation("jumping")
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("thrown_item", "ignored")
  vulnerable = false
  jumping = true
  self:set_can_attack(false)
end

function self:on_obstacle_reached()

  self:on_movement_finished(self:get_movement())
end

function self:on_movement_finished(movement)

  if sprite:get_animation() == "jumping" then
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
      self:set_life(24)
      phase = 2
    end
  end

  jumping = false
  self:set_attack_consequence("thrown_item", "immobilized")
  self:set_can_attack(true)
  self:restart()
end

function self:on_sprite_animation_finished(sprite, animation)

  if animation == "jumping" then
    finish_jump()
  end
end

function destroy_floor(prefix, first, last)

  local index = first
  local delay = 30

  function repeat_change()
    if index % 10 == 1 then
      sol.audio.play_sound("stone")
    end
 
    sol.map:get_entity(prefix .. index):set_enabled(false)

    if index ~= last then
      sol.timer.start(delay, repeat_change)
    end
    index = index + 1
  end
  repeat_change()
end

function attack()

  if phase == 1 or math.random(2) == 1 then
    throw_flames()
  else
    throw_bats()
  end
end

function throw_flames()

  if vulnerable or jumping or attacking then
    return
  end

  if cancel_next_attack then
    cancel_next_attack = false
    return
  end

  attacking = true

  local prefix = self:get_id() .. "_flame_"
  local nb_to_create = (1 + nb_floors_destroyed) * 5

  function repeat_throw_flame()

    if vulnerable then
      -- immobilized while shooting flames
      attacking = false
      return
    end

    sol.audio.play_sound("lamp")
    nb_flames_created = nb_flames_created + 1
    local son_id = prefix .. nb_flames_created
    self:create_son{id = son_id, breed = "red_flame", dy = -24}
    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      timers[#timers + 1] = sol.timer.start(150, repeat_throw_flame)
    else
      attacking = false
      attack_scheduled = false
      self:restart()
    end
  end
  self:stop_movement()
  repeat_throw_flame()
end

function throw_bats()

  if jumping or attacking then
    return
  end

  if cancel_next_attack then
    cancel_next_attack = false
    return
  end

  attacking = true

  local prefix = self:get_id() .. "_bat_"
  local nb_to_create = 9

  function repeat_throw_bat()

    sol.audio.play_sound("lamp")
    nb_bats_created = nb_bats_created + 1
    local son_id = prefix .. nb_bats_created
    local son = self:create_son{id = son_id, breed = "fire_bat", dy = -21}
    if math.random(6) == 1 then
      son:set_treasure("magic_flask", 1, -1)
    end
    son:go_circle()
    local go_hero_delay = 2000 + (nb_to_create * 150)
    timers[#timers + 1] = sol.timer.start(go_hero_delay, function()
      son:go_hero()
    end)

    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      timers[#timers + 1] = sol.timer.start(233, repeat_throw_bat)
    else
      attacking = false
      attack_scheduled = false
      if not vulnerable then
	self:restart()
      else
	cancel_next_attack = false
      end
    end
  end
  self:stop_movement()
  repeat_throw_bat()
end

function schedule_attack()

  timers[#timers + 1] = sol.timer.start(math.random(3000, 6000), attack)
  attack_scheduled = true
end

function self:on_hurt(attack, life_lost)

  if self:get_life() <= 0 then
    sol.timer.stop_all(timers)
  end
end

