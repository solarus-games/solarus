------------------------------
-- Outside fields SO script --
------------------------------

-- Function called when the map starts.
-- The NPCs are initialized.
function event_map_started(destination_point_name)
  -- make the NPCs walk
  if sol.game.savegame_get_boolean(48) then
    random_monkey_run(48) -- the monkey has not the boots anymore
  else
    random_monkey_run(128)
  end
end

function random_monkey_run(speed)

  local m = sol.main.random_path_movement_create(speed)
  sol.map.npc_start_movement("forest_monkey", m)
  sol.main.sprite_set_animation(sol.map.npc_get_sprite("forest_monkey"), "walking")
end


-- Function called when the player wants to talk to a non-playing character.
function event_npc_interaction(npc_name)
  if npc_name == "guard" then
    if sol.map.hero_get_direction() == 0 then
      sol.map.dialog_start("outside_fields_SO.guard_ok")
    else
      sol.map.dialog_start("outside_fields_SO.guard_nok")
    end
  elseif npc_name == "forest_monkey" then
    sol.main.play_sound("monkey")
    if sol.game.savegame_get_boolean(48) then -- has boots
      sol.map.dialog_start("outside_fields_SO.forest_monkey_end")
    elseif sol.game.get_item_amount("apple_pie_counter") > 0 then -- has apple pie
      sol.map.dialog_start("outside_fields_SO.forest_monkey_give_boots")
    else
      sol.map.dialog_start("outside_fields_SO.forest_monkey_start")
    end
  end
end

-- Function called when the dialog box is being closed.
-- If the player was talking to the guard, we do the appropriate action
function event_dialog_finished(first_message_id, answer)
  if first_message_id == "outside_fields_SO.guard_ok" then

    local s = sol.map.npc_get_sprite("guard")
    if sol.main.sprite_get_animation(s) ~= "walking" then
      -- make the guard move
      local m = sol.main.path_movement_create("000000000066", 24)
      sol.main.sprite_set_animation(s, "walking")
      sol.map.npc_start_movement("guard", m)
    end
  elseif first_message_id == "outside_fields_SO.forest_monkey_give_boots" then
    sol.map.treasure_give("pegasus_shoes", 1, 48)
    sol.game.remove_item_amount("apple_pie_counter", 1)
  end
end

function event_chest_empty(chest_name)
   if chest_name == "chest_link_house" then
    sol.map.dialog_start("outside_fields_SO.chest_link_house")
    sol.map.hero_unfreeze()
  end
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "pegasus_shoes" then
    random_monkey_run(48) -- reduce the speed
  end
end

