local item = ...

-- The mystic mirror allows the hero to traverse a specific waterfall
-- in the mountains

local i = 1 -- index of the waterfall tiles
local timer

function item:on_npc_interaction(npc_name)

  -- give a hint when looking at the waterfall
  if npc_name:find("^riverfall_detector_[we]$") then
    self:get_map():dialog_start("outside_world.mountain_riverfall")
  end
end

function item:on_npc_interaction_item(npc_name, item_name, variant)

  local side = npc_name:match("^riverfall_detector_([we])$")
  if side ~= nil and item_name == "mystic_mirror" then
    -- using the mirror with the waterfall: reverse the waterfall
    -- and traverse it
    self:get_map():hero_freeze()
    sol.audio.play_sound("water_drain")
    if timer ~= nil then
      timer:stop()
    end
    self:get_map():tile_set_group_enabled("riverfall_", false)

    local path
    if side == "w" then
      path = "00000000"
    else
      path = "44444444"
    end
    i = 1

    function repeat_change_riverfall()
      self:get_map():tile_set_enabled("riverfall_" .. i, true)
      i = i + 1
      if i <= 8 then
        timer = sol.timer.start(350, repeat_change_riverfall)
      else
        sol.audio.play_sound("secret")
        self:get_map():hero_walk(path, false, true)
        timer = sol.timer.start(2000, repeat_restore_riverfall)
      end
    end

    function repeat_restore_riverfall()
      i = i - 1
      self:get_map():tile_set_enabled("riverfall_" .. i, false)
      if i > 1 then
        timer = sol.timer.start(350, repeat_restore_riverfall)
      end
    end

    timer = sol.timer.start(350, repeat_change_riverfall)

    -- tell the engine that an interaction occured:
    -- event_use() won't be called
    return true
  end

  return false
end

function item:on_use()

  -- using the mirror at a wrong place
  sol.audio.play_sound("wrong")
  self:get_map():hero_unfreeze()
end

function item:on_map_changed()

  -- stop disabling tiles, they don't exist anymore
  -- TODO this should now be useless, timers are attached to the map
  if timer ~= nil then
    timer:stop()
    timer = nil
  end
end

