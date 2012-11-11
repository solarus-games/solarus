local map = ...
-- Lyriann Shop

function map:on_started(destination)

  if map:get_game():is_dungeon_finished(4)
      and (not map:get_game():get_value("b938")
           or not map:get_game():get_value("b939")
           or not map:get_game():get_value("b940")) then
    -- after dungeon 4, replace the usual item by advanced equipment
    -- unless all advanced equipment was bought
    apples:remove()
    heart:remove()
    lamp:remove()

    if not map:get_game():get_value("b938") then
      -- a bomb bag still available in this shop
      if map:get_game():get_value("b510") then
	-- already found the other one
	bomb_bag_2:remove()
      else
	bomb_bag_3:remove()
      end
    end

    if not map:get_game():get_value("b939") then
      -- a quiver still available in this shop
      if map:get_game():get_value("b941") then
	-- already found the other one
	quiver_2:remove()
      else
	quiver_3:remove()
      end
    end

  else
    -- disable the advanced equipment and let usual items
    bomb_bag_2:remove()
    bomb_bag_3:remove()
    quiver_2:remove()
    quiver_3:remove()
    rupee_bag:remove()

    if map:get_game():get_item("rupee_bag"):has_variant(2)
        and not map:get_game():is_dungeon_finished(2) then
      lamp:remove()
    end
  end
end

