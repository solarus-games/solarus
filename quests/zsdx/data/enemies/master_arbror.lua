-- A tree boss from Newlink

function event_appear()

  sol.enemy.set_life(8)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/master_arbror")
  sol.enemy.set_size(264, 312)
  sol.enemy.set_origin(132, 309)
  --sol.enemy.set_invincible()
end

function event_restart()

  go()
end

function go()
  m = sol.main.pixel_movement_create("434343373737707070010101151515545454", 20)
  sol.main.movement_set_property(m, "loop", true)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
end

function event_hurt(attack, life_lost)

  if sol.enemy.get_life() <= 0 then
    sol.map.dialog_start("dungeon_3.arbror_killed")
    sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
  end
end

