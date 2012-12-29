local item = ...

function item:on_created()
  self:set_savegame_variable("i1100")
  self:set_assignable(true)
end

function item:on_using()
  sol.audio.play_sound("jump")
  local hero = self:get_map():get_entity("hero")
  local direction4 = hero:get_direction()
  hero:start_jumping(direction4 * 2, 32, false)
  self:set_finished()
end

