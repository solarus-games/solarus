local map = ...
-- Cake shop

function map:on_started(destination_point_name)

  if not has_obtained_bottle() or not map:get_game():is_dungeon_finished(1) then
    map:shop_item_remove("apple_pie")
  end
end

function has_talked_about_apples()
  return map:get_game():get_boolean(46)
end

function has_obtained_bottle()
  return map:get_game():get_boolean(32)
end

function map:on_hero_on_sensor(sensor_name)

  if not has_obtained_bottle() and not has_talked_about_apples() then
    map:start_dialog("cake_shop.dont_leave")
  end
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "cake_shop.dont_leave" 
    or dialog_id == "cake_shop.seller.ask_apples_again" then

    map:get_game():set_boolean(46, true)

    if answer == 0 then
      if map:get_game():has_item("apples_counter") then
	if map:get_game():get_item_amount("apples_counter") >= 6 then
	  map:start_dialog("cake_shop.thank_you")
	  map:get_game():remove_item_amount("apples_counter", 6)
	else
	  map:start_dialog("cake_shop.not_enough_apples")
	end
      else
	map:start_dialog("cake_shop.no_apples")
      end
    end
  elseif dialog_id == "cake_shop.thank_you" then
    map:hero_start_treasure("bottle_1", 1, 32)
  end
end

function map:on_npc_interaction(npc_name)
  if npc_name == "seller_talking_place"
      or npc_name == "seller" then
    talk_to_seller()
  end
end

function talk_to_seller()
  if not has_talked_about_apples() or has_obtained_bottle() then
    map:start_dialog("cake_shop.seller.choose_item")
  else 
    map:start_dialog("cake_shop.seller.ask_apples_again")
  end
end

