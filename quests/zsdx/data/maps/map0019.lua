local map = ...
-- Cake shop

function map:on_started(destination_point)

  if not has_obtained_bottle() or not map:get_game():is_dungeon_finished(1) then
    apple_pie:remove()
  end
end

local function has_talked_about_apples()
  return map:get_game():get_boolean(46)
end

local function has_obtained_bottle()
  return map:get_game():get_boolean(32)
end

function leaving_shop_sensor:on_activated()

  if not has_obtained_bottle() and not has_talked_about_apples() then
    map:start_dialog("cake_shop.dont_leave", apples_question_finished)
  end
end

local function apples_question_finished(answer)

  map:get_game():set_boolean(46, true)

  if answer == 0 then
    if map:get_game():gas_item("apples_counter"):has_variant() then
      if map:get_game():get_item("apples_counter"):has_amount(6) then
        map:start_dialog("cake_shop.thank_you" function()
          hero:start_treasure("bottle_1", 1, 32)
        end)
        map:get_game():get_item("apples_counter"):remove_amount(6)
      else
        map:start_dialog("cake_shop.not_enough_apples")
      end
    else
      map:start_dialog("cake_shop.no_apples")
    end
  end
end

function seller_talking_place:on_interaction()
  talk_to_seller()
end

function seller:on_interaction()
  talk_to_seller()
end

local function talk_to_seller()

  if not has_talked_about_apples() or has_obtained_bottle() then
    map:start_dialog("cake_shop.seller.choose_item")
  else
    map:start_dialog("cake_shop.seller.ask_apples_again", apples_question_finished)
  end
end

