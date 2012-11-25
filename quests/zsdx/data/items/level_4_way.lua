local item = ...

function item:on_created()

  self:set_savegame_variable("i1117")
end

function item:on_variant_changed(variant)

  if variant == 4 then
    -- Tiger scrolls
    self:get_game():set_ability("sword_knowledge", 1)
  end
end

