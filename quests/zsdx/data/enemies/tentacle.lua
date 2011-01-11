life = 1
damage = 2
sprite = "enemies/tentacle"
size = {16, 16}
origin = {8, 13}

function event_appear()
  m = sol.main.path_finding_movement_create(32)
  sol.enemy.start_movement(m)
end
