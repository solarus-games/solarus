-- Billy's cave

billy_leave_step = 0

function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(134) then
    -- the player already gave the golden bars and obtained the edelweiss
    sol.map.npc_remove("billy")
  end
end

function event_npc_interaction(npc_name)

  if npc_name == "billy" then
    if not sol.game.savegame_get_boolean(135) then
      sol.map.dialog_start("billy_cave.hello")
      sol.game.savegame_set_boolean(135, true)
    else
      sol.map.dialog_start("billy_cave.what_do_you_have")
    end
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "billy_cave.what_do_you_have" then
    if sol.game.get_item("level_4_way") == 2 then
      -- the player has the golden bars
      sol.map.dialog_start("billy_cave.with_golden_bars")
    else 
      sol.map.dialog_start("billy_cave.without_golden_bars")
    end

  elseif first_message_id == "billy_cave.without_golden_bars" then
    if answer == 0 then
      give_croissant()
     else
      give_apple_pie()
    end

  elseif first_message_id == "billy_cave.with_golden_bars" then
    if answer == 0 then
      give_golden_bars()
     else
      give_apple_pie()
    end

  elseif first_message_id == "billy_cave.give_golden_bars" then
    sol.map.treasure_give("level_4_way", 3, 134)
  end
end

function give_croissant()

  if sol.game.has_item_amount("croissants_counter", 1) then
    sol.map.dialog_start("billy_cave.give_croissant")
  else
    sol.map.dialog_start("billy_cave.give_croissant_without")
  end
end

function give_apple_pie()
  if sol.game.get_item("level_4_way") == 1 then
    sol.map.dialog_start("billy_cave.give_apple_pie")
  else
    sol.map.dialog_start("billy_cave.give_apple_pie_without")
  end
end

function give_golden_bars()
  sol.map.dialog_start("billy_cave.give_golden_bars")
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "level_4_way" and variant == 3 then
    -- got the edelweiss: make Billy leave
    billy_leave()
  end
end

function event_npc_movement_finished(npc_name)

  if npc_name == "billy" then
    billy_leave()
   end
end

function billy_leave()

  billy_leave_step = billy_leave_step + 1
  local sprite = sol.map.npc_get_sprite("billy")

  if billy_leave_step == 1 then
    sol.map.hero_freeze()
    local m = sol.main.path_movement_create("4444444", 48)
    sol.main.movement_set_property(m, "ignore_obstacles", true)
    sol.map.npc_start_movement("billy", m)
    sol.main.sprite_set_animation(sprite, "walking")
  elseif billy_leave_step == 2 then
    sol.main.sprite_set_direction(sprite, 1)
    sol.main.timer_start(billy_leave, 500)
  elseif billy_leave_step == 3 then    
    sol.map.door_open("door")
    sol.main.timer_start(billy_leave, 500)
  elseif billy_leave_step == 4 then 
    local m = sol.main.path_movement_create("22222222", 48)
    sol.map.npc_start_movement("billy", m)
    sol.main.sprite_set_animation(sprite, "walking")
  else
    sol.map.door_close("door")
    sol.map.npc_remove("billy")
    sol.map.hero_unfreeze()
  end
end

