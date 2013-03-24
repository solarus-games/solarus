local item = ...

function item:on_obtaining(variant, savegame_variable)

  -- Save the possession of the compass in the current dungeon.
  local game = self:get_game()
  local dungeon = game:get_dungeon_index()
  if dungeon == nil then
    error("This map is not in a dungeon")
  end
  game:set_value("dungeon_" .. dungeon .. "_compass", true)
end

