created = false -- indicates a Somaria block has been created on the current map

function event_use()

  local magic_needed = 7
  if sol.game.get_magic() >= magic_needed then

    local x, y, layer = get_block_position_from_hero()

    if not created then

      sol.main.play_sound("cane")
      sol.main.play_sound("magic_bar")
      sol.game.remove_magic(magic_needed)

      -- create the Somaria block
      sol.map.block_create(x, y, layer, "somaria_block",
      {
	sprite_name = "entities/somaria_block",
	can_be_pushed = true,
	can_be_pulled = true,
	maximum_moves = 2 -- infinite moves
      })
      created = true
    else
      -- reuse the old one
      local old_x, old_y, old_layer = sol.map.block_get_position("somaria_block")
      if x ~= old_x or y ~= old_y or layer ~= old_layer then

	sol.main.play_sound("cane")
	sol.main.play_sound("magic_bar")
	sol.game.remove_magic(magic_needed)

	sol.map.block_set_position("somaria_block", x, y, layer)
      end
    end
  else

    if created and sol.map.block_get_position("somaria_block") ~= -100 then
      -- remove the previous block
      sol.main.play_sound("cane")
      sol.map.block_set_position("somaria_block", -100, 0)
    else
      sol.main.play_sound("wrong")
    end
  end
  sol.item.set_finished()
end

-- Called when the current map changes
function event_map_changed()

  -- no more Somaria block on the new map
  created = false
end


function get_block_position_from_hero()

  -- compute a position
  local x, y, layer = sol.map.hero_get_position()
  local direction = sol.map.hero_get_direction()
  if direction == 0 then
    x = x + 20
  elseif direction == 1 then
    y = y - 20
  elseif direction == 2 then
    x = x - 20
  elseif direction == 3 then
    y = y + 20
  end

  -- snap the center of the block to the 8*8 grid
  x = (x + 4) - (x + 4) % 8
  y = (y - 1) - (y - 1) % 8 + 5

  return x, y, layer
end

function event_obtained(variant, savegame_variable)
  
  -- give the magic bar if necessary
  if sol.game.get_max_magic() == 0 then
    sol.game.set_max_magic(42)
  end
end

