local enemy = ...

function enemy:on_appear()

  self:set_size(32, 24)
  self:set_origin(16, 21)

  if self:test_obstacles(0, 0) then
    -- don't appear on stones previously created
    self:get_map():enemy_remove(self:get_name())
  end

  self:set_life(1)
  self:set_damage(1)
  self:create_sprite("enemies/drakomos_lava_spawner")
  self:set_invincible()
end

function enemy:on_restart()

  self:set_can_attack(false)
  sol.timer.start(1000, function()
    local sprite = self:get_sprite()
    sprite:set_animation("disappearing")
    sol.audio.play_sound("ice")

    if math.random(2) == 1 or self:get_distance_to_hero() < 24 then
      local son_name = self:get_name() .. "_son"
      self:create_son(son_name, "red_helmasaur", 0, 0)
      if self:get_game():get_life() <= self:get_game():get_max_life() / 3 then
        self:get_map():enemy_set_treasure(son_name, "heart", 1, -1)
      end
    else
      local x, y, layer = self:get_position()
      self:get_map():destructible_create("black_stone", x, y, layer)
    end
  end)
end

function enemy:on_sprite_animation_finished(sprite, animation)

  if animation == "disappearing" then
    self:get_map():enemy_set_enabled(self:get_name(), false)
  end
end

