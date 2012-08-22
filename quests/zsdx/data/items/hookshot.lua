local item = ...

function item:on_using()
  self:get_map():get_hero():start_hookshot()
  self:set_finished()
end

