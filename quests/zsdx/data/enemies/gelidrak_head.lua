-- Gelidrak's head

local vulnerable = false      -- becomes vulnerable when the tail is hurt
local vulnerable_delay = 5000 -- delay while the head remains vulnerable
local nb_flames_created = 0

function event_appear()

  sol.enemy.set_life(24)
  sol.enemy.set_damage(12)
  sol.enemy.create_sprite("enemies/gelidrak_head")
  sol.enemy.set_size(32, 40)
  sol.enemy.set_origin(16, 24)
  sol.enemy.set_hurt_style("boss")
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_no_treasure()
  sol.enemy.set_layer_independent_collisions(true)
  sol.enemy.set_push_hero_on_sword(true)

  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_pushed_back_when_hurt(false)
end

function event_restart()

  if not vulnerable then
    sol.main.timer_stop_all()
    sol.main.timer_start(throw_flames, math.random(2000, 5000))
    go_back()
  else
    sol.enemy.set_can_attack(false)
  end
end

function event_collision_enemy(other_name, other_sprite, my_sprite)

  if not vulnerable then
    go_back()
  end
end

function go_back()

  local x, y = sol.map.enemy_get_position(sol.enemy.get_father())
  local m = sol.main.target_movement_create(16, x, y + 48)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
end

function event_movement_finished(movement)

  local m = sol.main.random_movement_create(16)
  sol.main.movement_set_property(m, "max_distance", 16)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
  sol.main.timer_start(go_back, 5000)
end

function event_message_received(src_enemy, message)

  if src_enemy == sol.enemy.get_father() then
    if message == "vulnerable" and not vulnerable then
      -- the head now becomes vulnerable
      vulnerable = true
      sol.enemy.stop_movement()
      sol.enemy.set_can_attack(false)
      sol.enemy.set_attack_consequence("sword", 1)
      local sprite = sol.enemy.get_sprite()
      sol.main.sprite_set_animation(sprite, "walking")
      sol.main.timer_stop_all()
      sol.main.timer_start(function()
	vulnerable = false
	event_restart()
	sol.enemy.set_can_attack(true)
        sol.enemy.set_attack_consequence("sword", "protected")
	sol.enemy.send_message(sol.enemy.get_father(), "recovered")
      end, vulnerable_delay)
    end
  end
end

function event_hurt(attack, life_lost)

  if sol.enemy.get_life() > 0 then
    -- notify the body (so that it is hurt too)
    sol.enemy.send_message(sol.enemy.get_father(), "hurt")
  else
    sol.main.timer_stop_all()
    sol.enemy.send_message(sol.enemy.get_father(), "dying")
  end
end

function event_dead()

  -- notify the body
  sol.enemy.send_message(sol.enemy.get_father(), "dead")
end

function throw_flames()

  if sol.map.enemy_get_group_count(sol.enemy.get_name() .. "_son_") < 5 then
    nb_flames_created = 0
    sol.enemy.stop_movement()
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "preparing_flame")
    sol.main.play_sound("lamp")
    sol.main.timer_start(repeat_flame, 500)
  end
end

function repeat_flame()

  local max_flames_created = 32 - sol.enemy.get_life()
  if nb_flames_created <= max_flames_created then
    nb_flames_created = nb_flames_created + 1
    local son_name = sol.enemy.get_name() .. "_son_" .. nb_flames_created
    local angle = math.random(360) * math.pi / 180
    sol.enemy.create_son(son_name, "blue_flame", 0, 16)
    sol.enemy.send_message(son_name, tostring(angle))
    sol.main.play_sound("lamp")
    sol.main.timer_start(repeat_flame, 150)
  else
    sol.main.timer_start(sol.enemy.restart, 500)
  end
end

