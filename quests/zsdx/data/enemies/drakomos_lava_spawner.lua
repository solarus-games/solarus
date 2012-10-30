local enemy = ...

local nb_sons_created = 0

function enemy:on_created()

  self:set_size(32, 24)
  self:set_origin(16, 21)

  if self:test_obstacles(0, 0) then
    -- Don't appear on stones previously created.
    self:remove()
  end

  self:set_life(1)
  self:set_damage(1)
  self:create_sprite("enemies/drakomos_lava_spawner")
  self:set_invincible()
end

function enemy:on_restarted()

  self:set_can_attack(false)
  sol.timer.start(self, 1000, function()
    local sprite = self:get_sprite()
    sprite:set_animation("disappearing")
    sol.audio.play_sound("ice")

    local hero = self:get_map():get_entity("hero")
    if math.random(2) == 1 or self:get_distance(hero) < 24 then
      nb_sons_created = nb_sons_created + 1
      local son_name = self:get_name() .. "_son_" .. nb_sons_created
      local son = self:create_enemy(son_name, "red_helmasaur", 0, 0)
      if self:get_game():get_life() <= self:get_game():get_max_life() / 3 then
	son:set_treasure("heart", 1, -1)
      end
    else
      local x, y, layer = self:get_position()
      self:get_map():create_destructible{
	subtype = "black_stone",
	x = x,
	y = y,
	layer = layer
      }
    end
  end)
end

function enemy:on_sprite_animation_finished(sprite, animation)

  if animation == "disappearing" then
    self:set_enabled(false)
  end
end

