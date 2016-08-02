-- Lua script of enemy fire.
local enemy = ...
local life_duration = 2000 -- The enemy disappears after this delay.

-- Event called when the enemy is initialized.
function enemy:on_created()

  -- Initialize the properties of your enemy here,
  -- like the sprite, the life and the damage.
  local sprite = enemy:create_sprite("enemies/" .. enemy:get_breed())
  self:set_life(1)
  self:set_damage(1)
  self:set_invincible()
  sol.audio.play_sound("lamp")
end

-- Event called when the enemy is restarted.
function enemy:on_restarted()
  sol.timer.start(self, life_duration, function()
    self:remove() -- Remove the flame after a delay.
  end)
end