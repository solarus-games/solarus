-- Temple of Stupidities 1F NE

cannonballs_enabled = false

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

function event_hero_on_sensor(sensor_name)

  if string.match(sensor_name, "^cannonballs_start_sensor") and not cannonballs_enabled then
    launch_cannonball()
    cannonballs_enabled = true
  elseif string.match(sensor_name, "^cannonballs_stop_sensor") and cannonballs_enabled then
    sol.main.timer_stop("launch_cannonball")
    cannonballs_enabled = false
  end
end

nb_cannonballs_created = 0
function launch_cannonball()

  sol.main.timer_start(1500, "launch_cannonball", false)
  nb_cannonballs_created = nb_cannonballs_created + 1
  sol.map.enemy_create("cannonball_"..nb_cannonballs_created, "cannonball", 0, 280, 725)
end

