-- Temple of Stupidities 1F NE

function event_door_open(door_name)

  if door_name == "weak_wall_compass"
      or door_name == "weak_wall_red_tunic" then
    sol.main.play_sound("secret")
  end
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if savegame_variable == 248 then
    sol.map.dialog_start("small_key_danger_east")
  end
end

