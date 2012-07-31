local item = ...

function item:on_use()
  sol.audio.play_sound("jump")
  local direction4 = self:get_map():hero_get_direction()
  self:get_map():hero_start_jumping(direction4 * 2, 32, false)
  self:set_finished()
end

