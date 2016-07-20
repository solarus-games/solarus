-- Witch script (custom entity).
local entity = ...

local state = "waiting" -- Possible values: "waiting", "mixing".

function entity:on_created()
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_traversable_by(false)
  self:set_drawn_in_y_order(true)
end

function entity:on_interaction()
  -- Do nothing while mixing.
  if state == "mixing" then return end
  -- Otherwise turn towards hero.
  local h = self:get_map():get_hero()
  local dir = self:get_direction4_to(h)
  self:set_direction(dir)
  -- CALL HERE YOUR CUSTOM DIALOGS!
end

function entity:use_broom(broom)
  state = "mixing"
  local broom_sprite = broom:get_sprite()
  local witch_sprite = self:get_sprite()
  -- Turn witch and start animation to take broom.
  self:set_direction(0)
  witch_sprite:set_animation("witch_mixing")
  witch_sprite:set_direction(1)
  sol.timer.start(500, function() 
    -- Start mixing animations.
    local x, y, layer = self:get_position()
    broom:set_position(x - 12, y - 8, layer)
    broom_sprite:set_direction(1)
    witch_sprite:set_animation("witch_mixing")
    witch_sprite:set_direction(0)
    sol.timer.start(5000, function() 
      self:release_broom(broom)
    end)
  end)
end

function entity:release_broom(broom)
  local broom_sprite = broom:get_sprite()
  local witch_sprite = self:get_sprite()
  -- Turn witch and start animation to release broom.
  self:set_direction(0)
  witch_sprite:set_animation("witch_mixing")
  witch_sprite:set_direction(1)
  local x, y, layer = self:get_position()
  broom:set_position(x + 16, y - 8, layer)
  broom_sprite:set_direction(0)
  broom:start_moving()
  sol.timer.start(500, function()
    -- Start waiting animation.
    self:set_direction(0)
    witch_sprite:set_animation("stopped")
    witch_sprite:set_direction(2)
    state = "waiting"
  end)
end
