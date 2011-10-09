function event_use()

  if sol.item.get_amount() == 0 then
    sol.main.play_sound("wrong")
  else
    sol.main.timer_start(remove_arrow, 300)
    sol.map.hero_start_bow()
  end
  sol.item.set_finished()
end

function event_amount_changed(amount)

  if sol.item.get_variant() ~= 0 then
    -- update the icon (with or without arrow)
    if amount == 0 then
      sol.item.set_variant(1)
    else
      sol.item.set_variant(2)
    end
  end
end

function event_obtaining(variant, savegame_variable)

  if variant == 1 then
    -- the player just obtained the bow in its variant without arrows:
    -- we need to set the arrows to zero manually because they are set to their maximum by default
    sol.item.set_amount(0)
  end
end

function remove_arrow()
  -- we remove the arrow from the equipemnt after a small delay because the hero
  -- does not shoot immediately
  sol.item.remove_amount(1)
end

