-- Pike that moves when the hero is close

state = "stopped" -- "stopped", "moving", "going_back", "paused"
initial_xy = {}
activation_distance = 20

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/pike_detect")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_can_hurt_hero_running(true)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("thrown_item", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")

  initial_xy.x, initial_xy.y = sol.enemy.get_position()
end

function event_update()

  if state == "stopped" and sol.enemy.get_distance_to_hero() <= 192 then
    -- check whether the hero is close
    local x, y = sol.enemy.get_position()
    local hero_x, hero_y = sol.map.hero_get_position()
    local dx, dy = hero_x - x, hero_y - y

    if math.abs(dy) < activation_distance then
      if dx > 0 then
	go(0)
      else
	go(2)
      end
    elseif math.abs(dx) < activation_distance then
      if dy > 0 then
	go(3)
      else
	go(1)
      end
    end
  end
end

function go(direction4)

  local dxy = {
    { x =  8, y =  0},
    { x =  0, y = -8},
    { x = -8, y =  0},
    { x =  0, y =  8}
  }
  
  -- check that we can make the move
  local index = direction4 + 1
  if not sol.enemy.test_obstacles(dxy[index].x * 2, dxy[index].y * 2) then

    state = "moving"

    local x, y = sol.enemy.get_position()
    local angle = direction4 * math.pi / 2
    local m = sol.main.straight_movement_create(192, angle)
    sol.main.movement_set_property(m, "max_distance", 104)
    sol.main.movement_set_property(m, "smooth", false)
    sol.enemy.start_movement(m)
  end
end

function event_obstacle_reached()

  go_back()
end

function event_movement_finished()

  go_back()
end

function event_collision_enemy(other_name, other_sprite, my_sprite)

  -- TODO: it would be better to have a way of getting the other's race
  if string.find(other_name, "^pike_") and state == "moving" then
    go_back()
  end
end

function go_back()

  if state == "moving" then

    state = "going_back"

    local m = sol.main.target_movement_create(64, initial_xy.x, initial_xy.y)
    sol.main.movement_set_property(m, "smooth", false)
    sol.enemy.start_movement(m)
    sol.main.play_sound("sword_tapping")

  elseif state == "going_back" then

    state = "paused"
    sol.main.timer_start(unpause, 500)
  end
end

function unpause()
  state = "stopped"
end

