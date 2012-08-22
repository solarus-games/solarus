local item = ...

function item:on_using()
  self:get_map():hero_start_hookshot()
  self:set_finished()
end

