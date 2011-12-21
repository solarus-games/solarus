-- Lizarmen: parody of the stupid miniboss of the dungeon 2 in Zelda Mystery of Solarus

local total_sons_created = 0

function event_appear()

  sol.enemy.set_life(20)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/lizarmen")
  sol.enemy.set_size(144, 176)
  sol.enemy.set_origin(77, 165)
  sol.enemy.set_pushed_back_when_hurt(false)
end

function event_restart()

  -- yes, there is no movement :)
  sol.main.timer_start(create_son, 1000)
end

function event_hurt(attack, life_lost)

  if sol.enemy.get_life() <= 0 then
    local sons_prefix = sol.enemy.get_name().."_son_"
    sol.map.enemy_remove_group(sons_prefix)
    sol.main.timer_stop_all()
  end
end

function create_son()

  local nb_current_sons = sol.map.enemy_get_group_count(sol.enemy.get_name()  .."_son")

  if nb_current_sons < 30 then
    local total_sons_created = total_sons_created + 1
    son_name = sol.enemy.get_name() .. "_son_" .. total_sons_created
    sol.enemy.create_son(son_name, "tentacle", 0, -77)
    sol.map.enemy_set_treasure(son_name, "heart", 1, -1)
  end

  sol.main.timer_start(create_son, 1000)
end

