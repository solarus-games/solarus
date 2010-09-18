----------------------
-- Cake shop script --
----------------------

function has_talked_about_apples()
  return savegame_get_boolean(46)
end

function has_obtained_bottle()
  return savegame_get_boolean(32)
end

function event_hero_on_sensor(sensor_name)

   if not has_obtained_bottle() and not has_talked_about_apples() then
      dialog_start("cake_shop.dont_leave")
   end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "cake_shop.dont_leave" 
    or first_message_id == "cake_shop.seller.ask_apples_again" then

    savegame_set_boolean(46, true)

    if answer == 0 then
      if equipment_has_item("apples_counter") then
	if equipment_get_item_amount("apples_counter") >= 6 then
	  dialog_start("cake_shop.thank_you")
	  equipment_remove_item_amount("apples_counter", 6)
	else
	  dialog_start("cake_shop.not_enough_apples")
	end
      else
	dialog_start("cake_shop.no_apples")
      end
    end
  elseif first_message_id == "cake_shop.thank_you" then
    treasure_give("bottle_1", 1, 32)
  end
end

function event_npc_dialog(npc)
  if npc == "seller" then
    talk_to_seller()
  end
end

function event_hero_interaction(entity_name)
  if entity_name == "seller_talking_place" then
    talk_to_seller()
  end
end

function talk_to_seller()
  if not has_talked_about_apples() or has_obtained_bottle() then
    dialog_start("cake_shop.seller.choose_item")
  else 
    dialog_start("cake_shop.seller.ask_apples_again")
  end
end

