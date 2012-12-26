local submenu = require("menus/pause_submenu")
local inventory_submenu = submenu:new()

local item_names = {
  "feather",
  "bombs_counter",
  "bow",
  "boomerang",
  "mystic_mirror",
  "pegasus_shoes",
  "bottle_1",

  "lamp",
  "hookshot",
  "cane_of_somaria",
  "apples_counter",
  "pains_au_chocolat_counter",
  "croissants_counter",
  "bottle_2",

  "rock_key",
  "bone_key",
  "clay_key",
  "fire_stones_counter",
  "flippers",
  "magic_cape",
  "bottle_3",

  "iron_key",
  "stone_key",
  "wooden_key",
  "ice_key",
  "glove",
  "level_4_way",
  "bottle_4"
}

function inventory_submenu:on_started()

  submenu.on_started(self)

  self.cursor_sprite = sol.sprite.create("menus/pause_cursor")
  self.sprites = {}
  self.counters = {}
  self.captions = {}

  for k = 1, 28 do
    -- Get the item, its possession state and amount.
    local item = self.game:get_item(item_names[k])
    local variant = item:get_variant()

    if variant > 0 and item:has_amount() then
      -- Show a counter in this case.
      local amount = item:get_amount()
      local maximum = item:get_max_amount()
      local x = 60 + ((k - 1) % 7) * 32
      local y = 81 + math.floor((k - 1) / 7) * 32

      self.counters[k] = sol.text_surface.create{
        horizontal_alignment = "center",
        vertical_alignment = "top"
      }
    end

    -- Initialize the sprite and the caption string.
    self.sprites[k] = sol.sprite.create("entities/items")
    self.sprites[k]:set_animation(item_names[k])
    self.sprites[k]:set_direction(variant - 1)
    self.captions[k] = "inventory.caption.item." .. item_names[k] .. "." .. variant
  end

  -- Initialize the cursor
  local index = self.game:get_value("pause_inventory_last_item_index")
  local row = math.floor(index / 7)
  local column = index % 7
  self:set_cursor_position(row, column)
end

function inventory_submenu:on_finished()

  if submenu.on_finished then
    submenu.on_finished(self)
  end

  if self:is_assigning_item() then
    self:finish_assigning_item()
  end

  self.game.hud.item_icon_1.surface:set_opacity(255)
  self.game.hud.item_icon_2.surface:set_opacity(255)
end

function inventory_submenu:set_cursor_position(row, column)

  self.cursor_row = row
  self.cursor_column = column

  local index = row * 7 + column
  self.game:set_value("pause_inventory_last_item_index", index)

  -- Update the caption text and the action icon.
  local item = self.game:get_item(item_names[index + 1])

  self:set_caption(self.captions[index + 1])
  local item_icon_opacity = 128
  if item:get_variant() > 0 then
    self.game:set_custom_command_effect("action", "info")
    if item:is_assignable() then
      item_icon_opacity = 255
    end
  else
    self.game:set_custom_command_effect("action", nil)
  end
  self.game.hud.item_icon_1.surface:set_opacity(item_icon_opacity)
  self.game.hud.item_icon_2.surface:set_opacity(item_icon_opacity)
end

function inventory_submenu:get_selected_index()

  return self.cursor_row * 7 + self.cursor_column
end

function inventory_submenu:is_item_selected()

  local item_name = item_names[self:get_selected_index() + 1]
  return self.game:get_item(item_name):get_variant() > 0
end

function inventory_submenu:on_command_pressed(command)
  
  local handled = submenu.on_command_pressed(self, command)

  if not handled then

    if command == "action" then
      if self.game:get_custom_command_effect("action") == "info" then
        self:show_info_message()
        handled = true
      end

    elseif command == "item_1" then
      if self:is_item_selected() then
        self:assign_item(1)
        handled = true
      end

    elseif command == "item_2" then
      if self:is_item_selected() then
        self:assign_item(2)
        handled = true
      end

    elseif command == "left" then
      if self.cursor_column == 0 then
        self:previous_submenu()
      else
        sol.audio.play_sound("cursor")
        self:set_cursor_position(self.cursor_row, self.cursor_column - 1)
      end
      handled = true

    elseif command == "right" then
      if self.cursor_column == 6 then
        self.next_submenu()
      else
        sol.audio.play_sound("cursor")
        self:set_cursor_position(self.cursor_row, self.cursor_column + 1)
      end
      self:next_submenu()
      handled = true

    elseif command == "up" then
      sol.audio.play_sound("cursor")
      self:set_cursor_position((self.cursor_row + 3) % 4, self.cursor_column)
      handled = true

    elseif command == "down" then
      sol.audio.play_sound("cursor")
      self:set_cursor_position((self.cursor_row + 1) % 4, self.cursor_column)
      handled = true

    end
  end

  return handled
end

function inventory_submenu:on_draw(dst_surface)

  self:draw_background(dst_surface)
  self:draw_save_dialog_if_any(dst_surface)
end

return inventory_submenu

