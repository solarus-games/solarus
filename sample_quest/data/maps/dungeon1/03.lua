local map = ...
local game = map:get_game()

function map:on_started()
  -- Remove the blacksmith if he is waiting in the fighting room.
  if game:get_value("blacksmith") == 1 then 
    blacksmith:remove()
    metal:remove()
    steam:remove()
  end
end

--[[ 
--------Blacksmith dialogs:
1- Welcome.
2- Propose battle.
3- Refuse battle. 
4- Accept battle.
5- En garde!
6- Congrats after battle.
7- Last greeting dialog.
--------Blacksmith variable:
0- Nothing done.
1- Waiting in fighting room.
2- Already beaten.
--]]
function map:dialog_with_blacksmith()
  local var = game:get_value("blacksmith")
  if var == nil then
    var = 0
    game:set_value("blacksmith", 0)
  end
  if var == 0 then -- We are before the battle event.
    game:start_dialog("blacksmith.1", function()
      game:start_dialog("blacksmith.2", function(answer)
        if answer == 2 then
          game:start_dialog("blacksmith.3") -- Refuse battle.
        else
          game:start_dialog("blacksmith.4", function() -- Accept battle.
            game:set_value("blacksmith", 1)
            hero:freeze()
            sol.timer.stop_all(blacksmith)
            local sprite = blacksmith:get_sprite()
            sprite:set_direction(1)
            sprite:set_animation("walking")
            local m = sol.movement.create("straight")
            m:set_angle(math.pi/2)
            m:set_speed(30)
            m:start(blacksmith)
            function m:on_obstacle_reached()
              blacksmith:remove() -- Destroy blacksmith.
              hero:unfreeze() -- Unfreeze hero.
            end
          end)
        end
      end)
    end)
  end
end
