local item = ...

-- The mystic mirror allows the hero to traverse a specific waterfall
-- in the mountains.

local i = 1  -- Index of the waterfall tiles.
local timer

function item:on_created()

  self:set_savegame_variable("i1108")
  self:set_assignable(true)
end

function item:on_npc_interaction(npc)

  -- Give a hint when looking at the waterfall.
  if npc:get_name():find("^riverfall_detector_[we]$") then
    npc:get_map():start_dialog("outside_world.mountain_riverfall")
  end
end

function item:on_npc_interaction_item(npc, item_used)

  local side = npc:get_name():match("^riverfall_detector_([we])$")
  if side ~= nil and item:get_name() == "mystic_mirror" then
    -- Using the mirror with the waterfall: reverse the waterfall
    -- and traverse it.
    self:get_map():hero_freeze()
    sol.audio.play_sound("water_drain")
    if timer ~= nil then
      timer:stop()
    end
    self:get_map():set_entities_enabled("riverfall_tile", false)

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
        self:get_map():get_entity("hero"):walk(path, false, true)
        timer = sol.timer.start(2000, repeat_restore_riverfall)
      end
    end

    function repeat_restore_riverfall()
      i = i - 1
      local tile = self:get_map():get_entity("riverfall_" .. i)
      tile:set_enabled(false)
      if i > 1 then
        timer = sol.timer.start(350, repeat_restore_riverfall)
      end
    end

    timer = sol.timer.start(350, repeat_change_riverfall)

    -- Tell the engine that an interaction occured:
    -- on_using() won't be called.
    return true
  end

  return false
end

function item:on_using()

  -- Using the mirror at a wrong place.
  sol.audio.play_sound("wrong")
  self:set_finished()
end

