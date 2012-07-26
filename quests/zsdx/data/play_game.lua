local game = ...

local hearts_class = require("hud/hearts")

function game:on_started()

  -- Initialize everything
  self.hud = {}
  self.hud.hearts = hearts_class:new(self)
end

function game:on_finished()
end

function game:on_post_display(surface)
  -- TODO only if the hud is enabled
  local width, height = surface:get_size()
  self.hud.hearts:display(surface, width - 104, 6)
end

-- Run the game.
game:start()

