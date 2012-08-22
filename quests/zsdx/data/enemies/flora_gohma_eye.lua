local enemy = ...

local petals = {
  {}, {}, {}, {}, {}
}
local remaining_petals = 5
local eye_sprite = nil
local arms_sprite = nil
local initial_xy = {}
local nb_sons_created = 0
local speed = 24
local timers = {}

function enemy:on_created()

  self:set_life(12)
  self:set_damage(4)
  eye_sprite = self:create_sprite("enemies/flora_gohma_eye")
  arms_sprite = self:create_sprite("enemies/flora_gohma_eye")
  self:set_size(104, 64)
  self:set_origin(52, 64)
  self:set_hurt_style("boss")
  self:set_no_treasure()
  self:set_push_hero_on_sword(true)
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence_sprite(eye_sprite, "hookshot", "protected")

  -- create the petals
  for i = 1, 5 do
    petals[i].sprite = self:create_sprite("enemies/flora_gohma_eye")
    petals[i].life = 3
    self:set_attack_consequence_sprite(petals[i].sprite, "hookshot", "custom")
  end

  initial_xy.x, initial_xy.y = self:get_position()

  -- go to the high layer
  self:set_layer_independent_collisions(true)
  self:set_position(initial_xy.x, initial_xy.y, 2)
end

function enemy:on_restarted()

  eye_sprite:set_animation("eye")
  for i = 1, 5 do
    if petals[i].sprite ~= nil then
      petals[i].sprite:set_animation("petal_"..i)
    end
  end

  local m = sol.movement.create("target")
  m:set_speed(speed)
  m:set_target(initial_xy.x, initial_xy.y)
  self:start_movement(m)
  for _, t in ipairs(timers) do t:stop() end

  self:repeat_create_son()
end

function enemy:on_update()

  local x, y = self:get_position()
  local hero_x, hero_y = self:get_map():hero_get_position()
  if hero_y < y - 60 and
      (petals[2].sprite ~= nil and hero_x <= x and hero_x > x - 32
      or petals[5].sprite ~= nil and hero_x >= x and hero_x < x + 32
      or petals[1].sprite ~= nil) then
    -- the top petals are too hard to reach: let the hookshot traverse
    -- the main sprite
    self:set_attack_consequence_sprite(eye_sprite, "hookshot", "ignored")
  else
    self:set_attack_consequence_sprite(eye_sprite, "hookshot", "protected")
  end
end

function enemy:on_custom_attack_received(attack, sprite)

  -- a petal was touched by the hookshot
  for i = 1, 5 do
    if petals[i].sprite == sprite
        and not string.find(petals[i].sprite:get_animation(), "hurt") then
      petals[i].life = petals[i].life - 1
      self:hurt(0)
      sol.audio.play_sound("enemy_hurt")
      petals[i].sprite:set_animation("petal_hurt_"..i)
      timers[#timers + 1] = sol.timer.start(300, function()

	if petals[i].life > 0 then
	  -- restore the petal animation
	  petals[i].sprite:set_animation("petal_"..i)
	else
	  -- destroy the petal
	  sol.audio.play_sound("stone")
	  self:remove_sprite(petals[i].sprite)
	  petals[i].sprite = nil

	  remaining_petals = remaining_petals - 1
	  if remaining_petals <= 0 then
	    -- no more petals: make the eye vulnerable
	    speed = 48
	    self:set_attack_consequence_sprite(eye_sprite, "sword", 1)
	    self:restart()
	  end
	end

      end)
    end
  end
end

function enemy:on_movement_finished(movement)

  local m = sol.movement.create("random")
  m:set_speed(speed)
  m:set_max_distance(24)
  self:start_movement(m)
end

function enemy:on_hurt(attack, life_lost)

  if self:get_life() <= 0 then
    -- notify the body to make it stop moving
    self:send_message(self:get_father(), "dying")
    for _, t in ipairs(timers) do t:stop() end

    -- remove the sons
    for i = 1, nb_sons_created do
      local son_prefix = self:get_name().."_son_"
      local son_name = son_prefix..i
      if not self:get_map():enemy_is_dead(son_name) then
	self:get_map():enemy_remove(son_name)
      end
    end
  end
end

function enemy:on_dead()

  -- notify the body
  self:send_message(self:get_father(), "dead")
end

function enemy:repeat_create_son()

  local son_prefix = self:get_name().."_son_"
  if self:get_map():enemy_get_group_count(son_prefix) < 10 then
    nb_sons_created = nb_sons_created + 1
    local son_name = son_prefix..nb_sons_created
    local _, _, layer = self:get_map():enemy_get_position(self:get_father())
    self:create_son(son_name, "snap_dragon", 0, 0, layer)
    if math.random(2) == 1 then
      self:get_map():enemy_set_treasure(son_name, "heart", 1, -1)
    end
  end

  timers[#timers + 1] = sol.timer.start(1000 + math.random(1000), function()
    self:repeat_create_son()
  end)
end

