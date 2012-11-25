local item = ...

function item:on_created()

  self:set_savegame_variable("i1112")
  self:set_assignable(true)
  self:set_amount_savegame_variable("i1027")
  self:set_max_amount(10)
end

function item:on_using()

  if self:get_amount() == 0 then
    sol.audio.play_sound("wrong")
  else
    self:remove_amount(1)
    self:get_game():add_life(4 * 7)
  end
  self:set_finished()
end

