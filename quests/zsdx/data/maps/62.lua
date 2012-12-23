local map = ...
-- Dungeon 4 icy room

function map:on_opening_transition_finished(destination)

  if not map:get_game():get_item("tunic"):has_variant(2) then

    map:start_dialog("dungeon_4.too_cold", function()
      hero:walk("2222", false, true)
    end)
  end
end

