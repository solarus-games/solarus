-- Let the hero run with the action key or an inventory item key
sol.game.set_ability("run", sol.item.get_variant())

function event_variant_changed(variant)

  sol.game.set_ability("run", variant)
end

function event_use()

  sol.map.hero_start_running()
  sol.item.set_finished()
end

