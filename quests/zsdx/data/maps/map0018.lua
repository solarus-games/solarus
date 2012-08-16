local map = ...
-- Lyriann Shop

function map:on_started(destination_point_name)

  if map:get_game():is_dungeon_finished(4)
      and (not map:get_game():get_boolean(938)
           or not map:get_game():get_boolean(939)
           or not map:get_game():get_boolean(940)) then
    -- after dungeon 4, replace the usual item by advanced equipment
    -- unless all advanced equipment was bought
    map:shop_item_remove("apples")
    map:shop_item_remove("heart")
    map:shop_item_remove("lamp")

    if not map:get_game():get_boolean(938) then
      -- a bomb bag still available in this shop
      if map:get_game():get_boolean(510) then
	-- already found the other one
	map:shop_item_remove("bomb_bag_2")
      else
	map:shop_item_remove("bomb_bag_3")
      end
    end

    if not map:get_game():get_boolean(939) then
      -- a quiver still available in this shop
      if map:get_game():get_boolean(941) then
	-- already found the other one
	map:shop_item_remove("quiver_2")
      else
	map:shop_item_remove("quiver_3")
      end
    end

  else
    -- disable the advanced equipment and let usual items
    map:shop_item_remove("bomb_bag_2")
    map:shop_item_remove("bomb_bag_3")
    map:shop_item_remove("quiver_2")
    map:shop_item_remove("quiver_3")
    map:shop_item_remove("rupee_bag")

    if map:get_game():get_item("rupee_bag") >= 2
        and not map:get_game():is_dungeon_finished(2) then
      map:shop_item_remove("lamp")
    end
  end
end

