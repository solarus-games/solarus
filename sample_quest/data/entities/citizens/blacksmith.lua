-- Blacksmith script (custom entity).
local entity = ...

local state = "stopped" -- Possible values: "stopped", "working".

function entity:on_created()
  self:set_size(32, 16)
  self:set_origin(16, 13)
  self:set_traversable_by(false)
  self:set_drawn_in_y_order(true)
  self:start_forging()
end

-- INITIALIZE YOUR CUSTOM DIALOGS HERE. REDEFINE THIS FUNCTION IF NECESSARY.
function entity:start_dialog()
  local map = self:get_map()
  if map.dialog_with_blacksmith then map:dialog_with_blacksmith() end
end

function entity:on_interaction()
  -- Do nothing while mixing.
  if state == "working" then return end
  -- Otherwise turn towards hero.
  local h = self:get_map():get_hero()
  local dir = self:get_direction4_to(h)
  self:set_direction(dir)
  self:start_dialog() -- Start dialog. 
end

-- Function to simulate jumps.
local function jump(properties, callback)
  local p = properties
  local e, max_height, jump_duration, dx, dy = p.entity_jumping, p.max_height, p.jump_duration, p.dx, p.dy
  local function f(t) -- Shifting function.
    return math.floor(4 * max_height * (t / jump_duration - (t / jump_duration) ^ 2))
  end
  -- Start jump
  local t = 0
  local refreshing_time = 10
  local sprite = e:get_sprite()
  sol.timer.start(e, refreshing_time, function() -- Update shift each 10 milliseconds.
    sprite:set_xy(0, -f(t))
    t = t + refreshing_time
    if t > jump_duration then
      if callback then callback() end 
      return false 
    else return true end
  end)
  -- Create movement.
  if dx ~= 0 or dy ~= 0 then
    local m = sol.movement.create("straight")
    local angle = sol.main.get_angle(0, 0, dx, dy)
    local jumping_speed = math.floor( 1000 * sol.main.get_distance(0, 0, dx, dy) / jump_duration )
    m:set_speed(jumping_speed)
    m:set_angle(angle)
    m:start(e)
  end
end

function entity:start_forging()
  -- Start forging metal.
  state = "working"
  local remaining_hits = 3
  self:set_direction(2)
  local metal = self:get_map():get_entity("metal")
  local tx, ty = self:get_position()
  local prop_low_jump = {entity_jumping = metal, max_height = 8, 
    jump_duration = 250, dx = 0, dy = 0}
  local prop_high_jump = {entity_jumping = metal, max_height = 24, 
    jump_duration = 1500, dx = 16, dy = -8}
  local sprite_blacksmith = self:get_sprite()
  sol.timer.start(self, 2000, function() -- Make the metal jump.
    remaining_hits = remaining_hits - 1  
    sprite_blacksmith:set_animation("forge", function()
      sprite_blacksmith:set_animation("forge_end")
      sol.audio.play_sound("bomb")
      if remaining_hits > 0 then
        jump(prop_low_jump)
      else
        entity:get_map():get_entity("steam"):set_enabled(false) -- Hide steam.
        local x_metal, y_metal, layer_metal = metal:get_position()
        metal:set_position(x_metal, y_metal, layer_metal + 1) -- Change layer to allow traversing blacksmith.
        jump(prop_high_jump, function() -- Callback: hit metal with hand.
          metal:stop_movement()
          sprite_blacksmith:set_animation("hand_lifted")
          prop_high_jump.dx = - prop_high_jump.dx -- Invert dx-direction.
          prop_high_jump.dy = - prop_high_jump.dy -- Invert dy-direction.
          sol.timer.start(self, 700, function() -- Metal jumps back.
            jump(prop_high_jump, function()
              metal:stop_movement()
              metal:set_position(x_metal, y_metal, layer_metal) -- Restore metal layer and initial position.
              prop_high_jump.dx = - prop_high_jump.dx -- Restore dx-direction.
              prop_high_jump.dy = - prop_high_jump.dy -- Restore dy-direction.
              entity:get_map():get_entity("steam"):set_enabled(true) -- Show steam.
            end)
            sprite_blacksmith:set_direction(0)
            sprite_blacksmith:set_animation("hurt_hammer") -- Start hurt with hammer animation.
            sol.audio.play_sound("enemy_hurt")
            sol.timer.start(entity, 3000, function() 
              -- Callback: return back to normal.
              sprite_blacksmith:set_animation("stopped")
              sprite_blacksmith:set_direction(3)
              state = "stopped"
              sol.timer.start(entity, 5000, function()
                entity:start_forging()
              end)
            end)
          end)
        end)
      end
    end)
    if remaining_hits > 0 then return true end -- Continue forging.     
  end) 
end
