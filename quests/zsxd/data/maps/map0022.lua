----------------------------------
-- Crazy House 2FA (south)      --
-- TODO: MAP TERMINEE, A TESTER --
----------------------------------

locked_door_A_value = 0

function event_map_started(destination_point_name)
	-- Coffre farceur	
	sol.map.chest_set_enabled("prankster_chest_bottom", false)
	sol.map.chest_set_enabled("prankster_chest_middle", false)
end

-- Vieillard --------------------------------------------------
function vieillard()
	if sol.game.savegame_get_boolean(124) == false then
		-- Première rencontre		
		sol.map.dialog_start("crazy_house.vieillard")
		sol.game.savegame_set_boolean(124, true)
	else
		if sol.game.savegame_get_boolean(125) == false then
			-- Vieillard n'a pas encore changé d'avis
			if sol.game.get_item_amount("poivron_counter") < 3 then
				-- N'a pas encore 3 poivrons
				sol.map.dialog_start("crazy_house.vieillard")
			else
				-- A les 3 poivrons
				sol.map.dialog_start("crazy_house.vieillard_poivron")
				-- Changement d'avis				
				sol.game.savegame_set_boolean(125, true)
			end
		else
			-- Vieillard veut du riz maintenant !
			if sol.game.get_item_amount("sac_riz_counter") < 5 then
				sol.map.dialog_start("crazy_house.vieillard_riz_quantite")
			else
				-- A les 5 sacs de riz
				sol.map.dialog_start("crazy_house.vieillard_riz_ok")
			end			
		end
	end
end

-- Guichet 21 -------------------------------------------------
function guichet_21()
	if sol.game.savegame_get_integer(1410) == 1 then		
		sol.map.dialog_start("crazy_house.guichet_21_ech_eq_1")
		sol.game.savegame_set_integer(1410, 2)
	else
		sol.map.dialog_start("crazy_house.guichet_21_ech_ne_1")
	end
end

-- Guichet 22A -------------------------------------------------
function guichet_22A()
	sol.map.dialog_start("crazy_house.guichet_22A")
end

-- Guichet 22B -------------------------------------------------
function guichet_22B()
	sol.map.dialog_start("crazy_house.guichet_22B")
end

-- Interactions avec capteur pour guichet (devanture)
function event_hero_interaction(entity_name)
	if entity_name == "GC21front" then
		guichet_21()
	elseif entity_name == "GC22A" then
		guichet_22A()
	elseif entity_name == "GC22B" then
		guichet_22B()
	end
end

-- Interactions avec un NPC pour guichet
function event_npc_dialog(entity_name)
	if entity_name == "Vieillard" then
		vieillard()
	elseif entity_name == "GC21" then
		guichet_21()
	end
end

function event_dialog_finished(first_message_id, answer)
	if first_message_id == "crazy_house.vieillard_riz_ok" then
		sol.map.treasure_give("bocal_epice", 1, -1)
		sol.game.remove_item_amount("sac_riz_counter", 5)
	elseif first_message_id == "crazy_house.guichet_22A" then
		if answer == 0 then
			if sol.game.get_item_amount("roc_magma_counter") < 3 then
				sol.main.play_sound("wrong")
				sol.map.dialog_start("crazy_house.guichet_22_rm_un")
			else
				sol.map.dialog_start("crazy_house.guichet_22_rm_ok")
			end
		end
	elseif first_message_id == "crazy_house.guichet_22B" then
		if answer == 0 then
			if sol.game.get_item_amount("sac_riz_counter") < 2 then
				sol.main.play_sound("wrong")
				sol.map.dialog_start("crazy_house.guichet_22_sr_un")
			else
				sol.map.dialog_start("crazy_house.guichet_22_sr_ok")
			end
		end
	elseif first_message_id == "crazy_house.guichet_22_rm_ok" then
		sol.map.treasure_give("balai", 1, -1)
		sol.game.remove_item_amount("roc_magma_counter", 3)
	elseif first_message_id == "crazy_house.guichet_22_sr_ok" then
		sol.map.treasure_give("tapisserie", 1, -1)
		sol.game.remove_item_amount("sac_riz_counter", 2)
	end
end

function event_hero_on_sensor(sensor_name)
	-- Mécanisme du coffre farceur dans la salle aux trois portes	
	if sensor_name == "prankster_sensor_top" then
		sol.map.chest_set_enabled("prankster_chest_bottom", false)
		sol.map.chest_set_enabled("prankster_chest_middle", true)
	elseif sensor_name == "prankster_sensor_middle" then
		sol.map.chest_set_enabled("prankster_chest_bottom", true)
		sol.map.chest_set_enabled("prankster_chest_middle", false)	
	elseif sensor_name == "prankster_sensor_bottom" then
		sol.map.chest_set_enabled("prankster_chest_bottom", false)
		sol.map.chest_set_enabled("prankster_chest_middle", true)
	end
end

function event_switch_activated(switch_name)
	-- Mécanisme de la porte qui s'ouvre grâce à deux boutons
  -- dans la salle aux trois portes	
	if switch_name == "locked_door_switch_A"
	or switch_name == "locked_door_switch_B" then
		if locked_door_A_value < 2 then
			locked_door_A_value = locked_door_A_value + 1
			if locked_door_A_value == 2 then
				sol.map.door_set_open("locked_door_A", true)
				sol.main.play_sound("door_open")
			end
		end
	end
end
