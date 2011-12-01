-- Lyriann Shop

function event_map_started(destination_point_name)

  if sol.game.is_dungeon_finished(4)
      and (not sol.game.savegame_get_boolean(938)
           or not sol.game.savegame_get_boolean(939)
           or not sol.game.savegame_get_boolean(940)) then
    -- after dungeon 4, replace the usual item by advanced equipment
    -- unless all advanced equipment was bought
    sol.map.shop_item_remove("apples")
    sol.map.shop_item_remove("heart")
    sol.map.shop_item_remove("lamp")

    if not sol.game.savegame_get_boolean(938) then
      -- a bomb bag still available in this shop
      if sol.game.savegame_get_boolean(510) then
	-- already found the other one
	sol.map.shop_item_remove("bomb_bag_2")
      else
	sol.map.shop_item_remove("bomb_bag_3")
      end
    end

    if not sol.game.savegame_get_boolean(939) then
      -- a quiver still available in this shop
      if sol.game.savegame_get_boolean(941) then
	-- already found the other one
	sol.map.shop_item_remove("quiver_2")
      else
	sol.map.shop_item_remove("quiver_3")
      end
    end

  else
    -- disable the advanced equipment and let usual items
    sol.map.shop_item_remove("bomb_bag_2")
    sol.map.shop_item_remove("bomb_bag_3")
    sol.map.shop_item_remove("quiver_2")
    sol.map.shop_item_remove("quiver_3")
    sol.map.shop_item_remove("rupee_bag")
  end
end

