local map = ...
-- Dungeon 6 4F

local fighting_boss = false
local nb_spawners_created = 0

-- possible positions of Drakomos lava spawners
local spawner_xy = {
  { x = 176, y = 301 },
  { x = 272, y = 325},
  { x = 320, y = 301},
  { x = 392, y = 365},
  { x = 400, y = 237},
  { x = 144, y = 245},
  { x = 152, y = 325},
  { x = 152, y = 365},
  { x = 312, y = 357},
  { x = 392, y = 325},
  { x = 232, y = 301},
  { x = 368, y = 301}
}

local function repeat_lava_spawner()

  if not map:get_game():get_value("b321") then
    nb_spawners_created = nb_spawners_created + 1
    local index = math.random(#spawner_xy)
    map:create_enemy{
      name = "spawner_" .. nb_spawners_created,
      breed = "drakomos_lava_spawner",
      layer = 1,
      x = spawner_xy[index].x,
      y = spawner_xy[index].y
    }
    sol.timer.start(5000 + math.random(10000), repeat_lava_spawner)
  end
end

function map:on_started(destination)

  map:set_doors_open("ne_door", true)
  map:set_doors_open("boss_door", true)
end

function ne_door_sensor:on_activated()

  if ne_door:is_open() then
    map:close_doors("ne_door")
  else
    map:open_doors("ne_door")
  end
end

function start_boss_sensor:on_activated()

  if not map:get_game():get_value("b321")
      and not fighting_boss then

    start_boss_sensor:set_enabled(false)
    hero:freeze()
    map:close_doors("boss_door")
    sol.timer.start(1000, function()
      sol.audio.play_music("boss")
      boss:set_enabled(true)
      hero:unfreeze()
      sol.timer.start(3000, repeat_lava_spawner)
      fighting_boss = true
    end)
  end
end

function map:on_obtained_treasure(item, variant, savegame_variable)

  if item:get_name() == "heart_container" then
    sol.audio.play_music("victory")
    hero:freeze()
    hero:set_direction(3)
    sol.timer.start(9000, function()

      sol.audio.play_music("dungeon_finished")
      hero:set_direction(1)
      tom:set_position(272, 237)
      map:move_camera(272, 232, 100, function()
	map:set_dialog_variable("dungeon_6.tom", map:get_game():get_player_name())
	map:start_dialog("dungeon_6.tom", function()
	  sol.audio.stop_music()
	  sol.timer.start(1000, function()
	    sol.audio.play_music("legend")
	    map:set_dialog_variable("dungeon_6.tom_revelation", map:get_game():get_player_name())
	    map:start_dialog("dungeon_6.tom_revelation", function()
	      local variant = 2
	      if map:get_game():get_value("b939") then
		variant = 3
	      end
	      hero:start_treasure("quiver", variant, "b941", function()
                map:get_game():set_dungeon_finished(6)
                map:get_game():set_value("b155", false) -- reopen the rupee house
                hero:teleport(7, "from_dungeon_6")
              end)
	    end)
	  end)
	end)
      end)
    end)
  end
end

