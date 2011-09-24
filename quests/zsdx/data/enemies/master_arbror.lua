-- A giant tree boss from Newlink

nb_sons_created = 0

function event_appear()

  sol.enemy.set_life(8)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/master_arbror")
  sol.enemy.set_size(264, 312)
  sol.enemy.set_origin(132, 269)
  sol.enemy.set_invincible()
end

function event_restart()

  go()
end

function go()
  local m = sol.main.pixel_movement_create("434343373737707070010101151515545454", 20)
  sol.main.movement_set_property(m, "loop", true)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
  sol.main.timer_start(2000, "create_son", false)
end

function event_hurt(attack, life_lost)

  if sol.enemy.get_life() <= 0 then
    sol.map.dialog_start("dungeon_3.arbror_killed")
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
  end
end

function create_son()

  -- create the son
  nb_sons_created = nb_sons_created + 1
  son_name = sol.enemy.get_name().."_son_"..nb_sons_created
  sol.enemy.create_son(son_name, "arbror_root", 0, 40)
  sol.main.play_sound("hero_pushes")
  sol.main.timer_start(1000, "son_out", false)
end

function son_out()

  sol.main.play_sound("stone")
end

