-- Cake shop

function event_map_started(destination_point_name)

  if not has_obtained_bottle() or not sol.game.is_dungeon_finished(1) then
    sol.map.shop_item_remove("apple_pie")
  end
end

function has_talked_about_apples()
  return sol.game.savegame_get_boolean(46)
end

function has_obtained_bottle()
  return sol.game.savegame_get_boolean(32)
end

function event_hero_on_sensor(sensor_name)

  if not has_obtained_bottle() and not has_talked_about_apples() then
    sol.map.dialog_start("cake_shop.dont_leave")
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "cake_shop.dont_leave" 
    or first_message_id == "cake_shop.seller.ask_apples_again" then

    sol.game.savegame_set_boolean(46, true)

    if answer == 0 then
      if sol.game.has_item("apples_counter") then
	if sol.game.get_item_amount("apples_counter") >= 6 then
	  sol.map.dialog_start("cake_shop.thank_you")
	  sol.game.remove_item_amount("apples_counter", 6)
	else
	  sol.map.dialog_start("cake_shop.not_enough_apples")
	end
      else
	sol.map.dialog_start("cake_shop.no_apples")
      end
    end
  elseif first_message_id == "cake_shop.thank_you" then
    sol.map.treasure_give("bottle_1", 1, 32)
  end
end

function event_npc_interaction(npc_name)
  if npc_name == "seller_talking_place"
      or npc_name == "seller" then
    talk_to_seller()
  end
end

function talk_to_seller()
  if not has_talked_about_apples() or has_obtained_bottle() then
    sol.map.dialog_start("cake_shop.seller.choose_item")
  else 
    sol.map.dialog_start("cake_shop.seller.ask_apples_again")
  end
end

