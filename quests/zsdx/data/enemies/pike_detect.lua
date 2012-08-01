local enemy = ...

-- Pike that moves when the hero is close

local state = "stopped" -- "stopped", "moving", "going_back", "paused"
local initial_xy = {}
local activation_distance = 24

function enemy:on_appear()

  self:set_life(1)
  self:set_damage(4)
  self:create_sprite("enemies/pike_detect")
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_can_hurt_hero_running(true)
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("thrown_item", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")

  initial_xy.x, initial_xy.y = self:get_position()
end

function enemy:on_update()

  if state == "stopped" and self:get_distance_to_hero() <= 192 then
    -- check whether the hero is close
    local x, y = self:get_position()
    local hero_x, hero_y = self:get_map():hero_get_position()
    local dx, dy = hero_x - x, hero_y - y

    if math.abs(dy) < activation_distance then
      if dx > 0 then
	go(0)
      else
	go(2)
      end
    end
    if state == "stopped" and math.abs(dx) < activation_distance then
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
  if not self:test_obstacles(dxy[index].x * 2, dxy[index].y * 2) then

    state = "moving"

    local x, y = self:get_position()
    local angle = direction4 * math.pi / 2
    local m = sol.movement.create("straight")
    m:set_speed(192)
    m:set_angle(angle)
    m:set_max_distance(104)
    m:set_smooth(false)
    self:start_movement(m)
  end
end

function enemy:on_obstacle_reached()

  go_back()
end

function enemy:on_movement_finished()

  go_back()
end

function enemy:on_collision_enemy(other_name, other_sprite, my_sprite)

  -- TODO: it would be better to have a way of getting the other's race
  if string.find(other_name, "^pike_") and state == "moving" then
    go_back()
  end
end

function go_back()

  if state == "moving" then

    state = "going_back"

    local m = sol.movement.create("target")
    m:set_speed(64)
    m:set_target(initial_xy.x, initial_xy.y)
    m:set_smooth(false)
    self:start_movement(m)
    sol.audio.play_sound("sword_tapping")

  elseif state == "going_back" then

    state = "paused"
    sol.timer.start(500, unpause)
  end
end

function unpause()
  state = "stopped"
end

