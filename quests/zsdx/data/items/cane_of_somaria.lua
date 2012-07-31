local item = ...
item.created = false -- indicates a Somaria block has been created on the current map

function item:on_use()

  local magic_needed = 7
  if self:get_game():get_magic() >= magic_needed then

    local x, y, layer = self:get_block_position_from_hero()

    if not self.created then

      sol.audio.play_sound("cane")
      sol.audio.play_sound("magic_bar")
      self:get_game():remove_magic(magic_needed)

      -- create the Somaria block
      self:get_map():block_create(x, y, layer, "somaria_block",
      {
	sprite_name = "entities/somaria_block",
	can_be_pushed = true,
	can_be_pulled = true,
	maximum_moves = 2 -- infinite moves
      })
      self.created = true
    else
      -- reuse the old one
      local old_x, old_y, old_layer = self:get_map():block_get_position("somaria_block")
      if x ~= old_x or y ~= old_y or layer ~= old_layer then

	sol.audio.play_sound("cane")
	sol.audio.play_sound("magic_bar")
	self:get_game():remove_magic(magic_needed)

	self:get_map():block_set_position("somaria_block", x, y, layer)
      end
    end
  else

    if self.created and self:get_map():block_get_position("somaria_block") ~= -100 then
      -- remove the previous block
      sol.audio.play_sound("cane")
      self:get_map():block_set_position("somaria_block", -100, 0)
    else
      sol.audio.play_sound("wrong")
    end
  end
  self:set_finished()
end

-- Called when the current map changes
function item:on_map_changed()

  -- no more Somaria block on the new map
  self.created = false
end

function item:get_block_position_from_hero()

  -- compute a position
  local x, y, layer = self:get_map():hero_get_position()
  local direction = self:get_map():hero_get_direction()
  if direction == 0 then
    x = x + 21
  elseif direction == 1 then
    y = y - 21
  elseif direction == 2 then
    x = x - 21
  elseif direction == 3 then
    y = y + 21
  end

  -- snap the center of the block to the 8*8 grid
  x = (x + 4) - (x + 4) % 8
  y = (y - 1) - (y - 1) % 8 + 5

  return x, y, layer
end

function item:on_obtained(variant, savegame_variable)
  
  -- give the magic bar if necessary
  if self:get_game():get_max_magic() == 0 then
    self:get_game():set_max_magic(42)
  end
end

