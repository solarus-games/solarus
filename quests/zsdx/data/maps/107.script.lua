local map = ...
-- Dungeon 9 3F

local fighting_miniboss = false

function map:on_started(destination)

  -- miniboss
  map:set_entities_enabled("miniboss_enemy", false)
  map:set_doors_open("miniboss_e_door", true)
  if map:get_game():get_value("866") then
    map:set_doors_open("miniboss_door", true)
  end
end

-- door A
local function door_a_enemy_dead(enemy)

  if not door_a:is_open()
      and not map:has_entities("door_a_enemy") then
    map:move_camera(2248, 648, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("door_a")
    end)
  end
end

for _, enemy in ipairs(map:get_entities("door_a_enemy")) do
  enemy.on_dead = door_a_enemy_dead
end

-- miniboss
local function miniboss_enemy_dead(enemy)

  if not map:has_entities("miniboss_enemy") then

    sol.audio.play_music("southern_shrine")
    sol.audio.play_sound("secret")
    map:open_doors("miniboss_door")
    map:get_game():set_value("866", true)
  end
end

for _, enemy in ipairs(map:get_entities("miniboss_enemy")) do
  enemy.on_dead = miniboss_enemy_dead
end

-- door B hint stones
local function door_b_hint_interaction(npc)

  if not door_b:is_open() then
    local door_b_next = map:get_game():get_value("dungeon_9.hint_stones") + 1
    local index = tonumber(npc:get_name():match("^door_b_hint_([1-8])$"))
    if index == door_b_next then
      -- correct
      if index < 8 then
        local directions = { 0, 0, 0, 3, 2, 2, 2 }
        map:start_dialog("dungeon_9.3f_door_b_hint_" .. directions[index])
        map:get_game():set_value("dungeon_9.hint_stones", index)
      else
        map:move_camera(1928, 1928, 500, function()
          sol.audio.play_sound("secret")
          map:open_doors("door_b")
        end)
      end
    else
      -- wrong
      sol.audio.play_sound("wrong")
      map:get_game():set_value("dungeon_9.hint_stones", 0)
    end
  end
end

for _, npc in ipairs(map:get_entities("door_b_hint")) do
  npc.on_interaction = door_b_hint_interaction
end

-- miniboss
function start_miniboss_sensor:on_activated()

  if not map:get_game():get_value("866")
      and not fighting_miniboss then

    hero:freeze()
    map:close_doors("miniboss_e_door")
    fighting_miniboss = true
    sol.timer.start(1000, function()
      sol.audio.play_music("boss")
      map:set_entities_enabled("miniboss_enemy", true)
      hero:unfreeze()
    end)
  end
end

