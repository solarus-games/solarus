local item = ...

function item:on_using()
  self:get_map():get_entity("hero"):start_hookshot()
  self:set_finished()
end

