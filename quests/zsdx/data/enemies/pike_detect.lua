local enemy = ...

-- Pike that moves when the hero is close.

local state = "stopped"  -- "stopped", "moving", "going_back" or "paused".
local initial_xy = {}
local activation_distance = 24

function enemy:on_created()

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

  local hero = self:get_map():get_entity("hero")
  if state == "stopped" and self:get_distance(hero) <= 192 then
    -- Check whether the hero is close.
    local x, y = self:get_position()
    local hero_x, hero_y = hero:get_position()
    local dx, dy = hero_x - x, hero_y - y

    if math.abs(dy) < activation_distance then
      if dx > 0 then
	self:go(0)
      else
	self:go(2)
      end
    end
    if state == "stopped" and math.abs(dx) < activation_distance then
      if dy > 0 then
	self:go(3)
      else
	self:go(1)
      end
    end
  end
end

function enemy:go(direction4)

  local dxy = {
    { x =  8, y =  0},
    { x =  0, y = -8},
    { x = -8, y =  0},
    { x =  0, y =  8}
  }

  -- Check that we can make the move.
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
    m:start(self)
  end
end

function enemy:on_obstacle_reached()

  self:go_back()
end

function enemy:on_movement_finished()

  self:go_back()
end

function enemy:on_collision_enemy(other_enemy, other_sprite, my_sprite)

  if other_enemy:get_breed() == self:get_breed() and state == "moving" then
    self:go_back()
  end
end

function enemy:go_back()

  if state == "moving" then

    state = "going_back"

    local m = sol.movement.create("target")
    m:set_speed(64)
    m:set_target(initial_xy.x, initial_xy.y)
    m:set_smooth(false)
    m:start(self)
    sol.audio.play_sound("sword_tapping")

  elseif state == "going_back" then

    state = "paused"
    sol.timer.start(self, 500, function() self:unpause() end)
  end
end

function enemy:unpause()
  state = "stopped"
end

