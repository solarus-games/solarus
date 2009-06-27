----------------------
-- Cake shop script --
----------------------

apples_question_asked = false

function event_hero_on_sensor(sensor_name)

   if not savegame_get_boolean(32) and not apples_question_asked then
      start_message("cake_shop.dont_leave")
      apples_question_asked = true
   end
end

function event_message_sequence_finished(first_message_id, answer)

   if first_message_id == "cake_shop.dont_leave" and answer == 0 then

      if inventory_item_get(10) == 1 then
	 if inventory_item_get_amount(10) >= 6 then
	    start_message("cake_shop.thank_you")
	    inventory_item_remove_amount(10, 6)
	 else
	    start_message("cake_shop.not_enough_apples")
	 end
      else
	 start_message("cake_shop.no_apples")
      end

   elseif first_message_id == "cake_shop.thank_you" then
      give_treasure(7, 32)
   end

end
