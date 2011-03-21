----------------------------------
-- Crazy House 1FA (south)       --
----------------------------------

function event_map_started(destination_point_name)
	if not sol.game.savegame_get_boolean(101) then
		sol.map.chest_set_enabled("CK3", false)
	end
end

function event_map_opening_transition_finished(destination_point_name)
	-- Affichage du nom du donjon quand on entre  
	if destination_point_name == "start_position" then
		sol.map.dialog_start("crazy_house.title")
 	end
end

function event_switch_activated(switch_name)
	if switch_name == "CS3" then
		-- Bouton qui fait apparaitre un coffre contenant la carte (CV3)
		sol.map.chest_set_enabled("CK3", true)
		sol.game.savegame_set_boolean(101, true)
		sol.main.play_sound("chest_appears")
	end
end

-- Hôtesse d'accueil ------------------------------------------
-- TODO: variables à finir
function accueil()
	if sol.game.savegame_get_integer(1410) == 0 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_0")
		sol.game.savegame_set_integer(1410, 1)
	elseif sol.game.savegame_get_integer(1410) == 1 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_1")
	elseif sol.game.savegame_get_integer(1410) == 2 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_2")
		sol.game.savegame_set_integer(1410, 3)
	elseif sol.game.savegame_get_integer(1410) == 3 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_3")
	elseif sol.game.savegame_get_integer(1410) == 4 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_4")
	elseif sol.game.savegame_get_integer(1410) == 5 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_5")
	elseif sol.game.savegame_get_integer(1410) == 6 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_6")
	elseif sol.game.savegame_get_integer(1410) == 7 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_7")
	elseif sol.game.savegame_get_integer(1410) == 8 then
		if sol.game.savegame_get_integer(1411) == 1 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_1-7")
    elseif sol.game.savegame_get_integer(1411) == 2 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_2-6")
    elseif sol.game.savegame_get_integer(1411) == 3 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_3-5")
    elseif sol.game.savegame_get_integer(1411) == 4 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_4-9")
    elseif sol.game.savegame_get_integer(1411) == 5 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_3-5")
    elseif sol.game.savegame_get_integer(1411) == 6 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_2-6")
    elseif sol.game.savegame_get_integer(1411) == 7 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_1-7")
    elseif sol.game.savegame_get_integer(1411) == 8 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_8-10")
    elseif sol.game.savegame_get_integer(1411) == 9 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_4-9")
    elseif sol.game.savegame_get_integer(1411) == 10 then
      sol.map.dialog_start("crazy_house.accueil_epi_eo_8-10")
    end
    if sol.game.savegame_get_integer(1412) == 1 then
      sol.map.dialog_start("crazy_house.accueil_bal_eo_1-6-8-10")
    elseif sol.game.savegame_get_integer(1412) == 2 then
      sol.map.dialog_start("crazy_house.accueil_bal_eq_2")
    elseif sol.game.savegame_get_integer(1412) == 3 then
      sol.map.dialog_start("crazy_house.accueil_bal_eq_3")
    elseif sol.game.savegame_get_integer(1412) == 4 then
      sol.map.dialog_start("crazy_house.accueil_bal_eq_4")
    elseif sol.game.savegame_get_integer(1412) == 5 then
      sol.map.dialog_start("crazy_house.accueil_bal_eo_5-9")
    elseif sol.game.savegame_get_integer(1412) == 6 then
      sol.map.dialog_start("crazy_house.accueil_bal_eo_1-6-8-10")
    elseif sol.game.savegame_get_integer(1412) == 7 then
      sol.map.dialog_start("crazy_house.accueil_bal_eq_7")
    elseif sol.game.savegame_get_integer(1412) == 8 then
      sol.map.dialog_start("crazy_house.accueil_bal_eo_1-6-8-10")
    elseif sol.game.savegame_get_integer(1412) == 9 then
      sol.map.dialog_start("crazy_house.accueil_bal_eo_5-9")
    elseif sol.game.savegame_get_integer(1412) == 10 then
      sol.map.dialog_start("crazy_house.accueil_bal_eo_1-6-8-10")
    end
	elseif sol.game.savegame_get_integer(1410) == 9 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_9")
	elseif sol.game.savegame_get_integer(1410) == 10 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_10")
	elseif sol.game.savegame_get_integer(1410) == 11 then
		sol.map.dialog_start("crazy_house.accueil_ech_eq_11")
	end
end

-- Guichet 11 -------------------------------------------------
-- TODO: dialogues à finir, script à finir
function guichet_11()
	if sol.game.savegame_get_integer(1410) == 9 then
		-- Chercher des haches
		sol.map.dialog_start("crazy_house.guichet_11_ech_eq_9")
	else
		-- S'adresser à l'accueil
		sol.map.dialog_start("crazy_house.guichet_11_ech_ne_9")
	end
end

-- Guichet 12a ------------------------------------------------
function guichet_12A()
	if sol.game.savegame_get_integer(1410) == 5 then
		sol.map.dialog_start("crazy_house.guichet_12A_ech_eq_5")
	else
		sol.map.dialog_start("crazy_house.guichet_12A_ech_ne_5")
	end
end

-- Guichet 12b -----------------------------------------------
-- TODO: dialogues presque finis, script à finir
function guichet_12B()
	if sol.game.savegame_get_integer(1410) == 3 then	
		sol.map.dialog_start("crazy_house.guichet_12B_ech_eq_3")
	elseif sol.game.savegame_get_integer(1410) == 7 then
		sol.map.dialog_start("crazy_house.guichet_12B_ech_eq_7")
	else

	end
end

function event_hero_interaction(entity_name)
-- Tableau de mario qui parle ---------------------------------
	if entity_name == "mario_message_1" then
		sol.main.play_sound("sm64_memario")
		sol.map.dialog_start("crazy_house.mario_message_1")
	elseif entity_name == "AccueilFront" then
		accueil()
	elseif entity_name == "GC11Front" then
		guichet_11()
	end
end

function event_npc_dialog(entity_name)
	if entity_name == "Accueil" then
		accueil()
	elseif entity_name == "GC11" then
		guichet_11()
	elseif entity_name == "GC12A" then
		guichet_12A()
	elseif entity_name == "GC12B" then
		guichet_12B()
	end
end


function event_hero_on_sensor(sensor_name)
	-- Fonctionnaire en grève
	if sensor_name == "passage_sensor_A" then
		sol.map.dialog_start("crazy_house.public_agent")
	end
end

function event_dialog_finished(first_message_id, answer)
	
end
