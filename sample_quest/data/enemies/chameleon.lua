local enemy = ...

local sprite, tongue -- Main sprites.

function enemy:on_created()
  -- Set properties. 
  self:set_life(1); self:set_damage(1); self:set_hurt_style("normal")
  self:set_pushed_back_when_hurt(true); self:set_push_hero_on_sword(false)
  self:set_traversable(true)
  -- Create sprite if necessary, etc.
  sprite = self:get_sprite()
  if not sprite then sprite = self:create_sprite("animals/chameleon") end
  function sprite:on_animation_finished(animation)
  	if animation == "looking" then sprite:set_animation("stopped") end --***
  end
  -- Start enemy.
  self:restart()
end

-- This function is called by the engine if the enemy is restarted (for instance, when starting or when hurt).
function enemy:on_restarted()
  sprite:set_direction(math.random(4)-1) -- Random direction.
  -- Start random actions. Start checking hero.
  sol.timer.start(self, 2000, function() self:random(); return true end)
  self:check_hero() 
end

function enemy:check_hero()
  local hero = self:get_map():get_hero()
  if self:get_distance(hero) < 48 then
    sol.timer.stop_all(self)
	self:teleport_behind()
	return
  end
  sol.timer.start(self, 100, function() self:check_hero() end)
end

-- Random action: look around (or teleport to other position - NOT YET IMPLEMENTED).
function enemy:random()
  if math.random() < 0.5 then sprite:set_animation("looking") end --***
end

-- Teleport next to hero. (Called before attacking with tongue.)
function enemy:teleport_behind()
  -- Make enemy invisible and invincible.
  self:set_can_attack(false); self:set_invincible_sprite(sprite)
  sprite:fade_out(20)
  sol.audio.play_sound("teleporter")
  sol.timer.start(enemy, 2000, function()
    local x,y,z = self:get_position()
    local hero = self:get_map():get_hero()
    local hx,hy,hz = hero:get_position()
    local dir = math.random(0,3) -- Random direction to appear next to hero.
    -- Teleport behind hero only if there are not obstacles. Otherwise stay in same position.
    local dx,dy = hx-x+24*math.cos(dir*math.pi/2), hy-y-24*math.sin(dir*math.pi/2)
    if not self:test_obstacles(dx, dy) then self:set_position(x+dx,y+dy,hz) end
	-- Stop making enemy invisible and invincible.
	local dir = enemy:get_direction4_to(hero); sprite:set_direction(dir)
	sprite:fade_in(5)
	sol.timer.start(enemy, 500, function()
    enemy:set_can_attack(true); enemy:set_default_attack_consequences_sprite(sprite)
	  enemy:attack()
	end)
  end)
end

-- Attack with tongue towards hero.
function enemy:attack()
  -- Create tongue.
  tongue = self:create_sprite("animals/chameleon")
  local dir = sprite:get_direction()
  local dx,dy = -4,-13
  if dir%2 == 0 then dx = 24*math.cos(dir*math.pi/2) -4 
  else dy = -24*math.sin(dir*math.pi/2) -13 end
  tongue:set_xy(dx,dy); tongue:set_direction(dir)
  -- Attack animation and sound.
  sprite:set_animation("attack")
  tongue:set_animation("tongue")
  sol.audio.play_sound("tongue")
  -- Destroy tongue after attack and restart enemy.
  function sprite:on_animation_finished()
    if tongue then 
	  enemy:remove_sprite(tongue) 
	  tongue = nil  
	end
    enemy:restart()
  end
end

-- Remove tongue if necessary before dying (to avoid the "hurt" animation for the sprite tongue).
function enemy:on_dying()
  if tongue then enemy:remove_sprite(tongue) end
end
