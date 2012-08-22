local item = ...

-- Let the hero run with the action key or an inventory item key
item:get_game():set_ability("run", item:get_variant())

function item:on_variant_changed(variant)

  self:get_game():set_ability("run", variant)
end

function item:on_using()

  self:get_map():hero_start_running()
  self:set_finished()
end

