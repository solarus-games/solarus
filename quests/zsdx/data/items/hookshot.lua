local item = ...

function item:on_use()
  self:get_map():hero_start_hookshot()
  self:set_finished()
end

