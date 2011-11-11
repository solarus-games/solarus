-- Dungeon 7 3F

function event_map_started(destination_point_name)

  -- block fallen into the hole
  if sol.game.savegame_get_boolean(623) then
    sol.map.block_set_enabled("nw_block", false)
  end
end

function event_block_moved(block_name)

  if block_name == "nw_block" then
    local x, y = sol.map.block_get_position(block_name)
    if x == 544 and y == 69 then
      -- make the block fall
      sol.map.block_set_enabled(block_name, false)
      sol.map.tile_set_enabled("hole_a", true)
      sol.map.teletransporter_set_enabled("hole_a_teletransporter", true)
      sol.game.savegame_set_boolean(623, true)
      sol.main.play_sound("jump")
      sol.main.timer_start(function() sol.main.play_sound("bomb") end, 500)
    end
  end
end

function event_update()

  if not sol.game.savegame_get_boolean(623) then
    -- blocks cannot overlap holes or teletransporters with the current engine,
    -- so we disable the hole A and its teletransporter when necessary

    local disable_hole = false
    local block_x, block_y = sol.map.block_get_position("nw_block")
    if block_y == 69 and block_x >= 520 and block_x < 544 then
      -- the block is just before the hole
      local hero_x = sol.map.hero_get_position()
      disable_hole = hero_x < block_x
    end

    if disable_hole and sol.map.tile_is_enabled("hole_a") then
      sol.map.tile_set_enabled("hole_a", false)
      sol.map.teletransporter_set_enabled("hole_a_teletransporter", false)
    elseif not disable_hole and not sol.map.tile_is_enabled("hole_a") then
      sol.map.tile_set_enabled("hole_a", true)
      sol.map.teletransporter_set_enabled("hole_a_teletransporter", true)
    end
  end
end

