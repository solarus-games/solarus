local item = ...

function item:on_created()

  self:set_savegame_variable("i1034")
end

function item:on_variant_changed(variant)

  -- Obtaining a rupee bag changes the max money.
  local max_moneys = {100, 300, 999}
  local max_money = max_moneys[variant]
  if max_money == nil then
    error("Invalid variant '" .. variant .. "' for item 'rupee_bag'")
  end

  self:get_game():set_max_money(max_money)
end

