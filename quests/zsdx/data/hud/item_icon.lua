-- An icon that shows the inventory item assigned to a slot.

local item_icon = {}

function item_icon:new(game, slot)

  local object = {}
  setmetatable(object, self)
  self.__index = self

  object:initialize(game, slot)

  return object
end

function item_icon:initialize(game, slot)

  self.game = game
  self.slot = slot
  self.surface = sol.surface.create(32, 28)
  self.surface:set_transparency_color{0, 0, 0}
  self.background_img = sol.surface.create("hud/item_icon_" .. slot .. ".png")
  self.item_sprite = sol.sprite.create("entities/items")
  self.item_displayed = nil
  self.item_variant_displayed = 0
  self.amount_text = sol.text_surface.create{
    horizontal_alignment = "center",
    vertical_alignment = "top"
  }
  self.amount_displayed = nil
  self.max_amount_displayed = nil

  self:check()
  self:rebuild_surface()
end

function item_icon:check()

  local need_rebuild = false

  -- Item assigned.
  local item = self.game:get_item_assigned(self.slot)
  if self.item_displayed ~= item then
    need_rebuild = true
    self.item_displayed = item
    self.item_variant_displayed = nil
    if item ~= nil then
      self.item_sprite:set_animation(item:get_name())
    end
  end

  if item ~= nil then
    -- Variant of the item.
    local item_variant = item:get_variant()
    if self.item_variant_displayed ~= item_variant then
      need_rebuild = true
      self.item_variant_displayed = item_variant
      self.item_sprite:set_direction(item_variant - 1)
    end

    -- Amount.
    if item:has_amount() then
      local amount = item:get_amount()
      local max_amount = item:get_max_amount()
      if self.amount_displayed ~= amount
          or self.max_amount_displayed ~= max_amount then
        need_rebuild = true
        self.amount_displayed = amount
        self.max_amount_displayed = max_amount
      end
    elseif self.amount_displayed ~= nil then
      need_rebuild = true
      self.amount_displayed = nil
      self.max_amount_displayed = nil
    end
  elseif self.amount_displayed ~= nil then
    need_rebuild = true
    self.amount_displayed = nil
    self.max_amount_displayed = nil
  end

  -- Redraw the surface only if something has changed.
  if need_rebuild then
    self:rebuild_surface()
  end

  -- Schedule the next check.
  sol.timer.start(self.game, 50, function()
    self:check()
  end)
end

function item_icon:rebuild_surface()

  self.surface:fill_color{0, 0, 0}

  -- Background image.
  self.background_img:draw(self.surface)

  if self.item_displayed ~= nil then
    -- Item.
    self.item_sprite:draw(self.surface, 12, 17)
    if self.amount_displayed ~= nil then
      -- Amount.
      self.amount_text:set_text(tostring(self.amount_displayed))
      if self.amount_displayed == self.max_amount_displayed then
        self.amount_text:set_font("green_digits")
      else
        self.amount_text:set_font("white_digits")
      end
      self.amount_text:draw(self.surface, 18, 16)
    end
  end
end

function item_icon:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function item_icon:on_draw(dst_surface)

  local map = self.game:get_map()
  if map == nil or not map:is_dialog_enabled() then
    local x, y = self.dst_x, self.dst_y
    local width, height = dst_surface:get_size()
    if x < 0 then
      x = width + x
    end
    if y < 0 then
      y = height + y
    end

    self.surface:draw(dst_surface, x, y)
  end
end

return item_icon

