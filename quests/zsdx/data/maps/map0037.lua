local map = ...
-- Billy's cave

billy_leave_step = 0

function map:on_started(destination_point_name)

  if map:get_game():get_boolean(134) then
    -- the player already gave the golden bars and obtained the edelweiss
    map:npc_remove("billy")
  end

  if destination_point_name ~= "from_outside" then
    map:door_set_open("door", true)
  end
end

function map:on_npc_interaction(npc_name)

  if npc_name == "billy" then
    if not map:get_game():get_boolean(135) then
      map:dialog_start("billy_cave.hello")
      map:get_game():set_boolean(135, true)
    else
      map:dialog_start("billy_cave.what_do_you_have")
    end
  end
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "billy_cave.what_do_you_have" then
    if map:get_game():get_item("level_4_way") == 2 then
      -- the player has the golden bars
      map:dialog_start("billy_cave.with_golden_bars")
    else 
      map:dialog_start("billy_cave.without_golden_bars")
    end

  elseif dialog_id == "billy_cave.without_golden_bars" then
    if answer == 0 then
      give_croissant()
     else
      give_apple_pie()
    end

  elseif dialog_id == "billy_cave.with_golden_bars" then
    if answer == 0 then
      give_golden_bars()
     else
      give_apple_pie()
    end

  elseif dialog_id == "billy_cave.give_golden_bars" then
    map:treasure_give("level_4_way", 3, 134)
  end
end

function give_croissant()

  if map:get_game():has_item_amount("croissants_counter", 1) then
    map:dialog_start("billy_cave.give_croissant")
  else
    map:dialog_start("billy_cave.give_croissant_without")
  end
end

function give_apple_pie()
  if map:get_game():get_item("level_4_way") == 1 then
    map:dialog_start("billy_cave.give_apple_pie")
  else
    map:dialog_start("billy_cave.give_apple_pie_without")
  end
end

function give_golden_bars()
  map:dialog_start("billy_cave.give_golden_bars")
end

function map:on_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "level_4_way" and variant == 3 then
    -- got the edelweiss: make Billy leave
    billy_leave()
  end
end

function map:on_npc_movement_finished(npc_name)

  if npc_name == "billy" then
    billy_leave()
   end
end

function billy_leave()

  billy_leave_step = billy_leave_step + 1
  local sprite = map:npc_get_sprite("billy")

  if billy_leave_step == 1 then
    map:hero_freeze()
    local m = sol.movement.create("path")
    m:set_path{4,4,4,4,4,4,4}
    m:set_speed(48)
    m:set_ignore_obstacles(true)
    map:npc_start_movement("billy", m)
    sprite:set_animation("walking")
  elseif billy_leave_step == 2 then
    sprite:set_direction(1)
    sol.timer.start(500, billy_leave)
  elseif billy_leave_step == 3 then
    map:door_open("door")
    sol.timer.start(500, billy_leave)
  elseif billy_leave_step == 4 then
    local m = sol.movement.create("path")
    m:set_path{2,2,2,2,2,2,2,2}
    m:set_speed(48)
    map:npc_start_movement("billy", m)
    sprite:set_animation("walking")
  else
    map:door_close("door")
    map:npc_remove("billy")
    map:hero_unfreeze()
  end
end

