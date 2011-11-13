-- The mystic mirror allows the hero to traverse a specific waterfall
-- in the mountains

i = 1 -- index of the waterfall tiles

function event_npc_interaction(npc_name)

  -- give a hint when looking at the waterfall
  if npc_name:find("^riverfall_detector_[we]$") then
    sol.map.dialog_start("outside_world.mountain_riverfall")
  end
end

function event_npc_interaction_item(npc_name, item_name, variant)

  local side = npc_name:match("^riverfall_detector_([we])$")
  if side ~= nil and item_name == "mystic_mirror" then
    -- using the mirror with the waterfall: reverse the waterfall
    -- and traverse it
    sol.map.hero_freeze()
    sol.main.play_sound("water_drain")
    sol.main.timer_stop_all()
    sol.map.tile_set_group_enabled("riverfall_", false)

    local path
    if side == "w" then
      path = "00000000"
    else
      path = "44444444"
    end
    i = 1

    function repeat_change_riverfall()
      sol.map.tile_set_enabled("riverfall_" .. i, true)
      i = i + 1
      if i <= 8 then
        sol.main.timer_start(repeat_change_riverfall, 350)
      else
        sol.main.play_sound("secret")
        sol.map.hero_walk(path, false, true)
        sol.main.timer_start(repeat_restore_riverfall, 2000)
      end
    end

    function repeat_restore_riverfall()
      i = i - 1
      sol.map.tile_set_enabled("riverfall_" .. i, false)
      if i > 1 then
        sol.main.timer_start(repeat_restore_riverfall, 350)
      end
    end

    sol.main.timer_start(repeat_change_riverfall, 350)

    -- tell the engine that an interaction occured:
    -- event_use() won't be called
    return true
  end

  return false
end

function event_use()

  -- using the mirror at a wrong place
  sol.main.play_sound("wrong")
  sol.map.hero_unfreeze()
end

function event_map_changed()

  -- stop disabling tiles, they don't exist anymore
  sol.main.timer_stop_all()
end

