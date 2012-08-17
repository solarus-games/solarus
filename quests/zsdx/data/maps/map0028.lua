local map = ...
-- Link's house

-- This is an intermediate version:
-- - The map is now a proper userdata and this file is a function that manages it.
-- - The map API is still the old one (entities are not userdata yet).

local map = ...

function map:on_started(destination_point_name)

  if destination_point_name == "from_intro" then
    -- the intro scene is playing
    self:get_game():set_hud_enabled(true)
    self:set_pause_enabled(false)
    self:dialog_set_style(0)
    local snores_sprite = self:npc_get_sprite("snores")
    local bed_sprite = self:npc_get_sprite("bed")
    snores_sprite:set_ignore_suspend(true)
    bed_sprite:set_animation("hero_sleeping")
    self:hero_freeze()
    self:hero_set_visible(false)
    sol.timer.start(2000, function()
      self:sahasrahla_dream()
    end)
  else
    self:npc_remove("snores")
  end
end

function map:sahasrahla_dream()
  self:dialog_start("link_house.dream")
  self:dialog_set_variable("link_house.dream", self:get_game():get_player_name())
end

function map:on_dialog_finished(dialog_id)
  
  if dialog_id == "link_house.dream" then
    sol.timer.start(1000, function()
      self:wake_up()
    end)
  end
end

function map:wake_up()
  self:npc_remove("snores")
  local bed_sprite = self:npc_get_sprite("bed")
  bed_sprite:set_animation("hero_waking")
  sol.timer.start(500, function()
    self:jump_from_bed()
  end)
end

function map:jump_from_bed()
  self:hero_set_visible(true)
  self:hero_start_jumping(4, 24, true)
  self:set_pause_enabled(true)
  local bed_sprite = self:npc_get_sprite("bed")
  bed_sprite:set_animation("empty_open")
  sol.audio.play_sound("hero_lands")
end

function map:on_door_open(door_name)

  if door_name == "weak_wall" then
    sol.audio.play_sound("secret")
  end
end


