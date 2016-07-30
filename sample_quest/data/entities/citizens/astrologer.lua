-- Astrologer script (custom entity).
local entity = ...
local map = entity:get_map()

local state = "waiting" -- Possible values: "waiting", "spell".

function entity:on_created()
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_traversable_by(false)
  self:set_drawn_in_y_order(true)
end

function entity:on_interaction()
  -- Do nothing while busy.
  if state == "spell" then return end
  -- Otherwise turn towards hero.
  local h = self:get_map():get_hero()
  local dir = self:get_direction4_to(h)
  self:set_direction(dir)
  self:start_dialog() -- Start dialog.
end

-- INITIALIZE YOUR CUSTOM DIALOGS HERE. REDEFINE THIS FUNCTION IF NECESSARY.
function entity:start_dialog()
  --local map = self:get_map()
  --if map.dialog_with_astrologer then map:dialog_with_astrologer() end
  self:start_spell()
end

-- Function to create magic cards that describe an elliptic trajectory.
function entity:create_card()
  local x, y, layer = self:get_position()
  local refreshing_time = 10 -- Refreshing time for positions of cards, given in milliseconds.
  local a0 = 7 * math.pi / 4 -- Initial angle, in radians.
  local prop_card = {x = x, y = y, layer = layer + 1, 
    direction = 0, width = 16, height = 16, sprite = "things/magic_deck", name = "card"}
  -- Initialize variables.
  local card = self:get_map():create_custom_entity(prop_card)
  local a = a0 -- Angle.
  card.cx = prop_card.x; card.cy = prop_card.y -- Center position for the orbits.
  local num_f = card:get_sprite():get_num_frames()
  local angular_speed = (1/2) * math.pi / 1000 -- Given in radians per millisecond.
  local axis_x, axis_y = 0, 0 -- Initial lenghts of axes of ellipse.
  local axes_angle = 0 -- Angle for the axes.
  -- Initialize getter/setter functions.
  function card:add_angle(ang) a = a + ang end
  function card:get_angle() return a end
  function card:set_angular_speed(speed) angular_speed = speed end
  function card:get_angular_speed() return angular_speed end
  function card:set_axes(axis_x2, axis_y2) axis_x, axis_y = axis_x2, axis_y2 end
  function card:get_axes() return axis_x, axis_y end
  function card:set_center_position(cx, cy) card.cx, card.cy = cx, cy end
  function card:get_center_position() return card.cx, card.cy end
  function card:set_axes_angle(angle) axes_angle = angle end
  function card:get_axes_angle() return axes_angle end
  -- Start updating frame and position of the card.
  function card:update_frame()
    local interval_angle_length = 2 * math.pi / num_f
    local f = math.floor( (a + (1/2) * interval_angle_length) / interval_angle_length )
    f = (f + 3) % 12 
    card:get_sprite():set_frame(f)
  end
  function card:update_position()
    -- Compute angle and coordinates. Rotate the coordinates if the axes are rotated.
    a = (a + refreshing_time * angular_speed) % (2 * math.pi)
    local tx, ty = axis_x * math.cos(a), axis_y * math.sin(a)
    local ttx = card.cx + math.cos(axes_angle) * tx + math.sin(axes_angle) * ty
    local tty = card.cy - math.sin(axes_angle) * tx + math.cos(axes_angle) * ty
    card:set_position(ttx, tty) -- Update position.
  end
  function card:start_updating()
    sol.timer.start(card, refreshing_time, function()
      card:update_position()
      card:update_frame()
      return true
    end)
  end
  card:start_updating()
  return card
end

-- Functions to modify the trajectory of each card.
function entity:start_moving_card_1(card)
  local axis_max_x, axis_max_y = 96, 32 -- 128, 64 -- Max axes for the ellipse. 
  local t = 0 -- Initial time, in milliseconds.
  local dt = 100 -- Time between changes of trajectory.
  sol.timer.start(card, dt, function()
    if t < 10 * 1000 then -- In 10 seconds we start the last function.
      local ax, ay = card:get_axes()
      ax = math.min(ax + 2, axis_max_x)
      ay = math.min(ay + 1, axis_max_y)
      card:set_axes(ax, ay)
    else
      sol.timer.start(self, 1000, function() entity:start_moving_card_3(card) end)
      return false
    end
    t = t + dt
    return true
  end)
end
function entity:start_moving_card_2()
  for card in map:get_entities("card") do
    card:set_angular_speed(math.pi / 1000) -- Accelerate!
    -- Start rotation angle for the axes.
    local axes_angular_speed = 2 * math.pi / 10000 -- Angular speed for the axes.
    local dtt = 20 -- Time to refresh changes of trajectory.
    sol.timer.start(card, dtt, function()
      local axes_angle = card:get_axes_angle()
      axes_angle = (axes_angle + axes_angular_speed * dtt) % (2 * math.pi)
      card:set_axes_angle(axes_angle)   
      return true
    end)
    sol.timer.start(card, 1000, function()
      local dt = 50 -- Time between changes of trajectory.
      local new_ax, new_ay = 32, 16
      sol.timer.start(card, dt, function()
        local ax, ay = card:get_axes()  
        ax = math.max(ax - 2, new_ax)
        ay = math.max(ay - 1, new_ay)
        card:set_axes(ax, ay)
        if ax >= new_ax or ay >= new_ay then return true 
        else card:set_angular_speed(3 * math.pi / 1000) -- Accelerate!
        end
      end)
    end)
  end
  sol.timer.start(entity, 7000, function()
    self:finish_spell()
  end)
end
function entity:start_moving_card_3(card)
  local ax, ay = card:get_axes()
  local dt = 100 -- Time between changes of trajectory.
  sol.timer.start(card, dt, function()
    ax = math.max(ax - 2, 0)
    ay = math.max(ay - 1, 0)
    if ax <= 5 or ay <= 5 then
      card:remove()
      return false
    else
      card:set_axes(ax, ay)
      return true
    end
  end)
end


-- Start spell.
function entity:start_spell()
  state = "spell"
  local sprite = self:get_sprite()
  sprite:set_direction(0); sprite:set_animation("spell")
  map:get_entity("magic_ball"):get_sprite():set_animation("light")
  local num_cards = 32 -- Number of cards.
  local time_between_cards = 125 -- Time between cards.
  -- Throw cards.
  sol.timer.start(self, time_between_cards, function()
    local card = self:create_card()
    self:start_moving_card_1(card) -- Start movement.
    num_cards = num_cards - 1
    if num_cards > 0 then return true end
  end)
  -- After a while, start second movement on all cards at the same time.
  sol.timer.start(self, 10000, function()
    self:start_moving_card_2()
  end)
end

-- Finish spell.
function entity:finish_spell()
  map:get_entity("magic_ball"):get_sprite():set_animation("stopped")
  self:get_sprite():set_animation("stopped")
  state = "waiting"
  -- START CUSTOM DIALOG HERE!
end
