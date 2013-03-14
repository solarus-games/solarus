local submenu = require("menus/pause_submenu")
local map_submenu = submenu:new()

local outside_world_size = { width = 2080, height = 3584 }
local outside_world_minimap_size = { width = 255, height = 388 }

function map_submenu:on_started()

  submenu.on_started(self)

  local dungeon = self.game:get_dungeon()
  if dungeon == nil then
    -- Not in a dungeon: show the world map.
    self:set_caption("map.caption.world_map")

    local hero_absolute_x, hero_absolute_y = self.game:get_map():get_location()
    if self.game:is_in_outside_world() then
      local hero_map_x, hero_map_y = self.game:get_map():get_entity("hero"):get_position()
      hero_absolute_x = hero_absolute_x + hero_map_x
      hero_absolute_y = hero_absolute_y + hero_map_y
    end

    local hero_minimap_x = hero_absolute_x * outside_world_minimap_size.width / outside_world_size.width
    local hero_minimap_y = hero_absolute_y * outside_world_minimap_size.height / outside_world_size.height
    self.hero_x = hero_minimap_x + 40
    self.hero_y = hero_minimap_y + 53

    if self.game:has_item("world_map") then
      self.world_minimap_img = sol.surface.create("menus/outside_world_map.png")
      self.world_minimap_visible_y = math.min(outside_world_minimap_size.height - 133, math.max(hero_minimap_y - 66))
    else
      self.world_minimap_img = sol.surface.create("menus/outside_world_clouds.png")
      self.world_minimap_visible_y = 0
    end
    self:rebuild_world_minimap_visible_surface()

  else
    -- In a dungeon.
    -- TODO
  end

  self.hero_head_sprite = sol.sprite.create("menus/hero_head")
  self.hero_head_sprite:set_animation("tunic" .. self.game:get_item("tunic"):get_variant())
  self.up_arrow_sprite = sol.sprite.create("menus/arrow")
  self.up_arrow_sprite:set_direction(1)
  self.down_arrow_sprite = sol.sprite.create("menus/arrow")
  self.down_arrow_sprite:set_direction(3)
end

function map_submenu:on_command_pressed(command)

  local handled = submenu.on_command_pressed(self, command)

  if handled then
    return handled
  end

  if command == "left" then
    self:previous_submenu()
    handled = true

  elseif command == "right" then
    self:next_submenu()
    handled = true

  elseif command == "up" or command == "down" then

    if not self.game:is_in_dungeon() then
      -- Move the outside world minimap.
      if self.game:has_item("world_map") then

        local dy = 0
        function repeat_move_world_minimap()
          local up = self.game:is_command_pressed("up")
          local down = self.game:is_command_pressed("down")
          if dy == -1 then
            if not up then
              if down then
                dy = 1
              else
                dy = 0
              end
            elseif self.world_minimap_visible_y <= 0 then
              dy = 0
            end
          elseif dy == 1 then
            if not down then
              if up then
                dy = -1
              else
                dy = 0
              end
            elseif self.world_minimap_visible_y > outside_world_minimap_size.height - 133 then
              dy = 0
            end
          end

          if dy ~= 0 then
            self.world_minimap_visible_y = self.world_minimap_visible_y + dy
            self:rebuild_world_minimap_visible_surface()
            sol.timer.start(10, repeat_move_world_minimap)
          end
        end  -- function

        if command == "up" then
          repeat_move_world_minimap(-1)
        else
          repeat_move_world_minimap(1)
        end

      end
    else
      -- We are in a dungeon: select another floor.
      -- TODO
    end
    handled = true
  end
  return handled
end

function map_submenu:rebuild_world_minimap_visible_surface()

  self.world_minimap_visible_surface = sol.surface.create(self.world_minimap_img, 0, self.world_minimap_visible_y, 255, 133)
end

function map_submenu:on_draw(dst_surface)

  self:draw_background(dst_surface)
  self:draw_caption(dst_surface)
  self:draw_save_dialog_if_any(dst_surface)

  if not self.game:is_in_dungeon() then
    self:draw_world_minimap(dst_surface)
  else
    -- TODO
  end
end

function map_submenu:draw_world_minimap(dst_surface)

  -- Draw the minimap.
  self.world_minimap_visible_surface:draw(dst_surface, 48, 59)

  if self.game:has_item("world_map") then
    -- Draw the hero's position.
    local hero_visible_y = self.hero_y - self.world_minimap_visible_y
    if hero_visible_y >= 51 and hero_visible_y <= 133 + 51 then
      self.hero_head_sprite:draw(dst_surface, self.hero_x, hero_visible_y)
    end

    -- Draw the arrows.
    if self.world_minimap_visible_y > 0 then
      self.up_arrow_sprite:draw(dst_surface, 96, 55)
      self.up_arrow_sprite:draw(dst_surface, 211, 55)
    end

    if self.world_minimap_visible_y < outside_world_minimap_size.height - 133 then
      self.down_arrow_sprite:draw(dst_surface, 96, 188)
      self.down_arrow_sprite:draw(dst_surface, 211, 188)
    end
  end
end

return map_submenu

