local submenu = require("menus/pause_submenu")
local map_submenu = submenu:new()

local outside_world_size = { width = 2080, height = 3584 }
local outside_world_minimap_size = { width = 225, height = 388 }

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

    local hero_minimap_x = math.floor(hero_absolute_x * outside_world_minimap_size.width / outside_world_size.width)
    local hero_minimap_y = math.floor(hero_absolute_y * outside_world_minimap_size.height / outside_world_size.height)
    self.hero_x = hero_minimap_x + 40
    self.hero_y = hero_minimap_y + 53

    self.world_minimap_movement = nil
    self.world_minimap_visible_xy = {x = 0, y = 0}
    if self.game:has_item("world_map") then
      self.world_minimap_img = sol.surface.create("menus/outside_world_map.png")
      self.world_minimap_visible_xy.y = math.min(outside_world_minimap_size.height - 133, math.max(0, hero_minimap_y - 65))
    else
      self.world_minimap_img = sol.surface.create("menus/outside_world_clouds.png")
      self.world_minimap_visible_xy.y = 0
    end
    self.world_minimap_visible_surface = sol.surface.create(self.world_minimap_img,
        self.world_minimap_visible_xy.x, self.world_minimap_visible_xy.y, 255, 133)

  else
    -- In a dungeon.
    local dungeon_index = self.game:get_dungeon_index()

    -- Caption text.
    self.set_caption("map.caption.dungeon_name_" .. dungeon_index)

    -- Item icons.
    self.dungeon_map_background_img = sol.surface.create("menus/dungeon_map_background.png")
    self.dungeon_map_icons_img = sol.surface.create("menus/dungeon_map_icons.png")
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

        if (command == "up" and self.world_minimap_visible_xy.y > 0) or
            (command == "down" and self.world_minimap_visible_xy.y < outside_world_minimap_size.height - 134) then

            local angle
            if command == "up" then
              angle = math.pi / 2
            else
              angle = 3 * math.pi / 2
            end

          if self.world_minimap_movement ~= nil then
            self.world_minimap_movement:stop()
          end

          local movement = sol.movement.create("straight")
          movement:set_speed(96)
          movement:set_angle(angle)
          local submenu = self

          function movement:on_position_changed()
            local x, y = self:get_xy()
            if (command == "up" and submenu.world_minimap_visible_xy.y > 0) or
                (command == "down" and submenu.world_minimap_visible_xy.y < outside_world_minimap_size.height - 134) then
              submenu.world_minimap_visible_surface = sol.surface.create(submenu.world_minimap_img,
                  submenu.world_minimap_visible_xy.x, submenu.world_minimap_visible_xy.y, 255, 133)

              if not submenu.game:is_command_pressed("up")
                  and not submenu.game:is_command_pressed("down") then
                self:stop()
                submenu.world_minimap_movement = nil
              end
            else
              self:stop()
              submenu.world_minimap_movement = nil
            end
          end

          function movement:on_finished()
            self:stop()
            submenu.world_minimap_movement = nil
          end

          movement:start(self.world_minimap_visible_xy)
          self.world_minimap_movement = movement
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

function map_submenu:on_draw(dst_surface)

  self:draw_background(dst_surface)
  self:draw_caption(dst_surface)

  if not self.game:is_in_dungeon() then
    self:draw_world_minimap(dst_surface)
  else
    -- TODO
  end

  self:draw_save_dialog_if_any(dst_surface)
end

function map_submenu:draw_world_minimap(dst_surface)

  -- Draw the minimap.
  self.world_minimap_visible_surface:draw(dst_surface, 48, 59)

  if self.game:has_item("world_map") then
    -- Draw the hero's position.
    local hero_visible_y = self.hero_y - self.world_minimap_visible_xy.y
    if hero_visible_y >= 51 and hero_visible_y <= 133 + 51 then
      self.hero_head_sprite:draw(dst_surface, self.hero_x, hero_visible_y)
    end

    -- Draw the arrows.
    if self.world_minimap_visible_xy.y > 0 then
      self.up_arrow_sprite:draw(dst_surface, 96, 55)
      self.up_arrow_sprite:draw(dst_surface, 211, 55)
    end

    if self.world_minimap_visible_xy.y < outside_world_minimap_size.height - 134 then
      self.down_arrow_sprite:draw(dst_surface, 96, 188)
      self.down_arrow_sprite:draw(dst_surface, 211, 188)
    end
  end
end

return map_submenu

