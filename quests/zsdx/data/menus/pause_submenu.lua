-- Base class of each submenu.

local submenu = {}

function submenu:new(game)
  local o = { game = game }
  setmetatable(o, self)
  self.__index = self
  return o
end

return submenu

