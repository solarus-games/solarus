local map = ...
local game = map:get_game()

function map:on_started()
  if game:get_value("blacksmith") ~= 1 then
    -- Remove the blacksmith if he is not waiting in the fighting room.
    blacksmith:remove()
  else
    -- Start battle.
    game:start_dialog("blacksmith.5", function() -- Dialog: En garde!
      map:start_battle() -- Start the battle.
    end)
  end
end


function map:start_battle()
  -- DEFINE MORE THINGS HERE IF NECESSARY!!!
end