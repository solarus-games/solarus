local map = ...
-- Beaumont cave: Inferno maze

function map:on_started(destination)

  -- Enable small keys on this map.
  map.small_keys_savegame_variable = "i1200"

  map:set_light(0)
end


