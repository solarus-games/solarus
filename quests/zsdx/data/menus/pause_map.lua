local submenu = require("menus/pause_submenu")
local map_submenu = submenu:new()

local outside_world_size = { width = 2080, height = 3584 }
local outside_world_minimap_size = { width = 225, height = 388 }

function map_submenu:on_started()

  submenu.on_started(self)

  self.dungeon = self.game:get_dungeon()
  if self.dungeon == nil then
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

  else
    -- In a dungeon.
    self.dungeon_index = self.game:get_dungeon_index()

    -- Caption text.
    self:set_caption("map.caption.dungeon_name_" .. self.dungeon_index)

    -- Item icons.
    self.dungeon_map_background_img = sol.surface.create("menus/dungeon_map_background.png")
    self.dungeon_map_icons_img = sol.surface.create("menus/dungeon_map_icons.png")
    self.small_keys_text = sol.text_surface.create{
      font = "white_digits",
      horizontal_alignment = "right",
      vertical_alignment = "top",
      text = self.game:get_num_small_keys()
    }

    -- Floors.
    self.dungeon_floors_img = sol.surface.create("floors.png", true)
    self.hero_floor = self.game:get_map():get_floor()
    self.nb_floors = self.dungeon.highest_floor - self.dungeon.lowest_floor + 1
    self.nb_floors_displayed = math.min(7, self.nb_floors)
    if self.hero_floor == nil then
      -- The hero is not on a known floor of the dungeon.
      self.highest_floor_displayed = self.dungeon.highest_floor
      self.selected_floor = self.dungeon.lowest_floor
    else
      -- The hero is on a known floor.
      self.selected_floor = self.hero_floor
      if self.nb_floors <= 7 then
        self.highest_floor_displayed = self.dungeon.highest_floor
      elseif self.floor >= self.dungeon.highest_floor - 2 then
        self.highest_floor_displayed = self.dungeon.highest_floor
      elseif self.floor <= self.dungeon.lowest_floor + 2 then
        self.highest_floor_displayed = self.dungeon.lowest_floor + 6
      else
        self.highest_floor_displayed = self.hero_floor + 3
      end
    end

    -- Minimap.
    self.dungeon_map_img = sol.surface.create(123, 119)
    self.dungeon_map_img:set_transparency_color{0, 0, 0}
    self:load_dungeon_map_image()
  end

  -- Common to dungeons and outside dungeons.
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
            if not submenu.game:is_command_pressed("up")
                and not submenu.game:is_command_pressed("down") then
              self:stop()
              submenu.world_minimap_movement = nil
            end

            if (command == "up" and submenu.world_minimap_visible_xy.y <= 0) or
                (command == "down" and submenu.world_minimap_visible_xy.y >= outside_world_minimap_size.height - 134) then
              self:stop()
              submenu.world_minimap_movement = nil
            end
          end

          movement:start(self.world_minimap_visible_xy)
          self.world_minimap_movement = movement
        end
      end
    else
      -- We are in a dungeon: select another floor.
      local new_selected_floor
      if command == "up" then
        new_selected_floor = self.selected_floor + 1
      else
        new_selected_floor = self.selected_floor - 1
      end
      if new_selected_floor >= self.dungeon.lowest_floor
          and new_selected_floor <= self.dungeon.highest_floor then
        -- The new floor is valid.
        sol.audio.play_sound("cursor")
        self.hero_head_sprite:set_frame(0)
        self.selected_floor = new_selected_floor
        self:load_dungeon_map_image()
        if self.selected_floor <= self.highest_floor_displayed - 7 then
          self.highest_floor_displayed = self.highest_floor_displayed - 1
        elseif self.selected_floor > self.highest_floor_displayed then
          self.highest_floor_displayed = self.highest_floor_displayed + 1
        end
      end
    end
    handled = true
  end
  return handled
end

function map_submenu:on_draw(dst_surface)

  self:draw_background(dst_surface)
  self:draw_caption(dst_surface)

  if not self.game:is_in_dungeon() then
    self:draw_world_map(dst_surface)
  else
    self:draw_dungeon_map(dst_surface)
  end

  self:draw_save_dialog_if_any(dst_surface)
end

function map_submenu:draw_world_map(dst_surface)

  -- Draw the minimap.
  self.world_minimap_img:draw_region(
      self.world_minimap_visible_xy.x, self.world_minimap_visible_xy.y, 255, 133,
      dst_surface, 48, 59)

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

function map_submenu:draw_dungeon_map(dst_surface)

  -- Background.
  self.dungeon_map_background_img:draw(dst_surface, 48, 59)

  -- Items.
  self:draw_dungeon_items(dst_surface)

  -- Floors.
  self:draw_dungeon_floors(dst_surface)

  -- The map itself.
  if self.hero_point_sprite ~= nil
      and self.selected_floor == self.hero_floor then
    self.hero_point_sprite:draw(self.dungeon_map_img, self.hero_x, self.hero_y)
  end
  self.dungeon_map_img:draw(dst_surface, 143, 66)
end

function map_submenu:draw_dungeon_items(dst_surface)

  -- Map.
  if self.game:has_dungeon_map() then
    self.dungeon_map_icons_img:draw_region(0, 0, 17, 17, dst_surface, 50, 168)
  end

  -- Compass.
  if self.game:has_dungeon_compass() then
    self.dungeon_map_icons_img:draw_region(17, 0, 17, 17, dst_surface, 69, 168)
  end

  -- Big key.
  if self.game:has_dungeon_big_key() then
    self.dungeon_map_icons_img:draw_region(34, 0, 17, 17, dst_surface, 88, 168)
  end

  -- Boss key.
  if self.game:has_dungeon_boss_key() then
    self.dungeon_map_icons_img:draw_region(51, 0, 17, 17, dst_surface, 107, 168)
  end

  -- Small keys.
  self.dungeon_map_icons_img:draw_region(68, 0, 9, 17, dst_surface, 126, 168)
  self.small_keys_text:draw(dst_surface, 140, 180)
end

function map_submenu:draw_dungeon_floors(dst_surface)

  -- Draw some floors.
  local src_x = 96
  local src_y = (15 - self.highest_floor_displayed) * 12
  local src_width = 32
  local src_height = self.nb_floors_displayed * 12 + 1
  local dst_x = 79
  local dst_y = 70 + (8 - self.nb_floors_displayed) * 6
  local old_dst_y = dst_y

  self.dungeon_floors_img:draw_region(src_x, src_y, src_width, src_height,
      dst_surface, dst_x, dst_y)

  -- Draw the current floor with other colors.
  src_x = 64
  src_y = (15 - self.selected_floor) * 12
  src_height = 13
  dst_y = old_dst_y + (self.highest_floor_displayed - self.selected_floor) * 12
  self.dungeon_floors_img:draw_region(src_x, src_y, src_width, src_height,
      dst_surface, dst_x, dst_y)
 
  -- Draw the hero's icon if any.
  local lowest_floor_displayed = self.highest_floor_displayed - self.nb_floors_displayed + 1
  if self.hero_floor ~= nil
      and self.hero_floor >= lowest_floor_displayed
      and self.hero_floor <= self.highest_floor_displayed then
    dst_x = 61
    dst_y = old_dst_y + (self.highest_floor_displayed - self.hero_floor) * 12
    self.hero_head_sprite:draw(dst_surface, dst_x, dst_y)
  end

  -- Draw the boss icon if any.
  if self.game:has_dungeon_compass()
      and self.boss_floor ~= nil
      and self.boss_floor >= lowest_floor_displayed
      and self.boss_floor <= highest_floor_displayed then

    dst_y = old_dst_y + (self.highest_floor_displayed - self.boss_floor) * 12 + 3
    self.dungeon_map_icons_img:draw_region(78, 0, 8, 8, dst_surface, 113, dst_y)
  end

  -- Draw the arrows.
  if lowest_floor_displayed > self.dungeon.lowest_floor then
    down_arrow_sprite:draw(dst_surface, 89, 89)
  end

  if self.highest_floor_displayed < self.dungeon.highest_floor then
    down_arrow_sprite:draw(dst_surface, 89, 56)
  end
end

-- Converts x,y relative to the real floor into coordinates relative
-- to the dungeon minimap.
function map_submenu:to_dungeon_minimap_coordinates(x, y)

  local minimap_x = 0
  local minimap_y = 0
  local minimap_width = 123
  local minimap_height = 119
  if (self.dungeon.floor_width * 119) / (self.dungeon.floor_height * 123) > 1 then
    -- The floor height does not use the entire vertical space.
    minimap_height = self.dungeon.floor_height * 123 / self.dungeon.floor_width
    minimap_y = (119 - minimap_height) / 2
  else
    -- The floor width does not use the entire horizontal space.
    minimap_width = self.dungeon.floor_width * 119 / self.dungeon.floor_height
    minimap_x = (123 - minimap_width) / 2
  end

  x = minimap_x + x * minimap_width / self.dungeon.floor_width
  y = minimap_y + y * minimap_height / self.dungeon.floor_height
  return x, y
end

-- Rebuilds the minimap of the current floor of the dungeon.
function map_submenu:load_dungeon_map_image()

  self.dungeon_map_img:fill_color{0, 0, 0}
  if self.game:has_dungeon_map() then
    -- Load the image of this floor.
    local floor_map_img = sol.surface.create(
        "menus/dungeon_maps/map" .. self.dungeon_index ..
        "_" .. self.selected_floor .. ".png")
    floor_map_img:draw(self.dungeon_map_img)
  end

  if self.game:has_dungeon_compass() then
    -- Hero.
    self.hero_point_sprite = sol.sprite.create("menus/hero_point")

    local hero_absolute_x, hero_absolute_y = self.game:get_map():get_location()
    local hero_map_x, hero_map_y = self.game:get_map():get_entity("hero"):get_position()
    hero_absolute_x = hero_absolute_x + hero_map_x
    hero_absolute_y = hero_absolute_y + hero_map_y

    self.hero_x, self.hero_y = self:to_dungeon_minimap_coordinates(
        hero_absolute_x, hero_absolute_y)

    -- Boss.
    local boss = self.dungeon.boss
    if boss ~= nil
        and boss.floor == self.selected_floor
        and boss.savegame_variable ~= nil
        and not self.game:get_value(boss.savegame_variable) then
      local dst_x, dst_y = self:to_dungeon_minimap_coordinates(boss.x, boss.y) 
      self.dungeon_map_icons_img:draw_region(78, 0, 8, 8,
          self.dungeon_map_img, dst_x, dst_y)
    end

    -- Chests.
    if self.dungeon.chests == nil then
      -- Lazily load the chest information.
      self:load_chests()
    end
    for _, chest in ipairs(self.dungeon.chests) do

      if chest.floor == self.selected_floor
          and chest.savegame_variable ~= nil
          and not self.game:get_value(chest.savegame_variable) then
        local dx, dy

        -- TODO change chests origin point to avoid this.
        if chest.big then
          -- Big chest.
          dx, dy = 16, 21
        else
          -- Small chest.
          dx, dy = 8, 13
        end
        local dst_x, dst_y = self:to_dungeon_minimap_coordinates(
            chest.x + dx, chest.y + dy)
        dst_y = dst_y - 1
        if chest.big then
          dst_x = dst_x - 1
          self.dungeon_map_icons_img:draw_region(78, 12, 6, 4,
          self.dungeon_map_img, dst_x, dst_y)
        else
          dst_x = dst_x - 2
          self.dungeon_map_icons_img:draw_region(78, 8, 4, 4,
          self.dungeon_map_img, dst_x, dst_y)
        end
      end
    end
  end
end

-- Parses all map data files of the current dungeon in order to determine the
-- position of its chests.
function map_submenu:load_chests()

  local dungeon = self.dungeon
  dungeon.chests = {}

  -- Here is the magic: set up a special environment to load map data files.
  local environment = {

    properties = function(map_properties)
      -- Remember the floor to be used for subsequent chests.
      dungeon.chests.current_floor = map_properties.floor
    end,

    chest = function(chest_properties) 
      -- Get the info about this chest and store it into the dungeon table.
      if dungeon.chests.current_floor ~= nil then
        dungeon.chests[#dungeon.chests + 1] = {
          floor = dungeon.chests.current_floor,
          x = chest_properties.x,
          y = chest_properties.y,
          big = chest_properties.is_big_chest,
          savegame_variable = chest_properties.treasure_savegame_variable,
        }
      end
    end,
  }

  -- Make any other function a no-op (tile(), enemy(), block(), etc.).
  setmetatable(environment, {
    __index = function()
      return function() end
    end
  })

  for _, map_id in ipairs(self.dungeon.maps) do

    -- Load the map data file as Lua.
    local chunk = sol.main.load_file("maps/" .. map_id .. ".dat")

    -- Apply our special environment (with functions properties() and chest()).
    setfenv(chunk, environment)

    -- Run it.
    chunk()
  end

  -- Cleanup temporary value.
  dungeon.chests.current_floor = nil
end

return map_submenu

