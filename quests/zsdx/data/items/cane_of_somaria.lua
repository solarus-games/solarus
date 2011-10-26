function event_use()

  local magic_needed = 7
  if sol.game.get_magic() >= magic_needed then
    sol.main.play_sound("bomb")
    sol.game.remove_magic(magic_needed)

    -- TODO: remove previous somaria blocks
    local x, y, layer = get_block_position_from_hero()
    sol.map.block_create(x, y, layer, "somaria_block",
    {
      sprite_name = "entities/somaria_block",
      can_be_pushed = true,
      can_be_pulled = true,
      maximum_moves = 2 -- infinite moves
    })
  else
    sol.main.play_sound("wrong")
  end
  sol.item.set_finished()
end

function get_block_position_from_hero()

  -- TODO snap to the grid

  local x, y, layer = sol.map.hero_get_position()
  local direction = sol.map.hero_get_direction()
  if direction == 0 then
    x = x + 16
  elseif direction == 1 then
    y = y - 16
  elseif direction == 2 then
    x = x - 16
  elseif direction == 3 then
    y = y + 16
  end

  return x, y, layer
end

function event_obtained(variant, savegame_variable)
  
  -- give the magic bar if necessary
  if sol.game.get_max_magic() == 0 then
    sol.game.set_max_magic(42)
  end
end

