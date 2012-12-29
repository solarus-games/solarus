local submenu = require("menus/pause_submenu")
local quest_status_submenu = submenu:new()

function quest_status_submenu:on_started()

  submenu.on_started(self)
  self.quest_items_surface = sol.surface.create(320, 240)
  self.cursor_sprite = sol.sprite.create("menus/pause_cursor")
  self.cursor_sprite_x = 0
  self.cursor_sprite_y = 0
  self.cursor_position = nil
  self.caption_text_keys = {}

  local item_sprite = sol.sprite.create("entities/items")

  -- Draw the items on a surface.
  self.quest_items_surface:set_transparency_color{0, 0, 0}
  self.quest_items_surface:fill_color{0, 0, 0}

  -- Tunic.
  local tunic = self.game:get_item("tunic"):get_variant()
  item_sprite:set_animation("tunic")
  item_sprite:set_direction(tunic - 1)
  item_sprite:draw(self.quest_items_surface, 185, 177)
  self.caption_text_keys[5] = "quest_status.caption.tunic_" .. tunic

  -- Sword.
  local sword = self.game:get_item("sword"):get_variant()
  if sword > 0 then
    item_sprite:set_animation("sword")
    item_sprite:set_direction(sword - 1)
    item_sprite:draw(self.quest_items_surface, 219, 177)
    self.caption_text_keys[6] = "quest_status.caption.sword_" .. sword
  end

  -- Shield.
  local shield = self.game:get_item("shield"):get_variant()
  if shield > 0 then
    item_sprite:set_animation("shield")
    item_sprite:set_direction(shield - 1)
    item_sprite:draw(self.quest_items_surface, 253, 177)
    self.caption_text_keys[7] = "quest_status.caption.shield_" .. shield
  end

  -- Wallet.
  local rupee_bag = self.game:get_item("rupee_bag"):get_variant()
  if rupee_bag > 0 then
    item_sprite:set_animation("rupee_bag")
    item_sprite:set_direction(rupee_bag - 1)
    item_sprite:draw(self.quest_items_surface, 68, 84)
    self.caption_text_keys[0] = "quest_status.caption.rupee_bag_" .. rupee_bag
  end

  -- Bomb bag.
  local bomb_bag = self.game:get_item("bomb_bag"):get_variant()
  if bomb_bag > 0 then
    item_sprite:set_animation("bomb_bag")
    item_sprite:set_direction(bomb_bag - 1)
    item_sprite:draw(self.quest_items_surface, 68, 113)
    self.caption_text_keys[1] = "quest_status.caption.bomb_bag_" .. bomb_bag
  end

  -- Quiver.
  local quiver = self.game:get_item("quiver"):get_variant()
  if quiver > 0 then
    item_sprite:set_animation("quiver")
    item_sprite:set_direction(quiver - 1)
    item_sprite:draw(self.quest_items_surface, 68, 143)
    self.caption_text_keys[2] = "quest_status.caption.quiver_" .. quiver
  end

  -- World map.
  local world_map = self.game:get_item("world_map"):get_variant()
  if world_map > 0 then
    item_sprite:set_animation("world_map")
    item_sprite:set_direction(world_map - 1)
    item_sprite:draw(self.quest_items_surface, 68, 177)
    self.caption_text_keys[3] = "quest_status.caption.world_map"
  end

  -- Pieces of heart.
  local pieces_of_heart_img = sol.surface.create("menus/quest_status_pieces_of_heart.png")
  local x = 51 * (self.game:get_value("i1030") or 0)
  local src_surface = sol.surface.create(pieces_of_heart_img, x, 0, 51, 50)
  src_surface:draw(self.quest_items_surface, 101, 82)
  self.caption_text_keys[4] = "quest_status.caption.pieces_of_heart"

  -- Dungeons finished
  local dungeons_img = sol.surface.create("menus/quest_status_dungeons.png")
  local dst_positions = {
    { 209,  69 },
    { 232,  74 },
    { 243,  97 },
    { 232, 120 },
    { 209, 127 },
    { 186, 120 },
    { 175,  97 },
    { 186,  74 },
  }
  for i, dst_position in ipairs(dst_positions) do
    if self.game:is_dungeon_finished(i) then
      local src_surface = sol.surface.create(dungeons_img,
          20 * (i - 1), 0, 20, 20)
      src_surface:draw(self.quest_items_surface, dst_position[1], dst_position[2])
    end
  end

  -- Cursor.
  self:set_cursor_position(0)
end

function quest_status_submenu:set_cursor_position(position)

  if position ~= self.cursor_position then
    self.cursor_position = position
    if position <= 3 then
      self.cursor_sprite_x = 68
    elseif position == 4 then
      self.cursor_sprite_x = 126
      self.cursor_sprite_y = 107
    else
      self.cursor_sprite_x = 15 + 34 * position
    end

    if position == 0 then
      self.cursor_sprite_y = 79
    elseif position == 1 then
      self.cursor_sprite_y = 108
    elseif position == 2 then
      self.cursor_sprite_y = 138
    elseif position == 4 then
      self.cursor_sprite_y = 107
    else
      self.cursor_sprite_y = 172
    end

    self:set_caption(self.caption_text_keys[position])
  end
end

function quest_status_submenu:on_command_pressed(command)

  local handled = submenu.on_command_pressed(self, command)

  if not handled then

    if command == "left" then
      if self.cursor_position <= 3 then
        self:previous_submenu()
      else
        sol.audio.play_sound("cursor")
        if self.cursor_position == 4 then
          self:set_cursor_position(0)
        elseif self.cursor_position == 5 then
          self:set_cursor_position(3)
        else
          self:set_cursor_position(self.cursor_position - 1)
        end
      end
      handled = true

    elseif command == "right" then
      if self.cursor_position == 4 or self.cursor_position == 7 then
        self:next_submenu()
      else
        sol.audio.play_sound("cursor")
        if self.cursor_position <= 2 then
          self:set_cursor_position(4)
        elseif self.cursor_position == 3 then
          self:set_cursor_position(5)
        else
          self:set_cursor_position(self.cursor_position + 1)
        end
      end
      handled = true

    elseif command == "down" then
      sol.audio.play_sound("cursor")
      self:set_cursor_position((self.cursor_position + 1) % 8)
      handled = true

    elseif command == "up" then
      sol.audio.play_sound("cursor")
      self:set_cursor_position((self.cursor_position + 7) % 8)
      handled = true
    end

  end

  return handled
end

function quest_status_submenu:on_draw(dst_surface)

  local width, height = dst_surface:get_size()
  local x = width / 2 - 160
  local y = height / 2 - 120
  self:draw_background(dst_surface)
  self.quest_items_surface:draw(dst_surface, x, y)
  self.cursor_sprite:draw(dst_surface, x + self.cursor_sprite_x, y + self.cursor_sprite_y)
  self:draw_save_dialog_if_any(dst_surface)
end

return quest_status_submenu

