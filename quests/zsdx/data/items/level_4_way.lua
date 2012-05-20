function event_variant_changed(variant)

  if variant == 4 then
    -- Tiger scrolls
    sol.map.get_game():set_ability("sword_knowledge", 1)
  end
end

