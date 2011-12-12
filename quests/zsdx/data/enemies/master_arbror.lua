-- A giant tree boss from Newlink

local nb_sons_created = 0
local nb_sons_immobilized = 0
local nb_sons_immobilized_needed = 3 -- number of sons immobilized needed to get him vulnerable
local vulnerable = false
local initial_life = 12

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/master_arbror")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_push_hero_on_sword(true)
end

function event_restart()

  local sprite = sol.enemy.get_sprite()
  if not vulnerable then
    go()
  else
    sol.main.sprite_set_animation(sprite, "vulnerable")
  end
end

function go()

  local m = sol.main.random_movement_create(16)
  sol.main.movement_set_property(m, "max_distance", 16)
  --sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
  sol.main.timer_stop_all()
  sol.main.timer_start(prepare_son, 4000)
end

function event_hurt(attack, life_lost)

  local life = sol.enemy.get_life()
  if life <= 0 then
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
    sol.map.dialog_start("dungeon_3.arbror_killed")
    sol.main.timer_stop_all()
    remove_sons()
  else
    if life > 9 then 
      nb_sons_immobilized_needed = 3
    elseif life > 7 then
      nb_sons_immobilized_needed = 4
    elseif life > 5 then
      nb_sons_immobilized_needed = 5
    else
      nb_sons_immobilized_needed = 6
    end
  end
end

function prepare_son()

  son_prefix = sol.enemy.get_name() .. "_son"
  if sol.map.enemy_get_group_count(son_prefix) < nb_sons_immobilized_needed then
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "preparing_son")
    sol.main.play_sound("hero_pushes")
    sol.main.timer_start(create_son, 1000)
    sol.enemy.stop_movement()
  end
end

function create_son()

  x = math.random(-7, 7) * 16

  nb_sons_created = nb_sons_created + 1
  son_name = sol.enemy.get_name().."_son_"..nb_sons_created
  sol.enemy.create_son(son_name, "arbror_root", x, 80)
  local speed = 32 + (initial_life - sol.enemy.get_life()) * 5
  sol.enemy.send_message(son_name, speed)
  sol.main.play_sound("stone")
end

function event_sprite_animation_finished(sprite, animation)

  if animation == "preparing_son" then
    sol.main.sprite_set_animation(sprite, "walking")
    sol.enemy.restart()
  elseif animation == "son_immobilized" then

    if nb_sons_immobilized >= nb_sons_immobilized_needed
        and not vulnerable then

      vulnerable = true
      sol.enemy.set_attack_consequence("sword", 1)
      sol.enemy.set_attack_consequence("arrow", 2)
      sol.enemy.stop_movement()
      sol.enemy.remove_life(1)
      sol.main.sprite_set_animation(sprite, "vulnerable")
      sol.main.play_sound("boss_hurt")
      sol.main.timer_stop_all()
      sol.main.timer_start(stop_vulnerable, 4000)
      remove_sons()
    else
      sol.main.sprite_set_animation(sprite, "walking")
    end
  end
end

function event_message_received(src_enemy, message)

  if message == "begin immobilized" then
    if nb_sons_immobilized < nb_sons_immobilized_needed then
      nb_sons_immobilized = nb_sons_immobilized + 1
      local sprite = sol.enemy.get_sprite()
      local animation = sol.main.sprite_get_animation(sprite)

      if animation == "preparing_son" then
        sol.enemy.restart()
      end

      sol.main.sprite_set_animation(sprite, "son_immobilized")
    end

  elseif message == "end immobilized" then
    if nb_sons_immobilized > 0 then
      nb_sons_immobilized = nb_sons_immobilized - 1
    end
  end
end

function stop_vulnerable()

  vulnerable = false
  remove_sons()
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.restart()
end

function remove_sons()
 
  local son_prefix = sol.enemy.get_name().."_son"
  --sol.map.enemy_remove_group(son_prefix) 
  nb_sons_immobilized = 0

  for i = 1, nb_sons_created do
    son = son_prefix.."_"..i
    if not sol.map.enemy_is_dead(son) then
      sol.enemy.send_message(son, "disappear")
    end
  end
end

