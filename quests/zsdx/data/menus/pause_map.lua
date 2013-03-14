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
      self.world_minimap_visible_y = math.min(outside_world_minimap_size.height - 133, math.max(self.hero_minimap_y - 66))
    else
      self.world_minimap_img = sol.surface.create("menus/outside_world_clouds.png")
      self.world_minimap_visible_y = 0
    end

    self.world_minimap_increment_y = 0
  else
    -- In a dungeon.
    -- TODO
  end

  self.hero_head_sprite = sol.sprite.create("menu/hero_head")
  self.hero_head_sprite:set_animation("tunic" .. self.game:get_item("tunic"):get_variant())
  self.up_arrow_sprite = sol.sprite.create("menu/arrow")
  self.up_arrow_sprite:set_direction(1)
  self.down_arrow_sprite = sol.sprite.create("menu/arrow")
  self.down_arrow_sprite:set_direction(3)
end

function map_submenu:on_command_pressed(command)

  local handled = submenu.on_command_pressed(self, command)

  if not handled then
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
        end
      else
        -- TODO
      end
    end
  end

  return handled
end

function map_submenu:on_draw(dst_surface)

  self:draw_background(dst_surface)
  self:draw_caption(dst_surface)
  self:draw_save_dialog_if_any(dst_surface)
end

return map_submenu

