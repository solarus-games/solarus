local game = ...

local hearts_class = require("hud/hearts")

function game:on_started()

  -- Initialize everything
  self.hud = {}
  self.hud.hearts = hearts_class:new(self)
  self:set_hud_enabled(true)
end

function game:on_finished()
end

function game:on_post_draw(surface)
  if game:is_hud_enabled() then
    local width, height = surface:get_size()
    self.hud.hearts:draw(surface, width - 104, 6)
  end
end

function game:is_hud_enabled()
  return self.hud_enabled
end

function game:set_hud_enabled(hud_enabled)
  game.hud_enabled = hud_enabled
end

-- Returns the item name of a bottle with the specified content,
-- or nil if there is none.
function game:get_first_bottle_with(variant)

  for i = 1, 4 do
    local item = self:get_item("bottle_" .. i)
    if item:get_variant() == variant then
      return item
    end
  end

  return nil
end

function game:get_first_empty_bottle()
  return self:get_first_bottle_with(1)
end

function game:has_bottle()

  for i = 1, 4 do
    local item = self:get_item("bottle_" .. i)
    if item:has_variant() then
      return item
    end
  end

  return nil
end

-- Run the game.
game:start()

