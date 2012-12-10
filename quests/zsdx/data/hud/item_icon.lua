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
  self.surface = sol.surface.create(72, 24)
  self.surface:set_transparency_color{0, 0, 0}
  self.opacity = 255
  self.background_img = sol.surface.create("hud/item_icon_" .. slot .. ".png")
  self.item_sprite = sol.sprite.create("entities/items")
  self.item_displayed = nil
  self.item_variant_displayed = nil
  self.amount_text = sol.text_surface.create()
  self.max_amount_displayed = nil

  self:check()
  self:rebuild_surface()
end

function item_icon:check()

  local need_rebuild = false

  local item = self.game:get_item_assigned(self.slot)
  if self.item_displayed ~= item then
    need_rebuild = true
    self.item_displaued = item
    self.item_variant_displayed = nil
    if item ~= nil then
      self.item_sprite:set_animation(item:get_name())
    end
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

  -- No item icon during a dialog.
  if not self.game:is_showing_dialog() then

    -- Background image.
    self.background_img:draw(self.surface, 12, 17)

    if self.item_displayed ~= nil then
      -- Item.
      self.item_sprite:draw(self.surface, 12, 17)
      if self.amount_text:get_text() ~= "" then
        -- Amount.
        self.amount_text:draw(self.surface, 8, 16)
      end
    end
  end
end

function item_icon:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function item_icon:on_draw(dst_surface)

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

return item_icon

