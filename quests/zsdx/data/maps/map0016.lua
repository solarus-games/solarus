-- Smith cave

-- Function called when the player wants to talk to a non-playing character.
function event_npc_interaction(npc_name)

  -- smith dialog
  if not sol.game.savegame_get_boolean(30) then
    -- the player has no sword yet
    sol.map.dialog_start("smith_cave.without_sword")
  else
    -- the player already has the sword
    sol.map.dialog_start("smith_cave.with_sword")
  end
end

-- Function called when the dialog box is being closed.
function event_dialog_finished(first_message_id, answer)

  if first_message_id == "smith_cave.without_sword" then
    -- the dialog was the question to buy the sword

    if answer == 1 then
      -- the player does not want to buy the sword
      sol.map.dialog_start("smith_cave.not_buying")
    else
      -- wants to buy the sword
      if sol.game.get_money() < 80 then
        -- not enough money
        sol.main.play_sound("wrong")
        sol.map.dialog_start("smith_cave.not_enough_money")
      else
        -- enough money: buy the sword
        sol.game.remove_money(80)
        sol.map.treasure_give("sword", 1, 30)
      end
    end
  end
end

-- Function called when the player has bought the sword.
function event_treasure_obtained(item_name, variant, savegame_variable)
   if (savegame_variable == 30) then
      sol.map.dialog_start("smith_cave.thank_you")
   end
end

