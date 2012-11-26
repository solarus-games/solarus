local game = ...

-- Returns the current dungeon if any, or nil.
function game:get_dungeon()

  local world = self:get_map():get_world()
  local index = world:match("^dungeon_([0-9]+)$")
  if index == nil then
    return nil
  end

  return self.dungeons[index]
end

function game:is_dungeon_finished(dungeon)
  return self:get_value("dungeon_" .. dungeon .. "_finished")
end

function game:set_dungeon_finished(dungeon, finished)
  if finished == nil then
    finished = true
  end
  self:set_value("dungeon_" .. dungeon .. "_finished", finished)
end


