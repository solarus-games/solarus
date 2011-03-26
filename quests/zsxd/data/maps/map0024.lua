----------------------------------
-- Crazy House 3F               --
-- TODO: MAP TERMINEE, A TESTER --
----------------------------------

giga_bouton_pushed = false
giga_bouton_camera = false
dialogue_trop_leger_fait = false

function event_map_started(destination_point_name)
	-- Desactivation des sensors pour la porte du couloir sans fin	
	sol.map.sensor_set_enabled("bowser_leave", false)
	sol.map.sensor_set_enabled("bowser_exit", false)

	-- Boutons sauvegardés
	if sol.game.savegame_get_boolean(126) then
		sol.map.sensor_set_enabled("infinite_corridor", false)
		sol.map.switch_set_activated("giga_bouton", true)
	end

	if sol.game.savegame_get_boolean(129) then
		sol.map.tile_set_group_enabled("barrier", false)
		sol.map.switch_set_activated("barrier_switch", true)
	end
end

function event_map_opening_transition_finished(destination_point_name)
	-- Affichage du nom du donjon quand on vient de l'escalier de dehors
	if destination_point_name == "fromOutsideSO" then
		sol.map.dialog_start("crazy_house.title")
	end
end

-- Guichet 31 -------------------------------------------------
function guichet_31()
	sol.map.dialog_start("crazy_house.guichet_31")
end

-- Guichet 32 -------------------------------------------------
function guichet_32()
	if sol.game.savegame_get_integer(1410) == 6 then
		sol.map.dialog_start("crazy_house.guichet_32_ech_le_6")
		sol.game.savegame_set_integer(1410, 7)
	else
		sol.map.dialog_start("crazy_house.guichet_32_ech_ne_6")
	end
	-- Incrémentation branche 1412
	branche1412 = sol.game.savegame_get_integer(1412)
	if branche1412 > 0 and branche1412 <= 4 then
		sol.game.savegame_set_integer(1412, 5)
	end
end

-- Guichet 33 -------------------------------------------------
function guichet_33()
	small_key_obtained = sol.game.savegame_get_boolean(123)
	if sol.game.savegame_get_integer(1410) == 3 and
	not small_key_obtained then	
		sol.map.dialog_start("crazy_house.guichet_33_ech_eq_3")
	elseif sol.game.savegame_get_integer(1410) >= 4 and
	not small_key_obtained  then
		if sol.game.get_item_amount("parfum_counter") >= 1 then
			-- A le parfum
			sol.map.dialog_start("crazy_house.guichet_33_parfum")
		else
			-- N'a pas encore le parfum			
			sol.map.dialog_start("crazy_house.guichet_33_ech_ge_4")
			if sol.game.savegame_get_integer(1410) == 4 then
				sol.game.savegame_set_integer(1410, 5)
			end
		end
	else
		sol.map.dialog_start("crazy_house.guichet_33_ech_le_2")
	end
end

-- Apothicaire ------------------------------------------------
function apothicaire()
	sol.map.dialog_start("crazy_house.apothicaire")
end

-- Guichet 36 -------------------------------------------------
function guichet_36()
	sol.map.dialog_start("crazy_house.guichet_36")
end

function event_npc_dialog(entity_name)
	if entity_name == "GC33" then
		guichet_33()
	elseif entity_name == "Apothicaire" then
		apothicaire()
	end
end

function event_hero_interaction(entity_name)
	if entity_name == "GC31" then
		guichet_31()
	elseif entity_name == "GC32" then
		guichet_32()
	elseif entity_name == "GC33Front" then
		guichet_33()
	elseif entity_name == "GC36" then
		guichet_36()
	elseif entity_name == "Apothicaire" then
		apothicaire()
	end
end

function event_hero_on_sensor(sensor_name)
	-- Link approche de la porte qui annonce le couloir sans fin	
	if sensor_name == "bowser_message" then
		if not sol.game.savegame_get_boolean(126) then
			-- Bouton pas appuyé
			sol.map.sensor_set_enabled("bowser_close", true)
			if not sol.map.door_is_open("bowser_door") then
				sol.map.dialog_start("crazy_house.infinite_greetings")
				sol.main.play_sound("sm64_bowser_message")
			else
				sol.map.sensor_set_enabled("bowser_leave", true)		
			end
		else
			-- Bonton déjà appuyé
			if not sol.map.door_is_open("bowser_door") then
				sol.map.door_open("bowser_door")
			end
			sol.map.sensor_set_enabled("bowser_leave", true)
		end	
	-- Fermeture de la porte derrière Link après être entré
	elseif sensor_name == "bowser_close" then
		if sol.map.door_is_open("bowser_door") then
			sol.map.door_close("bowser_door")
		end
		sol.map.sensor_set_enabled("bowser_exit", true)
		sol.map.sensor_set_enabled("bowser_close", false)
	-- Ouverture de la porte si Link souhaite sortir
	elseif sensor_name == "bowser_exit" then
		if not sol.map.door_is_open("bowser_door") then
			sol.map.door_open("bowser_door")
		end
		sol.map.sensor_set_enabled("bowser_message", false)
		sol.map.sensor_set_enabled("bowser_close", true)
		sol.map.sensor_set_enabled("bowser_exit", false)
	-- Fermeture de la porte derrière Link après être sorti
	elseif sensor_name == "bowser_leave" then
		if sol.map.door_is_open("bowser_door") then
			sol.map.door_close("bowser_door")
		end
		sol.map.sensor_set_enabled("bowser_message", true)
		sol.map.sensor_set_enabled("bowser_leave", false)
	-- Mécanisme du couloir sans fin
	elseif sensor_name == "infinite_corridor" then
		sol.map.hero_set_position(1136, 797, 1)
	-- Giga bouton : Link trop léger
	elseif sensor_name == "giga_bouton_sensor"
	and not dialogue_trop_leger_fait then
		sol.map.dialog_start("crazy_house.giga_bouton_trop_leger")
		dialogue_trop_leger_fait = true
	end
end

function event_dialog_finished(first_message_id, answer)
	if first_message_id == "crazy_house.infinite_greetings" then
		-- Ouverture de la porte vers le couloir sans fin
		if not sol.map.door_is_open("bowser_door") then
			sol.map.door_open("bowser_door")
		end
		sol.map.sensor_set_enabled("bowser_leave", true)
	elseif first_message_id == "crazy_house.guichet_33_parfum" then
		-- Obtention clé du guichet 33 suite à l'apport du parfum
		if sol.game.get_item_amount("parfum_counter") > 0 then
			sol.map.treasure_give("small_key", 1, 123)
			sol.game.remove_item_amount("parfum_counter", 1)
		end
	elseif first_message_id == "crazy_house.apothicaire" then
		-- Achat de sacs de riz à l'apothicaire		
		if answer == 0 then
			if sol.game.get_money() >= 20 then
				sol.map.dialog_start("crazy_house.apothicaire_oui")
				sol.game.remove_money(20)

				-- Incrémentation branche 1411
				branche1411 = sol.game.savegame_get_integer(1411)
				if branche1411 > 0 and branche1411 <= 4 then
					sol.game.savegame_set_integer(1411, 5)
				end

				-- Incrémentation branche 1412
				branche1412 = sol.game.savegame_get_integer(1412)
				if branche1412 >= 6 and branche1412 <= 7 then
					sol.game.savegame_set_integer(1412, 8)
				end
			else
				sol.main.play_sound("wrong")
				sol.map.dialog_start("crazy_house.apothicaire_rubis")
			end
		else
			sol.map.dialog_start("crazy_house.apothicaire_non")
		end
	elseif first_message_id == "crazy_house.apothicaire_oui" then
		-- Remise des sacs de riz achetés à l'apothicaire
		sol.map.treasure_give("sac_riz", 1, -1)
		sol.game.add_item_amount("sac_riz_counter", 4)
	elseif first_message_id == "crazy_house.guichet_36" then
		-- Achat de sacs de riz à Panoda Fichage
		if answer == 0 then
			if sol.game.get_money() >= 10 then
				sol.game.remove_money(10)
				sol.map.treasure_give("sac_riz", 1, -1)
				sol.game.add_item_amount("sac_riz_counter", 2)
				-- Incrémentation branche 1411
				branche1411 = sol.game.savegame_get_integer(1411)
				if branche1411 > 0 and branche1411 <= 4 then
					sol.game.savegame_set_integer(1411, 5)
				end
				-- Incrémentation branche 1412
				branche1412 = sol.game.savegame_get_integer(1412)
				if branche1412 >= 6 and branche1412 <= 7 then
					sol.game.savegame_set_integer(1412, 8)
				end
			else
				sol.main.play_sound("wrong")
				sol.map.dialog_start("crazy_house.guichet_36_un")
			end
		end
	elseif first_message_id == "crazy_house.guichet_32_ech_ne_6" then
		-- Echange de hache contre cuilleres		
		if answer == 0 then
			if sol.game.get_item_amount("hache_counter") >= 1 then
				sol.map.treasure_give("cuillere", 1, -1)
				sol.game.add_item_amount("cuillere_counter", 1)
				sol.game.remove_item_amount("hache_counter", 1)
			else
				sol.main.play_sound("wrong")
				sol.map.dialog_start("crazy_house.guichet_32_ech_ne_6_un")
			end
		else
			sol.map.dialog_start("crazy_house.guichet_32_ech_ne_6_no")
		end
	end
end

function event_block_moved(block_name)
	if giga_bouton_pushed == false then
		if block_name == "GBS" then	
			x, y = sol.map.block_get_position(block_name)
			if x == 1664 and y == 797 then
				giga_bouton_pushed = true
				giga_bouton_activated()
			end
		end
	end
end

-- Déplacement de la camera le long du couloir après appui sur
-- le giga bouton d'extra collision à charge supérieure de 15.000 megawatts
function giga_bouton_camera_move()
	sol.map.camera_move(888, 792, 250)
	sol.map.sensor_set_enabled("infinite_corridor", false)
end

-- Retour de la camera suite à l'appui sur le giga bouton d'extra
-- collision à charge supérieure de 15.000 megawatts
function giga_bouton_camera_back()
	sol.map.camera_restore()
end

-- Appui sur le giga bouton d'extra collision à charge supérieure de 15.000 megawatts
function giga_bouton_activated()
	sol.main.play_sound("switch")	
	sol.map.switch_set_activated("giga_bouton", true)
	sol.map.hero_freeze()
	sol.map.sensor_set_enabled("infinite_corridor", false)	
	sol.main.timer_start(500, "giga_bouton_camera_move", false)
	sol.game.savegame_set_boolean(126, true)
	giga_bouton_camera = true
end

function event_camera_reached_target()
	if giga_bouton_camera == true then	
		sol.main.timer_start(1500, "giga_bouton_camera_back", false)
		sol.main.play_sound("secret")
	end
end

function event_camera_back()
	sol.map.hero_unfreeze()
end

function event_treasure_obtained(item_name, variant, savegame_variable)

	if item_name == "glove" then
		-- Fin du donjon
		sol.map.hero_start_victory_sequence()
		sol.main.timer_start(2000, "leave_dungeon", false)
	end
end

function leave_dungeon()
	sol.main.play_sound("warp")
	sol.map.hero_set_map(3, "from_crazy_house", 1)
end

function event_switch_activated(switch_name)

	-- Ouverture de la barrière près du giga bouton
	if switch_name == "barrier_switch" then
		sol.main.play_sound("secret")
		sol.map.tile_set_group_enabled("barrier", false)
		sol.game.savegame_set_boolean(129, true)
	end
end



