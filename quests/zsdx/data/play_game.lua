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

-- Run the game.
game:start()

