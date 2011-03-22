----------------------------------
-- Crazy House 2FB (north)      --
-- TODO: MAP TERMINEE, A TESTER --
----------------------------------

-- Guichet 82 -------------------------------------------------
function guichet_82()
	sol.map.dialog_start("crazy_house.guichet_82")
end

-- Guichet 84 -------------------------------------------------
function guichet_84()
	if sol.game.savegame_get_integer(1410) == 3 then
		sol.map.dialog_start("crazy_house.guichet_84_ech_eq_3")
	else
		if sol.game.get_item_amount("hache_counter") >= 1 then
			sol.map.dialog_start("crazy_house.guichet_84_ech_ne_3_hh")
		else
			sol.map.dialog_start("crazy_house.guichet_84_ech_ne_3_nh")
		end
	end
end

function event_npc_dialog(entity_name)
	if entity_name == "GC82" then
		guichet_82()
	end
end

function event_hero_interaction(entity_name)
	if entity_name == "GC82Front" then
		guichet_82()
	elseif entity_name == "GC84" then
		guichet_84()
	end
end

function event_dialog_finished(first_message_id, answer)
	if first_message_id == "crazy_house.guichet_82" then
		-- Choix de réponse au guichet 82		
		if answer == 0 then
			-- Contrôle de la quantité
			if sol.game.get_item_amount("sac_olive_counter") < 2 then
				sol.main.play_sound("wrong")
				sol.map.dialog_start("crazy_house.guichet_82_un")
			else
				sol.map.dialog_start("crazy_house.guichet_82_ok")
			end
		end
	elseif first_message_id == "crazy_house.guichet_82_ok" then
		-- Obtention du roc magma au guichet 82		
		sol.map.treasure_give("roc_magma", 1, -1)
		sol.main.remove_item_amount("sac_olive_counter", 2)
	elseif first_message_id == "crazy_house.guichet_84_ech_ne_3_hh" then
		-- Choix de réponse au guichet 84		
		if answer == 0 then
			-- Contrôle de la quantité
			if sol.game.get_item_amount("hache_counter") < 5 then
				sol.main.play_sound("wrong")
				sol.map.dialog_start("crazy_house.guichet_84_ech_ne_3_un")
			else
				sol.map.treasure_give("poivron", 1, -1)
				sol.game.add_item_amount("poivron_counter", 2)
				sol.game.remove_item_amount("hache_counter", 5)
			end
		end
	end
end
